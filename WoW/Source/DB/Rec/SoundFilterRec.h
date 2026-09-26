#ifndef WOW_SOURCE_DB_REC_SOUNDFILTERREC_H
#define WOW_SOURCE_DB_REC_SOUNDFILTERREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundFilterRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;

// VERIFY: 0bf505c0 auto
    // OFFSET: 0x008BD7E0
    static const char* GetFilename() {
        return "DBFilesClient\\SoundFilter.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead) {
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
