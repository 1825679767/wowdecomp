#ifndef WOW_SOURCE_DB_REC_TERRAINTYPESOUNDSREC_H
#define WOW_SOURCE_DB_REC_TERRAINTYPESOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TerrainTypeSoundsRec {
    /* 0000 */ int32_t m_id;

// VERIFY: 3a32e0ef auto
    // OFFSET: 0x008BA2B0
    static const char* GetFilename() {
        return "DBFilesClient\\TerrainTypeSounds.dbc";
    }

// VERIFY: 5c325423 auto
    // OFFSET: 0x008AFFD0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);

        if (!idRead) {
            return false;
        }

        return true;
    }
};

#endif
