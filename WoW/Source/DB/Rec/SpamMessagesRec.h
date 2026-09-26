#ifndef WOW_SOURCE_DB_REC_SPAMMESSAGESREC_H
#define WOW_SOURCE_DB_REC_SPAMMESSAGESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpamMessagesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;

// VERIFY: 57b2aa63 auto
    // OFFSET: 0x008B7290
    static const char* GetFilename() {
        return "DBFilesClient\\SpamMessages.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t textRead = SFileReadFile(file, &textOffset, sizeof(textOffset), nullptr, nullptr, 0);

        if (!idRead
            | !textRead) {
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
