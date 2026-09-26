#ifndef WOW_SOURCE_DB_REC_FILEDATAREC_H
#define WOW_SOURCE_DB_REC_FILEDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct FileDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_filename;
    /* 0008 */ const char* m_filepath;

// VERIFY: 4a6b2e8c auto
    // OFFSET: 0x008AF920
    static const char* GetFilename() {
        return "DBFilesClient\\FileData.dbc";
    }

// VERIFY: 3b0f6e72 auto
    // OFFSET: 0x008AF930
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t filenameOffset;
        uint32_t filepathOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t filenameRead = SFileReadFile(file, &filenameOffset, sizeof(filenameOffset), nullptr, nullptr, 0);
        int32_t filepathRead = SFileReadFile(file, &filepathOffset, sizeof(filepathOffset), nullptr, nullptr, 0);

        if (!idRead
            | !filenameRead
            | !filepathRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_filename = &stringBuffer[filenameOffset];
            this->m_filepath = &stringBuffer[filepathOffset];
        } else {
            this->m_filename = "";
            this->m_filepath = "";
        }

        return true;
    }
};

#endif
