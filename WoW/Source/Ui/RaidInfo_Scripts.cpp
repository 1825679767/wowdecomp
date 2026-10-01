#include "RaidInfo.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// VERIFY: 3ff51581 manual
// OFFSET: 0x00572B40
static int32_t Script_GetNumRaidMembers(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::m_numMembers);
    return 1;
}

// VERIFY: fcb45632 manual
// OFFSET: 0x00572B80
static int32_t Script_GetRealNumRaidMembers(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::m_realNumMembers);
    return 1;
}

// VERIFY: 8d0b38de auto
// OFFSET: 0x00573A60
static int32_t Script_IsRaidLeader(lua_State* L) {
    if (ClntObjMgrGetActivePlayer() == CGPartyInfo::m_leader) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: ad4569ee auto
// OFFSET: 0x00573AB0
static int32_t Script_IsRealRaidLeader(lua_State* L) {
    if (ClntObjMgrGetActivePlayer() == CGPartyInfo::m_realLeader) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b9f445b5 manual
// OFFSET: 0x00572BC0
static int32_t Script_SetRaidRosterSelection(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetRaidRosterSelection(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGRaidInfo::m_numMembers) {
        CGRaidInfo::m_selectedMember = WOWGUID();
        return 0;
    }

    CGRaidInfo::m_selectedMember = CGRaidInfo::m_members[index]->m_guid;
    return 0;
}

// VERIFY: 452e857c manual
// OFFSET: 0x00572C80
static int32_t Script_GetReadyCheckTimeLeft(lua_State* L) {
    if (CGRaidInfo::m_numMembers || CGPartyInfo::m_members[0]) {
        uint32_t now = OsGetAsyncTimeMs();
        uint32_t remaining;

        if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0) {
            remaining = CGRaidInfo::m_readyCheckTime - now;
        } else {
            remaining = 0;
        }

        lua_pushnumber(L, remaining / 1000);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: c4260696 manual
// OFFSET: 0x00574A00
static int32_t Script_ConvertToRaid(lua_State* L) {
    if (CGPartyInfo::m_members[0] && CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= 10) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_GROUP_RAID_CONVERT));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    return 0;
}

// VERIFY: 2efa072d manual
// OFFSET: 0x005740C0
static int32_t Script_ConfirmReadyCheck(lua_State* L) {
    if (CGRaidInfo::m_numMembers || CGPartyInfo::m_members[0]) {
        uint32_t now = OsGetAsyncTimeMs();
        uint32_t remaining;

        if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0) {
            remaining = CGRaidInfo::m_readyCheckTime - now;
        } else {
            remaining = 0;
        }

        if (remaining) {
            uint8_t ready = StringToBOOL(L, 1, 0);
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(MSG_RAID_READY_CHECK));
            msg.Put(ready);
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    return 0;
}

// VERIFY: a6457528 auto
// OFFSET: 0x00573B00
static int32_t Script_IsRaidOfficer(lua_State* L) {
    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

    if (member && member->m_rank > 0) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9510fc76 manual
// OFFSET: 0x00574180
static int32_t Script_GetReadyCheckStatus(lua_State* L) {
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, nullptr));
    const char* status = nullptr;

    if (!CGRaidInfo::m_numMembers) {
        if (!CGPartyInfo::m_members[0] || CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
            lua_pushstring(L, status);
            return 1;
        }

        int32_t index = CGRaidInfo::GetPartyMemberIndex(guid);
        uint32_t now = OsGetAsyncTimeMs();

        if (!CGRaidInfo::m_readyCheckTime || static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) >= 0 || CGRaidInfo::m_readyCheckTime == now) {
            lua_pushstring(L, status);
            return 1;
        }

        if (index == -1) {
            if (guid == CGRaidInfo::m_readyCheckInitiator) {
                status = "ready";
            }

            lua_pushstring(L, status);
            return 1;
        }

        if (CGRaidInfo::IsPartyMemberReadyCheckPending(index)) {
            lua_pushstring(L, "waiting");
            return 1;
        }

        if (CGRaidInfo::IsPartyMemberReadyCheckReady(index)) {
            lua_pushstring(L, "ready");
            return 1;
        }

        lua_pushstring(L, "notready");
        return 1;
    }

    CGRaidMember* self = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

    if (!self || self->m_rank <= 0) {
        lua_pushstring(L, status);
        return 1;
    }

    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);
    uint32_t now = OsGetAsyncTimeMs();

    if (!CGRaidInfo::m_readyCheckTime || static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) >= 0 || CGRaidInfo::m_readyCheckTime == now || !member) {
        lua_pushstring(L, status);
        return 1;
    }

    if (guid == CGRaidInfo::m_readyCheckInitiator) {
        lua_pushstring(L, "ready");
        return 1;
    }

    if (member->m_readyCheckPending) {
        lua_pushstring(L, "waiting");
        return 1;
    }

    if (member->m_readyCheckReady) {
        lua_pushstring(L, "ready");
        return 1;
    }

    lua_pushstring(L, "notready");
    return 1;
}

// STATIC: 0x00ACE790
// VERIFY: 86d61574 auto
// OFFSET: 0x00572AB0
static int32_t Script_GetRaidTargetIndex(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetRaidTargetIndex(unit)");
    }

    uint32_t index = GetRaidTargetIndexFromGuid(GetGuidByKeyword(lua_tolstring(L, 1, nullptr)));

    if (index == 8) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, index + 1);
    }

    return 1;
}

static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumRaidMembers", &Script_GetNumRaidMembers },
    { "GetRealNumRaidMembers", &Script_GetRealNumRaidMembers },
    { "GetRaidRosterInfo", &Script_GetRaidRosterInfo },
    { "SetRaidRosterSelection", &Script_SetRaidRosterSelection },
    { "GetRaidRosterSelection", &Script_GetRaidRosterSelection },
    { "IsRaidLeader", &Script_IsRaidLeader },
    { "IsRealRaidLeader", &Script_IsRealRaidLeader },
    { "IsRaidOfficer", &Script_IsRaidOfficer },
    { "SetRaidSubgroup", &Script_SetRaidSubgroup },
    { "SwapRaidSubgroup", &Script_SwapRaidSubgroup },
    { "ConvertToRaid", &Script_ConvertToRaid },
    { "PromoteToLeader", &Script_PromoteToLeader },
    { "PromoteToAssistant", &Script_PromoteToAssistant },
    { "DemoteAssistant", &Script_DemoteAssistant },
    { "SetRaidTarget", &Script_SetRaidTarget },
    { "GetRaidTargetIndex", &Script_GetRaidTargetIndex },
    { "DoReadyCheck", &Script_DoReadyCheck },
    { "ConfirmReadyCheck", &Script_ConfirmReadyCheck },
    { "GetReadyCheckTimeLeft", &Script_GetReadyCheckTimeLeft },
    { "GetReadyCheckStatus", &Script_GetReadyCheckStatus },
};

// VERIFY: 8b29d591 auto
// OFFSET: 0x00572D10
void RaidInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
