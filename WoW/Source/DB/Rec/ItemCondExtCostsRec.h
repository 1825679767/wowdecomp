#ifndef WOW_SOURCE_DB_REC_ITEMCONDEXTCOSTSREC_H
#define WOW_SOURCE_DB_REC_ITEMCONDEXTCOSTSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemCondExtCostsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_condExtendedCost;
    /* 0008 */ int32_t m_itemExtendedCostEntry;
    /* 000C */ int32_t m_arenaSeason;

// VERIFY: 571012fe auto
    // OFFSET: 0x008B02F0
    static const char* GetFilename() {
        return "DBFilesClient\\ItemCondExtCosts.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t condExtendedCostRead = SFileReadFile(file, &this->m_condExtendedCost, sizeof(this->m_condExtendedCost), nullptr, nullptr, 0);
        int32_t itemExtendedCostEntryRead = SFileReadFile(file, &this->m_itemExtendedCostEntry, sizeof(this->m_itemExtendedCostEntry), nullptr, nullptr, 0);
        int32_t arenaSeasonRead = SFileReadFile(file, &this->m_arenaSeason, sizeof(this->m_arenaSeason), nullptr, nullptr, 0);

        if (!idRead
            | !condExtendedCostRead
            | !itemExtendedCostEntryRead
            | !arenaSeasonRead) {
            return false;
        }

        return true;
    }
};

#endif
