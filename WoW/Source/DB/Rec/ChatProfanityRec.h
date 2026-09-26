#ifndef WOW_SOURCE_DB_REC_CHATPROFANITYREC_H
#define WOW_SOURCE_DB_REC_CHATPROFANITYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ChatProfanityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
    /* 0008 */ int32_t m_language;

// VERIFY: 7c5af7e4 auto
    // OFFSET: 0x008ABA90
    static const char* GetFilename() {
        return "DBFilesClient\\ChatProfanity.dbc";
    }

// VERIFY: 2799758f auto
    // OFFSET: 0x008BCB60
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t textRead = SFileReadFile(file, &textOffset, sizeof(textOffset), nullptr, nullptr, 0);
        int32_t languageRead = SFileReadFile(file, &this->m_language, sizeof(this->m_language), nullptr, nullptr, 0);

        if (!idRead
            | !textRead
            | !languageRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_text = &stringBuffer[textOffset];
        } else {
            this->m_text = "";
        }

        return true;
    }
};

#endif
