#ifndef WOW_SOURCE_DB_REC_ITEMVISUALEFFECTSREC_H
#define WOW_SOURCE_DB_REC_ITEMVISUALEFFECTSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemVisualEffectsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_model;

// VERIFY: b6ff3014 auto
    // OFFSET: 0x008B1930
    static const char* GetFilename() {
        return "DBFilesClient\\ItemVisualEffects.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t modelOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t modelRead = SFileReadFile(file, &modelOffset, sizeof(modelOffset), nullptr, nullptr, 0);

        if (!idRead
            | !modelRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_model = &stringBuffer[modelOffset];
        } else {
            this->m_model = "";
        }

        return true;
    }
};

#endif
