#ifndef WOW_SOURCE_DB_REC_MOVIEFILEDATAREC_H
#define WOW_SOURCE_DB_REC_MOVIEFILEDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct MovieFileDataRec {
    /* 0000 */ int32_t m_fileDataID;
    /* 0004 */ int32_t m_resolution;

// VERIFY: b0934982 auto
    // OFFSET: 0x008B47F0
    static const char* GetFilename() {
        return "DBFilesClient\\MovieFileData.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t fileDataIDRead = SFileReadFile(file, &this->m_fileDataID, sizeof(this->m_fileDataID), nullptr, nullptr, 0);
        int32_t resolutionRead = SFileReadFile(file, &this->m_resolution, sizeof(this->m_resolution), nullptr, nullptr, 0);

        if (!fileDataIDRead
            | !resolutionRead) {
            return false;
        }

        return true;
    }
};

#endif
