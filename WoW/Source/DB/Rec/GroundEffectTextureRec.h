#ifndef WOW_SOURCE_DB_REC_GROUNDEFFECTTEXTUREREC_H
#define WOW_SOURCE_DB_REC_GROUNDEFFECTTEXTUREREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GroundEffectTextureRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_doodadID [4];
    /* 0014 */ int32_t m_doodadWeight [4];
    /* 0024 */ int32_t m_density;
    /* 0028 */ int32_t m_sound;

// VERIFY: ceadfa6b auto
    // OFFSET: 0x008AFEC0
    static const char* GetFilename() {
        return "DBFilesClient\\GroundEffectTexture.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t doodadIDRead = SFileReadFile(file, this->m_doodadID, sizeof(this->m_doodadID), nullptr, nullptr, 0);
        int32_t doodadWeightRead = SFileReadFile(file, this->m_doodadWeight, sizeof(this->m_doodadWeight), nullptr, nullptr, 0);
        int32_t densityRead = SFileReadFile(file, &this->m_density, sizeof(this->m_density), nullptr, nullptr, 0);
        int32_t soundRead = SFileReadFile(file, &this->m_sound, sizeof(this->m_sound), nullptr, nullptr, 0);

        if (!idRead
            | !doodadIDRead
            | !doodadWeightRead
            | !densityRead
            | !soundRead) {
            return false;
        }

        return true;
    }
};

#endif
