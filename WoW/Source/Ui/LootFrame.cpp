#include "LootFrame.h"

// STATIC: 0x00BFA8D8
WOWGUID CGLootInfo::m_lootObj;

// STATIC: 0x00D38130
static uint32_t s_currencyMultiplier[3];

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
