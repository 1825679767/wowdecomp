#include "GMTicketInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00ACFAC0
static StaticConstant s_staticConstants[7] = {
    { "STATIC_CONSTANTS", 0 },
    { "Loot", 1 },
    { "AuctionHouse", 2 },
    { "Mail", 3 },
    { "Chat", 4 },
    { "Movement", 5 },
    { "Spell", 6 },
};

// VERIFY: 030dc6aa auto
// OFFSET: 0x005AC320
static int32_t Script_RegisterStaticConstants(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: RegisterStaticConstants(table)");
        return 0;
    }

    for (uint32_t i = 0; i < sizeof(s_staticConstants) / sizeof(s_staticConstants[0]); i++) {
        lua_pushstring(L, s_staticConstants[i].m_name);
        lua_pushnumber(L, s_staticConstants[i].m_value);
        lua_settable(L, 1);
    }

    return 0;
}

// VERIFY: 05686244 manual
// OFFSET: 0x005AD1C0
static int32_t Script_GetGMStatus(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_GET_SYSTEM_STATUS));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: bddd0a67 auto
// OFFSET: 0x005AD070
static int32_t Script_GetGMTicket(lua_State* L) {
    SendGetTicketOpcode();
    return 0;
}

// STATIC: 0x00ACFAF8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetGMTicket", &Script_GetGMTicket },
    { "NewGMTicket", &Script_NewGMTicket },
    { "UpdateGMTicket", &Script_UpdateGMTicket },
    { "DeleteGMTicket", &Script_DeleteGMTicket },
    { "GMResponseNeedMoreHelp", &Script_GMResponseNeedMoreHelp },
    { "GMResponseResolve", &Script_GMResponseResolve },
    { "GetGMStatus", &Script_GetGMStatus },
    { "GMSurveyQuestion", &Script_GMSurveyQuestion },
    { "GMSurveyNumAnswers", &Script_GMSurveyNumAnswers },
    { "GMSurveyAnswer", &Script_GMSurveyAnswer },
    { "GMSurveyAnswerSubmit", &Script_GMSurveyAnswerSubmit },
    { "GMSurveyCommentSubmit", &Script_GMSurveyCommentSubmit },
    { "GMSurveySubmit", &Script_GMSurveySubmit },
    { "GMReportLag", &Script_GMReportLag },
    { "RegisterStaticConstants", &Script_RegisterStaticConstants },
};

// VERIFY: 030f1972 auto
// OFFSET: 0x005AC4D0
void GMTicketInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
