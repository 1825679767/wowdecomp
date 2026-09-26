#ifndef WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct OverrideSpellDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [10];
    /* 002C */ int32_t m_flags;

// VERIFY: a26616df auto
    // OFFSET: 0x008B4940
    static const char* GetFilename() {
        return "DBFilesClient\\OverrideSpellData.dbc";
    }

// VERIFY: ef98b1a0 manual
    // OFFSET: 0x008B4950
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t spellsRead = SFileReadFile(file, this->m_spells, sizeof(this->m_spells), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !spellsRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
