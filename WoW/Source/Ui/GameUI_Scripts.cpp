#include "GameUI.h"
#include <cmath>
#include <cstdarg>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/Object/ObjectClient/Trade_C.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/SimpleFrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ExhaustionRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/CGMinigameInfo.h"
#include "WoW/Source/Ui/CGQuestPOIFrame.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/DanceStudio.h"
#include "WoW/Source/Ui/DressUpModelFrame.h"
#include "WoW/Source/Ui/DuelInfo.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GMSurveyInfo.h"
#include "WoW/Source/Ui/GMTicketInfo.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/GuildRegistrar.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/Ui/KnowledgeBase.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/PetitionInfo.h"
#include "WoW/Source/Ui/PetitionVendor.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/Screen.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SkillInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TabardModelFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/Ui/VideoOptions.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "_Triage/CWOWClientStatus.h"
#include "_Triage/GxRestart.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/Object/Movement.h"

// STATIC: 0x00BD0CC4
static CWOWClientStatus* s_loadAddOnStatus;

// STATIC: 0x00BD0CC8
static int32_t s_loadAddOnDepth;

// STATIC: 0x00AC8074
static const char* s_instanceTypeTokens[5] = { "none", "party", "raid", "pvp", "arena" };

// VERIFY: 2dd9570a auto
// OFFSET: 0x005109D0
static int32_t Script_BeginTrade(lua_State* L) {
    Trade_C_BeginTrade();
    return 0;
}

// VERIFY: 3e32da49 auto
// OFFSET: 0x005109E0
static int32_t Script_CancelTrade(lua_State* L) {
    Trade_C_CancelTrade();
    return 0;
}

// VERIFY: 64da4b3d auto
// OFFSET: 0x00510A00
static int32_t Script_ForceQuit(lua_State* L) {
    ClientPostClose(0);
    return 0;
}

// VERIFY: 8710af84 auto
// OFFSET: 0x00510DC0
int32_t Script_RestoreVideoResolutionDefaults(lua_State* L) {
    ConsoleDeviceDefaults(OPTION_PAGE_0);
    return 0;
}

// VERIFY: f8aec6b2 auto
// OFFSET: 0x00510DD0
int32_t Script_RestoreVideoEffectsDefaults(lua_State* L) {
    ConsoleDeviceDefaults(OPTION_PAGE_1);
    return 0;
}

// VERIFY: e74fa653 auto
// OFFSET: 0x00511830
static int32_t Script_ResetCPUUsage(lua_State* L) {
    FrameScript_ResetCPUUsage();
    return 0;
}

// VERIFY: 572cb276 auto
// OFFSET: 0x00515560
static int32_t Script_TargetDirectionFinished(lua_State* L) {
    CGGameUI::m_directionalTarget = WOWGUID();
    return 0;
}

// VERIFY: 0f8c89fc auto
// OFFSET: 0x00517D20
static int32_t Script_DeclineLevelGrant(lua_State* L) {
    CGGameUI::m_pendingLevelGranter = WOWGUID();
    return 0;
}

// VERIFY: 8972f8b9 auto
// OFFSET: 0x00522F90
static int32_t Script_NotWhileDeadError(lua_State* L) {
    CGGameUI::DisplayError(ERR_PLAYER_DEAD);
    return 0;
}

// VERIFY: 1e86b873 auto
// OFFSET: 0x0050FE80
static int32_t Script_IsDebugBuild(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

// VERIFY: 362d8ac1 auto
// OFFSET: 0x00510DE0
static int32_t Script_SetEuropeanNumbers(lua_State* L) {
    SetDecimalConversion(StringToBOOL(L, 1, 1));
    return 0;
}

// VERIFY: 8c53a6db auto
// OFFSET: 0x00510E00
static int32_t Script_GetRealmName(lua_State* L) {
    lua_pushstring(L, ClientServices::GetSelectedRealmName());
    return 1;
}

// VERIFY: 5e600bbc auto
// OFFSET: 0x00511510
static int32_t Script_UpdateAddOnMemoryUsage(lua_State* L) {
    lua_gcupdatesizes(L);
    return 0;
}

// VERIFY: 94964fee auto
// OFFSET: 0x00511640
static int32_t Script_UpdateAddOnCPUUsage(lua_State* L) {
    lua_gcupdatetimes(L);
    return 0;
}

// VERIFY: 201e323d auto
// OFFSET: 0x00511A40
static int32_t Script_ResetDisabledAddOns(lua_State* L) {
    LoadAddOnEnableState(ClientServices::GetCharacterName());
    return 0;
}

// VERIFY: dd437cf6 auto
// OFFSET: 0x0051A3B0
static int32_t Script_ClearCursor(lua_State* L) {
    CGGameUI::ClearCursor(1, 1);
    return 1;
}

// VERIFY: 55878a9c auto
// OFFSET: 0x005101D0
static int32_t Script_SetWaterDetail(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetWaterDetail(value)");
    }

    return 0;
}

// VERIFY: 218d4185 auto
// OFFSET: 0x00510200
static int32_t Script_GetFarclip(lua_State* L) {
    lua_pushnumber(L, CVar::Lookup("farclip")->m_floatValue);
    return 1;
}

// VERIFY: 7624bb8f auto
// OFFSET: 0x005102B0
static int32_t Script_GetTexLodBias(lua_State* L) {
    lua_pushnumber(L, CVar::Lookup("texLodBias")->m_floatValue);
    return 1;
}

// VERIFY: 92892b19 auto
// OFFSET: 0x00510360
static int32_t Script_GetBaseMip(lua_State* L) {
    lua_pushnumber(L, 1.0 - CVar::Lookup("baseMip")->m_floatValue);
    return 1;
}

// VERIFY: 43339e6b auto
// OFFSET: 0x00511610
static int32_t Script_GetScriptCPUUsage(lua_State* L) {
    lua_pushnumber(L, FrameScript_GetScriptCPUUsage());
    return 1;
}

// VERIFY: 47e47f12 manual
// OFFSET: 0x00511FB0
static int32_t Script_IsDesaturateSupported(lua_State* L) {
    if (!CSimpleTexture::GetImageModePixelShader(ImageMode_Desaturate)) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: cee80fa8 auto
// OFFSET: 0x00515570
static int32_t Script_GetZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_zoneText ? CGGameUI::m_zoneText : "");
    return 1;
}

// VERIFY: 74ec3f6a auto
// OFFSET: 0x005155A0
static int32_t Script_GetRealZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_realZoneText ? CGGameUI::m_realZoneText : "");
    return 1;
}

// VERIFY: 56b17cd5 manual
// OFFSET: 0x0051AA90
static int32_t Script_RepopMe(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->HandleRepopRequest(false);
    }

    return 0;
}

// VERIFY: 89954634 manual
// OFFSET: 0x0051AAC0
static int32_t Script_AcceptResurrect(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(1);
    }

    return 0;
}

// VERIFY: 9e0bca08 manual
// OFFSET: 0x0051AAF0
static int32_t Script_DeclineResurrect(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(0);
    }

    return 0;
}

// VERIFY: 8786f8a9 manual
// OFFSET: 0x0051ABA0
static int32_t Script_DeclineGroup(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::DeclineGroup();
    }

    return 0;
}

// VERIFY: 6ca6b3d8 manual
// OFFSET: 0x0051ABD0
static int32_t Script_AcceptGuild(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptGuild();
    }

    return 0;
}

// VERIFY: 12010dbc manual
// OFFSET: 0x0051AC00
static int32_t Script_DeclineGuild(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::DeclineGuild();
    }

    return 0;
}

// VERIFY: c7f92481 manual
// OFFSET: 0x0051AC30
static int32_t Script_AcceptArenaTeam(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptArenaTeam();
    }

    return 0;
}

// VERIFY: ad1300af manual
// OFFSET: 0x0051AC60
static int32_t Script_DeclineArenaTeam(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::DeclineArenaTeam();
    }

    return 0;
}

// VERIFY: 3d89e6f6 auto
// OFFSET: 0x00522950
static int32_t Script_DropCursorMoney(lua_State* L) {
    if (CGGameUI::m_cursorMoney && CGGameUI::m_hasControl && CGGameUI::m_cursorItemType == 2) {
        CGGameUI::ClearCursor(1, 1);
    }

    return 0;
}

// VERIFY: b7f1abc8 auto
// OFFSET: 0x00608560
static int32_t Script_GetWaterDetail(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: c070e858 manual
// OFFSET: 0x00511C40
int32_t Script_GetBillingTimeRested(lua_State* L) {
    lua_pushnumber(L, ClientServices::Connection()->m_billingTimeRested);
    return 1;
}

// VERIFY: e7e4ff0d auto
// OFFSET: 0x00515100
static int32_t Script_CursorHasItem(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 1 || CGGameUI::m_cursorItemType == 9) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: e744d727 auto
// OFFSET: 0x00515140
static int32_t Script_CursorHasSpell(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 3) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b1fc85d9 auto
// OFFSET: 0x00515180
static int32_t Script_CursorHasMacro(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2230e17f auto
// OFFSET: 0x005151C0
static int32_t Script_CursorHasMoney(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: fc971425 auto
// OFFSET: 0x00515750
static int32_t Script_GetInstanceDifficulty(lua_State* L) {
    lua_pushnumber(L, CGGameUI::m_instanceDifficulty + 1);
    return 1;
}

// VERIFY: a8667e56 auto
// OFFSET: 0x00516760
static int32_t Script_InCinematic(lua_State* L) {
    if (CGGameUI::m_inCinematic) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: acdd7b2e auto
// OFFSET: 0x00517D30
static int32_t Script_IsThreatWarningEnabled(lua_State* L) {
    if (CGGameUI::m_threatWarningEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 99d6c5bc auto
// OFFSET: 0x00512090
static int32_t Script_ConsoleAddMessage(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ConsoleAddMessage(string)");
    }

    const char* text = lua_tolstring(L, 1, nullptr);

    if (text && *text) {
        ConsolePrintf("%s", text);
    }

    return 0;
}

// VERIFY: 612c638a auto
// OFFSET: 0x0050F830
static int32_t Script_FrameXML_Debug(lua_State* L) {
    int32_t level = FrameXML_GetDebugLevel();

    if (lua_isnumber(L, 1)) {
        level = static_cast<int32_t>(lua_tonumber(L, 1));
        FrameXML_SetDebugLevel(level);
    }

    lua_pushnumber(L, level);
    return 1;
}

// VERIFY: 4100f310 auto
// OFFSET: 0x005144C0
static int32_t Script_IsLeftShiftKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = CGUIBindings::s_bindings->m_heldModifiers & 1;
    } else {
        down = EventIsKeyDown(KEY_LSHIFT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: c4e94562 auto
// OFFSET: 0x00514520
static int32_t Script_IsRightShiftKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 1) & 1;
    } else {
        down = EventIsKeyDown(KEY_RSHIFT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 898d5169 auto
// OFFSET: 0x00514610
static int32_t Script_IsLeftControlKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 2) & 1;
    } else {
        down = EventIsKeyDown(KEY_LCONTROL);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 946c00ee auto
// OFFSET: 0x00514670
static int32_t Script_IsRightControlKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 3) & 1;
    } else {
        down = EventIsKeyDown(KEY_RCONTROL);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 81c65012 auto
// OFFSET: 0x00514760
static int32_t Script_IsLeftAltKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 4) & 1;
    } else {
        down = EventIsKeyDown(KEY_LALT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 5d240461 auto
// OFFSET: 0x005147C0
static int32_t Script_IsRightAltKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 5) & 1;
    } else {
        down = EventIsKeyDown(KEY_RALT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 94368160 manual
// OFFSET: 0x00516280
static int32_t Script_GetCorpseRecoveryDelay(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_corpseReclaimDelay || (remaining = CGGameUI::m_corpseReclaimDelay - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// VERIFY: a39caf14 manual
// OFFSET: 0x005162E0
static int32_t Script_GetInstanceBootTimeRemaining(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_instanceBootTime || (remaining = CGGameUI::m_instanceBootTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// VERIFY: 6a68c02d manual
// OFFSET: 0x005164B0
static int32_t Script_GetSummonConfirmTimeLeft(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_summonConfirmTime || (remaining = CGGameUI::m_summonConfirmTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// VERIFY: 5b2771f3 auto
// OFFSET: 0x00516580
static int32_t Script_GetSummonConfirmAreaName(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_summonConfirmArea);

    if (area) {
        lua_pushstring(L, area->m_areaName);
        return 1;
    }

    lua_pushstring(L, "");
    return 1;
}

// VERIFY: 5b10428b auto
// OFFSET: 0x005167E0
static int32_t Script_IsSubZonePVPPOI(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (area && (area->m_flags & 0x8000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 264561d8 auto
// OFFSET: 0x00516B90
static int32_t Script_GetAreaSpiritHealerTime(lua_State* L) {
    int32_t remaining = 0;

    if (CGGameUI::m_areaSpiritHealerTime) {
        remaining = CGGameUI::m_areaSpiritHealerTime - OsGetAsyncTimeMs();

        if (remaining < 0) {
            remaining = 0;
        }
    }

    lua_pushnumber(L, remaining / 1000);
    return 1;
}

// VERIFY: 2ceedaf7 manual
// OFFSET: 0x0051B6E0
static int32_t Script_CheckSpiritHealerDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckSpiritHealerDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 3bf7a0f7 manual
// OFFSET: 0x0051B740
static int32_t Script_CheckTalentMasterDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckTalentMasterDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 06a05e3b manual
// OFFSET: 0x0051B7A0
static int32_t Script_CheckBinderDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckBinderDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: ffe47e4b manual
// OFFSET: 0x0051BD00
static int32_t Script_GetPVPTimer(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t timer = player ? player->GetPVPToggleTimer() + 1000 : 0;
    lua_pushnumber(L, timer);
    return 1;
}

// VERIFY: 4d714989 auto
// OFFSET: 0x0054C810
int32_t Script_CanHearthAndResurrectFromArea(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (area && (area->m_flags & 0x8000000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 3e89a8d8 manual
// OFFSET: 0x00510AC0
static int32_t Script_GetNetStats(lua_State* L) {
    float bytesIn;
    float bytesOut;
    uint32_t latency;
    ClientServices::Connection()->GetNetStats(bytesIn, bytesOut, latency);
    lua_pushnumber(L, bytesIn);
    lua_pushnumber(L, bytesOut);
    lua_pushnumber(L, latency);
    return 3;
}

// VERIFY: 07999454 manual
// OFFSET: 0x005117C0
static int32_t Script_GetEventCPUUsage(lua_State* L) {
    double time = 0.0;
    uint32_t count = 0;
    FrameScript_GetEventCPUUsage(lua_tolstring(L, 1, nullptr), &time, &count);
    lua_pushnumber(L, time);
    lua_pushnumber(L, count);
    return 2;
}

// VERIFY: 63c2507c auto
// OFFSET: 0x00514BA0
static int32_t Script_GetMouseButtonName(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        lua_pushstring(L, GetMouseButtonName(IndexToMouseButton(static_cast<uint32_t>(lua_tonumber(L, 1)))));
        return 1;
    }

    if (lua_isstring(L, 1)) {
        lua_settop(L, 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 35d652a0 manual
// OFFSET: 0x00515630
static int32_t Script_ResetInstances(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RESET_INSTANCES));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 0785053a auto
// OFFSET: 0x00515810
static int32_t Script_GetRaidDifficulty(lua_State* L) {
    uint32_t difficulty = CGPartyInfo::m_raidDifficulty;

    if (!CGRaidInfo::m_numMembers) {
        difficulty = CGGameUI::m_raidDifficulty;
    }

    lua_pushnumber(L, difficulty + 1);
    lua_pushnumber(L, CGGameUI::m_raidDifficulty + 1);
    return 2;
}

// VERIFY: 37009759 manual
// OFFSET: 0x00515B70
static int32_t Script_GuildLeave(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_LEAVE));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: bf377447 manual
// OFFSET: 0x00515BE0
static int32_t Script_GuildDisband(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_DISBAND));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: e7ba93f8 manual
// OFFSET: 0x00515C50
static int32_t Script_GuildInfo(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_INFO));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: c8a56a12 auto
// OFFSET: 0x00516210
static int32_t Script_GetReleaseTimeRemaining(lua_State* L) {
    int32_t remaining;

    if (CGGameUI::m_deadNoRepopTimer) {
        remaining = -1000;
    } else if (!CGGameUI::m_repopTime || (remaining = CGGameUI::m_repopTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, remaining / 1000);
    return 1;
}

// VERIFY: ec7c8150 manual
// OFFSET: 0x005166F0
static int32_t Script_OpeningCinematic(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_OPENING_CINEMATIC));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 5578a0c0 manual
// OFFSET: 0x00516840
static int32_t Script_TogglePVP(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TOGGLE_PVP));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 1e926a4a auto
// OFFSET: 0x00516B20
static int32_t Script_GetBindLocation(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGPlayer_C::GetBindAreaID());

    if (area) {
        lua_pushstring(L, area->m_areaName);
        return 1;
    }

    lua_pushstring(L, FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE));
    return 1;
}

// VERIFY: 1ab86e70 manual
// OFFSET: 0x0051BD60
static int32_t Script_IsPVPTimerRunning(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x40000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 7aea54e4 manual
// OFFSET: 0x0051BFD0
static int32_t Script_ShowingHelm(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_player->m_playerFlags & 0x400)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: b0c5ab4d manual
// OFFSET: 0x0051C040
static int32_t Script_ShowingCloak(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_player->m_playerFlags & 0x800)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: b6e39278 manual
// OFFSET: 0x0051D100
static int32_t Script_GetTaxiBenchmarkMode(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: c8a55e32 manual
// OFFSET: 0x0051D5A0
static int32_t Script_GetAllowLowLevelRaid(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x10000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 2631915b manual
// OFFSET: 0x0051CB30
static int32_t Script_PartialPlayTime(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t partial;

    if (player && (partial = (player->m_player->m_playerFlags & 0x1000) != 0)) {
        lua_pushnumber(L, partial);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: a028dc32 manual
// OFFSET: 0x0051CBA0
static int32_t Script_NoPlayTime(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t none;

    if (player && (none = (player->m_player->m_playerFlags & 0x2000) != 0)) {
        lua_pushnumber(L, none);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 40453f65 manual
// OFFSET: 0x0051BCA0
static int32_t Script_GetPVPDesired(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t desired = player ? (player->m_player->m_playerFlags >> 9) & 1 : 0;
    lua_pushnumber(L, desired);
    return 1;
}

// VERIFY: d93c6665 auto
// OFFSET: 0x00510230
static int32_t Script_SetFarclip(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetFarclip(value)");
        return 0;
    }

    CVar* var = CVar::Lookup("farclip");
    char value[16];
    SStrPrintf(value, sizeof(value), "%f", lua_tonumber(L, 1));
    var->Set(value, true, false, false, true);
    return 0;
}

// VERIFY: 17168813 auto
// OFFSET: 0x005102E0
static int32_t Script_SetTexLodBias(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetTexLodBias(value)");
        return 0;
    }

    CVar* var = CVar::Lookup("texLodBias");
    char value[16];
    SStrPrintf(value, sizeof(value), "%f", lua_tonumber(L, 1));
    var->Set(value, true, false, false, true);
    return 0;
}

// VERIFY: af1c16bf auto
// OFFSET: 0x00510390
static int32_t Script_SetBaseMip(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetBaseMip(value)");
        return 0;
    }

    CVar* var = CVar::Lookup("baseMip");
    char value[16];
    SStrPrintf(value, sizeof(value), "%d", 1.0 - lua_tonumber(L, 1));
    var->Set(value, true, false, false, true);
    return 0;
}

// VERIFY: 26416364 auto
// OFFSET: 0x00515790
static int32_t Script_GetDungeonDifficulty(lua_State* L) {
    uint32_t difficulty;

    if (!CGPartyInfo::m_members[0] || (difficulty = CGPartyInfo::m_dungeonDifficulty, CGRaidInfo::m_numMembers)) {
        difficulty = CGGameUI::m_dungeonDifficulty;
    }

    lua_pushnumber(L, difficulty + 1);
    lua_pushnumber(L, CGGameUI::m_dungeonDifficulty + 1);
    return 2;
}

// VERIFY: d333f20d manual
// OFFSET: 0x00515880
static int32_t Script_RequestTimePlayed(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PLAYED_TIME));
    msg.Put(static_cast<uint8_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 6aaefa51 manual
// OFFSET: 0x0051AB20
static int32_t Script_AcceptGroup(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        uint32_t roles = 0;

        if (StringToBOOL(L, 1, 0)) {
            roles = 2;
        }

        if (StringToBOOL(L, 2, 0)) {
            roles |= 4;
        }

        if (StringToBOOL(L, 3, 0)) {
            roles |= 8;
        }

        CGPlayer_C::AcceptGroup(roles);
    }

    return 0;
}

// VERIFY: 135c28b3 manual
// OFFSET: 0x00517B50
static int32_t Script_SetTaxiBenchmarkMode(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TAXI_BENCHMARK_MODE));
    msg.Put(static_cast<uint8_t>(StringToBOOL(L, 1, 1) != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 77bfe9c9 auto
// OFFSET: 0x00517D70
static int32_t Script_CanMapChangeDifficulty(lua_State* L) {
    int32_t mapId;

    if (lua_isnumber(L, 1)) {
        mapId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        mapId = CGGameUI::m_iCurrentMapID;
    }

    const MapRec* map = g_mapDB.GetRecord(mapId);

    if (map && (map->m_flags & 0x100)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: fb60fbd2 manual
// OFFSET: 0x005262E0
static int32_t Script_TargetTotem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TargetTotem(slot)");
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
    }

    TotemInfo* totem = &CGGameUI::m_totems[slot];

    if (!totem || !totem->m_guid) {
        return 0;
    }

    CGGameUI::Target(totem->m_guid);
    return 0;
}

// VERIFY: 3e7e55ee manual
// OFFSET: 0x00511720
static int32_t Script_GetFunctionCPUUsage(lua_State* L) {
    if (lua_type(L, 1) != LUA_TFUNCTION) {
        luaL_error(L, "Usage: GetFunctionCPUUsage(function[, includeSubroutines])");
    }

    int32_t includeSubroutines = StringToBOOL(L, 2, 1);
    lua_settop(L, 1);
    double time;
    double totalTime;
    int32_t calls;
    lua_gcgetfunctiontime(L, &time, &totalTime, &calls);
    lua_pushnumber(L, includeSubroutines ? totalTime : time);
    lua_pushnumber(L, static_cast<uint32_t>(calls));
    return 2;
}

// VERIFY: a17fd64f manual
// OFFSET: 0x0051AFB0
static int32_t Script_GetDamageBonusStat(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const ChrClassesRec* rec;

    if (player && (rec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]))) {
        lua_pushnumber(L, rec->m_damageBonusStat + 1);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: ca8c83c5 manual
// OFFSET: 0x0051B800
static int32_t Script_RetrieveCorpse(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(0);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RECLAIM_CORPSE));
    msg.Put(CGGameUI::m_corpseGUID.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 92ce34ec manual
// OFFSET: 0x005168B0
static int32_t Script_SetPVP(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TOGGLE_PVP));
    uint8_t enable;

    if (lua_isnumber(L, 1)) {
        enable = static_cast<uint8_t>(lua_tonumber(L, 1));
    } else {
        enable = 0;
    }

    msg.Put(enable);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 5cacdc88 manual
// OFFSET: 0x0051B110
static int32_t Script_CancelSummon(lua_State* L) {
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SUMMON_RESPONSE));
    msg.Put(CGGameUI::m_summonConfirmGUID.m_guid);
    msg.Put(static_cast<uint8_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: f351dab3 manual
// OFFSET: 0x00522A20
static int32_t Script_GuildInvite(lua_State* L) {
    const char* target = lua_tolstring(L, 1, nullptr);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_INVITE));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 38a6c581 manual
// OFFSET: 0x00522AF0
static int32_t Script_GuildUninvite(lua_State* L) {
    const char* target = lua_tolstring(L, 1, nullptr);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_OFFICER_REMOVE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 6621126a manual
// OFFSET: 0x00522BC0
static int32_t Script_GuildPromote(lua_State* L) {
    const char* target = lua_tolstring(L, 1, nullptr);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_PROMOTE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 4a5ca774 manual
// OFFSET: 0x00522C90
static int32_t Script_GuildDemote(lua_State* L) {
    const char* target = lua_tolstring(L, 1, nullptr);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_DEMOTE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: feef4e01 manual
// OFFSET: 0x00522D60
static int32_t Script_GuildSetLeader(lua_State* L) {
    const char* target = lua_tolstring(L, 1, nullptr);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_LEADER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 39b52d84 manual
// OFFSET: 0x00516610
static int32_t Script_RandomRoll(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: RandomRoll(\"max\") or RandomRoll(\"min\", \"max\")");
    }

    int32_t min = SStrToInt(lua_tolstring(L, 1, nullptr));
    int32_t max = SStrToInt(lua_tolstring(L, 2, nullptr));

    if ((min <= 0 && max <= 0) || max < min || max > 1000000) {
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_RANDOM_ROLL));
    msg.Put(static_cast<uint32_t>(min));
    msg.Put(static_cast<uint32_t>(max));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 91337dba manual
// OFFSET: 0x0050FF50
static int32_t Script_GetCVarInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarInfo(\"cvar\")");
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    lua_pushstring(L, var->m_defaultValue.m_str);

    if (var->m_flags & 0x10) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (var->m_flags & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// VERIFY: 8bba5f56 manual
// OFFSET: 0x0051D600
static int32_t Script_SetAllowLowLevelRaid(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        uint8_t allow = StringToBOOL(L, 1, 1) != 0;
        CDataStore msg1;
        msg1.Put(static_cast<uint32_t>(CMSG_LOW_LEVEL_RAID1));
        msg1.Put(allow);
        msg1.Finalize();
        ClientServices::Send2(&msg1);
        CDataStore msg2;
        msg2.Put(static_cast<uint32_t>(CMSG_LOW_LEVEL_RAID2));
        msg2.Put(allow);
        msg2.Finalize();
        ClientServices::Send2(&msg2);
    }

    return 0;
}

// VERIFY: c3d049ab manual
// OFFSET: 0x005148B0
static int32_t Script_IsMouseButtonDown(lua_State* L) {
    int32_t down;

    if (lua_isnumber(L, 1)) {
        down = EventIsButtonDown(IndexToMouseButton(static_cast<uint32_t>(lua_tonumber(L, 1))));
    } else if (lua_isstring(L, 1)) {
        down = EventIsButtonDown(StringToMouseButton(lua_tolstring(L, 1, nullptr)));
    } else {
        down = EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x80)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x100)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x200)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x400)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x800)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x80000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x100000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x200000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x400000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x800000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40000000));
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 00d6780a manual
// OFFSET: 0x00510E20
static int32_t Script_GetItemQualityColor(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetItemQualityColor(index)");
        return 0;
    }

    uint32_t quality = static_cast<int32_t>(lua_tonumber(L, 1));
    const CImVector* color = CGTooltip::GetItemQualityColor(quality);
    lua_pushnumber(L, color->r * 0.00392156862745098);
    lua_pushnumber(L, color->g * 0.00392156862745098);
    lua_pushnumber(L, color->b * 0.00392156862745098);
    lua_pushstring(L, CGTooltip::GetItemQualityColorString(quality));
    return 4;
}

// VERIFY: fe358aee manual
// OFFSET: 0x00511B30
static int32_t Script_IsAddOnLoaded(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: IsAddOnLoaded(index or \"name\")");
        }

        name = lua_tolstring(L, 1, nullptr);
    }

    if (IsAddOnLoading(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (IsAddOnLoaded(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// VERIFY: b380dd1d manual
// OFFSET: 0x00515A50
static int32_t Script_GetCursorMoney(lua_State* L) {
    lua_pushnumber(L, CGGameUI::m_cursorMoney);
    return 1;
}

// VERIFY: 1c08ff80 manual
// OFFSET: 0x00510040
static int32_t Script_GetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVar(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    return 1;
}

// VERIFY: 968466d6 manual
// OFFSET: 0x005100C0
static int32_t Script_GetCVarBool(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarBool(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (var && !(var->m_flags & 0x40) && StringToBOOL(var->m_stringValue.m_str)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b062b828 manual
// OFFSET: 0x00510150
static int32_t Script_GetCVarDefault(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarDefault(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    lua_pushstring(L, var->m_defaultValue.m_str);
    return 1;
}

// VERIFY: f62939f8 auto
// OFFSET: 0x00510A10
static int32_t Script_GetCursorPosition(lua_State* L) {
    float x = 0.0f;
    float y = 0.0f;

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    NDCToDDC(CSimpleTop::s_instance->m_mousePosition.m_x, CSimpleTop::s_instance->m_mousePosition.m_y, &x, &y);
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y));
    return 2;
}

// VERIFY: 19aabffe auto
// OFFSET: 0x00514430
static int32_t Script_IsModifierKeyDown(lua_State* L) {
    for (int32_t key = KEY_LSHIFT; key <= KEY_RALT; key++) {
        if (CGGameUI::IsKeyDown(static_cast<KEY>(key))) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 16fc0e95 auto
// OFFSET: 0x00514580
static int32_t Script_IsShiftKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LSHIFT) || CGGameUI::IsKeyDown(KEY_RSHIFT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: e03161c3 auto
// OFFSET: 0x005146D0
static int32_t Script_IsControlKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LCONTROL) || CGGameUI::IsKeyDown(KEY_RCONTROL)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 5cf9d269 auto
// OFFSET: 0x00514820
static int32_t Script_IsAltKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LALT) || CGGameUI::IsKeyDown(KEY_RALT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9094ae9a manual
// OFFSET: 0x00514C10
static int32_t Script_SetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCVar(\"cvar\", value [, \"scriptCvar\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, nullptr);
    CVar* var = CVar::LookupRegistered(name);

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", name);
        return 0;
    }

    if ((var->m_flags & 0x4) || (var->m_flags & 0x100)) {
        luaL_error(L, "\"%s\" is read-only", name);
        return 0;
    }

    if ((var->m_flags & 0x8) && !CSimpleTop::GetInstance()->m_allowProtectedActions) {
        CGGameUI::ShowBlockedActionFeedback(nullptr, 2);
        return 0;
    }

    const char* value = lua_tolstring(L, 2, nullptr);

    if (!value) {
        value = "0";
    }

    var->Set(value, true, false, false, true);

    if (lua_isstring(L, 3)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CVAR_UPDATE, "%s%s", lua_tolstring(L, 3, nullptr), value);
    }

    return 0;
}

// VERIFY: c3cc832c manual
// OFFSET: 0x00514D40
static int32_t Script_GetCVarMin(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarMin(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    const char* name = var->m_key.m_str;

    if (!SStrCmpI(name, "extShadowQuality", STORM_MAX_STR)) {
        lua_pushnumber(L, 0.0);
    } else if (!SStrCmpI(name, "farclip", STORM_MAX_STR)) {
        lua_pushnumber(L, 177.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2b500bc1 manual
// OFFSET: 0x00514E30
static int32_t Script_GetCVarMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarMax(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    const char* name = var->m_key.m_str;

    if (!SStrCmpI(name, "extShadowQuality", STORM_MAX_STR)) {
        uint32_t mode = 0;
        uint32_t maxMode = CShadowCache::GetMaxShadowMode();

        do {
            if (!CShadowCache::ValidateShadowMode(static_cast<EShadowMode>(mode))) {
                break;
            }

            mode++;
        } while (mode <= maxMode);

        lua_pushnumber(L, mode - 1);
    } else if (!SStrCmpI(name, "farclip", STORM_MAX_STR)) {
        lua_pushnumber(L, 1277.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: fb7bea1c auto
// OFFSET: 0x0051AEF0
static int32_t Script_GetScreenWidth(lua_State* L) {
    float width = NDCToDDCWidth(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (width / scale)));
    return 1;
}

// VERIFY: 53f1b9fb auto
// OFFSET: 0x0051AF50
static int32_t Script_GetScreenHeight(lua_State* L) {
    float height = NDCToDDCHeight(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (height / scale)));
    return 1;
}

// VERIFY: 2ffb3aeb manual
// OFFSET: 0x00528920
static int32_t Script_LoadAddOn(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: LoadAddOn(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, nullptr);
    }

    if (!s_loadAddOnDepth) {
        s_loadAddOnStatus = new (__FILE__, __LINE__) CWOWClientStatus("Logs\\FrameXML.log", 1);
    }

    s_loadAddOnDepth++;
    bool loaded = LoadAddOn(name, true, s_loadAddOnStatus);

    if (--s_loadAddOnDepth == 0) {
        if (s_loadAddOnStatus) {
            delete s_loadAddOnStatus;
        }

        s_loadAddOnStatus = nullptr;
    }

    if (loaded) {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnil(L);

    UIADDON_LOADABLE_REASON reason;
    UIADDON_LOADABLE_REASON dependencyReason;

    if (!IsAddOnLoadable(name, true, reason, dependencyReason, ClientServices::GetCharacterName()) && (reason || dependencyReason)) {
        char buffer[128];
        lua_pushstring(L, AddOnUnloadableToken(reason, dependencyReason, buffer, sizeof(buffer)));
        return 2;
    }

    lua_pushstring(L, "UNKNOWN_ERROR");
    return 2;
}

// VERIFY: a3c4eae5 auto
// OFFSET: 0x005155D0
static int32_t Script_GetSubZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_subZoneText ? CGGameUI::m_subZoneText : "");
    return 1;
}

// VERIFY: 76cc0988 auto
// OFFSET: 0x00515600
static int32_t Script_GetMinimapZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_minimapZoneText ? CGGameUI::m_minimapZoneText : "");
    return 1;
}

// VERIFY: 198cd81d auto
// OFFSET: 0x005156A0
static int32_t Script_IsInInstance(lua_State* L) {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        lua_pushnil(L);
        lua_pushstring(L, s_instanceTypeTokens[0]);
        return 2;
    }

    if (map->m_instanceType) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    uint32_t type = map->m_instanceType;
    lua_pushstring(L, type < sizeof(s_instanceTypeTokens) / sizeof(s_instanceTypeTokens[0]) ? s_instanceTypeTokens[type] : s_instanceTypeTokens[0]);
    return 2;
}

// VERIFY: 46914859 manual
// OFFSET: 0x00516BF0
static int32_t Script_GetMouseFocus(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleFrame* focus = CSimpleTop::s_instance->m_mouseFocus;

    if (!focus) {
        lua_pushnil(L);
        return 1;
    }

    if (!focus->m_luaRegistered) {
        focus->RegisterScriptObject(nullptr);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, focus->m_luaObjectRef);
    return 1;
}

// VERIFY: 03727723 manual
// OFFSET: 0x0051BA50
static int32_t Script_GetZonePVPInfo(lua_State* L) {
    int32_t flags = -1;
    int32_t subzone = 0;
    AreaTableRec* subzoneRec = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (subzoneRec) {
        flags = subzoneRec->m_flags;
        subzone = 1;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (flags == -1) {
        subzone = 0;
        flags = area ? area->m_flags : 0;
    }

    const char* type = nullptr;

    if (flags & 0x800) {
        type = "sanctuary";
    } else if (flags & 0x80) {
        type = "arena";
    } else if (flags & 0x1000000) {
        type = "combat";
    }

    if (type) {
        lua_pushstring(L, type);

        if (subzone) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    int32_t contested = (flags & 0x40000) != 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!area || !player || (!s_isPvPRealm && !(area->m_flags & 0x10))) {
        return 0;
    }

    if (!contested) {
        FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);

        if (!faction || (area->m_factionGroupMask & faction->m_friendGroup)) {
            type = "friendly";
        } else if (area->m_factionGroupMask & faction->m_enemyGroup) {
            type = "hostile";
        }
    }

    lua_pushstring(L, type ? type : "contested");
    const char* factionName = "";

    for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
        FactionGroupRec* group = i >= 0 && i < g_factionGroupDB.m_numRecords ? &g_factionGroupDB.m_records[i] : nullptr;

        if (((1 << group->m_maskID) & area->m_factionGroupMask) && *group->m_name) {
            factionName = group->m_name;
            break;
        }
    }

    lua_pushnil(L);
    lua_pushstring(L, factionName);
    return 3;
}

// VERIFY: cfaa1624 auto
// OFFSET: 0x0050F950
static int32_t Script_GetMouseButtonClicked(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    lua_pushstring(L, CSimpleTop::s_instance->m_mouseButtonName);
    return 1;
}

// VERIFY: 5a4a2960 manual
// OFFSET: 0x00517BE0
static int32_t ToggleInteractModelIcon(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        object->SetInteractIconVisible(CGGameUI::m_visible);
    }

    return 1;
}

// VERIFY: acc56927 auto
// OFFSET: 0x00517C20
static int32_t Script_SetUIVisibility(lua_State* L) {
    int32_t visible = StringToBOOL(L, 1, 1);

    if (CGGameUI::m_visible != visible) {
        CGGameUI::m_visible = visible;
        CGGameUI::UpdateUnitHighlights();
        ClntObjMgrEnumVisibleObjects(ToggleInteractModelIcon, nullptr);
    }

    return 0;
}

// VERIFY: 0647bfed auto
// OFFSET: 0x0051AC90
static int32_t Script_CancelLogout(lua_State* L) {
    if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1250) {
        CGGameUI::ShowBlockedActionFeedback(nullptr, 1);
        return 0;
    }

    ClientServices::Connection()->CharacterAbortLogout();
    return 0;
}

// VERIFY: fe990a77 auto
// OFFSET: 0x00522480
static int32_t Script_FocusUnit(lua_State* L) {
    WOWGUID guid;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, true)) {
        CGGameUI::SetFocusTarget(guid);
    }

    return 0;
}

// VERIFY: fc534941 auto
// OFFSET: 0x0050F8F0
static int32_t Script_SetLayoutMode(lua_State* L) {
    int32_t enabled = 1;

    if (lua_isnumber(L, 1)) {
        enabled = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->m_moveResizeEnabled = enabled;
    return 0;
}

// VERIFY: 07ccda23 auto
// OFFSET: 0x00511CC0
static int32_t Script_InCombatLockdown(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (CSimpleTop::s_instance->m_allowProtectedActions) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: e0877fd5 manual
// OFFSET: 0x0051D520
static int32_t Script_DestroyTotem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DestroyTotem(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(slot - 1) > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
    }

    CGGameUI::DestroyTotem(slot - 1);
    return 0;
}

// VERIFY: e88c65f9 manual
// OFFSET: 0x00515CC0
static int32_t Script_ArenaTeamInviteByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamInviteByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, nullptr);
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;

    if (!playerName || !*playerName) {
        return 0;
    }

    if (SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_INVITE));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 19afb6de manual
// OFFSET: 0x00515DD0
static int32_t Script_ArenaTeamLeave(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArenaTeamLeave(team)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_LEAVE));
    msg.Put(teamId);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: a03dbb95 manual
// OFFSET: 0x00515EB0
static int32_t Script_ArenaTeamUninviteByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamUninviteByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, nullptr);

    if (playerName && SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
    }

    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_REMOVE));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: da85970d manual
// OFFSET: 0x00515FF0
static int32_t Script_ArenaTeamSetLeaderByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamSetLeaderByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, nullptr);

    if (playerName && SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
    }

    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_LEADER));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 275e4a2d manual
// OFFSET: 0x00516130
static int32_t Script_ArenaTeamDisband(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArenaTeamDisband(team)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_DISBAND));
    msg.Put(teamId);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 6acc0cbe manual
// OFFSET: 0x0051D1B0
static int32_t Script_IsReferAFriendLinked(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, nullptr));

    if (player && player->IsRAFPlayer(guid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: a6b340a9 manual
// OFFSET: 0x00511D30
static int32_t Script_GetSummonFriendCooldown(lua_State* L) {
    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(CGSpellBook::m_summonFriendSpell, 0, &duration, &start, &enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// VERIFY: 768dc8b2 manual
// OFFSET: 0x0051D330
static int32_t Script_GetTotemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTotemInfo(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
        return 0;
    }

    TotemInfo* totem = slot < sizeof(CGGameUI::m_totems) / sizeof(CGGameUI::m_totems[0]) ? &CGGameUI::m_totems[slot] : nullptr;

    if (totem && totem->m_guid) {
        lua_pushboolean(L, 1);
        lua_pushstring(L, totem->m_name);
        lua_pushnumber(L, totem->m_startTime / 1000);
        lua_pushnumber(L, totem->m_duration / 1000);
        lua_pushstring(L, totem->m_icon);
        return 5;
    }

    bool found = false;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t category = 0;

        switch (slot) {
            case 0:
                category = 4;
                break;

            case 1:
                category = 2;
                break;

            case 2:
                category = 5;
                break;

            case 3:
                category = 3;
                break;
        }

        found = player->m_bag.FindTotemCategory(category, 8);
    }

    lua_pushboolean(L, found);
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    return 5;
}

// VERIFY: 277ddc33 auto
// OFFSET: 0x00511C80
static int32_t Script_GetExistingLocales(lua_State* L) {
    int32_t count = 0;

    for (int32_t i = 0; i < 12; i++) {
        if (g_hasIsoLocale[i]) {
            lua_pushstring(L, g_localeNames[i]);
            count++;
        }
    }

    return count;
}

// VERIFY: 432ef71b manual
// OFFSET: 0x0051A8C0
static int32_t Script_GetInstanceInfo(lua_State* L) {
    int32_t mapId = CGGameUI::m_iCurrentMapID;
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map) {
        lua_pushnil(L);
        lua_pushstring(L, s_instanceTypeTokens[0]);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 7;
    }

    lua_pushstring(L, map->m_mapName);
    uint32_t type = map->m_instanceType;
    lua_pushstring(L, type < 5 ? s_instanceTypeTokens[type] : s_instanceTypeTokens[0]);
    uint32_t difficulty = CGGameUI::m_instanceDifficulty;
    lua_pushnumber(L, difficulty + 1);
    MapDifficultyRec* rec = GetMapDifficultyEntryByMapIdAndDifficulty(mapId, difficulty, nullptr);

    if (rec) {
        lua_pushstring(L, FrameScript_GetText(rec->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
        lua_pushnumber(L, rec->m_maxPlayers);
    } else {
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
    }

    lua_pushnumber(L, CGPartyInfo::m_dynamicDifficulty);
    map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    lua_pushboolean(L, map && (map->m_flags & 0x100));
    return 7;
}

// VERIFY: 0ee08026 manual
// OFFSET: 0x0051B8B0
static int32_t Script_GetRestState(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ExhaustionRec* rec = player ? g_exhaustionDB.GetRecord(player->m_player->m_playerBytes2[3]) : nullptr;

    if (!rec) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushnumber(L, rec->m_id);
    lua_pushstring(L, rec->m_name);
    lua_pushnumber(L, rec->m_factor);
    return 3;
}

// VERIFY: 20f4f8ae manual
// OFFSET: 0x0051B960
static int32_t Script_GetXPExhaustion(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ExhaustionRec* rested = g_exhaustionDB.GetRecord(1);

    if (!player || player->m_player->m_playerBytes2[3] != 1) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t experience = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerRestStateExperience : 0;
    lua_pushnumber(L, experience * rested->m_factor);
    return 1;
}

// VERIFY: 94225d6a auto
// OFFSET: 0x00525FC0
static int32_t Script_ClearTarget(lua_State* L) {
    int32_t hadTarget = CGGameUI::m_lockedTarget != WOWGUID();
    CGGameUI::unk_BD08CC = 0;

    if (CGGameUI::m_lockedTarget) {
        CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
        CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

        if (Spell_C_GetAutoRepeatingSpell()) {
            Spell_C_CancelAutoRepeat(true);
        }
    }

    if (hadTarget) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: a9e1a777 auto
// OFFSET: 0x00510450
static int32_t Script_Quit(lua_State* L) {
    ClientServices::Connection()->CharacterLogout(true, false);
    return 0;
}

// VERIFY: 5cc4abd6 manual
// OFFSET: 0x0052B470
static int32_t Script_RegisterForSave(lua_State* L) {
    if (lua_tainted) {
        luaL_error(L, "RegisterForSave() is only available to Blizzard scripts");
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RegisterForSave(\"variable\")");
    }

    CGGameUI::RegisterSavedVariable(lua_tolstring(L, 1, nullptr), 0);
    return 0;
}

// VERIFY: 6677c3e4 manual
// OFFSET: 0x0052B4E0
static int32_t Script_RegisterForSavePerCharacter(lua_State* L) {
    if (lua_tainted) {
        luaL_error(L, "RegisterForSavePerCharacter() is only available to Blizzard scripts");
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RegisterForSavePerCharacter(\"variable\")");
    }

    CGGameUI::RegisterSavedVariable(lua_tolstring(L, 1, nullptr), 1);
    return 0;
}

// VERIFY: 40b1cff8 manual
// OFFSET: 0x0051CC10
static int32_t Script_GetMirrorTimerInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMirrorTimerInfo(\"timer\")");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index > 2) {
        luaL_error(L, "ERROR: Invalid timer (Ex: \"BREATH\"");
        return 0;
    }

    MirrorTimerInfo& timer = CGGameUI::m_mirrorTimerInfo[index];
    lua_pushstring(L, MirrorTimerTypeToken(timer.m_type));
    lua_pushnumber(L, timer.m_value);
    lua_pushnumber(L, timer.m_maxValue);
    lua_pushnumber(L, timer.m_scale);
    lua_pushnumber(L, timer.m_paused);
    lua_pushstring(L, GetMirrorTimerLabel(timer.m_type, timer.m_spellId));
    return 6;
}

// VERIFY: facf946d auto
// OFFSET: 0x00510470
static int32_t Script_GetFramerate(lua_State* L) {
    lua_pushnumber(L, World::GetFramerate());
    return 1;
}

// VERIFY: ee32b2a0 auto
// OFFSET: 0x005165E0
static int32_t Script_GetNumTitles(lua_State* L) {
    lua_pushnumber(L, g_charTitlesDB.m_maxID + 1);
    return 1;
}

// VERIFY: 289ea831 manual
// OFFSET: 0x0051B3B0
static int32_t Script_GetCurrentTitle(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerChosenTitle));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: c9010307 manual
// OFFSET: 0x00510FC0
static int32_t Script_GetItemCooldown(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, nullptr));
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;

    if (itemId) {
        CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);

        if (item) {
            Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        } else {
            Spell_C_GetItemCooldown(itemId, &duration, &start, &enable);
        }
    }

    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// VERIFY: 75f296f7 auto
// OFFSET: 0x00517100
static int32_t Script_GetItemSpell(lua_State* L) {
    SpellRec spell;
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, nullptr));
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);

    if (g_spellDB.GetRecord(spellId, &spell)) {
        lua_pushstring(L, spell.m_name);
        lua_pushstring(L, spell.m_nameSubtext);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

// VERIFY: 42d23833 manual
// OFFSET: 0x00517C60
static int32_t Script_GetTotemTimeLeft(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTotemTimeLeft(slot)");
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
    }

    TotemInfo* totem = &CGGameUI::m_totems[slot];

    if (totem && totem->m_guid.m_guid) {
        lua_pushnumber(L, totem->GetSpawnTimeLeft() / 1000);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 8266a857 manual
// OFFSET: 0x00525A90
static int32_t Script_TargetNearest(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 0);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// VERIFY: 54ce30e6 manual
// OFFSET: 0x00525AD0
static int32_t Script_TargetNearestEnemy(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 1);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// VERIFY: 32d545fd manual
// OFFSET: 0x00525B10
static int32_t Script_TargetNearestEnemyPlayer(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 2);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// VERIFY: bcf021cd manual
// OFFSET: 0x00525B50
static int32_t Script_TargetNearestFriend(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 3);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// VERIFY: 04298fa1 manual
// OFFSET: 0x00525B90
static int32_t Script_TargetNearestFriendPlayer(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 4);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// VERIFY: b149bc61 auto
// OFFSET: 0x00525BD0
static int32_t Script_TargetNearestPartyMember(lua_State* L) {
    TargetNearestEnemy(StringToBOOL(L, 1, 0), 5);
    return 0;
}

// VERIFY: 3be6e022 auto
// OFFSET: 0x00525C00
static int32_t Script_TargetNearestRaidMember(lua_State* L) {
    TargetNearestEnemy(StringToBOOL(L, 1, 0), 6);
    return 0;
}

// STATIC: 0x00AC80B0
// VERIFY: ea127e1f manual
// OFFSET: 0x00511FE0
static int32_t Script_GetThreatStatusColor(lua_State* L) {
    uint32_t color = *CGTooltip::GetThreatStateColor(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1) + 1.0)));
    lua_pushnumber(L, ((color >> 16) & 0xFF) * 0.0039215689);
    lua_pushnumber(L, ((color >> 8) & 0xFF) * 0.0039215689);
    lua_pushnumber(L, (color & 0xFF) * 0.0039215689);
    return 3;
}

// VERIFY: 6ef43428 auto
// OFFSET: 0x00510920
static int32_t Script_ResetCursor(lua_State* L) {
    CursorResetMode();
    return 0;
}

// VERIFY: 10d25911 manual
// OFFSET: 0x00515200
static int32_t Script_GetCursorInfo(lua_State* L) {
    switch (CGGameUI::m_cursorItemType) {
        case 1: {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__));

            if (!item) {
                return 0;
            }

            lua_pushstring(L, "item");
            lua_pushnumber(L, item->m_obj->m_entryID);
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
            return 3;
        }

        case 2:
            lua_pushstring(L, "money");
            lua_pushnumber(L, CGGameUI::m_cursorMoney);
            return 2;

        case 3: {
            int32_t spellId = CGGameUI::m_cursorSpell;
            SpellRec spell;
            g_spellDB.GetRecord(spellId, &spell);

            if (SpellRec::WillSummonCritter(&spell)) {
                int32_t type = CGSpellBook::GetCompanionType(&spell);
                lua_pushstring(L, "companion");
                lua_pushnumber(L, CGSpellBook::FindCompanionSlotByID(spellId, type) + 1);

                if (type == 0) {
                    lua_pushstring(L, "CRITTER");
                } else {
                    if (type != 1) {
                        lua_pushstring(L, "UNKNOWN");
                        SErrDisplayAppFatalMessage("", "", 0);
                    }

                    lua_pushstring(L, "MOUNT");
                }
            } else {
                int32_t petSpell = 0;
                int32_t slot = GetSpellSlot(spellId, 0);

                if (slot < 0) {
                    slot = GetSpellSlot(spellId, 1);

                    if (slot >= 0) {
                        petSpell = 1;
                    }
                }

                lua_pushstring(L, "spell");
                lua_pushnumber(L, slot + 1);
                lua_pushstring(L, petSpell ? "pet" : "spell");
            }

            return 3;
        }

        case 5:
            lua_pushstring(L, "merchant");
            lua_pushnumber(L, CGGameUI::m_cursorVirtualSlot + 1);
            return 2;

        case 7:
        case 9:
        case 11: {
            int32_t itemId = CGGameUI::m_cursorVirtualID;
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, nullptr, nullptr, false);

            if (!stats) {
                return 0;
            }

            lua_pushstring(L, "item");
            lua_pushnumber(L, itemId);
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, stats->m_quality, 0, 0, 0));
            return 3;
        }

        case 8:
            lua_pushstring(L, "macro");
            lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(CGGameUI::m_cursorMacro) + 1);
            return 2;

        case 12:
            lua_pushstring(L, "guildbankmoney");
            lua_pushnumber(L, CGGameUI::m_cursorMoney);
            return 2;

        case 13: {
            const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGGameUI::m_cursorEquipmentSet);

            if (!set) {
                return 0;
            }

            lua_pushstring(L, "equipmentset");
            lua_pushstring(L, set->m_name);
            return 2;
        }

        default:
            return 0;
    }
}

// VERIFY: dfb090c7 manual
// OFFSET: 0x0051A680
static int32_t Script_AutoEquipCursorItem(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->AutoEquipCursorItem(0);
    }

    return 0;
}

// VERIFY: c2688421 manual
// OFFSET: 0x0051AE80
static int32_t Script_HasKey(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_bag.FindItemOfBagFamily(0x100, 79)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2f1870ab manual
// OFFSET: 0x0051B410
static int32_t Script_IsTitleKnown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsTitleKnown(titleMaskID)");
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t mask = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (player && mask && (player->m_player->m_playerFieldKnownTitles[mask >> 6] & (1ull << (mask & 63)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: ad446345 manual
// OFFSET: 0x0051B4E0
static int32_t Script_GetTitleName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTitleName(titleMaskID)");
    }

    int32_t maskId = static_cast<int32_t>(lua_tonumber(L, 1));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t gender = player ? player->m_player->m_playerBytes3[0] : 0;
    char title[256];
    title[0] = 0;
    CharTitlesRec* rec = nullptr;

    for (int32_t i = 0; i < g_charTitlesDB.m_numRecords; i++) {
        CharTitlesRec* candidate = g_charTitlesDB.GetRecordByIndex(i);

        if (candidate && candidate->m_maskID == maskId) {
            rec = candidate;
            break;
        }
    }

    if (!rec) {
        return 0;
    }

    if (gender == 1) {
        strcpy(title, strcmp(rec->m_name1, "") ? rec->m_name1 : rec->m_name);
    } else {
        strcpy(title, strcmp(rec->m_name, "") ? rec->m_name : rec->m_name1);
    }

    char stripped[256];
    int32_t length = 0;
    bool skipping = false;

    for (uint32_t i = 0; i < SStrLen(title); i++) {
        char c = title[i];

        if (!c) {
            break;
        }

        if (c == '%') {
            skipping = true;
        } else if (!skipping) {
            stripped[length++] = c;
        } else if (c == ' ') {
            skipping = false;
            stripped[length++] = c;
        }
    }

    stripped[length] = 0;
    lua_pushstring(L, stripped);
    return 1;
}

// VERIFY: 56c60ec4 manual
// OFFSET: 0x0051B1D0
static int32_t Script_SitStandOrDescendStart(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    DescendStart();

    if (player->m_movement->m_flags & 0x2000000) {
        return 0;
    }

    if (!player->m_unit->m_unitFieldBytes1[0]) {
        player->ChangeStandState(1);
    } else {
        player->ChangeStandState(0);
    }

    return 0;
}

// VERIFY: f1043ab0 auto
// OFFSET: 0x005104A0
static int32_t Script_SetCursor(lua_State* L) {
    struct CursorName {
        uint32_t m_mode;
        const char* m_name;
    };

    CursorName cursors[52] = { { 1, "POINT_CURSOR" }, { 2, "CAST_CURSOR" }, { 3, "BUY_CURSOR" }, { 4, "ATTACK_CURSOR" }, { 5, "INTERACT_CURSOR" }, { 6, "SPEAK_CURSOR" }, { 7, "INSPECT_CURSOR" }, { 8, "PICKUP_CURSOR" }, { 9, "TAXI_CURSOR" }, { 10, "TRAINER_CURSOR" }, { 11, "MINE_CURSOR" }, { 12, "SKIN_CURSOR" }, { 13, "GATHER_CURSOR" }, { 14, "LOCK_CURSOR" }, { 15, "MAIL_CURSOR" }, { 16, "LOOT_ALL_CURSOR" }, { 17, "REPAIR_CURSOR" }, { 18, "REPAIRNPC_CURSOR" }, { 19, "ITEM_CURSOR" }, { 20, "SKIN_HORDE_CURSOR" }, { 21, "SKIN_ALLIANCE_CURSOR" }, { 22, "INNKEEPER_CURSOR" }, { 23, "QUEST_CURSOR" }, { 24, "QUEST_REPEATABLE_CURSOR" }, { 25, "QUEST_TURNIN_CURSOR" }, { 26, "VEHICLE_CURSOR" }, { 27, "POINT_ERROR_CURSOR" }, { 28, "CAST_ERROR_CURSOR" }, { 29, "BUY_ERROR_CURSOR" }, { 30, "ATTACK_ERROR_CURSOR" }, { 31, "INTERACT_ERROR_CURSOR" }, { 32, "SPEAK_ERROR_CURSOR" }, { 33, "INSPECT_ERROR_CURSOR" }, { 34, "PICKUP_ERROR_CURSOR" }, { 35, "TAXI_ERROR_CURSOR" }, { 36, "TRAINER_ERROR_CURSOR" }, { 37, "MINE_ERROR_CURSOR" }, { 38, "SKIN_ERROR_CURSOR" }, { 39, "GATHER_ERROR_CURSOR" }, { 40, "LOCK_ERROR_CURSOR" }, { 41, "MAIL_ERROR_CURSOR" }, { 42, "LOOT_ALL_ERROR_CURSOR" }, { 43, "REPAIR_ERROR_CURSOR" }, { 44, "REPAIRNPC_ERROR_CURSOR" }, { 45, "ITEM_ERROR_CURSOR" }, { 46, "SKIN_HORDE_ERROR_CURSOR" }, { 47, "SKIN_ALLIANCE_ERROR_CURSOR" }, { 48, "INNKEEPER_ERROR_CURSOR" }, { 49, "QUEST_ERROR_CURSOR" }, { 50, "QUEST_REPEATABLE_ERROR_CURSOR" }, { 51, "QUEST_TURNIN_ERROR_CURSOR" }, { 52, "VEHICLE_ERROR_CURSOR" } };

    if (lua_type(L, 1) == LUA_TNIL) {
        CursorResetMode();
        return 0;
    }

    const char* name = lua_tolstring(L, 1, nullptr);

    if (!name) {
        luaL_error(L, "Usage: SetCursor(\"cursor\" or nil)");
    }

    for (uint32_t i = 0; i < 52; i++) {
        if (!SStrCmpI(cursors[i].m_name, name, STORM_MAX_STR)) {
            CursorSetMode(cursors[i].m_mode);
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    if (CursorSetFromName(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static FrameScript_Method s_ScriptFunctions[] = {
    { "FrameXML_Debug", &Script_FrameXML_Debug },
    { "GetBuildInfo", &Script_GetBuildInfo },
    { "ReloadUI", &Script_ReloadUI },
    { "RegisterForSave", &Script_RegisterForSave },
    { "RegisterForSavePerCharacter", &Script_RegisterForSavePerCharacter },
    { "SetLayoutMode", &Script_SetLayoutMode },
    { "IsModifierKeyDown", &Script_IsModifierKeyDown },
    { "IsLeftShiftKeyDown", &Script_IsLeftShiftKeyDown },
    { "IsRightShiftKeyDown", &Script_IsRightShiftKeyDown },
    { "IsShiftKeyDown", &Script_IsShiftKeyDown },
    { "IsLeftControlKeyDown", &Script_IsLeftControlKeyDown },
    { "IsRightControlKeyDown", &Script_IsRightControlKeyDown },
    { "IsControlKeyDown", &Script_IsControlKeyDown },
    { "IsLeftAltKeyDown", &Script_IsLeftAltKeyDown },
    { "IsRightAltKeyDown", &Script_IsRightAltKeyDown },
    { "IsAltKeyDown", &Script_IsAltKeyDown },
    { "IsMouseButtonDown", &Script_IsMouseButtonDown },
    { "GetMouseButtonName", &Script_GetMouseButtonName },
    { "GetMouseButtonClicked", &Script_GetMouseButtonClicked },
    { "SetConsoleKey", &Script_SetConsoleKey },
    { "Screenshot", &Script_Screenshot },
    { "GetFramerate", &Script_GetFramerate },
    { "TogglePerformanceDisplay", &Script_TogglePerformanceDisplay },
    { "TogglePerformancePause", &Script_TogglePerformancePause },
    { "TogglePerformanceValues", &Script_TogglePerformanceValues },
    { "ResetPerformanceValues", &Script_ResetPerformanceValues },
    { "GetDebugStats", &Script_GetDebugStats },
    { "IsDebugBuild", &Script_IsDebugBuild },
    { "RegisterCVar", &Script_RegisterCVar },
    { "GetCVarInfo", &Script_GetCVarInfo },
    { "SetCVar", &Script_SetCVar },
    { "GetCVar", &Script_GetCVar },
    { "GetCVarBool", &Script_GetCVarBool },
    { "GetCVarDefault", &Script_GetCVarDefault },
    { "GetCVarMin", &Script_GetCVarMin },
    { "GetCVarMax", &Script_GetCVarMax },
    { "GetCVarAbsoluteMin", &Script_GetCVarAbsoluteMin },
    { "GetCVarAbsoluteMax", &Script_GetCVarAbsoluteMax },
    { "GetWaterDetail", &Script_GetWaterDetail },
    { "SetWaterDetail", &Script_SetWaterDetail },
    { "GetFarclip", &Script_GetFarclip },
    { "SetFarclip", &Script_SetFarclip },
    { "GetTexLodBias", &Script_GetTexLodBias },
    { "SetTexLodBias", &Script_SetTexLodBias },
    { "SetBaseMip", &Script_SetBaseMip },
    { "GetBaseMip", &Script_GetBaseMip },
    { "ToggleTris", &Script_ToggleTris },
    { "TogglePortals", &Script_TogglePortals },
    { "ToggleCollision", &Script_ToggleCollision },
    { "ToggleCollisionDisplay", &Script_ToggleCollisionDisplay },
    { "TogglePlayerBounds", &Script_TogglePlayerBounds },
    { "Stuck", &Script_Stuck },
    { "Logout", &Script_Logout },
    { "Quit", &Script_Quit },
    { "SetCursor", &Script_SetCursor },
    { "ResetCursor", &Script_ResetCursor },
    { "ClearCursor", &Script_ClearCursor },
    { "CursorHasItem", &Script_CursorHasItem },
    { "CursorHasSpell", &Script_CursorHasSpell },
    { "CursorHasMacro", &Script_CursorHasMacro },
    { "CursorHasMoney", &Script_CursorHasMoney },
    { "GetCursorInfo", &Script_GetCursorInfo },
    { "EquipCursorItem", &Script_EquipCursorItem },
    { "DeleteCursorItem", &Script_DeleteCursorItem },
    { "EquipPendingItem", &Script_EquipPendingItem },
    { "CancelPendingEquip", &Script_CancelPendingEquip },
    { "TargetUnit", &Script_TargetUnit },
    { "TargetNearest", &Script_TargetNearest },
    { "TargetNearestEnemy", &Script_TargetNearestEnemy },
    { "TargetNearestEnemyPlayer", &Script_TargetNearestEnemyPlayer },
    { "TargetNearestFriend", &Script_TargetNearestFriend },
    { "TargetNearestFriendPlayer", &Script_TargetNearestFriendPlayer },
    { "TargetNearestPartyMember", &Script_TargetNearestPartyMember },
    { "TargetNearestRaidMember", &Script_TargetNearestRaidMember },
    { "TargetDirectionEnemy", &Script_TargetDirectionEnemy },
    { "TargetDirectionFriend", &Script_TargetDirectionFriend },
    { "TargetDirectionFinished", &Script_TargetDirectionFinished },
    { "TargetLastTarget", &Script_TargetLastTarget },
    { "TargetLastEnemy", &Script_TargetLastEnemy },
    { "TargetLastFriend", &Script_TargetLastFriend },
    { "AttackTarget", &Script_AttackTarget },
    { "AssistUnit", &Script_AssistUnit },
    { "FocusUnit", &Script_FocusUnit },
    { "FollowUnit", &Script_FollowUnit },
    { "InteractUnit", &Script_InteractUnit },
    { "ClearTarget", &Script_ClearTarget },
    { "ClearFocus", &Script_ClearFocus },
    { "AutoEquipCursorItem", &Script_AutoEquipCursorItem },
    { "ToggleSheath", &Script_ToggleSheath },
    { "GetZoneText", &Script_GetZoneText },
    { "GetRealZoneText", &Script_GetRealZoneText },
    { "GetSubZoneText", &Script_GetSubZoneText },
    { "GetMinimapZoneText", &Script_GetMinimapZoneText },
    { "InitiateTrade", &Script_InitiateTrade },
    { "CanInspect", &Script_CanInspect },
    { "NotifyInspect", &Script_NotifyInspect },
    { "InviteUnit", &Script_InviteUnit },
    { "UninviteUnit", &Script_UninviteUnit },
    { "RequestTimePlayed", &Script_RequestTimePlayed },
    { "RepopMe", &Script_RepopMe },
    { "AcceptResurrect", &Script_AcceptResurrect },
    { "DeclineResurrect", &Script_DeclineResurrect },
    { "ResurrectGetOfferer", &Script_ResurrectGetOfferer },
    { "ResurrectHasSickness", &Script_ResurrectHasSickness },
    { "ResurrectHasTimer", &Script_ResurrectHasTimer },
    { "BeginTrade", &Script_BeginTrade },
    { "CancelTrade", &Script_CancelTrade },
    { "AcceptGroup", &Script_AcceptGroup },
    { "DeclineGroup", &Script_DeclineGroup },
    { "AcceptGuild", &Script_AcceptGuild },
    { "DeclineGuild", &Script_DeclineGuild },
    { "AcceptArenaTeam", &Script_AcceptArenaTeam },
    { "DeclineArenaTeam", &Script_DeclineArenaTeam },
    { "CancelLogout", &Script_CancelLogout },
    { "ForceLogout", &Script_ForceLogout },
    { "ForceQuit", &Script_ForceQuit },
    { "GetCursorMoney", &Script_GetCursorMoney },
    { "DropCursorMoney", &Script_DropCursorMoney },
    { "PickupPlayerMoney", &Script_PickupPlayerMoney },
    { "HasSoulstone", &Script_HasSoulstone },
    { "UseSoulstone", &Script_UseSoulstone },
    { "HasKey", &Script_HasKey },
    { "GuildInvite", &Script_GuildInvite },
    { "GuildUninvite", &Script_GuildUninvite },
    { "GuildPromote", &Script_GuildPromote },
    { "GuildDemote", &Script_GuildDemote },
    { "GuildSetLeader", &Script_GuildSetLeader },
    { "GuildSetMOTD", &Script_GuildSetMOTD },
    { "GuildLeave", &Script_GuildLeave },
    { "GuildDisband", &Script_GuildDisband },
    { "GuildInfo", &Script_GuildInfo },
    { "ArenaTeamInviteByName", &Script_ArenaTeamInviteByName },
    { "ArenaTeamLeave", &Script_ArenaTeamLeave },
    { "ArenaTeamUninviteByName", &Script_ArenaTeamUninviteByName },
    { "ArenaTeamSetLeaderByName", &Script_ArenaTeamSetLeaderByName },
    { "ArenaTeamDisband", &Script_ArenaTeamDisband },
    { "GetScreenWidth", &Script_GetScreenWidth },
    { "GetScreenHeight", &Script_GetScreenHeight },
    { "GetDamageBonusStat", &Script_GetDamageBonusStat },
    { "GetReleaseTimeRemaining", &Script_GetReleaseTimeRemaining },
    { "GetCorpseRecoveryDelay", &Script_GetCorpseRecoveryDelay },
    { "GetInstanceBootTimeRemaining", &Script_GetInstanceBootTimeRemaining },
    { "GetInstanceLockTimeRemaining", &Script_GetInstanceLockTimeRemaining },
    { "GetInstanceLockTimeRemainingEncounter", &Script_GetInstanceLockTimeRemainingEncounter },
    { "GetSummonConfirmTimeLeft", &Script_GetSummonConfirmTimeLeft },
    { "GetSummonConfirmSummoner", &Script_GetSummonConfirmSummoner },
    { "GetSummonConfirmAreaName", &Script_GetSummonConfirmAreaName },
    { "ConfirmSummon", &Script_ConfirmSummon },
    { "CancelSummon", &Script_CancelSummon },
    { "GetCursorPosition", &Script_GetCursorPosition },
    { "GetNetStats", &Script_GetNetStats },
    { "SitStandOrDescendStart", &Script_SitStandOrDescendStart },
    { "StopCinematic", &Script_StopCinematic },
    { "RunScript", &Script_RunScript },
    { "CheckInteractDistance", &Script_CheckInteractDistance },
    { "RandomRoll", &Script_RandomRoll },
    { "OpeningCinematic", &Script_OpeningCinematic },
    { "InCinematic", &Script_InCinematic },
    { "IsWindowsClient", &Script_IsWindowsClient },
    { "IsMacClient", &Script_IsMacClient },
    { "IsLinuxClient", &Script_IsLinuxClient },
    { "AcceptXPLoss", &Script_AcceptXPLoss },
    { "CheckSpiritHealerDist", &Script_CheckSpiritHealerDist },
    { "CheckTalentMasterDist", &Script_CheckTalentMasterDist },
    { "CheckBinderDist", &Script_CheckBinderDist },
    { "RetrieveCorpse", &Script_RetrieveCorpse },
    { "BindEnchant", &Script_BindEnchant },
    { "ReplaceEnchant", &Script_ReplaceEnchant },
    { "ReplaceTradeEnchant", &Script_ReplaceTradeEnchant },
    { "NotWhileDeadError", &Script_NotWhileDeadError },
    { "GetRestState", &Script_GetRestState },
    { "GetXPExhaustion", &Script_GetXPExhaustion },
    { "GetTimeToWellRested", &Script_GetTimeToWellRested },
    { "GMRequestPlayerInfo", &Script_GMRequestPlayerInfo },
    { "GetCoinIcon", &Script_GetCoinIcon },
    { "GetCoinText", &Script_GetCoinText },
    { "GetCoinTextureString", &Script_GetCoinTextureString },
    { "IsSubZonePVPPOI", &Script_IsSubZonePVPPOI },
    { "GetZonePVPInfo", &Script_GetZonePVPInfo },
    { "TogglePVP", &Script_TogglePVP },
    { "SetPVP", &Script_SetPVP },
    { "GetPVPDesired", &Script_GetPVPDesired },
    { "GetPVPTimer", &Script_GetPVPTimer },
    { "IsPVPTimerRunning", &Script_IsPVPTimerRunning },
    { "ConfirmBindOnUse", &Script_ConfirmBindOnUse },
    { "SetPortraitToTexture", &Script_SetPortraitToTexture },
    { "GetLocale", &Script_GetLocale },
    { "GetGMTicketCategories", &Script_GetGMTicketCategories },
    { "DropItemOnUnit", &Script_DropItemOnUnit },
    { "RestartGx", &Script_RestartGx },
    { "RestoreVideoResolutionDefaults", &Script_RestoreVideoResolutionDefaults },
    { "RestoreVideoEffectsDefaults", &Script_RestoreVideoEffectsDefaults },
    { "RestoreVideoStereoDefaults", &Script_RestoreVideoStereoDefaults },
    { "GetBindLocation", &Script_GetBindLocation },
    { "ConfirmTalentWipe", &Script_ConfirmTalentWipe },
    { "ConfirmBinder", &Script_ConfirmBinder },
    { "ShowingHelm", &Script_ShowingHelm },
    { "ShowingCloak", &Script_ShowingCloak },
    { "ShowHelm", &Script_ShowHelm },
    { "ShowCloak", &Script_ShowCloak },
    { "SetEuropeanNumbers", &Script_SetEuropeanNumbers },
    { "GetAreaSpiritHealerTime", &Script_GetAreaSpiritHealerTime },
    { "AcceptAreaSpiritHeal", &Script_AcceptAreaSpiritHeal },
    { "CancelAreaSpiritHeal", &Script_CancelAreaSpiritHeal },
    { "GetMouseFocus", &Script_GetMouseFocus },
    { "GetRealmName", &Script_GetRealmName },
    { "GetItemQualityColor", &Script_GetItemQualityColor },
    { "GetItemInfo", &Script_GetItemInfo },
    { "GetItemGem", &Script_GetItemGem },
    { "GetExtendedItemInfo", &Script_GetExtendedItemInfo },
    { "GetItemIcon", &Script_GetItemIcon },
    { "GetItemFamily", &Script_GetItemFamily },
    { "GetItemCount", &Script_GetItemCount },
    { "GetItemSpell", &Script_GetItemSpell },
    { "GetItemCooldown", &Script_GetItemCooldown },
    { "PickupItem", &Script_PickupItem },
    { "IsCurrentItem", &Script_IsCurrentItem },
    { "IsUsableItem", &Script_IsUsableItem },
    { "IsHelpfulItem", &Script_IsHelpfulItem },
    { "IsHarmfulItem", &Script_IsHarmfulItem },
    { "IsConsumableItem", &Script_IsConsumableItem },
    { "IsEquippableItem", &Script_IsEquippableItem },
    { "IsEquippedItem", &Script_IsEquippedItem },
    { "IsEquippedItemType", &Script_IsEquippedItemType },
    { "IsDressableItem", &Script_IsDressableItem },
    { "ItemHasRange", &Script_ItemHasRange },
    { "IsItemInRange", &Script_IsItemInRange },
    { "GetNumAddOns", &Script_GetNumAddOns },
    { "GetAddOnInfo", &Script_GetAddOnInfo },
    { "GetAddOnMetadata", &Script_GetAddOnMetadata },
    { "UpdateAddOnMemoryUsage", &Script_UpdateAddOnMemoryUsage },
    { "GetAddOnMemoryUsage", &Script_GetAddOnMemoryUsage },
    { "GetScriptCPUUsage", &Script_GetScriptCPUUsage },
    { "UpdateAddOnCPUUsage", &Script_UpdateAddOnCPUUsage },
    { "GetAddOnCPUUsage", &Script_GetAddOnCPUUsage },
    { "GetFunctionCPUUsage", &Script_GetFunctionCPUUsage },
    { "GetFrameCPUUsage", &Script_GetFrameCPUUsage },
    { "GetEventCPUUsage", &Script_GetEventCPUUsage },
    { "ResetCPUUsage", &Script_ResetCPUUsage },
    { "GetAddOnDependencies", &Script_GetAddOnDependencies },
    { "EnableAddOn", &Script_EnableAddOn },
    { "EnableAllAddOns", &Script_EnableAllAddOns },
    { "DisableAddOn", &Script_DisableAddOn },
    { "DisableAllAddOns", &Script_DisableAllAddOns },
    { "ResetDisabledAddOns", &Script_ResetDisabledAddOns },
    { "IsAddOnLoadOnDemand", &Script_IsAddOnLoadOnDemand },
    { "IsAddOnLoaded", &Script_IsAddOnLoaded },
    { "LoadAddOn", &Script_LoadAddOn },
    { "PartialPlayTime", &Script_PartialPlayTime },
    { "NoPlayTime", &Script_NoPlayTime },
    { "GetBillingTimeRested", &Script_GetBillingTimeRested },
    { "CanShowResetInstances", &Script_CanShowResetInstances },
    { "ResetInstances", &Script_ResetInstances },
    { "IsInInstance", &Script_IsInInstance },
    { "GetInstanceDifficulty", &Script_GetInstanceDifficulty },
    { "GetInstanceInfo", &Script_GetInstanceInfo },
    { "GetDungeonDifficulty", &Script_GetDungeonDifficulty },
    { "SetDungeonDifficulty", &Script_SetDungeonDifficulty },
    { "GetRaidDifficulty", &Script_GetRaidDifficulty },
    { "SetRaidDifficulty", &Script_SetRaidDifficulty },
    { "ReportBug", &Script_ReportBug },
    { "ReportSuggestion", &Script_ReportSuggestion },
    { "GetMirrorTimerInfo", &Script_GetMirrorTimerInfo },
    { "GetMirrorTimerProgress", &Script_GetMirrorTimerProgress },
    { "GetNumTitles", &Script_GetNumTitles },
    { "GetCurrentTitle", &Script_GetCurrentTitle },
    { "SetCurrentTitle", &Script_SetCurrentTitle },
    { "IsTitleKnown", &Script_IsTitleKnown },
    { "GetTitleName", &Script_GetTitleName },
    { "UseItemByName", &Script_UseItemByName },
    { "EquipItemByName", &Script_EquipItemByName },
    { "GetExistingLocales", &Script_GetExistingLocales },
    { "InCombatLockdown", &Script_InCombatLockdown },
    { "StartAttack", &Script_StartAttack },
    { "StopAttack", &Script_StopAttack },
    { "SetTaxiBenchmarkMode", &Script_SetTaxiBenchmarkMode },
    { "GetTaxiBenchmarkMode", &Script_GetTaxiBenchmarkMode },
    { "Dismount", &Script_Dismount },
    { "VoicePushToTalkStart", &Script_VoicePushToTalkStart },
    { "VoicePushToTalkStop", &Script_VoicePushToTalkStop },
    { "SetUIVisibility", &Script_SetUIVisibility },
    { "IsReferAFriendLinked", &Script_IsReferAFriendLinked },
    { "CanGrantLevel", &Script_CanGrantLevel },
    { "GrantLevel", &Script_GrantLevel },
    { "CanSummonFriend", &Script_CanSummonFriend },
    { "SummonFriend", &Script_SummonFriend },
    { "GetSummonFriendCooldown", &Script_GetSummonFriendCooldown },
    { "GetTotemInfo", &Script_GetTotemInfo },
    { "GetTotemTimeLeft", &Script_GetTotemTimeLeft },
    { "TargetTotem", &Script_TargetTotem },
    { "DestroyTotem", &Script_DestroyTotem },
    { "GetNumDeclensionSets", &Script_GetNumDeclensionSets },
    { "DeclineName", &Script_DeclineName },
    { "AcceptLevelGrant", &Script_AcceptLevelGrant },
    { "DeclineLevelGrant", &Script_DeclineLevelGrant },
    { "UploadSettings", &Script_UploadSettings },
    { "DownloadSettings", &Script_DownloadSettings },
    { "GetMovieResolution", &Script_GetMovieResolution },
    { "GameMovieFinished", &Script_GameMovieFinished },
    { "IsDesaturateSupported", &Script_IsDesaturateSupported },
    { "GetThreatStatusColor", &Script_GetThreatStatusColor },
    { "IsThreatWarningEnabled", &Script_IsThreatWarningEnabled },
    { "ConsoleAddMessage", &Script_ConsoleAddMessage },
    { "GetItemUniqueness", &Script_GetItemUniqueness },
    { "EndRefund", &Script_EndRefund },
    { "EndBoundTradeable", &Script_EndBoundTradeable },
    { "CanMapChangeDifficulty", &Script_CanMapChangeDifficulty },
    { "GetExpansionLevel", &Script_GetAccountExpansionLevel },
    { "GetAllowLowLevelRaid", &Script_GetAllowLowLevelRaid },
    { "SetAllowLowLevelRaid", &Script_SetAllowLowLevelRaid },
};

// VERIFY: 2d4b8add manual
// OFFSET: 0x005120E0
void LoadScriptFunctions() {
    RegisterSimpleFrameScriptMethods();
    CGTooltip::CreateScriptMetaTable();
    CGCooldown::CreateScriptMetaTable();
    CGMinimapFrame::CreateScriptMetaTable();
    CGCharacterModelBase::CreateScriptMetaTable();
    CGDressUpModelFrame::CreateScriptMetaTable();
    CGTabardModelFrame::CreateScriptMetaTable();
    CGQuestPOIFrame::CreateScriptMetaTable();

    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }

    UIBindingsRegisterScriptFunctions();
    UIMacrosRegisterScriptFunctions();
    InputControlRegisterScriptFunctions();
    CameraRegisterScriptFunctions();
    SI2::RegisterScriptFunctions();
    SpellRegisterScriptFunctions();
    ScriptEventsRegisterFunctions();
    MinimapRegisterScriptFunctions();
    UnitCombatLogRegisterScriptFunctions();
    ActionBarRegisterScriptFunctions();
    CGPartyInfo::RegisterScriptFunctions();
    ChatRegisterScriptFunctions();
    AutoCompleteRegisterScriptFunctions();
    SpellBookRegisterScriptFunctions();
    CharacterInfoRegisterScriptFunctions();
    LootInfoRegisterScriptFunctions();
    ItemTextRegisterScriptFunctions();
    GossipInfoRegisterScriptFunctions();
    QuestInfoRegisterScriptFunctions();
    QuestLogRegisterScriptFunctions();
    CGTaxiMap::RegisterScriptFunctions();
    ClassTrainerRegisterScriptFunctions();
    TradeSkillRegisterScriptFunctions();
    MerchantRegisterScriptFunctions();
    TradeInfoRegisterScriptFunctions();
    ContainerRegisterScriptFunctions();
    BankRegisterScriptFunctions();
    FriendList::RegisterScriptFunctions();
    PetInfoRegisterScriptFunctions();
    WorldMapRegisterScriptFunctions();
    ReputationInfoRegisterScriptFunctions();
    TabardCreationRegisterScriptFunctions();
    GuildRegistrarRegisterScriptFunctions();
    DuelInfoRegisterScriptFunctions();
    TutorialRegisterScriptFunctions();
    PetitionInfoRegisterScriptFunctions();
    SkillInfoRegisterScriptFunctions();
    GuildInfoRegisterScriptFunctions();
    MailInfoRegisterScriptFunctions();
    BattlefieldInfoRegisterScriptFunctions();
    TalentInfoRegisterScriptFunctions();
    AuctionHouseRegisterScriptFunctions();
    StableInfoRegisterScriptFunctions();
    RaidInfoRegisterScriptFunctions();
    WorldStateInfoRegisterScriptFunctions();
    MinigameInfoRegisterScriptFunctions();
    KnowledgeBase_RegisterScriptFunctions();
    ItemSocketInfoRegisterScriptFunctions();
    PetitionVendorRegisterScriptFunctions();
    ArenaTeamInfoRegisterScriptFunctions();
    LookingForGroupRegisterScriptFunctions();
    VoiceRegisterScriptFunctions();
    CalendarRegisterScriptFunctions();
    CGCommentator::RegisterScriptFunctions();
    CGBarberShop::RegisterScriptFunctions();
    CGDanceStudio::RegisterScriptFunctions();
    GuildBankRegisterScriptFunctions();
    GlyphInfoRegisterScriptFunctions();
    AchievementInfoRegisterScriptFunctions();
    CurrencyTypesRegisterScriptFunctions();
    CGVideoOptions::RegisterScriptFunctions();
    EquipmentManagerRegisterScriptFunctions();
    GMTicketInfoRegisterScriptFunctions();
    BattlenetUI_RegisterScriptFunctions();
}
