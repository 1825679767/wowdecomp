#ifndef WOW_SOURCE_UI_MAILINFO_H
#define WOW_SOURCE_UI_MAILINFO_H

#include <cstdint>

struct lua_State;

class CGMailInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static void UpdatePendingMail(float elapsed);
};

int32_t Script_CloseMail(lua_State* L);
int32_t Script_ClearSendMail(lua_State* L);
int32_t Script_ClickSendMailItemButton(lua_State* L);
int32_t Script_SetSendMailMoney(lua_State* L);
int32_t Script_GetSendMailMoney(lua_State* L);
int32_t Script_SetSendMailCOD(lua_State* L);
int32_t Script_GetSendMailCOD(lua_State* L);
int32_t Script_GetNumStationeries(lua_State* L);
int32_t Script_GetStationeryInfo(lua_State* L);
int32_t Script_SelectStationery(lua_State* L);
int32_t Script_GetSelectedStationeryTexture(lua_State* L);
int32_t Script_GetNumPackages(lua_State* L);
int32_t Script_GetPackageInfo(lua_State* L);
int32_t Script_SelectPackage(lua_State* L);
int32_t Script_GetSendMailItem(lua_State* L);
int32_t Script_GetSendMailItemLink(lua_State* L);
int32_t Script_GetSendMailPrice(lua_State* L);
int32_t Script_SendMail(lua_State* L);
int32_t Script_CheckInbox(lua_State* L);
int32_t Script_GetInboxNumItems(lua_State* L);
int32_t Script_GetInboxHeaderInfo(lua_State* L);
int32_t Script_GetInboxText(lua_State* L);
int32_t Script_GetInboxInvoiceInfo(lua_State* L);
int32_t Script_GetInboxItem(lua_State* L);
int32_t Script_GetInboxItemLink(lua_State* L);
int32_t Script_TakeInboxMoney(lua_State* L);
int32_t Script_TakeInboxItem(lua_State* L);
int32_t Script_TakeInboxTextItem(lua_State* L);
int32_t Script_ReturnInboxItem(lua_State* L);
int32_t Script_DeleteInboxItem(lua_State* L);
int32_t Script_InboxItemCanDelete(lua_State* L);
int32_t Script_HasNewMail(lua_State* L);
int32_t Script_ComplainInboxItem(lua_State* L);
int32_t Script_CanComplainInboxItem(lua_State* L);
int32_t Script_GetLatestThreeSenders(lua_State* L);
int32_t Script_SetSendMailShowing(lua_State* L);
int32_t Script_AutoLootMailItem(lua_State* L);
int32_t Script_RespondMailLockSendItem(lua_State* L);
void MailInfoRegisterScriptFunctions();

#endif
