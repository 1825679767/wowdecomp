#ifndef WOW_SOURCE_DB_REC_STABLESLOTPRICESREC_H
#define WOW_SOURCE_DB_REC_STABLESLOTPRICESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct StableSlotPricesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost;

// VERIFY: c170ace5 auto
    // OFFSET: 0x008B97E0
    static const char* GetFilename() {
        return "DBFilesClient\\StableSlotPrices.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t costRead = SFileReadFile(file, &this->m_cost, sizeof(this->m_cost), nullptr, nullptr, 0);

        if (!idRead
            | !costRead) {
            return false;
        }

        return true;
    }
};

#endif
