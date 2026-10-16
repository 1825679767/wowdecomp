// Source-owned port of extensions/wxl-quest-marker. No binary hooks or offsets.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "QuestMarker.hpp"
#include "QuestPresentation.hpp"
#include "NativeFeatures.hpp"
#include "NativeProtocol.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/UIUtil/Camera.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <deque>
#include <limits>
#include <mutex>
#include <span>
#include <vector>

namespace wxl::native::quest_marker {
namespace {
static_assert(std::endian::native == std::endian::little);
constexpr uint16_t RequestId=0x51f,MarkerId=0x520,KillId=0x536,CorpseId=0x537;
struct Marker {uint32_t quest=0,active=0;double x=0,y=0,z=0;uint32_t type=0;};
struct Kills {uint32_t count=0;std::array<uint32_t,4> entries{};};
struct Corpse {uint32_t active=0;double x=0,y=0,z=0;};
struct Settings {bool enabled=true,show=true,offscreen=true;uint32_t alpha=165;float worldScale=1,offScale=1,offsetX=0,offsetY=0;};
std::mutex mutex;
std::deque<Marker> markersIn;
std::deque<Kills> killsIn;
std::deque<Corpse> corpsesIn;
// Presentation is exclusively the Lua/owner thread. Receive queues contain values only.
std::vector<Marker> markers;
Corpse corpse;
Settings settings;
Kills objectiveEntries;
bool dead=false,changed=false,installed=false;
uint64_t generation=0;
uint32_t map=0;
uint64_t uiGeneration=0;
uint32_t uiMap=0;
bool diamond=false;
float diamondX=0,diamondY=0,diamondW=0,diamondH=0;
CTexture* ringTexture=nullptr;

bool Position(double x,double y,double z) {
 const double limit=std::numeric_limits<float>::max();
 return std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&std::abs(x)<=limit&&std::abs(y)<=limit&&std::abs(z)<=limit;
}
void ClearLocked(){markersIn.clear();killsIn.clear();corpsesIn.clear();objectiveEntries={};changed=false;generation=0;map=0;}
void Reset(void*) noexcept {try{std::lock_guard lock(mutex);ClearLocked();}catch(...){}}
struct Reader {
 std::span<const uint8_t> bytes;size_t cursor=0;
 template<class T> bool Read(T& value){if(cursor>bytes.size()||sizeof(T)>bytes.size()-cursor)return false;std::memcpy(&value,bytes.data()+cursor,sizeof(T));cursor+=sizeof(T);return true;}
};
template<class T>void Queue(std::deque<T>& queue,const T& value,size_t bound){if(queue.size()==bound)queue.pop_front();queue.push_back(value);changed=true;}
void Receive(uint16_t opcode,const uint8_t* data,uint32_t size,uint64_t epoch,void*) noexcept {
 // Runs under protocol mutex. Do not invoke Lua or reenter the protocol owner.
 if(!QuestMarkerEnabled()||!epoch||!data||!ClntObjMgrGetCurrent()||!ClntObjMgrGetActivePlayer())return;
 try{
  Reader r{{data,size}};Marker marker;Kills kills;Corpse body;
  if(opcode==MarkerId){
   if(size!=36||!r.Read(marker.quest)||!r.Read(marker.active)||!r.Read(marker.x)||!r.Read(marker.y)||!r.Read(marker.z)||!r.Read(marker.type)||marker.active>1||marker.type>2||!Position(marker.x,marker.y,marker.z))return;
  }else if(opcode==KillId){
   if(!r.Read(kills.count)||kills.count>4||size!=4+4*kills.count)return;
   for(uint32_t i=0;i<kills.count;++i)if(!r.Read(kills.entries[i]))return;
  }else if(opcode==CorpseId){
   if(!r.Read(body.active)||body.active>1||(body.active==0&&size!=4)||(body.active==1&&size!=28))return;
   if(body.active&&(!r.Read(body.x)||!r.Read(body.y)||!r.Read(body.z)||!Position(body.x,body.y,body.z)))return;
  }else return;
  if(r.cursor!=size)return;
  const auto currentMap=ClntObjMgrGetMapID();
  std::lock_guard lock(mutex);
  if(generation!=epoch||map!=currentMap){ClearLocked();generation=epoch;map=currentMap;}
  if(opcode==MarkerId)Queue(markersIn,marker,256);
  else if(opcode==CorpseId)Queue(corpsesIn,body,16);
  else{objectiveEntries=kills;Queue(killsIn,kills,32);}
 }catch(...){Reset(nullptr);}
}
bool Ready(lua_State* L){return QuestMarkerEnabled()&&protocol::SessionGeneration(L)&&ClntObjMgrGetCurrent()&&bool(ClntObjMgrGetActivePlayer());}
bool Integer(lua_State* L,int index,uint32_t maximum,uint32_t& result){
 const double n=lua_tonumber(L,index);if(lua_type(L,index)!=LUA_TNUMBER||!std::isfinite(n)||n<0||n>maximum||std::floor(n)!=n)return false;result=uint32_t(n);return true;
}
bool Point(lua_State* L,int first,double& x,double& y,double& z){
 if(lua_type(L,first)!=LUA_TNUMBER||lua_type(L,first+1)!=LUA_TNUMBER||lua_type(L,first+2)!=LUA_TNUMBER)return false;
 x=lua_tonumber(L,first);y=lua_tonumber(L,first+1);z=lua_tonumber(L,first+2);return Position(x,y,z);
}
bool Sync(lua_State* L){
 const auto epoch=Ready(L)?protocol::SessionGeneration(L):0;
 const auto currentMap=epoch?ClntObjMgrGetMapID():0;
 const bool reset=epoch!=uiGeneration||currentMap!=uiMap;
 {std::lock_guard lock(mutex);if(epoch!=generation||map!=currentMap){ClearLocked();generation=epoch;map=currentMap;}}
 uiGeneration=epoch;uiMap=currentMap;
 if(reset||!epoch){markers.clear();corpse={};dead=false;diamond=false;}
 return epoch!=0;
}
int Request(lua_State* L){
 uint32_t kind=0,quest=0;bool sent=false;
 if(Sync(L)&&Integer(L,1,103,kind)&&(kind==101||kind==102||kind==103)&&
    (kind==102||Integer(L,2,UINT32_MAX,quest))){
  std::array<uint8_t,6> wire{};uint16_t sub=uint16_t(kind);std::memcpy(wire.data(),&sub,2);std::memcpy(wire.data()+2,&quest,4);
  sent=protocol::Send(RequestId,wire.data(),kind==102?2:6);
 }
 lua_pushboolean(L,sent);return 1;
}
template<class T>bool Pop(lua_State* L,std::deque<T>& queue,T& value){if(!Sync(L))return false;std::lock_guard lock(mutex);if(queue.empty())return false;value=queue.front();queue.pop_front();return true;}
int PopMarker(lua_State* L){Marker p;bool have=false;try{have=Pop(L,markersIn,p);}catch(...){}if(!have)return 0;
 lua_pushnumber(L,p.quest);lua_pushnumber(L,p.active);lua_pushnumber(L,p.x);lua_pushnumber(L,p.y);lua_pushnumber(L,p.z);lua_pushnumber(L,p.type);return 6;}
int PopKills(lua_State* L){Kills p;bool have=false;try{have=Pop(L,killsIn,p);}catch(...){}if(!have)return 0;
 lua_pushnumber(L,p.count);for(uint32_t i=0;i<p.count;++i)lua_pushnumber(L,p.entries[i]);return 1+int(p.count);}
int PopCorpse(lua_State* L){Corpse p;bool have=false;try{have=Pop(L,corpsesIn,p);}catch(...){}if(!have)return 0;
 lua_pushnumber(L,p.active);if(!p.active)return 1;lua_pushnumber(L,p.x);lua_pushnumber(L,p.y);lua_pushnumber(L,p.z);return 4;}
int SetMarker(lua_State* L){
 uint32_t quest=0,active=0;Marker p;
 if(!Sync(L)||!Integer(L,1,UINT32_MAX,quest)||!Integer(L,2,1,active))return 0;
 auto found=std::find_if(markers.begin(),markers.end(),[quest](const Marker& m){return m.quest==quest;});
 if(!active){if(found!=markers.end())markers.erase(found);return 0;}
 if(!quest||!Point(L,3,p.x,p.y,p.z))return 0;p.quest=quest;p.active=1;
 if(found!=markers.end())*found=p;else if(markers.size()<256)markers.push_back(p);return 0;
}
int ClearMarkers(lua_State*){markers.clear();return 0;}
int SetQuestEntries(lua_State* L){uint32_t count=0;if(!Sync(L)||!Integer(L,1,4,count))return 0;Kills next;
 for(uint32_t i=0;i<count;++i){uint32_t entry=0;if(!Integer(L,int(i)+2,UINT32_MAX,entry))return 0;if(entry)next.entries[next.count++]=entry;}
 std::lock_guard lock(mutex);objectiveEntries=next;return 0;}
int ClearQuestEntries(lua_State* L){if(Sync(L)){std::lock_guard lock(mutex);objectiveEntries={};}return 0;}
int SetCorpse(lua_State* L){Corpse p;if(Sync(L)&&Point(L,1,p.x,p.y,p.z)){p.active=1;corpse=p;}return 0;}
int ClearCorpse(lua_State*){corpse={};return 0;}
int IsCorpse(lua_State* L){if(Sync(L)&&corpse.active)lua_pushnumber(L,1);else lua_pushnil(L);return 1;}
int SetDead(lua_State* L){uint32_t value=0;if(Sync(L)&&Integer(L,1,1,value))dead=value!=0;return 0;}
bool PlayerPosition(C3Vector& out){auto* player=ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(),TYPEMASK_PLAYER,__FILE__,__LINE__);if(!player)return false;out=player->GetPosition();return Position(out.x,out.y,out.z);}
int PlayerPos(lua_State* L){C3Vector p{};if(!Sync(L)||!PlayerPosition(p)){lua_pushnil(L);return 1;}lua_pushnumber(L,p.x);lua_pushnumber(L,p.y);lua_pushnumber(L,p.z);return 3;}
bool Project(double x,double y,double z,float& nx,float& ny,double& distance){
 auto* frame=CGWorldFrame::s_currentWorldFrame;C3Vector player{},screen{};
 if(!frame||!frame->m_camera||!PlayerPosition(player))return false;
 const double dx=x-player.x,dy=y-player.y,dz=z-player.z;distance=std::sqrt(dx*dx+dy*dy+dz*dz);
 if(!std::isfinite(distance)||distance>100000)return false;
 const float width=NDCToDDCWidth(1),height=NDCToDDCHeight(1);uint32_t clip=0;
 const bool visible=frame->GetScreenCoordinates(C3Vector(float(x),float(y),float(z)),&screen,&clip);
 if(!visible||!(screen.z>0)||!(width>0)||!(height>0)||!Position(screen.x,screen.y,screen.z))return false;
 nx=2*screen.x/width-1;ny=2*screen.y/height-1;return std::isfinite(nx)&&std::isfinite(ny)&&std::abs(nx)<=1&&std::abs(ny)<=1;
}
int Distance(lua_State* L,bool body){float x=0,y=0;double d=0;bool ok=Sync(L)&&settings.enabled;
 if(body)ok=ok&&corpse.active&&Project(corpse.x,corpse.y,corpse.z,x,y,d);
 else ok=ok&&!dead&&!markers.empty()&&Project(markers.front().x,markers.front().y,markers.front().z,x,y,d);
 if(ok){lua_pushnumber(L,x);lua_pushnumber(L,y);lua_pushnumber(L,d);}else{lua_pushnil(L);lua_pushnil(L);lua_pushnil(L);}return 3;
}
int Dist(lua_State* L){return Distance(L,false);}int CorpseDist(lua_State* L){return Distance(L,true);}
int Alpha(lua_State* L){uint32_t alpha=0;if(Integer(L,1,255,alpha))settings.alpha=alpha;return 0;}
float Number(lua_State* L,int i,float fallback,float low,float high){const double n=lua_tonumber(L,i);return lua_type(L,i)==LUA_TNUMBER&&std::isfinite(n)?float(std::clamp(n,double(low),double(high))):fallback;}
int Reload(lua_State* L){
 settings.enabled=Number(L,1,1,0,1)!=0;settings.show=Number(L,2,1,0,1)!=0;settings.alpha=uint32_t(Number(L,3,165,0,255));
 settings.worldScale=Number(L,4,1,.5f,2);settings.offScale=Number(L,5,1,.5f,2);settings.offsetX=Number(L,6,0,-500,500);settings.offsetY=Number(L,7,0,-350,350);settings.offscreen=Number(L,8,1,0,1)!=0;return 0;
}
int Diamond(lua_State* L){diamond=Number(L,1,0,0,1)!=0;diamondX=Number(L,2,0,-1,1);diamondY=Number(L,3,0,-1,1);diamondW=Number(L,4,0,0,1);diamondH=Number(L,5,0,0,1);return 0;}
int Visual(lua_State* L){
 if(!Sync(L)||!settings.enabled||!settings.show){lua_pushnil(L);return 1;}
 float x=0,y=0;double d=0;bool visible=false;
 if(dead&&corpse.active)visible=Project(corpse.x,corpse.y,corpse.z,x,y,d);
 else if(!dead&&!markers.empty())visible=Project(markers.front().x,markers.front().y,markers.front().z,x,y,d);
 bool offscreen=false;double angle=0;
 if(!visible&&settings.offscreen){
  auto* frame=CGWorldFrame::s_currentWorldFrame;C3Vector player{};
  const bool has=dead?corpse.active!=0:!markers.empty();
  if(has&&frame&&frame->m_camera&&PlayerPosition(player)){
   const double dx=(dead?corpse.x:markers.front().x)-player.x,dy=(dead?corpse.y:markers.front().y)-player.y;
   const auto right=frame->m_camera->Right(),forward=frame->m_camera->Forward();
   const double side=dx*right.x+dy*right.y,ahead=dx*forward.x+dy*forward.y;
   if(std::isfinite(side)&&std::isfinite(ahead)){
    float px=0,py=0;double ignored=0;
    if(Project(player.x,player.y,player.z+2.8,px,py,ignored)){
     const float width=NDCToDDCWidth(1),height=NDCToDDCHeight(1);
     x=std::clamp(std::clamp(px,-.8f,.8f)+2*settings.offsetX/width,-.9f,.9f);
     y=std::clamp(std::clamp(py+.055f,-.7f,.72f)+2*settings.offsetY/height,-.82f,.82f);
     angle=std::atan2(side,ahead);offscreen=true;visible=true;
    }
   }
  }
 }
 if(!visible){lua_pushnil(L);return 1;}
 lua_pushnumber(L,x);lua_pushnumber(L,y);lua_pushnumber(L,offscreen?settings.offScale:settings.worldScale);lua_pushnumber(L,settings.alpha/255.0);lua_pushboolean(L,dead);lua_pushboolean(L,offscreen);lua_pushnumber(L,angle);return 7;
}
int DiamondVisual(lua_State* L){if(!Sync(L)||!settings.enabled||!diamond||dead){lua_pushnil(L);return 1;}lua_pushnumber(L,diamondX);lua_pushnumber(L,diamondY);lua_pushnumber(L,diamondW);lua_pushnumber(L,diamondH);return 4;}
int RotateArrow(lua_State* L){
 if(!Ready(L))return 0;
 const float angle=Number(L,2,0,-3.141593f,3.141593f);
 auto* texture=static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L,CSimpleTexture::GetObjectType()));
 // Rotate actual geometry rather than atlas UVs, which would sample adjacent art.
 texture->StoreAnimPosition();texture->AddAnimRotation(FRAMEPOINT_CENTER,C2Vector(0,0),-angle);return 0;
}
int Pump(lua_State* L){
 bool notify=false;
 if(Sync(L)){std::lock_guard lock(mutex);notify=changed;changed=false;}
 if(notify)FrameScript_Execute("if wxlwow and wxlwow.quest_tracker and wxlwow.quest_tracker._NativeChanged then wxlwow.quest_tracker._NativeChanged() end","native-quest-marker-update",nullptr);
 return 0;
}
struct RingState{Kills entries;C3Vector player;uint32_t drawn=0;};
int32_t RenderUnit(WOWGUID guid,void* context){
 auto& state=*static_cast<RingState*>(context);auto& entries=state.entries;
 if(state.drawn>=24)return 0;
 auto* unit=static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid,TYPEMASK_UNIT,__FILE__,__LINE__));
 if(!unit||unit->m_typeId!=ID_UNIT||!unit->m_obj||!unit->m_obj->m_entryID||unit->GetPredictedHealth()<=0||!unit->m_worldModel)return 1;
 if(std::find(entries.entries.begin(),entries.entries.begin()+entries.count,unit->m_obj->m_entryID)==entries.entries.begin()+entries.count)return 1;
 const C3Vector position=unit->GetModelPosition();const float r=unit->m_selectionRadius;
 if(!Position(position.x,position.y,position.z)||!std::isfinite(r)||r<.5f||r>30)return 1;
 const auto delta=position-state.player;if(delta.SquaredMag()>60*60)return 1;
 CAaBox box;box.b={position.x-r,position.y-r,position.z-r};box.t={position.x+r,position.y+r,position.z+r};
 C44Matrix rotation;CGObject_C::BuildSelectionRotMatrix(&rotation,&position);
 // Donor fixed FFFFC800, brightness .40, opacity .85; explicit BGRA channels.
 ProjectTex::Render2d(box,CImVector(0,80,102,216),&rotation,.5f,0x200122,1,.4f);++state.drawn;return 1;
}
}
bool Install(){
 if(!installed){const protocol::NativeDeclaration ids[]={{RequestId,true},{MarkerId,false,Receive,Reset},{KillId,false,Receive,Reset},{CorpseId,false,Receive,Reset}};installed=protocol::RegisterNativeDispatch(ids,4);}
 if(!installed)return false;
 struct Function{const char* name;lua_CFunction function;};
 const Function functions[]={
 {"SetSelCircleQuestEntries",SetQuestEntries},{"ClearSelCircleQuestEntries",ClearQuestEntries},
 {"SetQuestMarker",SetMarker},{"ClearAllQuestMarkers",ClearMarkers},{"GetDistInfo",Dist},{"SetMarkerAlpha",Alpha},{"QuestTrackerReloadSettings",Reload},
 {"SetCorpseMarker",SetCorpse},{"ClearCorpseMarker",ClearCorpse},{"IsCorpseActive",IsCorpse},{"GetCorpseDistInfo",CorpseDist},{"SetDeadState",SetDead},
 {"GetPlayerPosition",PlayerPos},{"SetTrackerDiamond",Diamond},{"_WXL_QUEST_TRACKER_REQUEST",Request},{"_WXL_QUEST_TRACKER_POP_MARKER",PopMarker},
 {"_WXL_QUEST_TRACKER_POP_KILL",PopKills},{"_WXL_QUEST_TRACKER_POP_CORPSE",PopCorpse},{"WXL_NativeQuestPump",Pump},{"WXL_NativeQuestVisual",Visual},{"WXL_NativeQuestDiamond",DiamondVisual},{"WXL_NativeQuestRotateArrow",RotateArrow}};
 for(const auto& f:functions)FrameScript_RegisterFunction(f.name,f.function);
 FrameScript_Execute(Bootstrap,"native-quest-marker-api",nullptr);return true;
}
void UiReady(){if(installed)FrameScript_Execute(Presentation,"native-quest-marker-presentation",nullptr);}
void RenderObjectives(){
 if(!QuestMarkerEnabled()||!settings.enabled||dead||!CGGameUI::m_visible||!ClntObjMgrGetCurrent()||!ClntObjMgrGetActivePlayer()||!g_theGxDevicePtr)return;
 RingState state;{std::lock_guard lock(mutex);if(!generation||map!=ClntObjMgrGetMapID())return;state.entries=objectiveEntries;}
 if(!state.entries.count||!PlayerPosition(state.player))return;
 if(!ringTexture){CStatus status;ringTexture=TextureCreate("Textures\\questmarker\\unitselecttexture-quest.blp",CGxTexFlags(),&status,1);}
 auto* texture=ringTexture?TextureGetGxTex(ringTexture,0,nullptr):nullptr;
 if(!texture)return;
 g_theGxDevicePtr->RsPush();
 if(CGObject_C::SetCircleRenderStates()){g_theGxDevicePtr->RsSet(GxRs_Texture0,texture);ClntObjMgrEnumVisibleObjects(RenderUnit,&state);}
 g_theGxDevicePtr->RsPop();
}
void Destroy() noexcept {
 Reset(nullptr);markers.clear();corpse={};dead=false;diamond=false;settings={};uiGeneration=0;uiMap=0;
 if(ringTexture)HandleClose(reinterpret_cast<HOBJECT>(ringTexture));ringTexture=nullptr;
}
}
