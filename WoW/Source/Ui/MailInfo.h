#ifndef WOW_SOURCE_UI_MAILINFO_H
#define WOW_SOURCE_UI_MAILINFO_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

struct MailSenderInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ int32_t m_entry;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ float unk_0014;
};

struct StationeryInfo {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_isDefault;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
};

struct MailboxMessageInfo {
    /* 0000 */ int32_t m_messageID;
    /* 0004 */ int32_t m_messageType;
    /* 0008 */ WOWGUID m_sender;
    /* 0010 */ int32_t m_senderEntry;
    /* 0014 */ char m_subject[256];
    /* 0114 */ char m_body[8000];
    /* 2054 */ int32_t unk_2054;
    /* 2058 */ int32_t m_stationery;
    /* 205C */ int32_t m_money;
    /* 2060 */ int32_t m_cod;
    /* 2064 */ uint32_t m_flags;
    /* 2068 */ float m_expireTime;
    /* 206C */ char unk_206C[256];
    /* 216C */ int32_t unk_216C;
    /* 2170 */ int32_t unk_2170;
    /* 2174 */ int32_t unk_2174;
    /* 2178 */ int32_t m_mailTemplateID;
    /* 217C */ int32_t unk_217C;
    /* 2180 */ uint32_t m_attachments[16][31];
    /* 2940 */ uint32_t unk_2940[2];
};

struct SendMailItem {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008[4];
};

class CGMailInfo {
    public:
    static SendMailItem m_sendItems[16];
    static uint8_t m_lockSendItems;
    static uint32_t unk_BEB1C4;
    static void ClearSendMail(bool signal);
    static void CloseMail(bool leavingWorld);
    static uint8_t m_sendMailShowing;
    static TSFixedArray<StationeryInfo*> m_stationeries;
    static TSGrowableArray<MailboxMessageInfo*> m_inbox;
    static void EnterWorld();
    static void InitializeGame();
    static void UpdatePendingMail(float elapsed);
    static uint32_t m_sendMoney;
    static uint32_t m_sendCOD;
    static float m_nextMailTime;
    static uint32_t m_numInboxItems;
    static uint32_t m_totalInboxItems;
    static int32_t m_selectedStationery;
    static int32_t m_selectedPackage;
    static WOWGUID m_object;
    static uint32_t m_nextUpdateTime;
    static int32_t m_commandPending;
};

int32_t Script_CloseMail(lua_State* L);
int32_t Script_ClearSendMail(lua_State* L);
int32_t Script_ClickSendMailItemButton(lua_State* L);
int32_t Script_SetSendMailMoney(lua_State* L);
int32_t Script_SetSendMailCOD(lua_State* L);
int32_t Script_GetNumStationeries(lua_State* L);
int32_t Script_GetSendMailItem(lua_State* L);
int32_t Script_GetSendMailItemLink(lua_State* L);
int32_t Script_GetSendMailPrice(lua_State* L);
int32_t Script_SendMail(lua_State* L);
int32_t Script_GetInboxHeaderInfo(lua_State* L);
int32_t Script_GetInboxText(lua_State* L);
int32_t Script_GetInboxInvoiceInfo(lua_State* L);
int32_t Script_GetInboxItem(lua_State* L);
int32_t Script_GetInboxItemLink(lua_State* L);
int32_t Script_TakeInboxItem(lua_State* L);
int32_t Script_ReturnInboxItem(lua_State* L);
int32_t Script_DeleteInboxItem(lua_State* L);
int32_t Script_InboxItemCanDelete(lua_State* L);
int32_t Script_ComplainInboxItem(lua_State* L);
int32_t Script_CanComplainInboxItem(lua_State* L);
int32_t Script_GetLatestThreeSenders(lua_State* L);
int32_t Script_AutoLootMailItem(lua_State* L);
int32_t Script_RespondMailLockSendItem(lua_State* L);
void MailInfoRegisterScriptFunctions();

int32_t Packet_SMSG_SEND_MAIL_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_MAIL_LIST_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_QUERY_NEXT_MAIL_TIME(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RECEIVED_MAIL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SHOW_MAILBOX(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
