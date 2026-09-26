#ifndef WOW_SOURCE_DB_REC_MOVIEVARIATIONREC_H
#define WOW_SOURCE_DB_REC_MOVIEVARIATIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct MovieVariationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_movieID;
    /* 0008 */ int32_t m_fileDataID;

// VERIFY: 76cbc4fb auto
    // OFFSET: 0x008B4800
    static const char* GetFilename() {
        return "DBFilesClient\\MovieVariation.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t movieIDRead = SFileReadFile(file, &this->m_movieID, sizeof(this->m_movieID), nullptr, nullptr, 0);
        int32_t fileDataIDRead = SFileReadFile(file, &this->m_fileDataID, sizeof(this->m_fileDataID), nullptr, nullptr, 0);

        if (!idRead
            | !movieIDRead
            | !fileDataIDRead) {
            return false;
        }

        return true;
    }
};

#endif
