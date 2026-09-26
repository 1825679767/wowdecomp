#ifndef WOW_SOURCE_DB_REC_HELMETGEOSETVISDATAREC_H
#define WOW_SOURCE_DB_REC_HELMETGEOSETVISDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct HelmetGeosetVisDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_hideGeoset [7];

// VERIFY: 7aca0e56 auto
    // OFFSET: 0x008B0060
    static const char* GetFilename() {
        return "DBFilesClient\\HelmetGeosetVisData.dbc";
    }

// VERIFY: 6746cb6d auto
    // OFFSET: 0x008B0070
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t hideGeosetRead = SFileReadFile(file, this->m_hideGeoset, sizeof(this->m_hideGeoset), nullptr, nullptr, 0);

        if (!idRead
            | !hideGeosetRead) {
            return false;
        }

        return true;
    }
};

#endif
