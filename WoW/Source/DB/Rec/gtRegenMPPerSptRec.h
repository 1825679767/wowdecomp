#ifndef WOW_SOURCE_DB_REC_GTREGENMPPERSPTREC_H
#define WOW_SOURCE_DB_REC_GTREGENMPPERSPTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct gtRegenMPPerSptRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;

// VERIFY: 75470503 auto
    // OFFSET: 0x008B0050
    static const char* GetFilename() {
        return "DBFilesClient\\gtRegenMPPerSpt.dbc";
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
