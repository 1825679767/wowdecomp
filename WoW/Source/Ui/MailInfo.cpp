#include "MailInfo.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BEB038
SendMailItem CGMailInfo::m_sendItems[16];

// STATIC: 0x00BEB1B8
uint8_t CGMailInfo::m_lockSendItems;

// STATIC: 0x00BEB1C4
uint32_t CGMailInfo::unk_BEB1C4;

// STATIC: 0x00BEAFE8
static MailSenderInfo s_recentSenders[3];

// STATIC: 0x00BEB1BC
uint32_t CGMailInfo::m_sendMoney;

// STATIC: 0x00BEB1C0
uint32_t CGMailInfo::m_sendCOD;

// STATIC: 0x00ACE60C
float CGMailInfo::m_nextMailTime = -1.0f;

// STATIC: 0x00BEB1D8
uint32_t CGMailInfo::m_numInboxItems;

// STATIC: 0x00BEB1DC
uint32_t CGMailInfo::m_totalInboxItems;

// STATIC: 0x00BEB1CC
int32_t CGMailInfo::m_selectedStationery;

// STATIC: 0x00BEB1D0
int32_t CGMailInfo::m_selectedPackage;

// STATIC: 0x00BEB030
WOWGUID CGMailInfo::m_object;

// STATIC: 0x00BEB1E0
uint32_t CGMailInfo::m_nextUpdateTime;

// STATIC: 0x00BEB1E4
int32_t CGMailInfo::m_commandPending;

// STATIC: 0x00BEB1B9
uint8_t CGMailInfo::m_sendMailShowing;

// STATIC: 0x00BEB1F0
TSFixedArray<StationeryInfo*> CGMailInfo::m_stationeries;

// STATIC: 0x00BEB1FC
TSGrowableArray<MailboxMessageInfo*> CGMailInfo::m_inbox;

// VERIFY: 4a1994f4 manual
// OFFSET: 0x00572210
void CGMailInfo::InitializeGame() {
    uint32_t numStationeries = g_stationeryDB.m_numRecords;
    CGMailInfo::m_stationeries.SetCount(numStationeries);

    for (int32_t i = 0; i < static_cast<int32_t>(numStationeries); i++) {
        StationeryRec* rec = i < 0 || i >= g_stationeryDB.m_numRecords ? nullptr : &g_stationeryDB.m_records[i];

        CGMailInfo::m_stationeries.m_data[i] = static_cast<StationeryInfo*>(SMemAlloc(sizeof(StationeryInfo), __FILE__, __LINE__, 0));
        CGMailInfo::m_stationeries.m_data[i]->m_id = rec->m_id;
        CGMailInfo::m_stationeries.m_data[i]->m_itemID = rec->m_itemID;
        CGMailInfo::m_stationeries.m_data[i]->m_isDefault = rec->m_flags & 1;
        CGMailInfo::m_stationeries.m_data[i]->unk_0010 = 0;
        CGMailInfo::m_stationeries.m_data[i]->unk_0014 = 0;
    }

    CGMailInfo::m_inbox.SetCount(8);

    for (uint32_t i = 0; i < 8; i++) {
        CGMailInfo::m_inbox.m_data[i] = static_cast<MailboxMessageInfo*>(SMemAlloc(sizeof(MailboxMessageInfo), __FILE__, __LINE__, 0));
    }

    memset(s_recentSenders, 0, sizeof(s_recentSenders));

    ClientServices::SetMessageHandler(SMSG_MAIL_COMMAND_RESULT, Packet_SMSG_SEND_MAIL_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_MAIL_LIST_RESULT, Packet_SMSG_MAIL_LIST_RESULT, nullptr);
    ClientServices::SetMessageHandler(MSG_QUERY_NEXT_MAIL_TIME, Packet_MSG_QUERY_NEXT_MAIL_TIME, nullptr);
    ClientServices::SetMessageHandler(SMSG_RECEIVED_MAIL, Packet_SMSG_RECEIVED_MAIL, nullptr);
    ClientServices::SetMessageHandler(SMSG_SHOW_MAILBOX, Packet_SMSG_SHOW_MAILBOX, nullptr);
}
