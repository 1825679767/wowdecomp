#ifndef WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H
#define WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct RandPropPointsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_epic [5];
    /* 0018 */ int32_t m_superior [5];
    /* 002C */ int32_t m_good [5];

// VERIFY: dcc96f6d auto
    // OFFSET: 0x008B5310
    static const char* GetFilename() {
        return "DBFilesClient\\RandPropPoints.dbc";
    }

// VERIFY: 73d92237 manual
    // OFFSET: 0x008B5320
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t epicRead = SFileReadFile(file, this->m_epic, sizeof(this->m_epic), nullptr, nullptr, 0);
        int32_t superiorRead = SFileReadFile(file, this->m_superior, sizeof(this->m_superior), nullptr, nullptr, 0);
        int32_t goodRead = SFileReadFile(file, this->m_good, sizeof(this->m_good), nullptr, nullptr, 0);

        if (!idRead
            | !epicRead
            | !superiorRead
            | !goodRead) {
            return false;
        }

        return true;
    }
};

#endif
