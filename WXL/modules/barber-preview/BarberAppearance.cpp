// Native barber extra-detail previews; persistence remains server-owned.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/BarberAppearance.hpp"
#include "WXL/RetailChoices.hpp"
#include "WXL/AppearanceCompat.hpp"
#include "WXL/AppearanceSession.hpp"
#include "WXL/NativeFeatures.hpp"
#include "WXL/NativeProtocol.hpp"
#include "wxl/AppearanceProtocol.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include <algorithm>
#include <vector>
#ifndef WXL_ENABLE_BARBER_PREVIEW
#define WXL_ENABLE_BARBER_PREVIEW 1
#endif
namespace wxl::native::barber_appearance {
namespace {
CVar* enabled=nullptr;
struct Scope {
 uint64_t session=0,guid=0,serial=0;
 CCharacterComponent* component=nullptr;
 CM2Model* model=nullptr;
 uint32_t handle=0,race=0,sex=0,recipe=0;
 bool ready=false;
 std::vector<RetailChoicePair> baseline;
} scope,deferred;
CGPlayer_C* Player() noexcept {return ClntObjMgrGetCurrent()?ClntObjMgrGetActivePlayerObj():nullptr;}
CCharacterComponent* Current() noexcept {
 auto* player=Player();
 if(!player||!player->m_unit||!player->m_player||
    player->m_unit->m_unitFieldDisplayid!=player->m_unit->m_unitFieldNativedisplayid)return nullptr;
 auto* c=player->m_characterComponent;
 if(!c||!c->m_wxlSerial||!c->m_data.m_model||(c->m_data.m_flags&1u))return nullptr;
 const auto& p=c->m_data.m_preferences;
 if(p.m_raceID!=player->m_unit->m_unitFieldBytes0[0]||p.m_sexID!=player->m_player->m_playerBytes3[0]||
    !RetailChoicesReady(p,c->m_data.m_model))return nullptr;
 return c;
}
bool Matches() noexcept {
 auto* c=Current();
 if(!scope.session||!c||c!=scope.component||c->m_wxlSerial!=scope.serial||c->m_data.m_model!=scope.model||
    c->m_data.m_model->m_handle!=scope.handle||ClntObjMgrGetActivePlayer().m_guid!=scope.guid||
    protocol::SessionGeneration(FrameScript_GetContext())!=scope.session)return false;
 const auto& p=c->m_data.m_preferences;
 return p.m_raceID==scope.race&&p.m_sexID==scope.sex&&RetailRecipeRace(*c)==scope.recipe;
}
int LuaTick(lua_State*) {Tick();return 0;}
int LuaActive(lua_State* L) {lua_pushboolean(L,Active());return 1;}
int LuaReset(lua_State*) {ResetPreview();return 0;}
constexpr char Timer[]=R"lua(
if not WXLNativeBarberPreviewTicker and type(CreateFrame)=="function" then
 local f=CreateFrame("Frame");WXLNativeBarberPreviewTicker=f;local elapsed=0
 f:SetScript("OnUpdate",function(self,dt)
  elapsed=elapsed+(dt or 0)
  if elapsed>=0.2 then elapsed=0;_WXL_BarberRetailTick() end
 end)
end
)lua";
void CaptureBaseline() {
 if(scope.ready||!Matches())return;
 appearance_session::RefreshCharacter(scope.guid,scope.component);
 wxl::appearance::Snapshot snapshot;
 if(!appearance_session::GetSnapshot(scope.guid,snapshot)||snapshot.selection.race!=scope.race||snapshot.selection.sex!=scope.sex||
    !AppearanceReady(*scope.component))return;
 const auto* race=wxl::appearance::FindRace(snapshot.selection.race);if(!race)return;
 const auto* rows=scope.recipe==race->retail?&snapshot.selection.choices:
     scope.recipe==wxl::appearance::AlternateRetailRace(snapshot.selection.race)?&snapshot.selection.alternate:nullptr;
 if(!rows||rows->empty()||rows->size()>wxl::appearance::MaxChoices)return;
 std::vector<RetailChoicePair> current;
 if(!CaptureRetailChoicePairs(*scope.component,current)||current.size()!=rows->size())return;
 for(const auto& row:*rows){const auto it=std::find_if(current.begin(),current.end(),[&](auto p){return p.option==row.option;});
  if(it==current.end()||it->choice!=row.choice)return;
 }
 scope.baseline=std::move(current);scope.ready=true;
}
void RestoreDeferred() {
 if(!deferred.ready)return;
 auto* p=Player();const auto epoch=protocol::SessionGeneration(FrameScript_GetContext());
 if(!p||epoch!=deferred.session||ClntObjMgrGetActivePlayer().m_guid!=deferred.guid||
    p->m_characterComponent!=deferred.component||p->m_characterComponent->m_wxlSerial!=deferred.serial){deferred={};return;}
 auto* c=Current();if(!c)return; // Never mutate a disguise; retry after normal form returns.
 if(c->m_data.m_preferences.m_raceID!=deferred.race||c->m_data.m_preferences.m_sexID!=deferred.sex||RetailRecipeRace(*c)!=deferred.recipe){deferred={};return;}
 if(RestoreRetailExtras(*c,deferred.baseline.data(),uint32_t(deferred.baseline.size())))deferred={};
}
}
void Register(){enabled=CVar::Register("wxlBarberPreview","WXL barber retail-detail previews; extra-detail saving requires server support",0,WXL_ENABLE_BARBER_PREVIEW?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept {return WXL_ENABLE_BARBER_PREVIEW&&AppearancesEnabled()&&AppearanceSaveEnabled()&&enabled&&enabled->m_intValue;}
void InitializeLua(){End();FrameScript_RegisterFunction("_WXL_BarberRetailTick",&LuaTick);FrameScript_RegisterFunction("WXL_IsBarberRetailPreview",&LuaActive);FrameScript_RegisterFunction("WXL_ResetBarberRetailPreview",&LuaReset);}
bool Active() noexcept {return CGBarberShop::m_barberShopEnabled!=0;}
void Begin() noexcept {try{
 End();RestoreDeferred();if(!Enabled()||!Active())return;
 auto* c=Current();const auto epoch=protocol::SessionGeneration(FrameScript_GetContext());if(!c||!epoch)return;
 const auto& p=c->m_data.m_preferences;
 scope.session=epoch;scope.guid=ClntObjMgrGetActivePlayer().m_guid;scope.serial=c->m_wxlSerial;
 scope.component=c;scope.model=c->m_data.m_model;scope.handle=scope.model->m_handle;
 scope.race=p.m_raceID;scope.sex=p.m_sexID;scope.recipe=RetailRecipeRace(*c);
 CaptureBaseline();FrameScript_Execute(Timer,"native-barber-retail-preview",nullptr);
}catch(...){End();}}
void Tick() noexcept {try{
 RestoreDeferred();
 if(!Active()||!Enabled()||!Matches()){End();return;}
 CaptureBaseline();if(scope.ready)UpdateRetailChoiceUi();
}catch(...){End();}}
bool Editing(uint64_t guid,const CCharacterComponent* c) noexcept {return scope.ready&&scope.guid==guid&&scope.component==c&&Active()&&Matches();}
CCharacterComponent* Editor() noexcept {return Enabled()&&scope.ready&&Active()&&Matches()?scope.component:nullptr;}
void ResetPreview() noexcept {try{
 if(scope.ready&&Matches())RestoreRetailExtras(*scope.component,scope.baseline.data(),uint32_t(scope.baseline.size()));
}catch(...){}}
void End() noexcept {try{
 if(scope.ready){
  if(Matches())ResetPreview();
  else deferred=scope;
 }
 scope={};HideRetailChoiceUi();
}catch(...){scope={};}}
bool BeforeApply() noexcept {try{
 if(!scope.ready||!Matches())return true;
 // Saved server extras form the baseline; their mere presence never blocks
 // ordinary stock barber edits. Only an actual local extra-detail delta does.
 if(RetailExtrasMatch(*scope.component,scope.baseline.data(),uint32_t(scope.baseline.size())))return true;
 FrameScript_Execute("if UIErrorsFrame then UIErrorsFrame:AddMessage('Retail detail previews cannot be saved at the barber yet. Reset those details to apply standard barber changes.',1,0.2,0.2) end","native-barber-retail-save-unavailable",nullptr);
 return false;
}catch(...){return false;}}
}
