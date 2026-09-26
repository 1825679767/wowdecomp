#ifndef WOW_SOURCE_DB_REC_WEAPONIMPACTSOUNDSREC_H
#define WOW_SOURCE_DB_REC_WEAPONIMPACTSOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WeaponImpactSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassID;
    /* 0008 */ int32_t m_parrySoundType;
    /* 000C */ int32_t m_impactSoundID [10];
    /* 0034 */ int32_t m_critImpactSoundID [10];

// VERIFY: d6f3bc64 auto
    // OFFSET: 0x008BB620
    static const char* GetFilename() {
        return "DBFilesClient\\WeaponImpactSounds.dbc";
    }

// VERIFY: c51c05aa manual
    // OFFSET: 0x008BB630
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t weaponSubClassIDRead = SFileReadFile(file, &this->m_weaponSubClassID, sizeof(this->m_weaponSubClassID), nullptr, nullptr, 0);
        int32_t parrySoundTypeRead = SFileReadFile(file, &this->m_parrySoundType, sizeof(this->m_parrySoundType), nullptr, nullptr, 0);
        int32_t impactSoundIDRead = SFileReadFile(file, this->m_impactSoundID, sizeof(this->m_impactSoundID), nullptr, nullptr, 0);
        int32_t critImpactSoundIDRead = SFileReadFile(file, this->m_critImpactSoundID, sizeof(this->m_critImpactSoundID), nullptr, nullptr, 0);

        if (!idRead
            | !weaponSubClassIDRead
            | !parrySoundTypeRead
            | !impactSoundIDRead
            | !critImpactSoundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
