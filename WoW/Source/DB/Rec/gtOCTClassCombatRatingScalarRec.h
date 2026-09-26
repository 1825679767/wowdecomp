#ifndef WOW_SOURCE_DB_REC_GTOCTCLASSCOMBATRATINGSCALARREC_H
#define WOW_SOURCE_DB_REC_GTOCTCLASSCOMBATRATINGSCALARREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct gtOCTClassCombatRatingScalarRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;

// VERIFY: 218b27a7 auto
    // OFFSET: 0x008B0010
    static const char* GetFilename() {
        return "DBFilesClient\\gtOCTClassCombatRatingScalar.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t dataRead = SFileReadFile(file, &this->m_data, sizeof(this->m_data), nullptr, nullptr, 0);

        if (!idRead
            | !dataRead) {
            return false;
        }

        return true;
    }
};

#endif
