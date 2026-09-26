#ifndef WOW_SOURCE_DB_REC_DUNGEONMAPCHUNKREC_H
#define WOW_SOURCE_DB_REC_DUNGEONMAPCHUNKREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DungeonMapChunkRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_wmogroupID;
    /* 000C */ int32_t m_dungeonMapID;
    /* 0010 */ float m_minZ;

// VERIFY: 68602426 auto
    // OFFSET: 0x008AED20
    static const char* GetFilename() {
        return "DBFilesClient\\DungeonMapChunk.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t mapIDRead = SFileReadFile(file, &this->m_mapID, sizeof(this->m_mapID), nullptr, nullptr, 0);
        int32_t wmogroupIDRead = SFileReadFile(file, &this->m_wmogroupID, sizeof(this->m_wmogroupID), nullptr, nullptr, 0);
        int32_t dungeonMapIDRead = SFileReadFile(file, &this->m_dungeonMapID, sizeof(this->m_dungeonMapID), nullptr, nullptr, 0);
        int32_t minZRead = SFileReadFile(file, &this->m_minZ, sizeof(this->m_minZ), nullptr, nullptr, 0);

        if (!idRead
            | !mapIDRead
            | !wmogroupIDRead
            | !dungeonMapIDRead
            | !minZRead) {
            return false;
        }

        return true;
    }
};

#endif
