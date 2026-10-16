// Typed source port of verified game-object portal selection; static doodads stay native.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ChallengeMode.hpp"
#include "NativeFeatures.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include <string_view>
#include <unordered_map>
namespace wxl::native::challenge_mode {
namespace {
constexpr char Normal[]="Spells\\wxl_instanceportal_normal.m2";
constexpr char Heroic[]="Spells\\wxl_instanceportal_heroic.m2";
constexpr char Mythic[]="Spells\\wxl_instanceportal_mythic.m2";
constexpr char Pedestal[]="World\\Dungeon\\Challenge\\challengemode_pedestal_1.m2";
constexpr char Dome[]="World\\Dungeon\\Challenge\\challengemode_wall_dome.m2";
constexpr char ChestLow[]="World\\Expansion06\\Doodads\\Dungeon\\Doodads\\7du_challengechest01.m2";
constexpr char ChestMid[]="World\\Expansion06\\Doodads\\Dungeon\\Doodads\\7du_challengechestuber01.m2";
constexpr char ChestHigh[]="World\\Expansion06\\Doodads\\Dungeon\\Doodads\\7du_challengechestuber02.m2";
struct PortalState {const char* desired=nullptr;CM2Model* resident=nullptr;};
std::unordered_map<uint64_t,PortalState> portals;
uint32_t portalMap=0;
uint64_t portalPlayer=0;
thread_local bool replacing=false;
bool NonRaid(const CGGameObject_C& object){
 if(!object.m_gameObject||!object.m_stats||object.m_gameObject->m_typeID!=31)return false;
 const int property=CGameObjectDef::GetPropNum(object.m_gameObject->m_typeID,87);
 if(property<0)return false;
 const auto id=object.GetPropertyValue(uint32_t(property));
 auto* map=g_mapDB.GetRecord(id);return map&&map->m_instanceType!=2;
}
int PortalDifficulty(const CGGameObject_C& object){
 if(!NonRaid(object))return -1;
 const int property=CGameObjectDef::GetPropNum(object.m_gameObject->m_typeID,88);
 return property<0?-1:int(object.GetPropertyValue(uint32_t(property)));
}
char Fold(char c){return c=='/'?'\\':c>='A'&&c<='Z'?char(c+'a'-'A'):c;}
bool Exact(const char* value,std::string_view stem){
 if(!value)return false;
 const std::string_view path(value);
 if(path.size()<stem.size())return false;
 for(size_t i=0;i<stem.size();++i)if(Fold(path[i])!=Fold(stem[i]))return false;
 auto extension=path.substr(stem.size());
 if(extension.empty())return true;
 if(extension.size()!=3&&extension.size()!=4)return false;
 return extension[0]=='.'&&Fold(extension[1])=='m'&&((extension.size()==3&&extension[2]=='2')||(extension.size()==4&&Fold(extension[2])=='d'&&Fold(extension[3])=='x'));
}
const char* NativeName(const CGGameObject_C& object){
 if(!object.m_gameObject)return nullptr;
 auto* display=g_gameObjectDisplayInfoDB.GetRecord(object.m_gameObject->m_displayID);
 return display?display->m_modelName:nullptr;
}
}
const char* SelectModel(const CGGameObject_C& object,const char* original) noexcept {
 if(!MythicDifficultyEnabled()||!object.m_obj||!object.m_gameObject)return original;
 switch(object.m_obj->m_entryID){
  case 5001004:return Pedestal;case 1000003:return Dome;
  case 5001100:return ChestLow;case 5001101:return ChestMid;case 5001102:return ChestHigh;
 }
 const int difficulty=PortalDifficulty(object);
 if(difficulty==0)return Normal;
 if(difficulty==1)return CGGameUI::GetCurrentDungeonDifficulty()==2?Mythic:Heroic;
 // Donor fallback is deliberately restricted to game-object model resolution.
 if(Exact(original,"spells\\instancenewportal_purple_skull"))return CGGameUI::GetCurrentDungeonDifficulty()==2?Mythic:Heroic;
 if(Exact(original,"spells\\instancenewportal_purple"))return Normal;
 return original;
}
int PortalHidden(const CGGameObject_C& object,int nativeHidden) noexcept {
 // Raid destinations retain the complete native 10/25/heroic visibility policy.
 return MythicDifficultyEnabled()&&CGGameUI::GetCurrentDungeonDifficulty()==2&&PortalDifficulty(object)==1?0:nativeHidden;
}
void RefreshPortal(CGGameObject_C& object){
 try {
 if(replacing||!object.m_obj||!object.m_worldModel||!object.m_worldModel->m_scene||!ClntObjMgrGetCurrent())return;
 const auto player=ClntObjMgrGetActivePlayer().m_guid;if(!player)return;
 const auto map=ClntObjMgrGetMapID();
 if(portalMap!=map||portalPlayer!=player){portals.clear();portalMap=map;portalPlayer=player;}
 const char* desired=SelectModel(object,NativeName(object));if(!desired)return;
 const auto guid=object.m_obj->m_guid.m_guid;
 auto found=portals.find(guid);
 // First observation adopts a fully constructed resident; no constructor-time swap.
 if(found==portals.end()){if(portals.size()<4096)portals.emplace(guid,PortalState{desired,object.m_worldModel});return;}
 auto& state=found->second;
 if(state.resident!=object.m_worldModel){state={desired,object.m_worldModel};return;}
 if(state.desired&&std::string_view(state.desired)==desired)return;
 auto* replacement=object.m_worldModel->m_scene->CreateModel(desired,0);
 if(!replacement)return;
 struct ReplacingGuard {ReplacingGuard(){replacing=true;}~ReplacingGuard(){replacing=false;}} guard;
 object.SetModel(replacement);replacement->Release();
 state={desired,object.m_worldModel};
 }catch(...){replacing=false;}
}
void DestroyPortals() noexcept {portals.clear();portalMap=0;portalPlayer=0;replacing=false;}
    constexpr char kBootstrap[] = R"lua(
do
    wxlwow = wxlwow or {}
    wxlwow.challenge_mode = wxlwow.challenge_mode or {}

    local function installMythicDifficulty()
        if type(UnitPopupButtons) ~= "table" or
           type(UnitPopupMenus) ~= "table" or
           type(UnitPopupMenus.DUNGEON_DIFFICULTY) ~= "table" then
            return false
        end

        -- 3.3.5 ships this string and the corresponding UnitPopup code as
        -- "Epic (Unused)". WarcraftXL gives the dormant mode its real role.
        DUNGEON_DIFFICULTY3 = "Mythic"
        UnitPopupButtons.DUNGEON_DIFFICULTY3 = {
            text = DUNGEON_DIFFICULTY3,
            dist = 0,
        }

        local menu = UnitPopupMenus.DUNGEON_DIFFICULTY
        for index = 1, #menu do
            if menu[index] == "DUNGEON_DIFFICULTY3" then
                wxlwow.challenge_mode.mythic_difficulty_installed = true
                return true
            end
        end
        table.insert(menu, "DUNGEON_DIFFICULTY3")
        wxlwow.challenge_mode.mythic_difficulty_installed = true
        return true
    end

    if not installMythicDifficulty() and type(CreateFrame) == "function" then
        local installer = CreateFrame("Frame")
        installer:RegisterEvent("PLAYER_LOGIN")
        installer:RegisterEvent("ADDON_LOADED")
        installer:SetScript("OnEvent", function(self)
            if installMythicDifficulty() then
                self:UnregisterAllEvents()
                self:SetScript("OnEvent", nil)
            end
        end)
    end
end
)lua";


void InstallMythicUi(){if(MythicDifficultyEnabled())FrameScript_Execute(kBootstrap,"native-mythic-difficulty-ui",nullptr);}
}
