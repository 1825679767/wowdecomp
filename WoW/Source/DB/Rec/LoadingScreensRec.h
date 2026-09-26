#ifndef WOW_SOURCE_DB_REC_LOADINGSCREENSREC_H
#define WOW_SOURCE_DB_REC_LOADINGSCREENSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LoadingScreensRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ int32_t m_hasWideScreen;

// VERIFY: 9b444f4f auto
    // OFFSET: 0x008B2B10
    static const char* GetFilename() {
        return "DBFilesClient\\LoadingScreens.dbc";
    }

// VERIFY: 6656545d manual
    // OFFSET: 0x008B2B20
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;
        uint32_t fileNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t fileNameRead = SFileReadFile(file, &fileNameOffset, sizeof(fileNameOffset), nullptr, nullptr, 0);
        int32_t hasWideScreenRead = SFileReadFile(file, &this->m_hasWideScreen, sizeof(this->m_hasWideScreen), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !fileNameRead
            | !hasWideScreenRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
            this->m_fileName = &stringBuffer[fileNameOffset];
        } else {
            this->m_name = "";
            this->m_fileName = "";
        }

        return true;
    }
};

#endif
