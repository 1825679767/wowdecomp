#ifndef WOW_SOURCE_DB_REC_TALENTREC_H
#define WOW_SOURCE_DB_REC_TALENTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TalentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_tabID;
    /* 0008 */ int32_t m_tierID;
    /* 000C */ int32_t m_columnIndex;
    /* 0010 */ int32_t m_spellRank [9];
    /* 0034 */ int32_t m_prereqTalent [3];
    /* 0040 */ int32_t m_prereqRank [3];
    /* 004C */ int32_t m_flags;
    /* 0050 */ int32_t m_requiredSpellID;
    /* 0054 */ int32_t m_categoryMask [2];

// VERIFY: bcdcf358 auto
    // OFFSET: 0x008B9960
    static const char* GetFilename() {
        return "DBFilesClient\\Talent.dbc";
    }

// VERIFY: d0d130e8 auto
    // OFFSET: 0x008B9970
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t tabIDRead = SFileReadFile(file, &this->m_tabID, sizeof(this->m_tabID), nullptr, nullptr, 0);
        int32_t tierIDRead = SFileReadFile(file, &this->m_tierID, sizeof(this->m_tierID), nullptr, nullptr, 0);
        int32_t columnIndexRead = SFileReadFile(file, &this->m_columnIndex, sizeof(this->m_columnIndex), nullptr, nullptr, 0);
        int32_t spellRankRead = SFileReadFile(file, this->m_spellRank, sizeof(this->m_spellRank), nullptr, nullptr, 0);
        int32_t prereqTalentRead = SFileReadFile(file, this->m_prereqTalent, sizeof(this->m_prereqTalent), nullptr, nullptr, 0);
        int32_t prereqRankRead = SFileReadFile(file, this->m_prereqRank, sizeof(this->m_prereqRank), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t requiredSpellIDRead = SFileReadFile(file, &this->m_requiredSpellID, sizeof(this->m_requiredSpellID), nullptr, nullptr, 0);
        int32_t categoryMaskRead = SFileReadFile(file, this->m_categoryMask, sizeof(this->m_categoryMask), nullptr, nullptr, 0);

        if (!idRead
            | !tabIDRead
            | !tierIDRead
            | !columnIndexRead
            | !spellRankRead
            | !prereqTalentRead
            | !prereqRankRead
            | !flagsRead
            | !requiredSpellIDRead
            | !categoryMaskRead) {
            return false;
        }

        return true;
    }
};

#endif
