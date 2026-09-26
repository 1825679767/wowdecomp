#ifndef WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H
#define WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CharHairGeosetsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_variationID;
    /* 0010 */ int32_t m_geosetID;
    /* 0014 */ int32_t m_showscalp;

// VERIFY: 9d6f9451 auto
    // OFFSET: 0x008AAE00
    static const char* GetFilename() {
        return "DBFilesClient\\CharHairGeosets.dbc";
    }

// VERIFY: 0b49c847 manual
    // OFFSET: 0x008B5200
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t sexIDRead = SFileReadFile(file, &this->m_sexID, sizeof(this->m_sexID), nullptr, nullptr, 0);
        int32_t variationIDRead = SFileReadFile(file, &this->m_variationID, sizeof(this->m_variationID), nullptr, nullptr, 0);
        int32_t geosetIDRead = SFileReadFile(file, &this->m_geosetID, sizeof(this->m_geosetID), nullptr, nullptr, 0);
        int32_t showscalpRead = SFileReadFile(file, &this->m_showscalp, sizeof(this->m_showscalp), nullptr, nullptr, 0);

        if (!idRead
            | !raceIDRead
            | !sexIDRead
            | !variationIDRead
            | !geosetIDRead
            | !showscalpRead) {
            return false;
        }

        return true;
    }
};

#endif
