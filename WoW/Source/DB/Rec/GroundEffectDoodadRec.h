#ifndef WOW_SOURCE_DB_REC_GROUNDEFFECTDOODADREC_H
#define WOW_SOURCE_DB_REC_GROUNDEFFECTDOODADREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GroundEffectDoodadRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_doodadpath;
    /* 0008 */ int32_t m_flags;

// VERIFY: 7ce1be8f auto
    // OFFSET: 0x008AFEB0
    static const char* GetFilename() {
        return "DBFilesClient\\GroundEffectDoodad.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t doodadpathOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t doodadpathRead = SFileReadFile(file, &doodadpathOffset, sizeof(doodadpathOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !doodadpathRead
            | !flagsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_doodadpath = &stringBuffer[doodadpathOffset];
        } else {
            this->m_doodadpath = "";
        }

        return true;
    }
};

#endif
