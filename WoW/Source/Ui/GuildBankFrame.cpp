#include "GuildBankFrame.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

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
