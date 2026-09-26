#ifndef WOW_SOURCE_DB_REC_CHARSTARTOUTFITREC_H
#define WOW_SOURCE_DB_REC_CHARSTARTOUTFITREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CharStartOutfitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_raceID;
    /* 0005 */ int8_t m_classID;
    /* 0006 */ int8_t m_sexID;
    /* 0007 */ int8_t m_outfitID;
    /* 0008 */ int32_t m_itemID [24];
    /* 0068 */ int32_t m_displayItemID [24];
    /* 00C8 */ int32_t m_inventoryType [24];

// VERIFY: 38c1be5c auto
    // OFFSET: 0x008AAE10
    static const char* GetFilename() {
        return "DBFilesClient\\CharStartOutfit.dbc";
    }

// VERIFY: 9efd03c4 manual
    // OFFSET: 0x008AAE20
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t classIDRead = SFileReadFile(file, &this->m_classID, sizeof(this->m_classID), nullptr, nullptr, 0);
        int32_t sexIDRead = SFileReadFile(file, &this->m_sexID, sizeof(this->m_sexID), nullptr, nullptr, 0);
        int32_t outfitIDRead = SFileReadFile(file, &this->m_outfitID, sizeof(this->m_outfitID), nullptr, nullptr, 0);
        int32_t itemIDRead = SFileReadFile(file, this->m_itemID, sizeof(this->m_itemID), nullptr, nullptr, 0);
        int32_t displayItemIDRead = SFileReadFile(file, this->m_displayItemID, sizeof(this->m_displayItemID), nullptr, nullptr, 0);
        int32_t inventoryTypeRead = SFileReadFile(file, this->m_inventoryType, sizeof(this->m_inventoryType), nullptr, nullptr, 0);

        if (!idRead
            | !raceIDRead
            | !classIDRead
            | !sexIDRead
            | !outfitIDRead
            | !itemIDRead
            | !displayItemIDRead
            | !inventoryTypeRead) {
            return false;
        }

        return true;
    }
};

#endif
