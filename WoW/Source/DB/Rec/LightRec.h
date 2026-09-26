#ifndef WOW_SOURCE_DB_REC_LIGHTREC_H
#define WOW_SOURCE_DB_REC_LIGHTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LightRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_gameCoords [3];
    /* 0014 */ float m_gameFalloffStart;
    /* 0018 */ float m_gameFalloffEnd;
    /* 001C */ int32_t m_lightParamsID [8];

// VERIFY: e0a0b2ce auto
    // OFFSET: 0x008BDFE0
    static const char* GetFilename() {
        return "DBFilesClient\\Light.dbc";
    }

// VERIFY: a9320d0c manual
    // OFFSET: 0x008BDFF0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t continentIDRead = SFileReadFile(file, &this->m_continentID, sizeof(this->m_continentID), nullptr, nullptr, 0);
        int32_t gameCoordsRead0 = SFileReadFile(file, &this->m_gameCoords[0], sizeof(this->m_gameCoords[0]), nullptr, nullptr, 0);
        int32_t gameCoordsRead1 = SFileReadFile(file, &this->m_gameCoords[1], sizeof(this->m_gameCoords[1]), nullptr, nullptr, 0);
        int32_t gameCoordsRead2 = SFileReadFile(file, &this->m_gameCoords[2], sizeof(this->m_gameCoords[2]), nullptr, nullptr, 0);
        int32_t gameFalloffStartRead = SFileReadFile(file, &this->m_gameFalloffStart, sizeof(this->m_gameFalloffStart), nullptr, nullptr, 0);
        int32_t gameFalloffEndRead = SFileReadFile(file, &this->m_gameFalloffEnd, sizeof(this->m_gameFalloffEnd), nullptr, nullptr, 0);
        int32_t lightParamsIDRead = SFileReadFile(file, this->m_lightParamsID, sizeof(this->m_lightParamsID), nullptr, nullptr, 0);

        if (!idRead
            | !continentIDRead
            | !gameCoordsRead0
            | !gameCoordsRead1
            | !gameCoordsRead2
            | !gameFalloffStartRead
            | !gameFalloffEndRead
            | !lightParamsIDRead) {
            return false;
        }

        return true;
    }
};

#endif
