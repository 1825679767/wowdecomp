#ifndef WOW_SOURCE_UI_GMTICKETINFO_H
#define WOW_SOURCE_UI_GMTICKETINFO_H

#include <cstddef>
#include <cstdint>

struct lua_State;

struct StaticConstant {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_value;
};

class CGGMTicketInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_GetGMTicket(lua_State* L);
int32_t Script_NewGMTicket(lua_State* L);
int32_t Script_UpdateGMTicket(lua_State* L);
int32_t Script_DeleteGMTicket(lua_State* L);
int32_t Script_GMResponseNeedMoreHelp(lua_State* L);
int32_t Script_GMResponseResolve(lua_State* L);
int32_t Script_GetGMStatus(lua_State* L);
int32_t Script_GMSurveyQuestion(lua_State* L);
int32_t Script_GMSurveyNumAnswers(lua_State* L);
int32_t Script_GMSurveyAnswer(lua_State* L);
int32_t Script_GMSurveyAnswerSubmit(lua_State* L);
int32_t Script_GMSurveyCommentSubmit(lua_State* L);
int32_t Script_GMSurveySubmit(lua_State* L);
int32_t Script_GMReportLag(lua_State* L);
void GMTicketInfoRegisterScriptFunctions();

#endif
