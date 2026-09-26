#ifndef WOW_SOURCE_DB_REC_SPELLRADIUSREC_H
#define WOW_SOURCE_DB_REC_SPELLRADIUSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellRadiusRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_radius;
    /* 0008 */ float m_radiusPerLevel;
    /* 000C */ float m_radiusMax;

// VERIFY: a7edf564 auto
    // OFFSET: 0x008B85F0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellRadius.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t radiusRead = SFileReadFile(file, &this->m_radius, sizeof(this->m_radius), nullptr, nullptr, 0);
        int32_t radiusPerLevelRead = SFileReadFile(file, &this->m_radiusPerLevel, sizeof(this->m_radiusPerLevel), nullptr, nullptr, 0);
        int32_t radiusMaxRead = SFileReadFile(file, &this->m_radiusMax, sizeof(this->m_radiusMax), nullptr, nullptr, 0);

        if (!idRead
            | !radiusRead
            | !radiusPerLevelRead
            | !radiusMaxRead) {
            return false;
        }

        return true;
    }
};

#endif
