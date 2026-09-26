#ifndef WOW_SOURCE_DB_REC_VEHICLEUIINDICATORREC_H
#define WOW_SOURCE_DB_REC_VEHICLEUIINDICATORREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct VehicleUIIndicatorRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_backgroundTexture;

// VERIFY: 3c030920 auto
    // OFFSET: 0x008A8350
    static const char* GetFilename() {
        return "DBFilesClient\\VehicleUIIndicator.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t backgroundTextureOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t backgroundTextureRead = SFileReadFile(file, &backgroundTextureOffset, sizeof(backgroundTextureOffset), nullptr, nullptr, 0);

        if (!idRead
            | !backgroundTextureRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_backgroundTexture = &stringBuffer[backgroundTextureOffset];
        } else {
            this->m_backgroundTexture = "";
        }

        return true;
    }
};

#endif
