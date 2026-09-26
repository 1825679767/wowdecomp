#ifndef WOW_SOURCE_DB_REC_GLYPHPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_GLYPHPROPERTIESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GlyphPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spellID;
    /* 0008 */ int32_t m_glyphSlotFlags;
    /* 000C */ int32_t m_spellIconID;

// VERIFY: 162df50d auto
    // OFFSET: 0x008AFDF0
    static const char* GetFilename() {
        return "DBFilesClient\\GlyphProperties.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t spellIDRead = SFileReadFile(file, &this->m_spellID, sizeof(this->m_spellID), nullptr, nullptr, 0);
        int32_t glyphSlotFlagsRead = SFileReadFile(file, &this->m_glyphSlotFlags, sizeof(this->m_glyphSlotFlags), nullptr, nullptr, 0);
        int32_t spellIconIDRead = SFileReadFile(file, &this->m_spellIconID, sizeof(this->m_spellIconID), nullptr, nullptr, 0);

        if (!idRead
            | !spellIDRead
            | !glyphSlotFlagsRead
            | !spellIconIDRead) {
            return false;
        }

        return true;
    }
};

#endif
