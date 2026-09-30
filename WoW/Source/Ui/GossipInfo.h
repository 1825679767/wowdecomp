#ifndef WOW_SOURCE_UI_GOSSIPINFO_H
#define WOW_SOURCE_UI_GOSSIPINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct GossipOption {
    /* 0000 */ char* m_text;
    /* 0004 */ char* unk_0004;
    /* 0008 */ int32_t m_index;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t m_type;
};

class CGGossipInfo {
    public:
    static uint32_t unk_C016F8;
    static void SetGossip(const WOWGUID& guid);
    static GossipOption m_options[32];
    static char m_gossipText[2048];
    static void InitializeGame();
    static WOWGUID m_gossip;
    static uint32_t m_gossipOptionsCount;
};

int32_t Script_GetNumGossipAvailableQuests(lua_State* L);
int32_t Script_GetNumGossipActiveQuests(lua_State* L);
int32_t Script_GetGossipAvailableQuests(lua_State* L);
int32_t Script_GetGossipActiveQuests(lua_State* L);
int32_t Script_SelectGossipOption(lua_State* L);
int32_t Script_SelectGossipAvailableQuest(lua_State* L);
int32_t Script_SelectGossipActiveQuest(lua_State* L);
int32_t Script_CloseGossip(lua_State* L);
void GossipInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_GOSSIP_MESSAGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_POI(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
