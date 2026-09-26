#ifndef WOW_SOURCE_DB_REC_DUNGEONMAPREC_H
#define WOW_SOURCE_DB_REC_DUNGEONMAPREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DungeonMapRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_floorIndex;
    /* 000C */ float m_minX;
    /* 0010 */ float m_maxX;
    /* 0014 */ float m_minY;
    /* 0018 */ float m_maxY;
    /* 001C */ int32_t m_parentWorldMapID;

// VERIFY: 57ed013e auto
    // OFFSET: 0x008AED10
    static const char* GetFilename() {
        return "DBFilesClient\\DungeonMap.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t mapIDRead = SFileReadFile(file, &this->m_mapID, sizeof(this->m_mapID), nullptr, nullptr, 0);
        int32_t floorIndexRead = SFileReadFile(file, &this->m_floorIndex, sizeof(this->m_floorIndex), nullptr, nullptr, 0);
        int32_t minXRead = SFileReadFile(file, &this->m_minX, sizeof(this->m_minX), nullptr, nullptr, 0);
        int32_t maxXRead = SFileReadFile(file, &this->m_maxX, sizeof(this->m_maxX), nullptr, nullptr, 0);
        int32_t minYRead = SFileReadFile(file, &this->m_minY, sizeof(this->m_minY), nullptr, nullptr, 0);
        int32_t maxYRead = SFileReadFile(file, &this->m_maxY, sizeof(this->m_maxY), nullptr, nullptr, 0);
        int32_t parentWorldMapIDRead = SFileReadFile(file, &this->m_parentWorldMapID, sizeof(this->m_parentWorldMapID), nullptr, nullptr, 0);

        if (!idRead
            | !mapIDRead
            | !floorIndexRead
            | !minXRead
            | !maxXRead
            | !minYRead
            | !maxYRead
            | !parentWorldMapIDRead) {
            return false;
        }

        return true;
    }
};

#endif
