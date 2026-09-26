#ifndef WOW_SOURCE_DB_REC_EMOTESTEXTSOUNDREC_H
#define WOW_SOURCE_DB_REC_EMOTESTEXTSOUNDREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct EmotesTextSoundRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_emotesTextID;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sexID;
    /* 0010 */ int32_t m_soundID;

// VERIFY: 68484b9e auto
    // OFFSET: 0x008AEDE0
    static const char* GetFilename() {
        return "DBFilesClient\\EmotesTextSound.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t emotesTextIDRead = SFileReadFile(file, &this->m_emotesTextID, sizeof(this->m_emotesTextID), nullptr, nullptr, 0);
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t sexIDRead = SFileReadFile(file, &this->m_sexID, sizeof(this->m_sexID), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);

        if (!idRead
            | !emotesTextIDRead
            | !raceIDRead
            | !sexIDRead
            | !soundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
