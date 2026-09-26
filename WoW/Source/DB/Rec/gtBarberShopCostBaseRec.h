#ifndef WOW_SOURCE_DB_REC_GTBARBERSHOPCOSTBASEREC_H
#define WOW_SOURCE_DB_REC_GTBARBERSHOPCOSTBASEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct gtBarberShopCostBaseRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;

// VERIFY: 07eb2989 auto
    // OFFSET: 0x008AFF80
    static const char* GetFilename() {
        return "DBFilesClient\\gtBarberShopCostBase.dbc";
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
