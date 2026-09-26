#ifndef WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H
#define WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemExtendedCostRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_honorPoints;
    /* 0008 */ int32_t m_arenaPoints;
    /* 000C */ int32_t m_arenaBracket;
    /* 0010 */ int32_t m_itemID [5];
    /* 0024 */ int32_t m_itemCount [5];
    /* 0038 */ int32_t m_requiredArenaRating;
    /* 003C */ int32_t m_itemPurchaseGroup;

// VERIFY: 9f761986 auto
    // OFFSET: 0x008B0300
    static const char* GetFilename() {
        return "DBFilesClient\\ItemExtendedCost.dbc";
    }

// VERIFY: 6653f012 manual
    // OFFSET: 0x008B0310
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t honorPointsRead = SFileReadFile(file, &this->m_honorPoints, sizeof(this->m_honorPoints), nullptr, nullptr, 0);
        int32_t arenaPointsRead = SFileReadFile(file, &this->m_arenaPoints, sizeof(this->m_arenaPoints), nullptr, nullptr, 0);
        int32_t arenaBracketRead = SFileReadFile(file, &this->m_arenaBracket, sizeof(this->m_arenaBracket), nullptr, nullptr, 0);
        int32_t itemIDRead = SFileReadFile(file, this->m_itemID, sizeof(this->m_itemID), nullptr, nullptr, 0);
        int32_t itemCountRead = SFileReadFile(file, this->m_itemCount, sizeof(this->m_itemCount), nullptr, nullptr, 0);
        int32_t requiredArenaRatingRead = SFileReadFile(file, &this->m_requiredArenaRating, sizeof(this->m_requiredArenaRating), nullptr, nullptr, 0);
        int32_t itemPurchaseGroupRead = SFileReadFile(file, &this->m_itemPurchaseGroup, sizeof(this->m_itemPurchaseGroup), nullptr, nullptr, 0);

        if (!idRead
            | !honorPointsRead
            | !arenaPointsRead
            | !arenaBracketRead
            | !itemIDRead
            | !itemCountRead
            | !requiredArenaRatingRead
            | !itemPurchaseGroupRead) {
            return false;
        }

        return true;
    }
};

#endif
