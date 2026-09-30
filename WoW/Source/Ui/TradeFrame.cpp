#include "TradeFrame.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00BFA658
WOWGUID CGTradeInfo::m_tradingPlayer;

// STATIC: 0x00BFA400
int32_t CGTradeInfo::m_targetDurability[7];

// STATIC: 0x00BFA41C
int32_t CGTradeInfo::m_targetMaxDurability[7];

// STATIC: 0x00BFA438
int32_t CGTradeInfo::m_targetLockID[7];

// STATIC: 0x00BFA454
int32_t CGTradeInfo::m_targetRandomPropertyID[7];

// STATIC: 0x00BFA470
int32_t CGTradeInfo::m_targetSuffixFactor[7];

// STATIC: 0x00BFA48C
int32_t CGTradeInfo::m_targetSpellCharges[7];

// STATIC: 0x00BFA4A8
WOWGUID CGTradeInfo::m_targetGiftCreator[7];

// STATIC: 0x00BFA4E0
int32_t CGTradeInfo::m_targetWrapped[7];

// STATIC: 0x00BFA4FC
int32_t CGTradeInfo::m_targetEnchant[7];

// STATIC: 0x00BFA518
WOWGUID CGTradeInfo::m_targetCreator[7];

// STATIC: 0x00BFA550
int32_t CGTradeInfo::m_targetSocketEnchants[7][3];

// STATIC: 0x00BFA5A4
int32_t CGTradeInfo::m_targetCount[7];

// STATIC: 0x00BFA5C0
int32_t CGTradeInfo::m_targetItemID[7];

// STATIC: 0x00BFA5DC
uint8_t CGTradeInfo::unk_BFA5DC[7];

// STATIC: 0x00BFA5E8
WOWGUID CGTradeInfo::unk_BFA5E8[7];

// STATIC: 0x00BFA620
WOWGUID CGTradeInfo::m_playerItems[7];

// STATIC: 0x00BFA660
int32_t CGTradeInfo::m_playerAccepted;

// STATIC: 0x00BFA664
int32_t CGTradeInfo::m_targetAccepted;

// STATIC: 0x00BFA668
int32_t CGTradeInfo::unk_BFA668;

// STATIC: 0x00BFA66C
int32_t CGTradeInfo::unk_BFA66C;

// STATIC: 0x00BFA670
int32_t CGTradeInfo::unk_BFA670;

// STATIC: 0x00BFA674
int32_t CGTradeInfo::unk_BFA674;

// STATIC: 0x00CA0B68
PlayerTradeState CGTradeInfo::m_playerTrade;

// VERIFY: ffaeea99 auto
// OFFSET: 0x005860A0
void CGTradeInfo::EnterWorld() {
    CGTradeInfo::m_tradingPlayer = WOWGUID();
    CGTradeInfo::m_playerAccepted = 0;
    CGTradeInfo::m_targetAccepted = 0;
    CGTradeInfo::unk_BFA668 = 0;
    CGTradeInfo::unk_BFA66C = 0;
    CGTradeInfo::unk_BFA670 = 0;
    CGTradeInfo::unk_BFA674 = 0;

    for (uint32_t i = 0; i < 7; i++) {
        CGTradeInfo::m_targetDurability[i] = 0;
        CGTradeInfo::m_targetMaxDurability[i] = 0;
        CGTradeInfo::m_targetRandomPropertyID[i] = 0;
        CGTradeInfo::m_targetSuffixFactor[i] = 0;
        CGTradeInfo::m_targetSpellCharges[i] = 0;
        CGTradeInfo::m_targetLockID[i] = 0;
        CGTradeInfo::m_targetWrapped[i] = 0;
        CGTradeInfo::m_targetGiftCreator[i] = WOWGUID();
        CGTradeInfo::m_targetCreator[i] = WOWGUID();
    }

    memset(CGTradeInfo::m_targetSocketEnchants, 0, sizeof(CGTradeInfo::m_targetSocketEnchants));

    for (uint32_t i = 0; i < 7; i++) {
        CGTradeInfo::m_targetEnchant[i] = 0;
        CGTradeInfo::m_targetCount[i] = 0;
        CGTradeInfo::m_targetItemID[i] = 0;
        CGTradeInfo::unk_BFA5DC[i] = 0;
        CGTradeInfo::unk_BFA5E8[i] = WOWGUID();
        CGTradeInfo::m_playerItems[i] = WOWGUID();
    }
}

// VERIFY: 67fa8d09 auto
// OFFSET: 0x005863C0
void CGTradeInfo::UpdatePlayerItem(WOWGUID guid) {
    if (!guid) {
        return;
    }

    for (int32_t i = 0; i < 7; i++) {
        if (CGTradeInfo::m_playerItems[i] == guid) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_PLAYER_ITEM_CHANGED, "%d", i + 1);
            return;
        }
    }
}

// VERIFY: da0e456c auto
// OFFSET: 0x00703CB0
uint32_t CGTradeInfo::GetPlayerTradeMoney() {
    return CGTradeInfo::m_playerTrade.m_money;
}

// VERIFY: c776083d auto
// OFFSET: 0x00703CC0
uint32_t CGTradeInfo::GetTargetTradeMoney() {
    return CGTradeInfo::m_playerTrade.m_targetMoney;
}
