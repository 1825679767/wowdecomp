#ifndef WOW_SOURCE_UI_GMTICKETINFO_H
#define WOW_SOURCE_UI_GMTICKETINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

int32_t Packet_Group_5(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

struct StaticConstant {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_value;
};

class CGGMTicketInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static int32_t m_ticketID;
    static int32_t m_responseID;
};

int32_t Script_NewGMTicket(lua_State* L);
int32_t Script_UpdateGMTicket(lua_State* L);
int32_t Script_DeleteGMTicket(lua_State* L);
int32_t Script_GMResponseNeedMoreHelp(lua_State* L);
int32_t Script_GMResponseResolve(lua_State* L);
int32_t Script_GMSurveyQuestion(lua_State* L);
int32_t Script_GMSurveyNumAnswers(lua_State* L);
int32_t Script_GMSurveyAnswer(lua_State* L);
int32_t Script_GMSurveyAnswerSubmit(lua_State* L);
int32_t Script_GMSurveyCommentSubmit(lua_State* L);
int32_t Script_GMSurveySubmit(lua_State* L);
int32_t Script_GMReportLag(lua_State* L);
void GMTicketInfoRegisterScriptFunctions();

void SendGetTicketOpcode();

#endif
