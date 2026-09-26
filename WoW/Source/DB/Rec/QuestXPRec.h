#ifndef WOW_SOURCE_DB_REC_QUESTXPREC_H
#define WOW_SOURCE_DB_REC_QUESTXPREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct QuestXPRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficulty [10];

// VERIFY: ba582c92 auto
    // OFFSET: 0x008B5300
    static const char* GetFilename() {
        return "DBFilesClient\\QuestXP.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t difficultyRead = SFileReadFile(file, this->m_difficulty, sizeof(this->m_difficulty), nullptr, nullptr, 0);

        if (!idRead
            | !difficultyRead) {
            return false;
        }

        return true;
    }
};

#endif
