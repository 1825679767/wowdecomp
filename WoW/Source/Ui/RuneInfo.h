#ifndef WOW_SOURCE_UI_RUNEINFO_H
#define WOW_SOURCE_UI_RUNEINFO_H

#include <cstdint>

class SpellRec;

class CGRuneInfo {
    public:
    static int32_t m_runes[8];
    static int32_t m_baseRunes[8];
    static uint32_t m_runeCooldowns[8];
    static uint32_t m_runeLastUsed[8];
    static uint32_t m_numRunes;
    static uint32_t m_predictedRunes;
    static void SetPredictedRunes(uint8_t available, uint8_t used, const uint8_t* cooldowns);
    static void InitializeGame();
    static bool HasRunesForSpell(int32_t spellId, int32_t* needed);
    static uint32_t GetRuneRechargeTime(int32_t index);
    static int32_t CountPredictedRunesOfType(int32_t type);
    static bool ComputeRuneSpellCooldown(const SpellRec* spell, uint32_t* cooldown);
    static bool IsRuneIndexValid(int32_t index);
    static int32_t GetRuneTypeByIndex(int32_t index, bool base);
};

#endif
