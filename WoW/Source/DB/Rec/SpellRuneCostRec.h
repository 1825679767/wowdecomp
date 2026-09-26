#ifndef WOW_SOURCE_DB_REC_SPELLRUNECOSTREC_H
#define WOW_SOURCE_DB_REC_SPELLRUNECOSTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellRuneCostRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_blood;
    /* 0008 */ int32_t m_unholy;
    /* 000C */ int32_t m_frost;
    /* 0010 */ int32_t m_runicPower;

// VERIFY: 9b9e1041 auto
    // OFFSET: 0x008B8B90
    static const char* GetFilename() {
        return "DBFilesClient\\SpellRuneCost.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t bloodRead = SFileReadFile(file, &this->m_blood, sizeof(this->m_blood), nullptr, nullptr, 0);
        int32_t unholyRead = SFileReadFile(file, &this->m_unholy, sizeof(this->m_unholy), nullptr, nullptr, 0);
        int32_t frostRead = SFileReadFile(file, &this->m_frost, sizeof(this->m_frost), nullptr, nullptr, 0);
        int32_t runicPowerRead = SFileReadFile(file, &this->m_runicPower, sizeof(this->m_runicPower), nullptr, nullptr, 0);

        if (!idRead
            | !bloodRead
            | !unholyRead
            | !frostRead
            | !runicPowerRead) {
            return false;
        }

        return true;
    }
};

#endif
