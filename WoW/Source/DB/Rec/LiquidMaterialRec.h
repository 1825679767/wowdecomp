#ifndef WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H
#define WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LiquidMaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lvf;
    /* 0008 */ int32_t m_flags;

// VERIFY: 5a678383 auto
    // OFFSET: 0x008B2A90
    static const char* GetFilename() {
        return "DBFilesClient\\LiquidMaterial.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t lvfRead = SFileReadFile(file, &this->m_lvf, sizeof(this->m_lvf), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !lvfRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
