#ifndef WOW_SOURCE_UI_TUTORIAL_H
#define WOW_SOURCE_UI_TUTORIAL_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

class CGTutorial {
    public:
    static void InitializeGame();
    static TSBitField<TSFixedArray<uint32_t> > m_tutorialShown;
    static TSBitField<TSFixedArray<uint32_t> > m_tutorialFlags;

    static void FlagTutorial(uint32_t tutorial);
    static void TriggerTutorial(uint32_t tutorial);
    static int32_t OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
};

int32_t Script_CanResetTutorials(lua_State* L);
int32_t Script_FlagTutorial(lua_State* L);
int32_t Script_IsTutorialFlagged(lua_State* L);
int32_t Script_TriggerTutorial(lua_State* L);
int32_t Script_ClearTutorials(lua_State* L);
int32_t Script_ResetTutorials(lua_State* L);
int32_t Script_GetNextCompleatedTutorial(lua_State* L);
int32_t Script_GetPrevCompleatedTutorial(lua_State* L);
void TutorialRegisterScriptFunctions();

#endif
