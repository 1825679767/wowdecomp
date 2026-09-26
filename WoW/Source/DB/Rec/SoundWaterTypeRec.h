#ifndef WOW_SOURCE_DB_REC_SOUNDWATERTYPEREC_H
#define WOW_SOURCE_DB_REC_SOUNDWATERTYPEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundWaterTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ int32_t m_soundSubtype;
    /* 000C */ int32_t m_soundID;

// VERIFY: bc31c322 auto
    // OFFSET: 0x008B7280
    static const char* GetFilename() {
        return "DBFilesClient\\SoundWaterType.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundTypeRead = SFileReadFile(file, &this->m_soundType, sizeof(this->m_soundType), nullptr, nullptr, 0);
        int32_t soundSubtypeRead = SFileReadFile(file, &this->m_soundSubtype, sizeof(this->m_soundSubtype), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);

        if (!idRead
            | !soundTypeRead
            | !soundSubtypeRead
            | !soundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
