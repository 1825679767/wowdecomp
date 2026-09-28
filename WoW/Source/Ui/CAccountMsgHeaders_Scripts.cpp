#include "CAccountMsgHeaders.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_AccountMsg_LoadHeaders(lua_State* L);
int32_t Script_AccountMsg_GetNumTotalMsgs(lua_State* L);
int32_t Script_AccountMsg_GetNumUnreadMsgs(lua_State* L);
int32_t Script_AccountMsg_GetNumUnreadUrgentMsgs(lua_State* L);
int32_t Script_AccountMsg_GetIndexHighestPriorityUnreadMsg(lua_State* L);
int32_t Script_AccountMsg_GetIndexNextUnreadMsg(lua_State* L);
int32_t Script_AccountMsg_GetHeaderSubject(lua_State* L);
int32_t Script_AccountMsg_GetHeaderPriority(lua_State* L);
int32_t Script_AccountMsg_LoadBody(lua_State* L);
int32_t Script_AccountMsg_GetBody(lua_State* L);
int32_t Script_AccountMsg_SetMsgRead(lua_State* L);

// STATIC: 0x00ACD418
static FrameScript_Method s_scriptFunctions[] = {
    { "AccountMsg_LoadHeaders", &Script_AccountMsg_LoadHeaders },
    { "AccountMsg_GetNumTotalMsgs", &Script_AccountMsg_GetNumTotalMsgs },
    { "AccountMsg_GetNumUnreadMsgs", &Script_AccountMsg_GetNumUnreadMsgs },
    { "AccountMsg_GetNumUnreadUrgentMsgs", &Script_AccountMsg_GetNumUnreadUrgentMsgs },
    { "AccountMsg_GetIndexHighestPriorityUnreadMsg", &Script_AccountMsg_GetIndexHighestPriorityUnreadMsg },
    { "AccountMsg_GetIndexNextUnreadMsg", &Script_AccountMsg_GetIndexNextUnreadMsg },
    { "AccountMsg_GetHeaderSubject", &Script_AccountMsg_GetHeaderSubject },
    { "AccountMsg_GetHeaderPriority", &Script_AccountMsg_GetHeaderPriority },
    { "AccountMsg_LoadBody", &Script_AccountMsg_LoadBody },
    { "AccountMsg_GetBody", &Script_AccountMsg_GetBody },
    { "AccountMsg_SetMsgRead", &Script_AccountMsg_SetMsgRead },
};

// VERIFY: be07ae82 auto
// OFFSET: 0x00550080
void AccountMsg_RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// VERIFY: 9920deda auto
// OFFSET: 0x005500B0
void AccountMsg_UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
