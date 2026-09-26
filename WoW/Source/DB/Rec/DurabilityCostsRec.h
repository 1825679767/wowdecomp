#ifndef WOW_SOURCE_DB_REC_DURABILITYCOSTSREC_H
#define WOW_SOURCE_DB_REC_DURABILITYCOSTSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DurabilityCostsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassCost [21];
    /* 0058 */ int32_t m_armorSubClassCost [8];

// VERIFY: 9b271905 auto
    // OFFSET: 0x008AED30
    static const char* GetFilename() {
        return "DBFilesClient\\DurabilityCosts.dbc";
    }

// VERIFY: 529e8a95 manual
    // OFFSET: 0x008AED40
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t weaponSubClassCostRead = SFileReadFile(file, this->m_weaponSubClassCost, sizeof(this->m_weaponSubClassCost), nullptr, nullptr, 0);
        int32_t armorSubClassCostRead = SFileReadFile(file, this->m_armorSubClassCost, sizeof(this->m_armorSubClassCost), nullptr, nullptr, 0);

        if (!idRead
            | !weaponSubClassCostRead
            | !armorSubClassCostRead) {
            return false;
        }

        return true;
    }
};

#endif
