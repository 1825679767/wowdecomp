#include "RuneInfo.h"
// STATIC: 0x00C24304
int32_t CGRuneInfo::m_runes[8];

// STATIC: 0x00C24324
int32_t CGRuneInfo::m_baseRunes[8];

// STATIC: 0x00C24344
uint32_t CGRuneInfo::m_runeCooldowns[8];

// STATIC: 0x00C24364
uint32_t CGRuneInfo::m_runeLastUsed[8];

// STATIC: 0x00C24384
uint32_t CGRuneInfo::m_numRunes;

// STATIC: 0x00C24388
uint32_t CGRuneInfo::m_predictedRunes;


// VERIFY: 6c8047e1 auto
// OFFSET: 0x005EDEF0
void CGRuneInfo::InitializeGame() {
    for (uint32_t i = 0; i < 8; i++) {
        CGRuneInfo::m_runeLastUsed[i] = 0;
    }

    for (uint32_t i = 0; i < 8; i++) {
        CGRuneInfo::m_runeCooldowns[i] = 0;
    }

    CGRuneInfo::m_runes[0] = 0;
    CGRuneInfo::m_baseRunes[0] = 0;
    CGRuneInfo::m_runes[1] = 0;
    CGRuneInfo::m_baseRunes[1] = 0;
    CGRuneInfo::m_runes[2] = 1;
    CGRuneInfo::m_baseRunes[2] = 1;
    CGRuneInfo::m_runes[3] = 1;
    CGRuneInfo::m_baseRunes[3] = 1;
    CGRuneInfo::m_predictedRunes = 0xFFFFFFFF;
    CGRuneInfo::m_numRunes = 6;
    CGRuneInfo::m_runes[4] = 2;
    CGRuneInfo::m_baseRunes[4] = 2;
    CGRuneInfo::m_runes[5] = 2;
    CGRuneInfo::m_baseRunes[5] = 2;
}

// VERIFY: 2b125080 manual
// OFFSET: 0x005EDFC0
bool CGRuneInfo::IsRuneIndexValid(int32_t index) {
    if (index > 7) {
        return false;
    }

    if (CGRuneInfo::m_numRunes) {
        return static_cast<uint32_t>(index) < CGRuneInfo::m_numRunes;
    }

    return index < 8;
}

// VERIFY: afb005fb auto
// OFFSET: 0x005EDFF0
int32_t CGRuneInfo::GetRuneTypeByIndex(int32_t index, bool base) {
    if (base) {
        return CGRuneInfo::m_baseRunes[index];
    }

    return CGRuneInfo::m_runes[index];
}
