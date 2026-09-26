#ifndef WOW_SOURCE_DB_REC_LOCKREC_H
#define WOW_SOURCE_DB_REC_LOCKREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LockRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type [8];
    /* 0024 */ int32_t m_index [8];
    /* 0044 */ int32_t m_skill [8];
    /* 0064 */ int32_t m_action [8];

// VERIFY: 3d913410 auto
    // OFFSET: 0x008B2CB0
    static const char* GetFilename() {
        return "DBFilesClient\\Lock.dbc";
    }

// VERIFY: 8af13bd5 manual
    // OFFSET: 0x008B2CC0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t typeRead = SFileReadFile(file, this->m_type, sizeof(this->m_type), nullptr, nullptr, 0);
        int32_t indexRead = SFileReadFile(file, this->m_index, sizeof(this->m_index), nullptr, nullptr, 0);
        int32_t skillRead = SFileReadFile(file, this->m_skill, sizeof(this->m_skill), nullptr, nullptr, 0);
        int32_t actionRead = SFileReadFile(file, this->m_action, sizeof(this->m_action), nullptr, nullptr, 0);

        if (!idRead
            | !typeRead
            | !indexRead
            | !skillRead
            | !actionRead) {
            return false;
        }

        return true;
    }
};

#endif
