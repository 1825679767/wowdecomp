#include "GameUI.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/World.h"
#include "_Triage/GxRestart.h"

// STATIC: 0x00AC804C
const char* g_macroTaint = "";

// STATIC: 0x00AC8AE0
CCapturePointUIManager g_capturePointUIManager;

// STATIC: 0x00BD078C
CSimpleTop* CGGameUI::m_simpleTop;

// STATIC: 0x00BD07A0
WOWGUID CGGameUI::m_currentObjectTrack;

// STATIC: 0x00BD07A8
WOWGUID CGGameUI::m_interactTarget;

// STATIC: 0x00BD07B0
WOWGUID CGGameUI::m_lockedTarget;

// STATIC: 0x00BD07C0
WOWGUID CGGameUI::m_lastEnemyTarget;

// STATIC: 0x00BD07D0
WOWGUID CGGameUI::unk_BD07D0;

// STATIC: 0x00BD0800
int32_t CGGameUI::m_inMovie;

// STATIC: 0x00BD0828
WOWGUID CGGameUI::m_corpseGUID;

// STATIC: 0x00BD0880
int32_t CGGameUI::m_initialized;

// STATIC: 0x00BD088C
int32_t CGGameUI::m_iCurrentMapID;

// VERIFY: 6c643fe3 manual
// OFFSET: 0x00512900
void OnGxRestartClearWorld() {
    if (CGWorldFrame::s_currentWorldFrame) {
        CWorldScene::ClearObjectFadeoutList();
    }

    UpdateAllPortraits();
}

// VERIFY: ff48c834 auto
// OFFSET: 0x00512A60
WOWGUID CGGameUI::GetPartyMember(uint32_t index) {
    return GetPartyMemberGUID(index);
}

// VERIFY: 3634a2e6 auto
// OFFSET: 0x00512A70
WOWGUID CGGameUI::GetPartyMemberPet(uint32_t index) {
    return GetPartyPetGUID(index);
}

// VERIFY: 62eb235c auto
// OFFSET: 0x00512A80
WOWGUID GetRaidMemberGUID(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers || !CGRaidInfo::m_members[index]) {
        return WOWGUID();
    }

    return CGRaidInfo::m_members[index]->m_guid;
}

// VERIFY: b87793f3 auto
// OFFSET: 0x00512C20
void CGGameUI::SetActiveCorpse(WOWGUID guid) {
    CGGameUI::m_corpseGUID = guid;
}

// VERIFY: 6ad55878 manual
// OFFSET: 0x00518CE0
WOWGUID CGGameUI::GetRaidMemberPet(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers) {
        return WOWGUID();
    }

    CGRaidMember* member = CGRaidInfo::m_members[index];

    if (!member) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return member->m_stats.m_pet.m_guid;
    }

    const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    return pet;
}

// VERIFY: 23e6422e auto
// OFFSET: 0x00513530
void CGGameUI::ShowBlockedActionFeedback(FrameScript_Object* object, uint32_t type) {
    // STATIC: 0x00BD0C68
    static int32_t showing;

    if (showing) {
        return;
    }

    showing = 1;

    lua_State* L = FrameScript_GetContext();
    char function[1024];
    FrameScript_GetCurrentFunction(L, function, sizeof(function));
    SStrPack(function, "()", sizeof(function));

    if (type) {
        if (lua_tainted == g_macroTaint) {
            FrameScript_SignalEvent(427, "%s", function);
        } else {
            FrameScript_SignalEvent(428, "%s%s", lua_tainted, function);
        }
    } else if (lua_tainted == g_macroTaint) {
        FrameScript_SignalEvent(425, "%s", function);
    } else {
        FrameScript_SignalEvent(426, "%s%s", lua_tainted, function);
    }

    if (lua_tainteventcallback) {
        if (type == 2) {
            lua_tainteventcallback(L, 4, nullptr, lua_tainted);
        } else {
            lua_tainteventcallback(L, 3, nullptr, lua_tainted);
        }
    }

    showing = 0;
}

// VERIFY: 9da22ed9 auto
// OFFSET: 0x005191C0
int32_t CGGameUI::CanPerformAction(uint32_t action) {
    if (lua_tainted) {
        switch (action) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 16:
            case 17:
                CGGameUI::ShowBlockedActionFeedback(nullptr, 0);
                return 0;

            case 6:
            case 7:
            case 8:
            case 9:
            case 15:
            case 18:
            case 19:
            case 20:
            case 21:
            case 23:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->unk_1250) {
                    CGGameUI::ShowBlockedActionFeedback(nullptr, 1);
                    return 0;
                }

                break;

            case 11:
            case 12:
            case 13:
            case 14:
            case 22:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->m_allowProtectedActions) {
                    CGGameUI::ShowBlockedActionFeedback(nullptr, 2);
                    return 0;
                }

                break;

            default:
                break;
        }
    }

    if (CGGameUI::m_simpleTop && !action) {
        CGGameUI::m_simpleTop->unk_1250 = 0;
    }

    return 1;
}
