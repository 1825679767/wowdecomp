#include "PaperDollInfoFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C24230
TSFixedArray<int32_t> CGCharacterInfo::m_invAlertStatus;

// STATIC: 0x00C24220
WOWGUID CGCharacterInfo::m_inspectTarget;

// STATIC: 0x00C24228
int32_t CGCharacterInfo::m_hasHonorData = -1;

// STATIC: 0x00C241C0
InspectArenaTeam CGCharacterInfo::m_inspectArenaTeams[3];

// STATIC: 0x00C24208
uint32_t CGCharacterInfo::m_inspectLifetimeHonorableKills;

// STATIC: 0x00C2420C
uint32_t CGCharacterInfo::m_inspectYesterdayHonor;

// STATIC: 0x00C24210
uint32_t CGCharacterInfo::m_inspectTodayHonor;

// STATIC: 0x00C24214
uint16_t CGCharacterInfo::m_inspectYesterdayHonorableKills;

// STATIC: 0x00C24216
uint8_t CGCharacterInfo::m_inspectLifetimeHighestRank;

// STATIC: 0x00C24218
uint16_t CGCharacterInfo::m_inspectTodayHonorableKills;

// STATIC: 0x00C2422C
int32_t CGCharacterInfo::m_honorRequestPending;

// VERIFY: 37e279b1 auto
// OFFSET: 0x005E8440
void CGCharacterInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_INSPECT_HONOR_STATS, Packet_MSG_INSPECT_HONOR_STATS, nullptr);
    ClientServices::SetMessageHandler(MSG_INSPECT_ARENA_TEAMS, Packet_MSG_INSPECT_ARENA_TEAMS, nullptr);
}

// VERIFY: 174a574b auto
// OFFSET: 0x005E8470
void CGCharacterInfo::RegisterMirrorHandlers(WOWGUID guid) {
    if (!guid) {
        return;
    }

    ClntObjMgrSetObjMirrorHandler(guid, ID_PLAYER, 0xDA0, 8, PlayerCharacterPointsUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(guid, ID_PLAYER, 0x1068, 4, PlayerEquipmentChangedHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: 18a6a029 manual
// OFFSET: 0x005E90D0
void CGCharacterInfo::UpdateInventoryAlerts() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    bool changed = false;

    for (uint32_t i = 0; i < 12; i++) {
        int32_t status = CGCharacterInfo::CalculateInventoryAlertStatus(player, i);

        if (status != CGCharacterInfo::m_invAlertStatus.m_data[i]) {
            CGCharacterInfo::m_invAlertStatus.m_data[i] = status;
            changed = true;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_INVENTORY_ALERTS, nullptr);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_INVENTORY_DURABILITY, nullptr);
}

// VERIFY: 315ec719 auto
// OFFSET: 0x005EAF30
void CGCharacterInfo::EnterWorld() {
    CGCharacterInfo::RegisterMirrorHandlers(ClntObjMgrGetActivePlayer());
    CGCharacterInfo::m_invAlertStatus.SetCount(12);
    CGCharacterInfo::UpdateInventoryAlerts();
}

// VERIFY: e09601e2 auto
// OFFSET: 0x005E7660
void UnitInventoryChangedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_INVENTORY_CHANGED, "%s", "player");
    }
}

// VERIFY: 45ff113c manual
// OFFSET: 0x005E7A00
int32_t CGCharacterInfo::CanInspect(const CGPlayer_C* unit) {
    if (!unit) {
        return 0;
    }

    return unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || unit->m_obj->m_guid == CGCharacterInfo::m_inspectTarget;
}

// VERIFY: 97f574c5 manual
// OFFSET: 0x005E7C00
int32_t ResolveInventorySlotArg(lua_State* L, int32_t index, int32_t& slot) {
    if (lua_isnumber(L, index)) {
        slot = static_cast<int32_t>(lua_tonumber(L, index)) - 1;
    } else if (lua_isstring(L, index)) {
        const char* name = lua_tolstring(L, index, nullptr);

        for (int32_t i = 0; i < g_paperDollItemFrameDB.m_numRecords; i++) {
            PaperDollItemFrameRec* rec = i >= 0 && i < g_paperDollItemFrameDB.m_numRecords ? &g_paperDollItemFrameDB.m_records[i] : nullptr;

            if (!SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                slot = rec->m_slotNumber;
                break;
            }
        }
    }

    return (slot >= 0 && slot <= 22) || (slot >= 39 && slot <= 66) || (slot >= 67 && slot <= 73) || slot == -1 || (slot >= 86 && slot <= 117);
}

// VERIFY: f340d760 auto
// OFFSET: 0x005E7CF0
int32_t GetUnitAndSlotFromLua(lua_State* L, const char*& unit, int32_t& slot) {
    if (!lua_isstring(L, 1)) {
        char buffer[1024];
        luaL_error(L, "Usage: %s(unit, slot)", FrameScript_GetCurrentFunction(L, buffer, sizeof(buffer)));
    }

    unit = lua_tolstring(L, 1, nullptr);
    return ResolveInventorySlotArg(L, 2, slot);
}

// VERIFY: d19c5b13 manual
// OFFSET: 0x005E8300
int32_t CGCharacterInfo::GetActivePlayerAmmoID(const CGPlayer_C* player) {
    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return player->m_player->m_playerAmmoId;
    }

    return 0;
}

// VERIFY: b07082fe auto
// OFFSET: 0x005E7A50
int32_t Packet_MSG_INSPECT_HONOR_STATS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (guid != CGCharacterInfo::m_inspectTarget) {
        msg->m_read = msg->m_size;
        return 1;
    }

    msg->Get(CGCharacterInfo::m_inspectLifetimeHighestRank);
    msg->Get(CGCharacterInfo::m_inspectTodayHonorableKills);
    msg->Get(CGCharacterInfo::m_inspectYesterdayHonorableKills);
    msg->Get(CGCharacterInfo::m_inspectTodayHonor);
    msg->Get(CGCharacterInfo::m_inspectYesterdayHonor);
    msg->Get(CGCharacterInfo::m_inspectLifetimeHonorableKills);
    CGCharacterInfo::m_hasHonorData = 1;
    CGCharacterInfo::m_honorRequestPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, nullptr);
    return 1;
}

// VERIFY: f1172d97 auto
// OFFSET: 0x005E7B00
int32_t Packet_MSG_INSPECT_ARENA_TEAMS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (guid != CGCharacterInfo::m_inspectTarget) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint8_t slot;
    msg->Get(slot);

    if (slot > 3) {
        msg->m_read = msg->m_size;
        return 1;
    }

    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_teamId);
    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_rating);
    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_weekGames);
    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_weekWins);
    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_seasonGames);
    msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_personalRating);
    FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, nullptr);
    return 1;
}
