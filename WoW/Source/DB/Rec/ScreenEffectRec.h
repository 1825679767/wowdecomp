#ifndef WOW_SOURCE_DB_REC_SCREENEFFECTREC_H
#define WOW_SOURCE_DB_REC_SCREENEFFECTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ScreenEffectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_effect;
    /* 000C */ int32_t m_param [4];
    /* 001C */ int32_t m_lightParamsID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_zoneMusicID;

// VERIFY: 4837ceb4 auto
    // OFFSET: 0x008B5A30
    static const char* GetFilename() {
        return "DBFilesClient\\ScreenEffect.dbc";
    }

// VERIFY: 1b398cc5 manual
    // OFFSET: 0x008B5A40
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t effectRead = SFileReadFile(file, &this->m_effect, sizeof(this->m_effect), nullptr, nullptr, 0);
        int32_t paramRead = SFileReadFile(file, this->m_param, sizeof(this->m_param), nullptr, nullptr, 0);
        int32_t lightParamsIDRead = SFileReadFile(file, &this->m_lightParamsID, sizeof(this->m_lightParamsID), nullptr, nullptr, 0);
        int32_t soundAmbienceIDRead = SFileReadFile(file, &this->m_soundAmbienceID, sizeof(this->m_soundAmbienceID), nullptr, nullptr, 0);
        int32_t zoneMusicIDRead = SFileReadFile(file, &this->m_zoneMusicID, sizeof(this->m_zoneMusicID), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !effectRead
            | !paramRead
            | !lightParamsIDRead
            | !soundAmbienceIDRead
            | !zoneMusicIDRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
