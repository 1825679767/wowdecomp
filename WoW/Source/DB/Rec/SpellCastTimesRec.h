#ifndef WOW_SOURCE_DB_REC_SPELLCASTTIMESREC_H
#define WOW_SOURCE_DB_REC_SPELLCASTTIMESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellCastTimesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_base;
    /* 0008 */ int32_t m_perLevel;
    /* 000C */ int32_t m_minimum;

// VERIFY: 29ad10b9 auto
    // OFFSET: 0x008B72A0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellCastTimes.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t baseRead = SFileReadFile(file, &this->m_base, sizeof(this->m_base), nullptr, nullptr, 0);
        int32_t perLevelRead = SFileReadFile(file, &this->m_perLevel, sizeof(this->m_perLevel), nullptr, nullptr, 0);
        int32_t minimumRead = SFileReadFile(file, &this->m_minimum, sizeof(this->m_minimum), nullptr, nullptr, 0);

        if (!idRead
            | !baseRead
            | !perLevelRead
            | !minimumRead) {
            return false;
        }

        return true;
    }
};

#endif
