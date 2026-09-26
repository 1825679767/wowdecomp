#ifndef WOW_SOURCE_DB_REC_WEATHERREC_H
#define WOW_SOURCE_DB_REC_WEATHERREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WeatherRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID;
    /* 0008 */ int32_t m_effectType;
    /* 000C */ float m_transitionSkyBox;
    /* 0010 */ float m_effectColor [3];
    /* 001C */ const char* m_effectTexture;

// VERIFY: 4d10daef auto
    // OFFSET: 0x008BB6F0
    static const char* GetFilename() {
        return "DBFilesClient\\Weather.dbc";
    }

// VERIFY: 085f87c1 manual
    // OFFSET: 0x008BB700
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t effectTextureOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t ambienceIDRead = SFileReadFile(file, &this->m_ambienceID, sizeof(this->m_ambienceID), nullptr, nullptr, 0);
        int32_t effectTypeRead = SFileReadFile(file, &this->m_effectType, sizeof(this->m_effectType), nullptr, nullptr, 0);
        int32_t transitionSkyBoxRead = SFileReadFile(file, &this->m_transitionSkyBox, sizeof(this->m_transitionSkyBox), nullptr, nullptr, 0);
        int32_t effectColorRead = SFileReadFile(file, this->m_effectColor, sizeof(this->m_effectColor), nullptr, nullptr, 0);
        int32_t effectTextureRead = SFileReadFile(file, &effectTextureOffset, sizeof(effectTextureOffset), nullptr, nullptr, 0);

        if (!idRead
            | !ambienceIDRead
            | !effectTypeRead
            | !transitionSkyBoxRead
            | !effectColorRead
            | !effectTextureRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_effectTexture = &stringBuffer[effectTextureOffset];
        } else {
            this->m_effectTexture = "";
        }

        return true;
    }
};

#endif
