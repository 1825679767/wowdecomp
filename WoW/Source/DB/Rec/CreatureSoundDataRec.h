#ifndef WOW_SOURCE_DB_REC_CREATURESOUNDDATAREC_H
#define WOW_SOURCE_DB_REC_CREATURESOUNDDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CreatureSoundDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundExertionID;
    /* 0008 */ int32_t m_soundExertionCriticalID;
    /* 000C */ int32_t m_soundInjuryID;
    /* 0010 */ int32_t m_soundInjuryCriticalID;
    /* 0014 */ int32_t m_soundInjuryCrushingBlowID;
    /* 0018 */ int32_t m_soundDeathID;
    /* 001C */ int32_t m_soundStunID;
    /* 0020 */ int32_t m_soundStandID;
    /* 0024 */ int32_t m_soundFootstepID;
    /* 0028 */ int32_t m_soundAggroID;
    /* 002C */ int32_t m_soundWingFlapID;
    /* 0030 */ int32_t m_soundWingGlideID;
    /* 0034 */ int32_t m_soundAlertID;
    /* 0038 */ int32_t m_soundFidget [5];
    /* 004C */ int32_t m_customAttack [4];
    /* 005C */ int32_t m_npcsoundID;
    /* 0060 */ int32_t m_loopSoundID;
    /* 0064 */ int32_t m_creatureImpactType;
    /* 0068 */ int32_t m_soundJumpStartID;
    /* 006C */ int32_t m_soundJumpEndID;
    /* 0070 */ int32_t m_soundPetAttackID;
    /* 0074 */ int32_t m_soundPetOrderID;
    /* 0078 */ int32_t m_soundPetDismissID;
    /* 007C */ float m_fidgetDelaySecondsMin;
    /* 0080 */ float m_fidgetDelaySecondsMax;
    /* 0084 */ int32_t m_birthSoundID;
    /* 0088 */ int32_t m_spellCastDirectedSoundID;
    /* 008C */ int32_t m_submergeSoundID;
    /* 0090 */ int32_t m_submergedSoundID;
    /* 0094 */ int32_t m_creatureSoundDataIdpet;

// VERIFY: cc026fa2 auto
    // OFFSET: 0x008ADBB0
    static const char* GetFilename() {
        return "DBFilesClient\\CreatureSoundData.dbc";
    }

// VERIFY: b0581a09 auto
    // OFFSET: 0x008ADBC0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundExertionIDRead = SFileReadFile(file, &this->m_soundExertionID, sizeof(this->m_soundExertionID), nullptr, nullptr, 0);
        int32_t soundExertionCriticalIDRead = SFileReadFile(file, &this->m_soundExertionCriticalID, sizeof(this->m_soundExertionCriticalID), nullptr, nullptr, 0);
        int32_t soundInjuryIDRead = SFileReadFile(file, &this->m_soundInjuryID, sizeof(this->m_soundInjuryID), nullptr, nullptr, 0);
        int32_t soundInjuryCriticalIDRead = SFileReadFile(file, &this->m_soundInjuryCriticalID, sizeof(this->m_soundInjuryCriticalID), nullptr, nullptr, 0);
        int32_t soundInjuryCrushingBlowIDRead = SFileReadFile(file, &this->m_soundInjuryCrushingBlowID, sizeof(this->m_soundInjuryCrushingBlowID), nullptr, nullptr, 0);
        int32_t soundDeathIDRead = SFileReadFile(file, &this->m_soundDeathID, sizeof(this->m_soundDeathID), nullptr, nullptr, 0);
        int32_t soundStunIDRead = SFileReadFile(file, &this->m_soundStunID, sizeof(this->m_soundStunID), nullptr, nullptr, 0);
        int32_t soundStandIDRead = SFileReadFile(file, &this->m_soundStandID, sizeof(this->m_soundStandID), nullptr, nullptr, 0);
        int32_t soundFootstepIDRead = SFileReadFile(file, &this->m_soundFootstepID, sizeof(this->m_soundFootstepID), nullptr, nullptr, 0);
        int32_t soundAggroIDRead = SFileReadFile(file, &this->m_soundAggroID, sizeof(this->m_soundAggroID), nullptr, nullptr, 0);
        int32_t soundWingFlapIDRead = SFileReadFile(file, &this->m_soundWingFlapID, sizeof(this->m_soundWingFlapID), nullptr, nullptr, 0);
        int32_t soundWingGlideIDRead = SFileReadFile(file, &this->m_soundWingGlideID, sizeof(this->m_soundWingGlideID), nullptr, nullptr, 0);
        int32_t soundAlertIDRead = SFileReadFile(file, &this->m_soundAlertID, sizeof(this->m_soundAlertID), nullptr, nullptr, 0);
        int32_t soundFidgetRead = SFileReadFile(file, this->m_soundFidget, sizeof(this->m_soundFidget), nullptr, nullptr, 0);
        int32_t customAttackRead = SFileReadFile(file, this->m_customAttack, sizeof(this->m_customAttack), nullptr, nullptr, 0);
        int32_t npcsoundIDRead = SFileReadFile(file, &this->m_npcsoundID, sizeof(this->m_npcsoundID), nullptr, nullptr, 0);
        int32_t loopSoundIDRead = SFileReadFile(file, &this->m_loopSoundID, sizeof(this->m_loopSoundID), nullptr, nullptr, 0);
        int32_t creatureImpactTypeRead = SFileReadFile(file, &this->m_creatureImpactType, sizeof(this->m_creatureImpactType), nullptr, nullptr, 0);
        int32_t soundJumpStartIDRead = SFileReadFile(file, &this->m_soundJumpStartID, sizeof(this->m_soundJumpStartID), nullptr, nullptr, 0);
        int32_t soundJumpEndIDRead = SFileReadFile(file, &this->m_soundJumpEndID, sizeof(this->m_soundJumpEndID), nullptr, nullptr, 0);
        int32_t soundPetAttackIDRead = SFileReadFile(file, &this->m_soundPetAttackID, sizeof(this->m_soundPetAttackID), nullptr, nullptr, 0);
        int32_t soundPetOrderIDRead = SFileReadFile(file, &this->m_soundPetOrderID, sizeof(this->m_soundPetOrderID), nullptr, nullptr, 0);
        int32_t soundPetDismissIDRead = SFileReadFile(file, &this->m_soundPetDismissID, sizeof(this->m_soundPetDismissID), nullptr, nullptr, 0);
        int32_t fidgetDelaySecondsMinRead = SFileReadFile(file, &this->m_fidgetDelaySecondsMin, sizeof(this->m_fidgetDelaySecondsMin), nullptr, nullptr, 0);
        int32_t fidgetDelaySecondsMaxRead = SFileReadFile(file, &this->m_fidgetDelaySecondsMax, sizeof(this->m_fidgetDelaySecondsMax), nullptr, nullptr, 0);
        int32_t birthSoundIDRead = SFileReadFile(file, &this->m_birthSoundID, sizeof(this->m_birthSoundID), nullptr, nullptr, 0);
        int32_t spellCastDirectedSoundIDRead = SFileReadFile(file, &this->m_spellCastDirectedSoundID, sizeof(this->m_spellCastDirectedSoundID), nullptr, nullptr, 0);
        int32_t submergeSoundIDRead = SFileReadFile(file, &this->m_submergeSoundID, sizeof(this->m_submergeSoundID), nullptr, nullptr, 0);
        int32_t submergedSoundIDRead = SFileReadFile(file, &this->m_submergedSoundID, sizeof(this->m_submergedSoundID), nullptr, nullptr, 0);
        int32_t creatureSoundDataIdpetRead = SFileReadFile(file, &this->m_creatureSoundDataIdpet, sizeof(this->m_creatureSoundDataIdpet), nullptr, nullptr, 0);

        if (!idRead
            | !soundExertionIDRead
            | !soundExertionCriticalIDRead
            | !soundInjuryIDRead
            | !soundInjuryCriticalIDRead
            | !soundInjuryCrushingBlowIDRead
            | !soundDeathIDRead
            | !soundStunIDRead
            | !soundStandIDRead
            | !soundFootstepIDRead
            | !soundAggroIDRead
            | !soundWingFlapIDRead
            | !soundWingGlideIDRead
            | !soundAlertIDRead
            | !soundFidgetRead
            | !customAttackRead
            | !npcsoundIDRead
            | !loopSoundIDRead
            | !creatureImpactTypeRead
            | !soundJumpStartIDRead
            | !soundJumpEndIDRead
            | !soundPetAttackIDRead
            | !soundPetOrderIDRead
            | !soundPetDismissIDRead
            | !fidgetDelaySecondsMinRead
            | !fidgetDelaySecondsMaxRead
            | !birthSoundIDRead
            | !spellCastDirectedSoundIDRead
            | !submergeSoundIDRead
            | !submergedSoundIDRead
            | !creatureSoundDataIdpetRead) {
            return false;
        }

        return true;
    }
};

#endif
