#ifndef WOW_SOURCE_DB_REC_AREATRIGGERREC_H
#define WOW_SOURCE_DB_REC_AREATRIGGERREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct AreaTriggerRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ float m_radius;
    /* 0018 */ float m_boxLength;
    /* 001C */ float m_boxWidth;
    /* 0020 */ float m_boxHeight;
    /* 0024 */ float m_boxYaw;

// VERIFY: cd93aa47 auto
    // OFFSET: 0x008AA0E0
    static const char* GetFilename() {
        return "DBFilesClient\\AreaTrigger.dbc";
    }

// VERIFY: 620fe2ba auto
    // OFFSET: 0x008BBDA0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t continentIDRead = SFileReadFile(file, &this->m_continentID, sizeof(this->m_continentID), nullptr, nullptr, 0);
        int32_t posRead0 = SFileReadFile(file, &this->m_pos[0], sizeof(this->m_pos[0]), nullptr, nullptr, 0);
        int32_t posRead1 = SFileReadFile(file, &this->m_pos[1], sizeof(this->m_pos[1]), nullptr, nullptr, 0);
        int32_t posRead2 = SFileReadFile(file, &this->m_pos[2], sizeof(this->m_pos[2]), nullptr, nullptr, 0);
        int32_t radiusRead = SFileReadFile(file, &this->m_radius, sizeof(this->m_radius), nullptr, nullptr, 0);
        int32_t boxLengthRead = SFileReadFile(file, &this->m_boxLength, sizeof(this->m_boxLength), nullptr, nullptr, 0);
        int32_t boxWidthRead = SFileReadFile(file, &this->m_boxWidth, sizeof(this->m_boxWidth), nullptr, nullptr, 0);
        int32_t boxHeightRead = SFileReadFile(file, &this->m_boxHeight, sizeof(this->m_boxHeight), nullptr, nullptr, 0);
        int32_t boxYawRead = SFileReadFile(file, &this->m_boxYaw, sizeof(this->m_boxYaw), nullptr, nullptr, 0);

        if (!idRead
            | !continentIDRead
            | !posRead0
            | !posRead1
            | !posRead2
            | !radiusRead
            | !boxLengthRead
            | !boxWidthRead
            | !boxHeightRead
            | !boxYawRead) {
            return false;
        }

        return true;
    }
};

#endif
