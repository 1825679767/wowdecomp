#ifndef WOW_SOURCE_DB_REC_ENVIRONMENTALDAMAGEREC_H
#define WOW_SOURCE_DB_REC_ENVIRONMENTALDAMAGEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct EnvironmentalDamageRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_enumID;
    /* 0008 */ int32_t m_visualkitID;

// VERIFY: 2c3e47b5 auto
    // OFFSET: 0x008AEEC0
    static const char* GetFilename() {
        return "DBFilesClient\\EnvironmentalDamage.dbc";
    }

// VERIFY: 5d36b411 manual
    // OFFSET: 0x008B2AA0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t enumIDRead = SFileReadFile(file, &this->m_enumID, sizeof(this->m_enumID), nullptr, nullptr, 0);
        int32_t visualkitIDRead = SFileReadFile(file, &this->m_visualkitID, sizeof(this->m_visualkitID), nullptr, nullptr, 0);

        if (!idRead
            | !enumIDRead
            | !visualkitIDRead) {
            return false;
        }

        return true;
    }
};

#endif
