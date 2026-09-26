#ifndef WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H
#define WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CurrencyTypesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_categoryID;
    /* 000C */ int32_t m_bitIndex;

// VERIFY: ad8f4d36 auto
    // OFFSET: 0x008AE2E0
    static const char* GetFilename() {
        return "DBFilesClient\\CurrencyTypes.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t itemIDRead = SFileReadFile(file, &this->m_itemID, sizeof(this->m_itemID), nullptr, nullptr, 0);
        int32_t categoryIDRead = SFileReadFile(file, &this->m_categoryID, sizeof(this->m_categoryID), nullptr, nullptr, 0);
        int32_t bitIndexRead = SFileReadFile(file, &this->m_bitIndex, sizeof(this->m_bitIndex), nullptr, nullptr, 0);

        if (!idRead
            | !itemIDRead
            | !categoryIDRead
            | !bitIndexRead) {
            return false;
        }

        return true;
    }
};

#endif
