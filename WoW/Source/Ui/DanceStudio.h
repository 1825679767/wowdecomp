#ifndef WOW_SOURCE_UI_DANCESTUDIO_H
#define WOW_SOURCE_UI_DANCESTUDIO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

int32_t Packet_SMSG_PLAY_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_STOP_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_NOTIFY_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_LEARNED_DANCE_MOVES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGDanceStudio {
    public:
    static void InitializeGame();
    static void RegisterScriptFunctions();
};

int32_t Script_PlayDance(lua_State* L);

#endif
