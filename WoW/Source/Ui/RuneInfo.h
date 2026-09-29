#ifndef WOW_SOURCE_UI_RUNEINFO_H
#define WOW_SOURCE_UI_RUNEINFO_H

#include <cstdint>

class CGRuneInfo {
    public:
    static void SetPredictedRunes(uint8_t available, uint8_t used, const uint8_t* cooldowns);
    static void InitializeGame();
    static bool HasRunesForSpell(int32_t spellId, int32_t* a2);
};

#endif
