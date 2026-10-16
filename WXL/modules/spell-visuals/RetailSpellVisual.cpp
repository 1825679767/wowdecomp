// Native owner for the existing donor spell447445 model presentation bridge.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "RetailSpellVisual.hpp"
#include "RetailSpellCatalog.hpp"
#include "AssetCompat.hpp"
#include "FdidCompat.hpp"
#include "NativeFeatures.hpp"
#include "NativeProtocol.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "common/Log.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
namespace wxl::native::spell_visuals {
namespace {
constexpr uint32_t Spell=447445,RootAttachment=19,RetryMs=250;
constexpr size_t MaxModels=64,MaxPending=64,MaxActive=256;
std::atomic<bool> enabled{WXL_ENABLE_RETAIL_SPELL_VISUALS!=0},playback{false},stopping{false};
enum class CatalogState {Empty,Loading,Ready,Failed};
std::atomic<CatalogState> catalogState{CatalogState::Empty};
struct Choice {GraphModel source;std::string path;float scale;};
std::mutex catalogMutex;std::vector<Choice> choices;std::thread worker;
bool Changed(CVar*,const char*,const char* text,void* arg){char* end=nullptr;const long n=text?std::strtol(text,&end,10):0;const bool value=text&&end!=text&&!*end&&n==1;
 if(arg)playback.store(value,std::memory_order_release);else enabled.store(WXL_ENABLE_RETAIL_SPELL_VISUALS&&value,std::memory_order_release);return true;}
bool On(){return enabled.load(std::memory_order_acquire)&&AssetsEnabled()&&Db2Enabled()&&!stopping.load(std::memory_order_acquire);}
std::string Canonical(const char* path){std::string result=path?path:"";for(auto& c:result){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c+'a'-'A');}if(result.size()>4&&result.compare(result.size()-4,4,".mdx")==0)result.replace(result.size()-4,4,".m2");return result;}
void BuildChoices() noexcept {
 try{
  auto graph=BuildCatalog();std::vector<Choice> result;
  if(!graph||!graph->error.empty()||graph->models.empty()||graph->models.size()>MaxModels){WLOG_WARN("native-retail-spell: catalog unavailable: %s",graph?graph->error.c_str():"empty graph");catalogState.store(CatalogState::Failed,std::memory_order_release);return;}
  result.reserve(graph->models.size());
  for(const auto& model:graph->models){if(stopping.load(std::memory_order_acquire))return;if(!model.fileDataId)continue;const auto* resolved=ResolveModel(model.fileDataId);if(!resolved||!*resolved)continue;
   auto path=Canonical(resolved);if(path.empty()||path.size()>=256)continue;
   float scale=std::isfinite(model.scale)&&model.scale>0?model.scale:1.0f;scale*=3.0f;
   if(!std::isfinite(scale)||scale<=0)continue;result.push_back({model,std::move(path),scale});
  }
  if(result.empty()){WLOG_WARN("native-retail-spell: catalog has no resolved model paths");catalogState.store(CatalogState::Failed,std::memory_order_release);return;}
  const auto count=result.size();{std::lock_guard lock(catalogMutex);choices=std::move(result);}
  catalogState.store(CatalogState::Ready,std::memory_order_release);WLOG_INFO("native-retail-spell: ready spell=%u models=%zu",Spell,count);
 }catch(...){catalogState.store(CatalogState::Failed,std::memory_order_release);WLOG_WARN("native-retail-spell: catalog construction failed");}
}
void StartCatalog(){if(!On())return;auto expected=CatalogState::Empty;if(!catalogState.compare_exchange_strong(expected,CatalogState::Loading))return;
 try{worker=std::thread(BuildChoices);}catch(...){catalogState.store(CatalogState::Failed,std::memory_order_release);}}
bool Reached(uint32_t now,uint32_t deadline){return int32_t(now-deadline)>=0;}
struct Pending {uint64_t guid;uint32_t expires,nextAttempt;};
enum class Phase {Waiting,Startup,Hold,Disabled};
struct Active {
 uint64_t guid{};CM2Model* root=nullptr;CM2Model* model=nullptr;uint32_t rootHandle{},expires{},started{},nextPlayback{};float scale=1;
 Phase phase=Phase::Waiting;std::string path;
 ~Active(){
  // The scene pre-destruction callback retires these references before the
  // native scene's force-release loop. Saved roots are held native references.
  if(model){model->SetEmittersEnabled(0);model->SetRibbonsEnabled(0);if(model->m_attachParent)model->DetachFromParent();model->Release();}
  if(root)root->Release();
 }
};
std::vector<Pending> pending;std::vector<std::unique_ptr<Active>> active;
uint64_t session=0;uint32_t map=0;lua_State* context=nullptr;CM2Scene* prewarmedScene=nullptr;uint32_t nextPrewarm=0;
void Clear(){active.clear();pending.clear();prewarmedScene=nullptr;nextPrewarm=0;}
bool SyncSession(lua_State* L){
 const auto epoch=On()?protocol::SessionGeneration(L):0;
 const bool world=epoch&&CGGameUI::s_inWorld&&ClntObjMgrGetCurrent()&&bool(ClntObjMgrGetActivePlayer());
 const uint32_t currentMap=world?ClntObjMgrGetMapID():0;
 if(!world){Clear();session=0;map=0;context=L;return false;}
 if(context!=L||session!=epoch||map!=currentMap){Clear();context=L;session=epoch;map=currentMap;}return true;
}
CGUnit_C* Unit(uint64_t guid){return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{guid},TYPEMASK_UNIT,__FILE__,__LINE__));}
void Stop(uint64_t guid){std::erase_if(pending,[&](const Pending& p){return p.guid==guid;});std::erase_if(active,[&](const auto& p){return p->guid==guid;});}
bool NativeModelName(const CM2Model* model,const std::string& path){return model&&model->m_shared&&Canonical(model->m_shared->m_fileName)==path;}
void Placement(CM2Model& model,float scale){
 C44Matrix matrix=model.m_worldTransform;
 auto normalize=[&](float& x,float& y,float& z){const float length=std::sqrt(x*x+y*y+z*z);if(!std::isfinite(length)||length<=0.0001f)return;const float factor=scale/length;x*=factor;y*=factor;z*=factor;};
 normalize(matrix.a0,matrix.a1,matrix.a2);normalize(matrix.b0,matrix.b1,matrix.b2);normalize(matrix.c0,matrix.c1,matrix.c2);model.SetWorldTransform(matrix);
}
void Prewarm(const std::vector<Choice>& current,uint32_t now){
 if(Reached(now,nextPrewarm)==false)return;
 auto* unit=Unit(ClntObjMgrGetActivePlayer().m_guid);auto* root=unit?unit->m_worldModel:nullptr;auto* scene=root?root->m_scene:nullptr;
 if(!scene||scene==prewarmedScene)return;nextPrewarm=now+RetryMs;
 for(const auto& choice:current){if(!SFile::FileExists(choice.path.c_str()))return;auto* model=scene->CreateModel(choice.path.c_str(),0);if(!model)return;const bool correct=NativeModelName(model,choice.path);model->Release();if(!correct)return;}
 prewarmedScene=scene;
}
bool Activate(const Pending& request,const std::vector<Choice>& current){
 auto* unit=Unit(request.guid);auto* root=unit?unit->m_worldModel:nullptr;
 if(!root||!root->m_scene||!root->IsLoaded(0,0)||current.empty()||current.size()>MaxActive-active.size())return false;
 // Reserve every container/string owner before attaching anything to native
 // parents. A failed group retires only the models owned by this request.
 std::vector<std::unique_ptr<Active>> group;group.reserve(current.size());active.reserve(active.size()+current.size());
 for(size_t i=0;i<current.size();++i){const auto& choice=current[i];if(!SFile::FileExists(choice.path.c_str()))return false;
  auto item=std::make_unique<Active>();item->path=choice.path;item->guid=request.guid;item->expires=request.expires;item->scale=choice.scale;item->rootHandle=root->m_handle;
  auto* model=root->m_scene->CreateModel(choice.path.c_str(),0);if(!model)return false;item->model=model;
  if(!NativeModelName(model,choice.path))return false;
  ++root->m_refCount;item->root=root;
  // Preserve donor root-slot attachment behavior through the typed API. Cleanup
  // never calls DetachAllChildrenById, so another owner's slot use is untouched.
  model->AttachToParent(root,RootAttachment+uint32_t(i),nullptr,1);
  if(model->m_attachParent!=root)return false;
  model->SetWorldTransform(C3Vector(0,0,0),0,item->scale);group.push_back(std::move(item));
 }
 for(auto& item:group)active.push_back(std::move(item));return true;
}
bool PlaybackContract(const CM2Model& model){
 if(!model.m_shared||!IsExactRiftSource(*model.m_shared)||!model.m_shared->m_data||!model.m_bones)return false;
 const auto& data=*model.m_shared->m_data;if(data.m_sequences.m_count!=4||!data.m_bones.m_count)return false;
 const uint16_t ids[]={213,0,158,159};const uint32_t durations[]={1000,1000,333,700};
 for(unsigned i=0;i<4;++i)if(data.m_sequences[i].m_id!=ids[i]||data.m_sequences[i].m_variationIndex||data.m_sequences[i].m_duration!=durations[i])return false;
 return true;
}
void UpdatePlayback(Active& visual,uint32_t now){
 if(!playback.load(std::memory_order_acquire)||visual.phase==Phase::Disabled||visual.phase==Phase::Hold||!Reached(now,visual.nextPlayback))return;
 if(!visual.model->IsLoaded(0,0)||visual.model->m_attachParent!=visual.root||!PlaybackContract(*visual.model)){visual.nextPlayback=now+RetryMs;return;}
 const auto request=visual.phase==Phase::Waiting?213u:now-visual.started>=1000?158u:UINT32_MAX;if(request==UINT32_MAX)return;
 if(!visual.model->HasSequence(request)){visual.phase=Phase::Disabled;return;}
 visual.model->SetBoneSequence(UINT32_MAX,request,UINT32_MAX,0,1.0f,1,1);
 if(request==213){visual.phase=Phase::Startup;visual.started=now;}else visual.phase=Phase::Hold;
 WLOG_INFO("native-retail-spell: requested exact-source Rift sequence=%u; lifetime unchanged",request);
}
void TickOwner(lua_State* L){
 if(!SyncSession(L))return;StartCatalog();const auto now=GetTickCount();
 for(auto it=active.begin();it!=active.end();){auto& item=**it;auto* unit=Unit(item.guid);
  if(Reached(now,item.expires)||!unit||unit->m_worldModel!=item.root||item.root->m_handle!=item.rootHandle||item.model->m_attachParent!=item.root||!NativeModelName(item.model,item.path)){it=active.erase(it);continue;}
  // Normalize only this owned model's local basis, matching donor per-frame
  // placement without reading/writing unit or world transforms.
  Placement(*item.model,item.scale);UpdatePlayback(item,now);++it;
 }
 if(catalogState.load(std::memory_order_acquire)!=CatalogState::Ready)return;
 std::vector<Choice> current;{std::lock_guard lock(catalogMutex);current=choices;}Prewarm(current,now);
 for(auto it=pending.begin();it!=pending.end();){if(Reached(now,it->expires)){it=pending.erase(it);continue;}if(!Reached(now,it->nextAttempt)){++it;continue;}it->nextAttempt=now+RetryMs;
  if(Activate(*it,current))it=pending.erase(it);else ++it;
 }
}
uint32_t Unsigned(lua_State* L,int index){if(lua_type(L,index)!=LUA_TNUMBER)return 0;const double value=lua_tonumber(L,index);return std::isfinite(value)&&value>=0&&value<=double(UINT32_MAX)?uint32_t(value):0;}
int LuaSpawn(lua_State* L){const auto spell=Unsigned(L,1),high=Unsigned(L,2),low=Unsigned(L,3),duration=Unsigned(L,4);if(spell!=Spell||(!high&&!low))return 0;
 try{if(!SyncSession(L))return 0;const uint64_t guid=(uint64_t(high)<<32)|low;Stop(guid);if(pending.size()>=MaxPending)return 0;const auto now=GetTickCount();const auto bounded=(std::clamp)(duration,250u,uint32_t(INT32_MAX));pending.push_back({guid,now+bounded,now});StartCatalog();}catch(...){Clear();}return 0;}
int LuaStop(lua_State* L){const auto spell=Unsigned(L,1),high=Unsigned(L,2),low=Unsigned(L,3);if(spell==Spell&&(high||low))try{if(SyncSession(L))Stop((uint64_t(high)<<32)|low);}catch(...){Clear();}return 0;}
int LuaTick(lua_State* L){try{TickOwner(L);}catch(...){Clear();}return 0;}
constexpr char Bootstrap[]=R"lua(
do
 if not CreateFrame or not UnitGUID then return end
 wxlwow=wxlwow or {}
 wxlwow.play_retail_spell_visual=_WXLWOW_PLAY_RETAIL_SPELL_VISUAL
 wxlwow.stop_retail_spell_visual=_WXLWOW_STOP_RETAIL_SPELL_VISUAL
 local owner=_G.WXLRetailSpellVisuals or {};_G.WXLRetailSpellVisuals=owner
 if not owner.frame then
  owner.frame=CreateFrame("Frame");owner.frame:RegisterEvent("CHAT_MSG_ADDON")
  owner.frame:SetScript("OnEvent",function(_,_,prefix,message)
   if prefix~="WXL_RETAIL" or type(message)~="string" then return end
   local spell,high,low,duration=string.match(message,"^SPAWN\t(%d+)\t(%d+)\t(%d+)\t(%d+)$")
   if spell then wxlwow.play_retail_spell_visual(tonumber(spell),tonumber(high),tonumber(low),tonumber(duration));return end
   spell,high,low=string.match(message,"^STOP\t(%d+)\t(%d+)\t(%d+)$")
   if spell then wxlwow.stop_retail_spell_visual(tonumber(spell),tonumber(high),tonumber(low))end
  end)
  owner.frame:SetScript("OnUpdate",function()_WXL_RETAIL_SPELL_TICK()end)
 end
end
)lua";
}
void Register(){
 auto* cv=CVar::Register("wxlRetailSpellVisuals","Source-owned retail spell447445 presentation",2,WXL_ENABLE_RETAIL_SPELL_VISUALS?"1":"0",Changed,0,false,nullptr,false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,nullptr);
 cv=CVar::Register("wxlRiftPlayback","Exact-source Rift startup213 then hold158; 0 native playback, 1 phase requests",2,"0",Changed,0,false,reinterpret_cast<void*>(1),false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,reinterpret_cast<void*>(1));
}
bool Enabled() noexcept {return On();}
void Install(){if(!FrameScript_GetContext())return;WorldLeave();
 FrameScript_RegisterFunction("_WXLWOW_PLAY_RETAIL_SPELL_VISUAL",LuaSpawn);FrameScript_RegisterFunction("_WXLWOW_STOP_RETAIL_SPELL_VISUAL",LuaStop);FrameScript_RegisterFunction("_WXL_RETAIL_SPELL_TICK",LuaTick);StartCatalog();}
void UiReady(){if(FrameScript_GetContext())FrameScript_Execute(Bootstrap,"WXLNativeRetailSpellVisuals",nullptr);}
void WorldLeave() noexcept {Clear();session=0;map=0;context=nullptr;}
void ReleaseScene(CM2Scene* scene) noexcept {if(!scene)return;std::erase_if(active,[&](const auto& item){return item->model->m_scene==scene||item->root->m_scene==scene;});if(prewarmedScene==scene){prewarmedScene=nullptr;nextPrewarm=0;}}
void Shutdown() noexcept {stopping.store(true,std::memory_order_release);WorldLeave();if(worker.joinable())worker.join();std::lock_guard lock(catalogMutex);choices.clear();}
}
