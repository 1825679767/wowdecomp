#ifndef WOW_SOURCE_DB_REC_STRINGLOOKUPSREC_H
#define WOW_SOURCE_DB_REC_STRINGLOOKUPSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct StringLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_string;

// VERIFY: a479af73 auto
    // OFFSET: 0x008B98C0
    static const char* GetFilename() {
        return "DBFilesClient\\StringLookups.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t stringOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t stringRead = SFileReadFile(file, &stringOffset, sizeof(stringOffset), nullptr, nullptr, 0);

        if (!idRead
            | !stringRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_string = &stringBuffer[stringOffset];
        } else {
            this->m_string = "";
        }

        return true;
    }
};

#endif
