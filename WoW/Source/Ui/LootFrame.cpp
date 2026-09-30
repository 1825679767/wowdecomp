#include "LootFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00BFA68C
uint32_t CGLootInfo::m_lootType;

// STATIC: 0x00BFA690
LootSlot CGLootInfo::m_slots[18];

// STATIC: 0x00BFA8D0
int32_t CGLootInfo::m_coins;

// STATIC: 0x00BFA8D8
WOWGUID CGLootInfo::m_lootObj;

// STATIC: 0x00D38130
static uint32_t s_currencyMultiplier[3];

// STATIC: 0x00AF4914
static const char* s_currencyAmountTokens[3] = { "COPPER_AMOUNT", "SILVER_AMOUNT", "GOLD_AMOUNT" };

// STATIC: 0x00D3813C
static uint8_t s_lootInitialized;

// VERIFY: 4a4538a7 auto
// OFFSET: 0x007E7FA0
void LootInitialize() {
    s_currencyMultiplier[0] = 1;
    s_currencyMultiplier[1] = 100;
    s_currencyMultiplier[2] = 10000;
    s_lootInitialized = 1;
}

// VERIFY: a4a2cb90 auto
// OFFSET: 0x00588170
void CGLootInfo::InitializeGame() {
    CGLootInfo::m_lootObj = WOWGUID();
}

// VERIFY: 58a0373b auto
// OFFSET: 0x007E7FD0
void LootDestroy() {
    s_lootInitialized = 0;
}

// VERIFY: c5565767 manual
// OFFSET: 0x007E7C70
void CurrencyBreakdown(int32_t amount, int32_t* parts) {
    for (int32_t i = 2; i; i--) {
        int32_t multiplier = static_cast<int32_t>(s_currencyMultiplier[i]);

        if (amount < multiplier) {
            parts[i] = 0;
        } else {
            parts[i] = amount / multiplier;
            amount %= multiplier;
        }
    }

    parts[0] = amount;
}

// VERIFY: 4b725563 auto
// OFFSET: 0x007E7D80
void GetCurrencyText(int32_t amount, char* buffer, uint32_t size, const char* separator) {
    int32_t parts[3];
    CurrencyBreakdown(amount, parts);
    *buffer = 0;

    for (int32_t i = 2; i >= 0; i--) {
        int32_t value = parts[i];

        if (!value) {
            continue;
        }

        if (*buffer) {
            SStrPack(buffer, separator, size);
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), FrameScript_GetText(s_currencyAmountTokens[i], -1, GENDER_NOT_APPLICABLE), value);
        SStrPack(buffer, text, size);
    }
}
