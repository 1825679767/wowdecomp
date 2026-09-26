#ifndef WOW_SOURCE_DB_REC_DEATHTHUDLOOKUPSREC_H
#define WOW_SOURCE_DB_REC_DEATHTHUDLOOKUPSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DeathThudLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sizeClass;
    /* 0008 */ int32_t m_terrainTypeSoundID;
    /* 000C */ int32_t m_soundEntryID;
    /* 0010 */ int32_t m_soundEntryIdwater;

// VERIFY: 9ce0d8f0 auto
    // OFFSET: 0x008AE670
    static const char* GetFilename() {
        return "DBFilesClient\\DeathThudLookups.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t sizeClassRead = SFileReadFile(file, &this->m_sizeClass, sizeof(this->m_sizeClass), nullptr, nullptr, 0);
        int32_t terrainTypeSoundIDRead = SFileReadFile(file, &this->m_terrainTypeSoundID, sizeof(this->m_terrainTypeSoundID), nullptr, nullptr, 0);
        int32_t soundEntryIDRead = SFileReadFile(file, &this->m_soundEntryID, sizeof(this->m_soundEntryID), nullptr, nullptr, 0);
        int32_t soundEntryIdwaterRead = SFileReadFile(file, &this->m_soundEntryIdwater, sizeof(this->m_soundEntryIdwater), nullptr, nullptr, 0);

        if (!idRead
            | !sizeClassRead
            | !terrainTypeSoundIDRead
            | !soundEntryIDRead
            | !soundEntryIdwaterRead) {
            return false;
        }

        return true;
    }
};

#endif
