#ifndef WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H
#define WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WeaponSwingSounds2Rec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_swingType;
    /* 0008 */ int32_t m_crit;
    /* 000C */ int32_t m_soundID;

// VERIFY: 0517a504 auto
    // OFFSET: 0x008BB6E0
    static const char* GetFilename() {
        return "DBFilesClient\\WeaponSwingSounds2.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t swingTypeRead = SFileReadFile(file, &this->m_swingType, sizeof(this->m_swingType), nullptr, nullptr, 0);
        int32_t critRead = SFileReadFile(file, &this->m_crit, sizeof(this->m_crit), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);

        if (!idRead
            | !swingTypeRead
            | !critRead
            | !soundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
