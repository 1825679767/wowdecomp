#ifndef WOW_SOURCE_DB_REC_NAMESRESERVEDREC_H
#define WOW_SOURCE_DB_REC_NAMESRESERVEDREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct NamesReservedRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_language;

// VERIFY: 2c2c3f93 auto
    // OFFSET: 0x008BCC00
    static const char* GetFilename() {
        return "DBFilesClient\\NamesReserved.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t languageRead = SFileReadFile(file, &this->m_language, sizeof(this->m_language), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !languageRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
