#ifndef WOW_SOURCE_DB_REC_WORLDMAPTRANSFORMSREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPTRANSFORMSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WorldMapTransformsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ float m_regionMin [2];
    /* 0010 */ float m_regionMax [2];
    /* 0018 */ int32_t m_newMapID;
    /* 001C */ float m_regionOffset [2];
    /* 0024 */ int32_t m_newDungeonMapID;

// VERIFY: 29f8801f auto
    // OFFSET: 0x008BBD90
    static const char* GetFilename() {
        return "DBFilesClient\\WorldMapTransforms.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t mapIDRead = SFileReadFile(file, &this->m_mapID, sizeof(this->m_mapID), nullptr, nullptr, 0);
        int32_t regionMinRead0 = SFileReadFile(file, &this->m_regionMin[0], sizeof(this->m_regionMin[0]), nullptr, nullptr, 0);
        int32_t regionMinRead1 = SFileReadFile(file, &this->m_regionMin[1], sizeof(this->m_regionMin[1]), nullptr, nullptr, 0);
        int32_t regionMaxRead0 = SFileReadFile(file, &this->m_regionMax[0], sizeof(this->m_regionMax[0]), nullptr, nullptr, 0);
        int32_t regionMaxRead1 = SFileReadFile(file, &this->m_regionMax[1], sizeof(this->m_regionMax[1]), nullptr, nullptr, 0);
        int32_t newMapIDRead = SFileReadFile(file, &this->m_newMapID, sizeof(this->m_newMapID), nullptr, nullptr, 0);
        int32_t regionRead0 = SFileReadFile(file, &this->m_regionOffset[0], sizeof(this->m_regionOffset[0]), nullptr, nullptr, 0);
        int32_t regionRead1 = SFileReadFile(file, &this->m_regionOffset[1], sizeof(this->m_regionOffset[1]), nullptr, nullptr, 0);
        int32_t newDungeonMapIDRead = SFileReadFile(file, &this->m_newDungeonMapID, sizeof(this->m_newDungeonMapID), nullptr, nullptr, 0);

        if (!idRead
            | !mapIDRead
            | !regionMinRead0
            | !regionMinRead1
            | !regionMaxRead0
            | !regionMaxRead1
            | !newMapIDRead
            | !regionRead0
            | !regionRead1
            | !newDungeonMapIDRead) {
            return false;
        }

        return true;
    }
};

#endif
