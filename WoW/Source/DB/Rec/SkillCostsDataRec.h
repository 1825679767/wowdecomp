#ifndef WOW_SOURCE_DB_REC_SKILLCOSTSDATAREC_H
#define WOW_SOURCE_DB_REC_SKILLCOSTSDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SkillCostsDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillCostsID;
    /* 0008 */ int32_t m_cost [3];

// VERIFY: 89f609d5 auto
    // OFFSET: 0x008B5C70
    static const char* GetFilename() {
        return "DBFilesClient\\SkillCostsData.dbc";
    }

// VERIFY: 2f3324c6 manual
    // OFFSET: 0x008B5C80
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t skillCostsIDRead = SFileReadFile(file, &this->m_skillCostsID, sizeof(this->m_skillCostsID), nullptr, nullptr, 0);
        int32_t costRead = SFileReadFile(file, this->m_cost, sizeof(this->m_cost), nullptr, nullptr, 0);

        if (!idRead
            | !skillCostsIDRead
            | !costRead) {
            return false;
        }

        return true;
    }
};

#endif
