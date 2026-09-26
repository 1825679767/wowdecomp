#ifndef WOW_SOURCE_DB_REC_GLYPHSLOTREC_H
#define WOW_SOURCE_DB_REC_GLYPHSLOTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GlyphSlotRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_tooltip;

// VERIFY: a9d23c99 auto
    // OFFSET: 0x008AFE00
    static const char* GetFilename() {
        return "DBFilesClient\\GlyphSlot.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t typeRead = SFileReadFile(file, &this->m_type, sizeof(this->m_type), nullptr, nullptr, 0);
        int32_t tooltipRead = SFileReadFile(file, &this->m_tooltip, sizeof(this->m_tooltip), nullptr, nullptr, 0);

        if (!idRead
            | !typeRead
            | !tooltipRead) {
            return false;
        }

        return true;
    }
};

#endif
