#ifndef WOW_SOURCE_DB_REC_SPELLDURATIONREC_H
#define WOW_SOURCE_DB_REC_SPELLDURATIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellDurationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_duration;
    /* 0008 */ int32_t m_durationPerLevel;
    /* 000C */ int32_t m_maxDuration;

// VERIFY: 06ac5dff auto
    // OFFSET: 0x008B7D80
    static const char* GetFilename() {
        return "DBFilesClient\\SpellDuration.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t durationRead = SFileReadFile(file, &this->m_duration, sizeof(this->m_duration), nullptr, nullptr, 0);
        int32_t durationPerLevelRead = SFileReadFile(file, &this->m_durationPerLevel, sizeof(this->m_durationPerLevel), nullptr, nullptr, 0);
        int32_t maxDurationRead = SFileReadFile(file, &this->m_maxDuration, sizeof(this->m_maxDuration), nullptr, nullptr, 0);

        if (!idRead
            | !durationRead
            | !durationPerLevelRead
            | !maxDurationRead) {
            return false;
        }

        return true;
    }
};

#endif
