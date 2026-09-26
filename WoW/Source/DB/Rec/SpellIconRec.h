#ifndef WOW_SOURCE_DB_REC_SPELLICONREC_H
#define WOW_SOURCE_DB_REC_SPELLICONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellIconRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_textureFilename;

// VERIFY: 51e19e9e auto
    // OFFSET: 0x008B7DC0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellIcon.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textureFilenameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t textureFilenameRead = SFileReadFile(file, &textureFilenameOffset, sizeof(textureFilenameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !textureFilenameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_textureFilename = &stringBuffer[textureFilenameOffset];
        } else {
            this->m_textureFilename = "";
        }

        return true;
    }
};

#endif
