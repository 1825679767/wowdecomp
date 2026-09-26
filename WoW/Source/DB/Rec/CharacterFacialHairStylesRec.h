#ifndef WOW_SOURCE_DB_REC_CHARACTERFACIALHAIRSTYLESREC_H
#define WOW_SOURCE_DB_REC_CHARACTERFACIALHAIRSTYLESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CharacterFacialHairStylesRec {
    /* 0000 */ int32_t m_raceID;
    /* 0004 */ int32_t m_sexID;
    /* 0008 */ int32_t m_variationID;
    /* 000C */ int32_t m_geoset [5];
    /* 0020 */ int32_t m_generatedID;

// VERIFY: e19e80f3 auto
    // OFFSET: 0x008AB490
    static const char* GetFilename() {
        return "DBFilesClient\\CharacterFacialHairStyles.dbc";
    }

// VERIFY: 2ac3bdeb manual
    // OFFSET: 0x008AB4A0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t sexIDRead = SFileReadFile(file, &this->m_sexID, sizeof(this->m_sexID), nullptr, nullptr, 0);
        int32_t variationIDRead = SFileReadFile(file, &this->m_variationID, sizeof(this->m_variationID), nullptr, nullptr, 0);
        int32_t geosetRead = SFileReadFile(file, this->m_geoset, sizeof(this->m_geoset), nullptr, nullptr, 0);

        if (!raceIDRead
            | !sexIDRead
            | !variationIDRead
            | !geosetRead) {
            return false;
        }

        return true;
    }
};

#endif
