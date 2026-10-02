#ifndef WOW_SOURCE_UI_CGMINIGAMEINFO_H
#define WOW_SOURCE_UI_CGMINIGAMEINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

struct lua_State;

class CGMinigameInfo {
    public:
    static void ShutdownGame();
    static void InitializeGame();
};

int32_t Script_GetMinigameType(lua_State* L);
int32_t Script_MakeMinigameMove(lua_State* L);
int32_t Script_GetMinigameState(lua_State* L);
void MinigameInfoRegisterScriptFunctions();
void MinigameInfoUnregisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_MINIGAME_SETUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_MINIGAME_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
