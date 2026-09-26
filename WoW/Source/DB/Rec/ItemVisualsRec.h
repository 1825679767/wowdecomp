#ifndef WOW_SOURCE_DB_REC_ITEMVISUALSREC_H
#define WOW_SOURCE_DB_REC_ITEMVISUALSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemVisualsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_slot [5];

// VERIFY: 470abe1d auto
    // OFFSET: 0x008B1940
    static const char* GetFilename() {
        return "DBFilesClient\\ItemVisuals.dbc";
    }

// VERIFY: 72aa0519 auto
    // OFFSET: 0x008B1950
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t slotRead = SFileReadFile(file, this->m_slot, sizeof(this->m_slot), nullptr, nullptr, 0);

        if (!idRead
            | !slotRead) {
            return false;
        }

        return true;
    }
};

#endif
