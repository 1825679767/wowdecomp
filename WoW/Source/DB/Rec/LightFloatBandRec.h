#ifndef WOW_SOURCE_DB_REC_LIGHTFLOATBANDREC_H
#define WOW_SOURCE_DB_REC_LIGHTFLOATBANDREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LightFloatBandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ float m_data [16];

// VERIFY: 2ba50921 auto
    // OFFSET: 0x008BE1A0
    static const char* GetFilename() {
        return "DBFilesClient\\LightFloatBand.dbc";
    }

// VERIFY: afd3f9e2 manual
    // OFFSET: 0x008BE110
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t numRead = SFileReadFile(file, &this->m_num, sizeof(this->m_num), nullptr, nullptr, 0);
        int32_t timeRead = SFileReadFile(file, this->m_time, sizeof(this->m_time), nullptr, nullptr, 0);
        int32_t dataRead = SFileReadFile(file, this->m_data, sizeof(this->m_data), nullptr, nullptr, 0);

        if (!idRead
            | !numRead
            | !timeRead
            | !dataRead) {
            return false;
        }

        return true;
    }
};

#endif
