#include "GuildBankFrame.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// STATIC: 0x00C1DC10
WOWGUID CGGuildBankInfo::m_banker;

// STATIC: 0x00C1DC18
WOWGUID CGGuildBankInfo::m_pendingBanker;

// STATIC: 0x00C1DC08
int32_t CGGuildBankInfo::m_numMoneyTransactions;

// STATIC: 0x00C11458
GuildBankTabInfo CGGuildBankInfo::m_guildBankTabInfo[6];

// STATIC: 0x00C1DC0C
uint8_t CGGuildBankInfo::m_numTabs;

// STATIC: 0x00C1DC0D
uint8_t CGGuildBankInfo::m_currentTab;

// STATIC: 0x00C1DC28
static uint32_t s_pendingItemQueries;

// STATIC: 0x00C1DC30
uint32_t CGGuildBankInfo::unk_C1DC30;

// STATIC: 0x00C1DC34
int32_t CGGuildBankInfo::m_remainingWithdrawMoney;

// STATIC: 0x00C1DC20
uint64_t CGGuildBankInfo::m_money;

// STATIC: 0x00C1DBF0
int32_t CGGuildBankInfo::m_numLogEntries[6];

// STATIC: 0x00ACF8C8
uint8_t CGGuildBankInfo::unk_ACF8C8 = 0xFF;

// STATIC: 0x00C0F8E8
int32_t CGGuildBankInfo::unk_C0F8E8[6];

// STATIC: 0x00C1DC38
TSGrowableArray<GuildBankSlotInfo> CGGuildBankInfo::m_tabSlots[6];

// VERIFY: a63db2b3 auto
// OFFSET: 0x005A77D0
void CGGuildBankInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GUILD_BANK_QUERY_RESULTS, Packet_SMSG_GUILD_BANK_LIST, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_BANK_LOG_QUERY, Packet_MSG_GUILD_BANK_LOG_QUERY, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_BANK_MONEY_WITHDRAWN, Packet_MSG_GUILD_BANK_MONEY_WITHDRAWN, nullptr);
    ClientServices::SetMessageHandler(MSG_QUERY_GUILD_BANK_TEXT, Packet_MSG_QUERY_GUILD_BANK_TEXT, nullptr);
    CGGuildBankInfo::m_numTabs = 0;
    memset(CGGuildBankInfo::m_guildBankTabInfo, 0, sizeof(CGGuildBankInfo::m_guildBankTabInfo));
}

// VERIFY: a782ac08 auto
// OFFSET: 0x005A4270
void CGGuildBankInfo::SetGuildBanker(const WOWGUID& banker) {
    if (banker) {
        CGGameUI::SetInteractTarget(banker);
        CGGuildBankInfo::m_banker = banker;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_OPENED, nullptr);
        return;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_CLOSED, nullptr);
    CGGameUI::ClearInteractTarget(CGGuildBankInfo::m_banker, 0);
    CGGuildBankInfo::m_banker = WOWGUID();
    CGGuildBankInfo::m_pendingBanker = WOWGUID();
}

// VERIFY: 9ecc3e00 manual
// OFFSET: 0x005A56F0
void CGGuildBankInfo::EnterWorld() {
    for (int32_t i = 0; i < 6; i++) {
        CGGuildBankInfo::m_numLogEntries[i] = 0;
    }

    CGGuildBankInfo::m_numMoneyTransactions = 0;
    CGGuildBankInfo::m_numTabs = 0;
    CGGuildBankInfo::m_banker = WOWGUID{ 0 };
    CGGuildBankInfo::m_pendingBanker = WOWGUID{ 0 };
    CGGuildBankInfo::unk_ACF8C8 = 0xFF;

    for (int32_t tab = 0; tab < 6; tab++) {
        for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[tab].m_count; i++) {
            CGGuildBankInfo::m_tabSlots[tab].m_data[i].m_itemID = 0;
        }

        CGGuildBankInfo::m_guildBankTabInfo[tab].unk_2140 = 1;
        CGGuildBankInfo::unk_C0F8E8[tab] = 1;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_GUILD_BANK_MONEY_WITHDRAWN));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: be4957f6 auto
// OFFSET: 0x005A42E0
void CGGuildBankInfo::GetTabAndSlotFromVirtual(uint32_t virtualSlot, uint32_t* tab, uint32_t* slot) {
    *tab = virtualSlot / 98;
    *slot = virtualSlot % 98;
}

// VERIFY: 0087723a manual
// OFFSET: 0x005A4C10
GuildBankSlotInfo* CGGuildBankInfo::GetGuildBankItem(uint32_t tab, uint32_t slot) {
    if (tab >= 6 || slot >= CGGuildBankInfo::m_tabSlots[tab].m_count) {
        return nullptr;
    }

    GuildBankSlotInfo* info = &CGGuildBankInfo::m_tabSlots[tab].m_data[slot];
    return info->m_itemID ? info : nullptr;
}

// VERIFY: 95bfeaf2 auto
// OFFSET: 0x005A4CE0
void CGGuildBankInfo::Unlock(int32_t itemId, uint32_t virtualSlot) {
    uint32_t tab = virtualSlot / 98;
    uint32_t slot = virtualSlot % 98;

    if (tab < 6 && slot < CGGuildBankInfo::m_tabSlots[tab].m_count) {
        GuildBankSlotInfo* info = &CGGuildBankInfo::m_tabSlots[tab].m_data[slot];

        if (itemId == info->m_itemID) {
            info->m_locked = 0;
        }
    }
}

// VERIFY: 19c3efd7 auto
// OFFSET: 0x005A7760
void CGGuildBankInfo::ClearAllTabs() {
    for (uint32_t i = 0; i < 6; i++) {
        CGGuildBankInfo::m_tabSlots[i].Clear();
        CGGuildBankInfo::m_guildBankTabInfo[i].unk_2140 = 1;
        CGGuildBankInfo::unk_C0F8E8[i] = 1;
    }

    CGGuildBankInfo::m_numTabs = 0;
    CGGuildBankInfo::m_currentTab = 0;
}

// VERIFY: 23cd4443 auto
// OFFSET: 0x005A7830
void CGGuildBankInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_GUILD_BANK_QUERY_RESULTS);
    ClientServices::ClearMessageHandler(MSG_GUILD_BANK_LOG_QUERY);
    ClientServices::ClearMessageHandler(MSG_GUILD_BANK_MONEY_WITHDRAWN);
    ClientServices::ClearMessageHandler(MSG_QUERY_GUILD_BANK_TEXT);
    CGGuildBankInfo::ClearAllTabs();
}

// VERIFY: fa9b79e8 auto
// OFFSET: 0x005A4AB0
int32_t Packet_MSG_GUILD_BANK_MONEY_WITHDRAWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(CGGuildBankInfo::m_remainingWithdrawMoney));
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_WITHDRAWMONEY, nullptr);

    return 1;
}

// VERIFY: fcedae56 auto
// OFFSET: 0x005A47A0
static void Signal_EVENT_GUILDBANKBAGSLOTS_CHANGED(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!s_pendingItemQueries || !--s_pendingItemQueries) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, nullptr);
    }
}

// VERIFY: 03f83d73 manual
// OFFSET: 0x005A4B60
void CGGuildBankInfo::ClearGuildTabList(uint32_t tab) {
    if (tab >= 6) {
        return;
    }

    for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[tab].m_count; i++) {
        GuildBankSlotInfo& slot = CGGuildBankInfo::m_tabSlots[tab].m_data[i];
        slot.m_itemID = 0;
        slot.m_randomPropertyID = 0;
        slot.m_suffixFactor = 0;
        slot.m_count = 0;
        slot.unk_0010 = 0;
        slot.unk_0014 = 0;
        slot.m_enchants[0] = 0;
        slot.m_enchants[1] = 0;
        slot.m_enchants[2] = 0;
        slot.m_locked = 0;
        slot.unk_0028 = 0;
    }
}

// VERIFY: 5a5cd21a manual
// OFFSET: 0x005A7250
int32_t Packet_SMSG_GUILD_BANK_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t money;
    uint8_t tab;
    uint8_t hasTabInfo;
    msg->Get(money);
    msg->Get(tab);
    msg->Get(CGGuildBankInfo::unk_C1DC30);
    msg->Get(hasTabInfo);

    if (s_pendingItemQueries) {
        s_pendingItemQueries = 0;

        for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[tab].m_count; i++) {
            g_nameDBCache.CancelCallback(*reinterpret_cast<uint64_t*>(&CGGuildBankInfo::m_tabSlots[tab].m_data[i]), Signal_EVENT_GUILDBANKBAGSLOTS_CHANGED, nullptr);
        }
    }

    if (money >= 0x7FFFFFFFFFFFFull) {
        money = 0x7FFFFFFFFFFFFull;
    }

    CGGuildBankInfo::m_money = money;

    if (!CGGuildBankInfo::m_tabSlots[tab].m_count) {
        CGGuildBankInfo::m_tabSlots[tab].SetCount(98);
        CGGuildBankInfo::ClearGuildTabList(tab);
    }

    if (hasTabInfo && !tab) {
        msg->Get(CGGuildBankInfo::m_numTabs);

        for (uint32_t i = 0; i < CGGuildBankInfo::m_numTabs; i++) {
            char name[64];
            char icon[256];
            msg->GetString(name, sizeof(name));
            msg->GetString(icon, sizeof(icon));
            ScrubString(name, CGGuildBankInfo::m_guildBankTabInfo[i].m_name, 16, sizeof(name), 1);
            SStrCopy(CGGuildBankInfo::m_guildBankTabInfo[i].m_icon, icon, STORM_MAX_STR);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_TABS, nullptr);
        }

        CGGuildBankInfo::ClearGuildTabList(tab);
    }

    uint8_t numSlots;
    msg->Get(numSlots);
    int32_t cursorId = 0;
    int32_t cursorSlot = 0;

    if (CGGameUI::m_cursorItemType == 11) {
        CGGameUI::GetCursorVirtualItem(cursorId, cursorSlot);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    WOWGUID playerGuid = player->m_obj->m_guid;

    for (uint32_t i = 0; i < numSlots; i++) {
        uint8_t slotIndex;
        msg->Get(slotIndex);

        if (cursorId && tab == static_cast<uint32_t>(cursorSlot) / 98 && slotIndex == static_cast<uint32_t>(cursorSlot) % 98) {
            CGGameUI::ClearCursor(1, 1);
            CGGameUI::UnlockVirtualItem(cursorId, cursorSlot, 0);
        }

        msg->Get(reinterpret_cast<uint32_t&>(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_itemID));
        GuildBankSlotInfo* slot = &CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex];

        if (!slot->m_itemID) {
            continue;
        }

        msg->Get(slot->unk_0028);
        msg->Get(reinterpret_cast<uint32_t&>(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_randomPropertyID));
        slot = &CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex];

        if (slot->m_randomPropertyID) {
            msg->Get(slot->m_suffixFactor);
        } else {
            slot->m_suffixFactor = 0;
        }

        msg->Get(reinterpret_cast<uint32_t&>(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_count));
        msg->Get(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].unk_0010);
        msg->Get(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].unk_0014);
        uint8_t numEnchants;
        msg->Get(numEnchants);
        slot = &CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex];
        slot->m_enchants[0] = 0;
        slot->m_enchants[1] = 0;
        slot->m_enchants[2] = 0;

        for (uint32_t j = 0; j < numEnchants; j++) {
            uint8_t enchantSlot;
            msg->Get(enchantSlot);
            msg->Get(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_enchants[enchantSlot]);
        }

        CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_locked = 0;

        if (!g_itemDBCache.GetRecord(CGGuildBankInfo::m_tabSlots[tab].m_data[slotIndex].m_itemID, playerGuid, Signal_EVENT_GUILDBANKBAGSLOTS_CHANGED, nullptr, false)) {
            s_pendingItemQueries++;
        }
    }

    CGGuildBankInfo::m_guildBankTabInfo[tab].unk_2140 = 0;

    if (CGGuildBankInfo::m_banker == WOWGUID()) {
        if (CGGuildBankInfo::m_pendingBanker == WOWGUID()) {
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_CLOSED, nullptr);
            CGGameUI::ClearInteractTarget(CGGuildBankInfo::m_banker, 0);
            CGGuildBankInfo::m_banker = WOWGUID();
            CGGuildBankInfo::m_pendingBanker = WOWGUID();
            return 1;
        }

        CGGameUI::SetInteractTarget(CGGuildBankInfo::m_pendingBanker);
        CGGuildBankInfo::m_banker = CGGuildBankInfo::m_pendingBanker;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_OPENED, nullptr);
    } else if (!s_pendingItemQueries) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, nullptr);
    }

    return 1;
}

// VERIFY: c082d582 auto
// OFFSET: 0x005A4310
void CGGuildBankInfo::SetTextUpdate(uint8_t tab, int32_t update) {
    CGGuildBankInfo::unk_C0F8E8[tab] = update;
}
