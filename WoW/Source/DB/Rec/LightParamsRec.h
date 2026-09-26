#ifndef WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H
#define WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LightParamsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_highlightSky;
    /* 0008 */ int32_t m_lightSkyboxID;
    /* 000C */ float m_glow;
    /* 0010 */ float m_waterShallowAlpha;
    /* 0014 */ float m_waterDeepAlpha;
    /* 0018 */ float m_oceanShallowAlpha;
    /* 001C */ float m_oceanDeepAlpha;
    /* 0020 */ int32_t m_flags;

// VERIFY: 629eaa22 auto
    // OFFSET: 0x008BDFD0
    static const char* GetFilename() {
        return "DBFilesClient\\LightParams.dbc";
    }

// VERIFY: d0b3989b manual
    // OFFSET: 0x008BD0C0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t highlightSkyRead = SFileReadFile(file, &this->m_highlightSky, sizeof(this->m_highlightSky), nullptr, nullptr, 0);
        int32_t lightSkyboxIDRead = SFileReadFile(file, &this->m_lightSkyboxID, sizeof(this->m_lightSkyboxID), nullptr, nullptr, 0);
        int32_t glowRead = SFileReadFile(file, &this->m_glow, sizeof(this->m_glow), nullptr, nullptr, 0);
        int32_t waterShallowAlphaRead = SFileReadFile(file, &this->m_waterShallowAlpha, sizeof(this->m_waterShallowAlpha), nullptr, nullptr, 0);
        int32_t waterDeepAlphaRead = SFileReadFile(file, &this->m_waterDeepAlpha, sizeof(this->m_waterDeepAlpha), nullptr, nullptr, 0);
        int32_t oceanShallowAlphaRead = SFileReadFile(file, &this->m_oceanShallowAlpha, sizeof(this->m_oceanShallowAlpha), nullptr, nullptr, 0);
        int32_t oceanDeepAlphaRead = SFileReadFile(file, &this->m_oceanDeepAlpha, sizeof(this->m_oceanDeepAlpha), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !highlightSkyRead
            | !lightSkyboxIDRead
            | !glowRead
            | !waterShallowAlphaRead
            | !waterDeepAlphaRead
            | !oceanShallowAlphaRead
            | !oceanDeepAlphaRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
