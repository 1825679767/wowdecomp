#ifndef WOW_SOURCE_UI_GOSSIPINFO_H
#define WOW_SOURCE_UI_GOSSIPINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

struct lua_State;

class CGGossipInfo {
    public:
    static void InitializeGame();
};

int32_t Script_GetGossipText(lua_State* L);
int32_t Script_GetNumGossipOptions(lua_State* L);
int32_t Script_GetNumGossipAvailableQuests(lua_State* L);
int32_t Script_GetNumGossipActiveQuests(lua_State* L);
int32_t Script_GetGossipOptions(lua_State* L);
int32_t Script_GetGossipAvailableQuests(lua_State* L);
int32_t Script_GetGossipActiveQuests(lua_State* L);
int32_t Script_SelectGossipOption(lua_State* L);
int32_t Script_SelectGossipAvailableQuest(lua_State* L);
int32_t Script_SelectGossipActiveQuest(lua_State* L);
int32_t Script_CloseGossip(lua_State* L);
int32_t Script_ForceGossip(lua_State* L);
void GossipInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_GOSSIP_MESSAGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_POI(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
