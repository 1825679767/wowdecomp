#ifndef WOW_SOURCE_DB_REC_SOUNDAMBIENCEREC_H
#define WOW_SOURCE_DB_REC_SOUNDAMBIENCEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundAmbienceRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID [2];

// VERIFY: 39ec4509 auto
    // OFFSET: 0x008B67C0
    static const char* GetFilename() {
        return "DBFilesClient\\SoundAmbience.dbc";
    }

// VERIFY: 46e303c1 auto
    // OFFSET: 0x008B67D0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t ambienceIDRead = SFileReadFile(file, this->m_ambienceID, sizeof(this->m_ambienceID), nullptr, nullptr, 0);

        if (!idRead
            | !ambienceIDRead) {
            return false;
        }

        return true;
    }
};

#endif
