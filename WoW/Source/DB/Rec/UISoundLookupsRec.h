#ifndef WOW_SOURCE_DB_REC_UISOUNDLOOKUPSREC_H
#define WOW_SOURCE_DB_REC_UISOUNDLOOKUPSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct UISoundLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID;
    /* 0008 */ const char* m_soundName;

// VERIFY: a3c7012f auto
    // OFFSET: 0x008BA730
    static const char* GetFilename() {
        return "DBFilesClient\\UISoundLookups.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t soundNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t soundNameRead = SFileReadFile(file, &soundNameOffset, sizeof(soundNameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !soundIDRead
            | !soundNameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_soundName = &stringBuffer[soundNameOffset];
        } else {
            this->m_soundName = "";
        }

        return true;
    }
};

#endif
