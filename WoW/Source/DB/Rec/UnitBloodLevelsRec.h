#ifndef WOW_SOURCE_DB_REC_UNITBLOODLEVELSREC_H
#define WOW_SOURCE_DB_REC_UNITBLOODLEVELSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct UnitBloodLevelsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_violencelevel [3];

// VERIFY: fbb9e44d auto
    // OFFSET: 0x008BA740
    static const char* GetFilename() {
        return "DBFilesClient\\UnitBloodLevels.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t violencelevelRead = SFileReadFile(file, this->m_violencelevel, sizeof(this->m_violencelevel), nullptr, nullptr, 0);

        if (!idRead
            | !violencelevelRead) {
            return false;
        }

        return true;
    }
};

#endif
