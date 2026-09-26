#ifndef WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CreatureSpellDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [4];
    /* 0014 */ int32_t m_availability [4];

// VERIFY: 5e62e3eb auto
    // OFFSET: 0x008ADFB0
    static const char* GetFilename() {
        return "DBFilesClient\\CreatureSpellData.dbc";
    }

// VERIFY: 456383bf manual
    // OFFSET: 0x008ADFC0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t spellsRead = SFileReadFile(file, this->m_spells, sizeof(this->m_spells), nullptr, nullptr, 0);
        int32_t availabilityRead = SFileReadFile(file, this->m_availability, sizeof(this->m_availability), nullptr, nullptr, 0);

        if (!idRead
            | !spellsRead
            | !availabilityRead) {
            return false;
        }

        return true;
    }
};

#endif
