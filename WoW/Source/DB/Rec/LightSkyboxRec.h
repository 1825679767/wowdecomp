#ifndef WOW_SOURCE_DB_REC_LIGHTSKYBOXREC_H
#define WOW_SOURCE_DB_REC_LIGHTSKYBOXREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LightSkyboxRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags;

// VERIFY: 72e85ebb auto
    // OFFSET: 0x008BDFC0
    static const char* GetFilename() {
        return "DBFilesClient\\LightSkybox.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !flagsRead) {
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
