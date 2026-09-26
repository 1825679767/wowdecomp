#ifndef WOW_SOURCE_DB_REC_TERRAINTYPEREC_H
#define WOW_SOURCE_DB_REC_TERRAINTYPEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TerrainTypeRec {
    /* 0000 */ int32_t m_terrainID;
    /* 0004 */ const char* m_terrainDesc;
    /* 0008 */ int32_t m_footstepSprayRun;
    /* 000C */ int32_t m_footstepSprayWalk;
    /* 0010 */ int32_t m_soundID;
    /* 0014 */ int32_t m_flags;
    /* 0018 */ int32_t m_generatedID;

// VERIFY: 5c706a10 auto
    // OFFSET: 0x008BA1A0
    static const char* GetFilename() {
        return "DBFilesClient\\TerrainType.dbc";
    }

// VERIFY: 0e6d9328 manual
    // OFFSET: 0x008BA1B0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t terrainDescOffset;

        int32_t terrainIDRead = SFileReadFile(file, &this->m_terrainID, sizeof(this->m_terrainID), nullptr, nullptr, 0);
        int32_t terrainDescRead = SFileReadFile(file, &terrainDescOffset, sizeof(terrainDescOffset), nullptr, nullptr, 0);
        int32_t footstepSprayRunRead = SFileReadFile(file, &this->m_footstepSprayRun, sizeof(this->m_footstepSprayRun), nullptr, nullptr, 0);
        int32_t footstepSprayWalkRead = SFileReadFile(file, &this->m_footstepSprayWalk, sizeof(this->m_footstepSprayWalk), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!terrainIDRead
            | !terrainDescRead
            | !footstepSprayRunRead
            | !footstepSprayWalkRead
            | !soundIDRead
            | !flagsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_terrainDesc = &stringBuffer[terrainDescOffset];
        } else {
            this->m_terrainDesc = "";
        }

        return true;
    }
};

#endif
