#ifndef WOW_SOURCE_DB_REC_CREATUREMOVEMENTINFOREC_H
#define WOW_SOURCE_DB_REC_CREATUREMOVEMENTINFOREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CreatureMovementInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_smoothFacingChaseRate;

// VERIFY: 6b161e3f auto
    // OFFSET: 0x008ADBA0
    static const char* GetFilename() {
        return "DBFilesClient\\CreatureMovementInfo.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t smoothFacingChaseRateRead = SFileReadFile(file, &this->m_smoothFacingChaseRate, sizeof(this->m_smoothFacingChaseRate), nullptr, nullptr, 0);

        if (!idRead
            | !smoothFacingChaseRateRead) {
            return false;
        }

        return true;
    }
};

#endif
