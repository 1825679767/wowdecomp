#ifndef WOW_SOURCE_DB_REC_STATIONERYREC_H
#define WOW_SOURCE_DB_REC_STATIONERYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct StationeryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ const char* m_texture;
    /* 000C */ int32_t m_flags;

// VERIFY: 5bc43f5a auto
    // OFFSET: 0x008B97F0
    static const char* GetFilename() {
        return "DBFilesClient\\Stationery.dbc";
    }

// VERIFY: d8ebece1 manual
    // OFFSET: 0x008B9800
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textureOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t itemIDRead = SFileReadFile(file, &this->m_itemID, sizeof(this->m_itemID), nullptr, nullptr, 0);
        int32_t textureRead = SFileReadFile(file, &textureOffset, sizeof(textureOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !itemIDRead
            | !textureRead
            | !flagsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_texture = &stringBuffer[textureOffset];
        } else {
            this->m_texture = "";
        }

        return true;
    }
};

#endif
