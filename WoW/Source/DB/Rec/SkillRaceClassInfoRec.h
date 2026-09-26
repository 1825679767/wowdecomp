#ifndef WOW_SOURCE_DB_REC_SKILLRACECLASSINFOREC_H
#define WOW_SOURCE_DB_REC_SKILLRACECLASSINFOREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SkillRaceClassInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillID;
    /* 0008 */ int32_t m_raceMask;
    /* 000C */ int32_t m_classMask;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_minLevel;
    /* 0018 */ int32_t m_skillTierID;
    /* 001C */ int32_t m_skillCostIndex;

// VERIFY: c4c49727 auto
    // OFFSET: 0x008B6730
    static const char* GetFilename() {
        return "DBFilesClient\\SkillRaceClassInfo.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t skillIDRead = SFileReadFile(file, &this->m_skillID, sizeof(this->m_skillID), nullptr, nullptr, 0);
        int32_t raceMaskRead = SFileReadFile(file, &this->m_raceMask, sizeof(this->m_raceMask), nullptr, nullptr, 0);
        int32_t classMaskRead = SFileReadFile(file, &this->m_classMask, sizeof(this->m_classMask), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t minLevelRead = SFileReadFile(file, &this->m_minLevel, sizeof(this->m_minLevel), nullptr, nullptr, 0);
        int32_t skillTierIDRead = SFileReadFile(file, &this->m_skillTierID, sizeof(this->m_skillTierID), nullptr, nullptr, 0);
        int32_t skillCostIndexRead = SFileReadFile(file, &this->m_skillCostIndex, sizeof(this->m_skillCostIndex), nullptr, nullptr, 0);

        if (!idRead
            | !skillIDRead
            | !raceMaskRead
            | !classMaskRead
            | !flagsRead
            | !minLevelRead
            | !skillTierIDRead
            | !skillCostIndexRead) {
            return false;
        }

        return true;
    }
};

#endif
