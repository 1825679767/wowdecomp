#ifndef WOW_SOURCE_DB_REC_SPELLVISUALKITREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALKITREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellVisualKitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_startAnimID;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_headEffect;
    /* 0010 */ int32_t m_chestEffect;
    /* 0014 */ int32_t m_baseEffect;
    /* 0018 */ int32_t m_leftHandEffect;
    /* 001C */ int32_t m_rightHandEffect;
    /* 0020 */ int32_t m_breathEffect;
    /* 0024 */ int32_t m_leftWeaponEffect;
    /* 0028 */ int32_t m_rightWeaponEffect;
    /* 002C */ int32_t m_specialEffect [3];
    /* 0038 */ int32_t m_worldEffect;
    /* 003C */ int32_t m_soundID;
    /* 0040 */ int32_t m_shakeID;
    /* 0044 */ int32_t m_charProc [4];
    /* 0054 */ float m_charParamZero [4];
    /* 0064 */ float m_charParamOne [4];
    /* 0074 */ float m_charParamTwo [4];
    /* 0084 */ float m_charParamThree [4];
    /* 0094 */ int32_t m_flags;

// VERIFY: 50e2fff8 auto
    // OFFSET: 0x008B9110
    static const char* GetFilename() {
        return "DBFilesClient\\SpellVisualKit.dbc";
    }

// VERIFY: 561a8357 auto
    // OFFSET: 0x008B9120
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t startAnimIDRead = SFileReadFile(file, &this->m_startAnimID, sizeof(this->m_startAnimID), nullptr, nullptr, 0);
        int32_t animIDRead = SFileReadFile(file, &this->m_animID, sizeof(this->m_animID), nullptr, nullptr, 0);
        int32_t headEffectRead = SFileReadFile(file, &this->m_headEffect, sizeof(this->m_headEffect), nullptr, nullptr, 0);
        int32_t chestEffectRead = SFileReadFile(file, &this->m_chestEffect, sizeof(this->m_chestEffect), nullptr, nullptr, 0);
        int32_t baseEffectRead = SFileReadFile(file, &this->m_baseEffect, sizeof(this->m_baseEffect), nullptr, nullptr, 0);
        int32_t leftHandEffectRead = SFileReadFile(file, &this->m_leftHandEffect, sizeof(this->m_leftHandEffect), nullptr, nullptr, 0);
        int32_t rightHandEffectRead = SFileReadFile(file, &this->m_rightHandEffect, sizeof(this->m_rightHandEffect), nullptr, nullptr, 0);
        int32_t breathEffectRead = SFileReadFile(file, &this->m_breathEffect, sizeof(this->m_breathEffect), nullptr, nullptr, 0);
        int32_t leftWeaponEffectRead = SFileReadFile(file, &this->m_leftWeaponEffect, sizeof(this->m_leftWeaponEffect), nullptr, nullptr, 0);
        int32_t rightWeaponEffectRead = SFileReadFile(file, &this->m_rightWeaponEffect, sizeof(this->m_rightWeaponEffect), nullptr, nullptr, 0);
        int32_t specialEffectRead = SFileReadFile(file, this->m_specialEffect, sizeof(this->m_specialEffect), nullptr, nullptr, 0);
        int32_t worldEffectRead = SFileReadFile(file, &this->m_worldEffect, sizeof(this->m_worldEffect), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t shakeIDRead = SFileReadFile(file, &this->m_shakeID, sizeof(this->m_shakeID), nullptr, nullptr, 0);
        int32_t charProcRead = SFileReadFile(file, this->m_charProc, sizeof(this->m_charProc), nullptr, nullptr, 0);
        int32_t charParamZeroRead = SFileReadFile(file, this->m_charParamZero, sizeof(this->m_charParamZero), nullptr, nullptr, 0);
        int32_t charParamOneRead = SFileReadFile(file, this->m_charParamOne, sizeof(this->m_charParamOne), nullptr, nullptr, 0);
        int32_t charParamTwoRead = SFileReadFile(file, this->m_charParamTwo, sizeof(this->m_charParamTwo), nullptr, nullptr, 0);
        int32_t charParamThreeRead = SFileReadFile(file, this->m_charParamThree, sizeof(this->m_charParamThree), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !startAnimIDRead
            | !animIDRead
            | !headEffectRead
            | !chestEffectRead
            | !baseEffectRead
            | !leftHandEffectRead
            | !rightHandEffectRead
            | !breathEffectRead
            | !leftWeaponEffectRead
            | !rightWeaponEffectRead
            | !specialEffectRead
            | !worldEffectRead
            | !soundIDRead
            | !shakeIDRead
            | !charProcRead
            | !charParamZeroRead
            | !charParamOneRead
            | !charParamTwoRead
            | !charParamThreeRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
