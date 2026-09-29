#ifndef WOW_SOURCE_UI_DUELINFO_H
#define WOW_SOURCE_UI_DUELINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGDuelInfo {
    public:
    static WOWGUID m_dueler;

    static void InitializeGame();
};

int32_t Script_StartDuel(lua_State* L);
int32_t Script_AcceptDuel(lua_State* L);
int32_t Script_CancelDuel(lua_State* L);
void DuelInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_DUEL_REQUESTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DUEL_OUTOFBOUNDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DUEL_INBOUNDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DUEL_COUNTDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DUEL_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DUEL_WINNER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
