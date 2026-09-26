#ifndef WOW_SOURCE_DB_REC_CHARBASEINFOREC_H
#define WOW_SOURCE_DB_REC_CHARBASEINFOREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CharBaseInfoRec {
    /* 0000 */ int8_t m_raceID;
    /* 0001 */ int8_t m_classID;
    /* 0004 */ int32_t m_generatedID;

// VERIFY: d0279645 auto
    // OFFSET: 0x008AADA0
    static const char* GetFilename() {
        return "DBFilesClient\\CharBaseInfo.dbc";
    }

// VERIFY: fe03cdc9 auto
    // OFFSET: 0x008AADB0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t classIDRead = SFileReadFile(file, &this->m_classID, sizeof(this->m_classID), nullptr, nullptr, 0);

        if (!raceIDRead
            | !classIDRead) {
            return false;
        }

        return true;
    }
};

#endif
