#ifndef WOW_SOURCE_DB_REC_BANKBAGSLOTPRICESREC_H
#define WOW_SOURCE_DB_REC_BANKBAGSLOTPRICESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct BankBagSlotPricesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost;

// VERIFY: 287034de auto
    // OFFSET: 0x008AA120
    static const char* GetFilename() {
        return "DBFilesClient\\BankBagSlotPrices.dbc";
    }

// VERIFY: 8cb8471b auto
    // OFFSET: 0x008B7350
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
