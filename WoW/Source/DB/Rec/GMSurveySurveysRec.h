#ifndef WOW_SOURCE_DB_REC_GMSURVEYSURVEYSREC_H
#define WOW_SOURCE_DB_REC_GMSURVEYSURVEYSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GMSurveySurveysRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004 [10];

// VERIFY: f4d3a0a1 auto
    // OFFSET: 0x008AFE40
    static const char* GetFilename() {
        return "DBFilesClient\\GMSurveySurveys.dbc";
    }

// VERIFY: 584b9fa4 auto
    // OFFSET: 0x008AFE50
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t unk_0004Read = SFileReadFile(file, this->unk_0004, sizeof(this->unk_0004), nullptr, nullptr, 0);

        if (!idRead
            | !unk_0004Read) {
            return false;
        }

        return true;
    }
};

#endif
