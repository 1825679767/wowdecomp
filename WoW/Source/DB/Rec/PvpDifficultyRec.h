#ifndef WOW_SOURCE_DB_REC_PVPDIFFICULTYREC_H
#define WOW_SOURCE_DB_REC_PVPDIFFICULTYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct PvpDifficultyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_rangeIndex;
    /* 000C */ int32_t m_minLevel;
    /* 0010 */ int32_t m_maxLevel;
    /* 0014 */ int32_t m_difficulty;

// VERIFY: a73bd36d auto
    // OFFSET: 0x008B51F0
    static const char* GetFilename() {
        return "DBFilesClient\\PvpDifficulty.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t mapIDRead = SFileReadFile(file, &this->m_mapID, sizeof(this->m_mapID), nullptr, nullptr, 0);
        int32_t rangeIndexRead = SFileReadFile(file, &this->m_rangeIndex, sizeof(this->m_rangeIndex), nullptr, nullptr, 0);
        int32_t minLevelRead = SFileReadFile(file, &this->m_minLevel, sizeof(this->m_minLevel), nullptr, nullptr, 0);
        int32_t maxLevelRead = SFileReadFile(file, &this->m_maxLevel, sizeof(this->m_maxLevel), nullptr, nullptr, 0);
        int32_t difficultyRead = SFileReadFile(file, &this->m_difficulty, sizeof(this->m_difficulty), nullptr, nullptr, 0);

        if (!idRead
            | !mapIDRead
            | !rangeIndexRead
            | !minLevelRead
            | !maxLevelRead
            | !difficultyRead) {
            return false;
        }

        return true;
    }
};

#endif
