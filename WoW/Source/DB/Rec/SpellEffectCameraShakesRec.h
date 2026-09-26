#ifndef WOW_SOURCE_DB_REC_SPELLEFFECTCAMERASHAKESREC_H
#define WOW_SOURCE_DB_REC_SPELLEFFECTCAMERASHAKESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellEffectCameraShakesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cameraShake [3];

// VERIFY: bafc8b16 auto
    // OFFSET: 0x008B7D90
    static const char* GetFilename() {
        return "DBFilesClient\\SpellEffectCameraShakes.dbc";
    }

// VERIFY: 5ebdd3b2 auto
    // OFFSET: 0x008BA750
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t cameraShakeRead = SFileReadFile(file, this->m_cameraShake, sizeof(this->m_cameraShake), nullptr, nullptr, 0);

        if (!idRead
            | !cameraShakeRead) {
            return false;
        }

        return true;
    }
};

#endif
