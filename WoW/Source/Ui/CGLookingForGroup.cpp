#include "LFGInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/Ui/LFGInfo.h"

// STATIC: 0x00A394B0
static const uint8_t s_classRoles[11] = { 0x0B, 0x0F, 0x09, 0x09, 0x0D, 0x0B, 0x0D, 0x09, 0x09, 0x00, 0x0F };

// STATIC: 0x00ACDD6C
static int32_t s_joinAttempts;

// STATIC: 0x00BEABC4
static float s_lastJoinTime;

// VERIFY: 7da483d6 auto
// OFFSET: 0x00552940
uint8_t CGLookingForGroup::GetClassRoles(uint32_t classID) {
    return s_classRoles[classID - 1];
}

// VERIFY: 29999d79 manual
// OFFSET: 0x00555920
void CGLookingForGroup::RequestLFGStatus() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_DF_GET_JOIN_STATUS));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 4918a5f7 manual
// OFFSET: 0x00555990
void CGLookingForGroup::RequestLFGPlayerInfo() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LFG_PLAYER_LOCK_INFO_REQUEST));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 40d9d329 auto
// OFFSET: 0x00557240
void CGLookingForGroup::EnterWorld() {
    CGLookingForGroup::m_lastProposalID = 0;
    CGLookingForGroup::RequestLFGStatus();
    CGLookingForGroup::RequestLFGPlayerInfo();
}

// VERIFY: 62f8faf7 manual
// OFFSET: 0x00553830
void DungeonEncounterCountMatchingRows(int32_t mapId, int32_t difficulty, uint32_t completedMask, uint32_t* total, uint32_t* completed) {
    *total = 0;
    *completed = 0;

    for (int32_t i = 0; i < g_dungeonEncounterDB.m_numRecords; i++) {
        DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

        if (encounter->m_mapID == mapId && encounter->m_difficulty == difficulty) {
            ++*total;

            if ((1 << encounter->m_bit) & completedMask) {
                ++*completed;
            }
        }
    }
}

// VERIFY: e291b497 auto
// OFFSET: 0x005538B0
int32_t PushDungeonEncounterInfo(lua_State* L, int32_t mapId, int32_t difficulty, int32_t index, uint32_t completedMask) {
    for (int32_t i = 0; i < g_dungeonEncounterDB.m_numRecords; i++) {
        DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

        if (encounter->m_mapID != mapId || encounter->m_difficulty != difficulty) {
            continue;
        }

        if (index) {
            index--;
            continue;
        }

        lua_pushstring(L, encounter->m_name);
        SpellIconRec* icon;

        if (encounter->m_spellIconID && (icon = g_spellIconDB.GetRecord(encounter->m_spellIconID)) && icon->m_textureFilename) {
            lua_pushstring(L, icon->m_textureFilename);
        } else {
            lua_pushnil(L);
        }

        lua_pushboolean(L, (completedMask & (1 << encounter->m_bit)) != 0);
        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// VERIFY: a21f028e manual
// OFFSET: 0x00553B50
static uint32_t FilterRolesByClass(uint8_t roles) {
    ChrClassesRec* rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
    return roles & s_classRoles[rec->m_id - 1];
}

// VERIFY: 49ccd8f7 manual
// OFFSET: 0x00554610
int32_t CGLookingForGroup::GetLFGRoleUpdate(lua_State* L) {
    lua_pushboolean(L, CGLookingForGroup::m_roleCheckState == 2);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckSlots.m_count);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckMembers.m_count);
    return 3;
}

// VERIFY: b42e78a8 auto
// OFFSET: 0x00554690
int32_t CGLookingForGroup::GetLFGRoleUpdateSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGRoleUpdateSlot(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLookingForGroup::m_roleCheckSlots.m_count)) {
        return 0;
    }

    lua_pushnumber(L, CGLookingForGroup::m_roleCheckSlots.m_data[index] >> 24);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckSlots.m_data[index] & 0xFFFFFF);
    return 2;
}

// VERIFY: 49637f08 manual
// OFFSET: 0x005572D0
void CGLookingForGroup::JoinLFG() {
    if ((CGLookingForGroup::m_localInfo.m_joined && !CGLookingForGroup::m_localInfo.unk_0003) || !s_cvLfgSelectedRoles) {
        return;
    }

    if (++s_joinAttempts > 2) {
        float now = OsGetAsyncTimeSec();

        if (now - s_lastJoinTime < 10.0f) {
            if (s_joinAttempts > 2) {
                return;
            }
        } else {
            s_lastJoinTime = now;
            s_joinAttempts = 0;
        }
    }

    CDataStore msg;
    uint32_t roles = FilterRolesByClass(static_cast<uint8_t>(s_cvLfgSelectedRoles->m_intValue));

    if (!(roles & ~0x1)) {
        return;
    }

    msg.Put(static_cast<uint32_t>(CMSG_LFG_JOIN));
    msg.Put(roles);
    msg.Put(static_cast<uint8_t>(CGLookingForGroup::m_localInfo.unk_0004 != 0));
    msg.Put(static_cast<uint8_t>(CGLookingForGroup::m_localInfo.unk_0005 != 0));
    msg.Put(static_cast<uint8_t>(CGLookingForGroup::m_localInfo.m_numDungeons));

    for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Head(); dungeon; dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Next(dungeon)) {
        msg.Put(dungeon->m_slot);
    }

    msg.Put(static_cast<uint8_t>(3));

    for (uint32_t i = 0; i < 3; i++) {
        msg.Put(CGLookingForGroup::m_localInfo.m_roles[i]);
    }

    msg.PutString(CGLookingForGroup::m_localInfo.m_comment);
    msg.Finalize();
    CGLookingForGroup::m_localInfo.m_joined = 1;
    CGLookingForGroup::m_localInfo.unk_0003 = 0;
    ClientServices::Send2(&msg);
}
