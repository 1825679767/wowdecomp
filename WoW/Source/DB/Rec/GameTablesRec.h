#ifndef WOW_SOURCE_DB_REC_GAMETABLESREC_H
#define WOW_SOURCE_DB_REC_GAMETABLESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GameTablesRec {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_numRows;
    /* 0008 */ int32_t m_numColumns;
    /* 000C */ int32_t m_generatedID;

// VERIFY: 29e69ac7 auto
    // OFFSET: 0x008AFD20
    static const char* GetFilename() {
        return "DBFilesClient\\GameTables.dbc";
    }

// VERIFY: 6538b758 auto
    // OFFSET: 0x008AFD30
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t numRowsRead = SFileReadFile(file, &this->m_numRows, sizeof(this->m_numRows), nullptr, nullptr, 0);
        int32_t numColumnsRead = SFileReadFile(file, &this->m_numColumns, sizeof(this->m_numColumns), nullptr, nullptr, 0);

        if (!nameRead
            | !numRowsRead
            | !numColumnsRead) {
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
