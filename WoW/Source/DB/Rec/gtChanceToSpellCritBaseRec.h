#ifndef WOW_SOURCE_DB_REC_GTCHANCETOSPELLCRITBASEREC_H
#define WOW_SOURCE_DB_REC_GTCHANCETOSPELLCRITBASEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct gtChanceToSpellCritBaseRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;

// VERIFY: 329f0402 auto
    // OFFSET: 0x008AFFF0
    static const char* GetFilename() {
        return "DBFilesClient\\gtChanceToSpellCritBase.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t dataRead = SFileReadFile(file, &this->m_data, sizeof(this->m_data), nullptr, nullptr, 0);

        if (!dataRead) {
            return false;
        }

        return true;
    }
};

#endif
