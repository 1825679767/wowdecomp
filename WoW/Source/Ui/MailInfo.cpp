#include "MailInfo.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

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

// STATIC: 0x00BEB1C8
uint32_t CGMailInfo::m_numStationeries;

// STATIC: 0x00BEB1D4
uint32_t CGMailInfo::m_pendingStationeryCallbacks;

// STATIC: 0x00BEB1E8
int32_t CGMailInfo::m_doPendingQuery;

// STATIC: 0x00BEB1EC
int32_t CGMailInfo::m_autoLooting;

// STATIC: 0x00BEB1F0
TSFixedArray<StationeryInfo*> CGMailInfo::m_stationeries;

// STATIC: 0x00BEB1FC
TSGrowableArray<MailboxMessageInfo*> CGMailInfo::m_inbox;

// VERIFY: c5353046 manual
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
        CGMailInfo::m_stationeries.m_data[i]->m_itemGUID = WOWGUID{ 0 };
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

// VERIFY: 2350f8eb auto
// OFFSET: 0x0056DED0
void CGMailInfo::UpdatePendingMail(float elapsed) {
    for (int32_t i = 0; i < 3; i++) {
        MailSenderInfo& sender = s_recentSenders[i];

        if ((sender.m_guid || sender.m_entry) && sender.unk_0014 > 0.0f) {
            float remaining = sender.unk_0014 - elapsed;
            sender.unk_0014 = remaining > 0.0f ? remaining : 0.0f;

            if (fabs(sender.unk_0014) < 0.00000023841858f) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, nullptr);
            }
        }
    }

    if (CGMailInfo::m_nextMailTime > 0.0f) {
        float remaining = CGMailInfo::m_nextMailTime - elapsed;
        CGMailInfo::m_nextMailTime = remaining > 0.0f ? remaining : 0.0f;

        if (fabs(CGMailInfo::m_nextMailTime) < 0.00000023841858f) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, nullptr);
        }
    }
}

// VERIFY: 4eaaa3df manual
// OFFSET: 0x0056DD00
static int32_t QSortStationery(const void* a, const void* b) {
    const StationeryInfo* infoA = *static_cast<StationeryInfo* const*>(a);
    const StationeryInfo* infoB = *static_cast<StationeryInfo* const*>(b);

    if (infoA->m_isDefault != infoB->m_isDefault) {
        return 2 * (infoA->m_isDefault == 0) - 1;
    }

    if (infoA->m_itemGUID && !infoB->m_itemGUID) {
        return -1;
    }

    if (infoB->m_itemGUID && !infoA->m_itemGUID) {
        return 1;
    }

    ItemStats_C* statsA = g_itemDBCache.GetRecord(infoA->m_itemID, WOWGUID{ 0 }, nullptr, nullptr, false);
    ItemStats_C* statsB = g_itemDBCache.GetRecord(infoB->m_itemID, WOWGUID{ 0 }, nullptr, nullptr, false);

    if (!statsA) {
        if (!statsB) {
            return 0;
        }

        return 2 * (statsA == nullptr) - 1;
    }

    if (!statsB) {
        return 2 * (statsA == nullptr) - 1;
    }

    if (statsA->m_buyPrice != statsB->m_buyPrice) {
        return 2 * (statsA->m_buyPrice >= statsB->m_buyPrice) - 1;
    }

    return SStrSortUTF8I(statsA->GetName(0), statsB->GetName(0), STORM_MAX_STR);
}

// VERIFY: 77c88115 manual
// OFFSET: 0x0056FC80
void CGMailInfo::UpdateStationeryList() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGMailInfo::m_numStationeries = 0;

    for (uint32_t i = 0; i < CGMailInfo::m_stationeries.m_count; i++) {
        StationeryRec* rec = g_stationeryDB.GetRecord(CGMailInfo::m_stationeries.m_data[i]->m_id);
        CGItem_C* item = player->m_bag.FindItemOfType(rec->m_itemID, 0);
        CGMailInfo::m_stationeries.m_data[i]->m_itemGUID = item ? item->m_obj->m_guid : WOWGUID{ 0 };

        if (CGMailInfo::m_stationeries.m_data[i]->m_isDefault || CGMailInfo::m_stationeries.m_data[i]->m_itemGUID) {
            if (g_itemDBCache.GetRecord(rec->m_itemID, WOWGUID{ 0 }, nullptr, nullptr, false)) {
                CGMailInfo::m_numStationeries++;
            }
        }
    }

    qsort(CGMailInfo::m_stationeries.m_data, CGMailInfo::m_stationeries.m_count, sizeof(StationeryInfo*), QSortStationery);
}

// VERIFY: 4741646d manual
// OFFSET: 0x0056FF70
void CGMailInfo::QueryNextMailTime() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_QUERY_NEXT_MAIL_TIME));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGMailInfo::m_nextMailTime = -1.0f;
    CGMailInfo::m_doPendingQuery = 0;
}

// VERIFY: a66c7d69 auto
// OFFSET: 0x00571520
void StationeryItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!--CGMailInfo::m_pendingStationeryCallbacks) {
        CGMailInfo::UpdateStationeryList();
    }
}

// VERIFY: a800705f auto
// OFFSET: 0x00571530
void CGMailInfo::EnterWorld() {
    CGMailInfo::m_object = WOWGUID{ 0 };
    memset(CGMailInfo::m_sendItems, 0, sizeof(CGMailInfo::m_sendItems));
    CGMailInfo::m_nextMailTime = -1.0f;
    CGMailInfo::m_sendMoney = 0;
    CGMailInfo::m_lockSendItems = 0;
    CGMailInfo::unk_BEB1C4 = 0;
    CGMailInfo::m_numStationeries = 0;
    CGMailInfo::m_selectedStationery = 0;
    CGMailInfo::m_selectedPackage = 0;
    CGMailInfo::m_pendingStationeryCallbacks = 0;
    CGMailInfo::m_numInboxItems = 0;
    CGMailInfo::m_totalInboxItems = 0;
    CGMailInfo::m_nextUpdateTime = 0;
    CGMailInfo::m_commandPending = 0;
    CGMailInfo::m_sendMailShowing = 0;

    for (uint32_t i = 0; i < CGMailInfo::m_stationeries.m_count; i++) {
        if (!g_itemDBCache.GetRecord(CGMailInfo::m_stationeries.m_data[i]->m_itemID, WOWGUID{ 0 }, StationeryItemCallback, nullptr, false)) {
            CGMailInfo::m_pendingStationeryCallbacks++;
        }
    }

    if (!CGMailInfo::m_pendingStationeryCallbacks) {
        CGMailInfo::UpdateStationeryList();
    }

    memset(s_recentSenders, 0, sizeof(s_recentSenders));
    CGMailInfo::QueryNextMailTime();
    CGMailInfo::m_autoLooting = 0;
}

// VERIFY: f68c3d1a auto
// OFFSET: 0x0056D3C0
uint32_t CGMailInfo::GetNumSendItems() {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; i++) {
        if (CGMailInfo::m_sendItems[i].m_guid) {
            count++;
        }
    }

    return count;
}

// VERIFY: b8f834cd manual
// OFFSET: 0x00753A10
bool CGMailInfo::IsSendItem(WOWGUID item) {
    if (!item) {
        return false;
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (CGMailInfo::m_sendItems[i].m_guid == item) {
            return true;
        }
    }

    return false;
}
