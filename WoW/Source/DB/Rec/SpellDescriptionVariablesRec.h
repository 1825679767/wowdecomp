#ifndef WOW_SOURCE_DB_REC_SPELLDESCRIPTIONVARIABLESREC_H
#define WOW_SOURCE_DB_REC_SPELLDESCRIPTIONVARIABLESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellDescriptionVariablesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_variables;

// VERIFY: ac8d3827 auto
    // OFFSET: 0x008B7A60
    static const char* GetFilename() {
        return "DBFilesClient\\SpellDescriptionVariables.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t variablesOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t variablesRead = SFileReadFile(file, &variablesOffset, sizeof(variablesOffset), nullptr, nullptr, 0);

        if (!idRead
            | !variablesRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_variables = &stringBuffer[variablesOffset];
        } else {
            this->m_variables = "";
        }

        return true;
    }
};

#endif
