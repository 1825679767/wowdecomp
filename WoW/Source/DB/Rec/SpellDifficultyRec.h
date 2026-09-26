#ifndef WOW_SOURCE_DB_REC_SPELLDIFFICULTYREC_H
#define WOW_SOURCE_DB_REC_SPELLDIFFICULTYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellDifficultyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficultySpellID [4];

// VERIFY: a26ffb6c auto
    // OFFSET: 0x008B7A70
    static const char* GetFilename() {
        return "DBFilesClient\\SpellDifficulty.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t difficultySpellIDRead = SFileReadFile(file, this->m_difficultySpellID, sizeof(this->m_difficultySpellID), nullptr, nullptr, 0);

        if (!idRead
            | !difficultySpellIDRead) {
            return false;
        }

        return true;
    }
};

#endif
