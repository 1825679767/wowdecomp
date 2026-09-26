#ifndef WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H
#define WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;

// VERIFY: 164ab210 auto
    // OFFSET: 0x008B7340
    static const char* GetFilename() {
        return "DBFilesClient\\SpellCategory.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
