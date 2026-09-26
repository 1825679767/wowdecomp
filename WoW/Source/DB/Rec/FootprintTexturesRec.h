#ifndef WOW_SOURCE_DB_REC_FOOTPRINTTEXTURESREC_H
#define WOW_SOURCE_DB_REC_FOOTPRINTTEXTURESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct FootprintTexturesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_footstepFilename;

// VERIFY: 0ad2cb8c auto
    // OFFSET: 0x008AF9E0
    static const char* GetFilename() {
        return "DBFilesClient\\FootprintTextures.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t footstepFilenameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t footstepFilenameRead = SFileReadFile(file, &footstepFilenameOffset, sizeof(footstepFilenameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !footstepFilenameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_footstepFilename = &stringBuffer[footstepFilenameOffset];
        } else {
            this->m_footstepFilename = "";
        }

        return true;
    }
};

#endif
