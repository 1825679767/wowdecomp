#include "GuildBankFrame.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"

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
