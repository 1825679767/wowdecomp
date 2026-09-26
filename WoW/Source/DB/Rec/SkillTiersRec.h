#ifndef WOW_SOURCE_DB_REC_SKILLTIERSREC_H
#define WOW_SOURCE_DB_REC_SKILLTIERSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SkillTiersRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost [16];
    /* 0044 */ int32_t m_value [16];

// VERIFY: a4ce5ba1 auto
    // OFFSET: 0x008B6740
    static const char* GetFilename() {
        return "DBFilesClient\\SkillTiers.dbc";
    }

// VERIFY: 558b2938 manual
    // OFFSET: 0x008B6750
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t costRead = SFileReadFile(file, this->m_cost, sizeof(this->m_cost), nullptr, nullptr, 0);
        int32_t valueRead = SFileReadFile(file, this->m_value, sizeof(this->m_value), nullptr, nullptr, 0);

        if (!idRead
            | !costRead
            | !valueRead) {
            return false;
        }

        return true;
    }
};

#endif
