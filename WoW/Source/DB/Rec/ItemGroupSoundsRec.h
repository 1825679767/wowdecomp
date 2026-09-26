#ifndef WOW_SOURCE_DB_REC_ITEMGROUPSOUNDSREC_H
#define WOW_SOURCE_DB_REC_ITEMGROUPSOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemGroupSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sound [4];

// VERIFY: 0154cdb8 auto
    // OFFSET: 0x008B0420
    static const char* GetFilename() {
        return "DBFilesClient\\ItemGroupSounds.dbc";
    }

// VERIFY: bce629b6 auto
    // OFFSET: 0x008B48F0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundRead = SFileReadFile(file, this->m_sound, sizeof(this->m_sound), nullptr, nullptr, 0);

        if (!idRead
            | !soundRead) {
            return false;
        }

        return true;
    }
};

#endif
