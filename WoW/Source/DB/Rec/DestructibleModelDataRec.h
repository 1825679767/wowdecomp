#ifndef WOW_SOURCE_DB_REC_DESTRUCTIBLEMODELDATAREC_H
#define WOW_SOURCE_DB_REC_DESTRUCTIBLEMODELDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DestructibleModelDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_state0ImpactEffectDoodadSet;
    /* 0008 */ int32_t m_state0AmbientDoodadSet;
    /* 000C */ int32_t m_state1WMO;
    /* 0010 */ int32_t m_state1DestructionDoodadSet;
    /* 0014 */ int32_t m_state1ImpactEffectDoodadSet;
    /* 0018 */ int32_t m_state1AmbientDoodadSet;
    /* 001C */ int32_t m_state2WMO;
    /* 0020 */ int32_t m_state2DestructionDoodadSet;
    /* 0024 */ int32_t m_state2ImpactEffectDoodadSet;
    /* 0028 */ int32_t m_state2AmbientDoodadSet;
    /* 002C */ int32_t m_state3WMO;
    /* 0030 */ int32_t m_state3InitDoodadSet;
    /* 0034 */ int32_t m_state3AmbientDoodadSet;
    /* 0038 */ int32_t m_ejectDirection;
    /* 003C */ int32_t m_repairGroundFx;
    /* 0040 */ int32_t m_doNotHighlight;
    /* 0044 */ int32_t m_healEffect;
    /* 0048 */ int32_t m_healEffectSpeed;

// VERIFY: 095bb469 auto
    // OFFSET: 0x008AE760
    static const char* GetFilename() {
        return "DBFilesClient\\DestructibleModelData.dbc";
    }

// VERIFY: 2f6e77ee auto
    // OFFSET: 0x008AE770
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t state0ImpactEffectDoodadSetRead = SFileReadFile(file, &this->m_state0ImpactEffectDoodadSet, sizeof(this->m_state0ImpactEffectDoodadSet), nullptr, nullptr, 0);
        int32_t state0AmbientDoodadSetRead = SFileReadFile(file, &this->m_state0AmbientDoodadSet, sizeof(this->m_state0AmbientDoodadSet), nullptr, nullptr, 0);
        int32_t state1WMORead = SFileReadFile(file, &this->m_state1WMO, sizeof(this->m_state1WMO), nullptr, nullptr, 0);
        int32_t state1DestructionDoodadSetRead = SFileReadFile(file, &this->m_state1DestructionDoodadSet, sizeof(this->m_state1DestructionDoodadSet), nullptr, nullptr, 0);
        int32_t state1ImpactEffectDoodadSetRead = SFileReadFile(file, &this->m_state1ImpactEffectDoodadSet, sizeof(this->m_state1ImpactEffectDoodadSet), nullptr, nullptr, 0);
        int32_t state1AmbientDoodadSetRead = SFileReadFile(file, &this->m_state1AmbientDoodadSet, sizeof(this->m_state1AmbientDoodadSet), nullptr, nullptr, 0);
        int32_t state2WMORead = SFileReadFile(file, &this->m_state2WMO, sizeof(this->m_state2WMO), nullptr, nullptr, 0);
        int32_t state2DestructionDoodadSetRead = SFileReadFile(file, &this->m_state2DestructionDoodadSet, sizeof(this->m_state2DestructionDoodadSet), nullptr, nullptr, 0);
        int32_t state2ImpactEffectDoodadSetRead = SFileReadFile(file, &this->m_state2ImpactEffectDoodadSet, sizeof(this->m_state2ImpactEffectDoodadSet), nullptr, nullptr, 0);
        int32_t state2AmbientDoodadSetRead = SFileReadFile(file, &this->m_state2AmbientDoodadSet, sizeof(this->m_state2AmbientDoodadSet), nullptr, nullptr, 0);
        int32_t state3WMORead = SFileReadFile(file, &this->m_state3WMO, sizeof(this->m_state3WMO), nullptr, nullptr, 0);
        int32_t state3InitDoodadSetRead = SFileReadFile(file, &this->m_state3InitDoodadSet, sizeof(this->m_state3InitDoodadSet), nullptr, nullptr, 0);
        int32_t state3AmbientDoodadSetRead = SFileReadFile(file, &this->m_state3AmbientDoodadSet, sizeof(this->m_state3AmbientDoodadSet), nullptr, nullptr, 0);
        int32_t ejectDirectionRead = SFileReadFile(file, &this->m_ejectDirection, sizeof(this->m_ejectDirection), nullptr, nullptr, 0);
        int32_t repairGroundFxRead = SFileReadFile(file, &this->m_repairGroundFx, sizeof(this->m_repairGroundFx), nullptr, nullptr, 0);
        int32_t doNotHighlightRead = SFileReadFile(file, &this->m_doNotHighlight, sizeof(this->m_doNotHighlight), nullptr, nullptr, 0);
        int32_t healEffectRead = SFileReadFile(file, &this->m_healEffect, sizeof(this->m_healEffect), nullptr, nullptr, 0);
        int32_t healEffectSpeedRead = SFileReadFile(file, &this->m_healEffectSpeed, sizeof(this->m_healEffectSpeed), nullptr, nullptr, 0);

        if (!idRead
            | !state0ImpactEffectDoodadSetRead
            | !state0AmbientDoodadSetRead
            | !state1WMORead
            | !state1DestructionDoodadSetRead
            | !state1ImpactEffectDoodadSetRead
            | !state1AmbientDoodadSetRead
            | !state2WMORead
            | !state2DestructionDoodadSetRead
            | !state2ImpactEffectDoodadSetRead
            | !state2AmbientDoodadSetRead
            | !state3WMORead
            | !state3InitDoodadSetRead
            | !state3AmbientDoodadSetRead
            | !ejectDirectionRead
            | !repairGroundFxRead
            | !doNotHighlightRead
            | !healEffectRead
            | !healEffectSpeedRead) {
            return false;
        }

        return true;
    }
};

#endif
