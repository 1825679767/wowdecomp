#ifndef WOW_SOURCE_DB_REC_DECLINEDWORDREC_H
#define WOW_SOURCE_DB_REC_DECLINEDWORDREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DeclinedWordRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_word;

// VERIFY: 8b1e5fb3 auto
    // OFFSET: 0x008AE680
    static const char* GetFilename() {
        return "DBFilesClient\\DeclinedWord.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t wordOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t wordRead = SFileReadFile(file, &wordOffset, sizeof(wordOffset), nullptr, nullptr, 0);

        if (!idRead
            | !wordRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_word = &stringBuffer[wordOffset];
        } else {
            this->m_word = "";
        }

        return true;
    }
};

#endif
