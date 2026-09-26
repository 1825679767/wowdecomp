#ifndef WOW_SOURCE_DB_REC_NPCSOUNDSREC_H
#define WOW_SOURCE_DB_REC_NPCSOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct NPCSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID [4];

// VERIFY: 5435c4c2 auto
    // OFFSET: 0x008B48E0
    static const char* GetFilename() {
        return "DBFilesClient\\NPCSounds.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);

        if (!idRead
            | !soundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
