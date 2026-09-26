#ifndef WOW_SOURCE_DB_REC_LANGUAGEWORDSREC_H
#define WOW_SOURCE_DB_REC_LANGUAGEWORDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LanguageWordsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_languageID;
    /* 0008 */ const char* m_word;

// VERIFY: e61eb785 auto
    // OFFSET: 0x008B19A0
    static const char* GetFilename() {
        return "DBFilesClient\\LanguageWords.dbc";
    }

// VERIFY: 55a6f7e9 auto
    // OFFSET: 0x008B19B0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t wordOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t languageIDRead = SFileReadFile(file, &this->m_languageID, sizeof(this->m_languageID), nullptr, nullptr, 0);
        int32_t wordRead = SFileReadFile(file, &wordOffset, sizeof(wordOffset), nullptr, nullptr, 0);

        if (!idRead
            | !languageIDRead
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
