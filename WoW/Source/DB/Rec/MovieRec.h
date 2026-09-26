#ifndef WOW_SOURCE_DB_REC_MOVIEREC_H
#define WOW_SOURCE_DB_REC_MOVIEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct MovieRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_filename;
    /* 0008 */ int32_t m_volume;

// VERIFY: 50948735 auto
    // OFFSET: 0x008B47E0
    static const char* GetFilename() {
        return "DBFilesClient\\Movie.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t filenameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t filenameRead = SFileReadFile(file, &filenameOffset, sizeof(filenameOffset), nullptr, nullptr, 0);
        int32_t volumeRead = SFileReadFile(file, &this->m_volume, sizeof(this->m_volume), nullptr, nullptr, 0);

        if (!idRead
            | !filenameRead
            | !volumeRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_filename = &stringBuffer[filenameOffset];
        } else {
            this->m_filename = "";
        }

        return true;
    }
};

#endif
