#ifndef WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H
#define WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct VocalUISoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vocalUienum;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_normalSoundID [2];
    /* 0014 */ int32_t m_pissedSoundID [2];

// VERIFY: d99a87d8 auto
    // OFFSET: 0x008BB560
    static const char* GetFilename() {
        return "DBFilesClient\\VocalUISounds.dbc";
    }

// VERIFY: b550917e manual
    // OFFSET: 0x008BB570
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t vocalUienumRead = SFileReadFile(file, &this->m_vocalUienum, sizeof(this->m_vocalUienum), nullptr, nullptr, 0);
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t normalSoundIDRead = SFileReadFile(file, this->m_normalSoundID, sizeof(this->m_normalSoundID), nullptr, nullptr, 0);
        int32_t pissedSoundIDRead = SFileReadFile(file, this->m_pissedSoundID, sizeof(this->m_pissedSoundID), nullptr, nullptr, 0);

        if (!idRead
            | !vocalUienumRead
            | !raceIDRead
            | !normalSoundIDRead
            | !pissedSoundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
