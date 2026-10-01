#include "LFGInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00A394B0
static const uint8_t s_classRoles[11] = { 0x0B, 0x0F, 0x09, 0x09, 0x0D, 0x0B, 0x0D, 0x09, 0x09, 0x00, 0x0F };

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
