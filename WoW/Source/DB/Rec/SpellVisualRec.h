#ifndef WOW_SOURCE_DB_REC_SPELLVISUALREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellVisualRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_precastKit;
    /* 0008 */ int32_t m_castKit;
    /* 000C */ int32_t m_impactKit;
    /* 0010 */ int32_t m_stateKit;
    /* 0014 */ int32_t m_stateDoneKit;
    /* 0018 */ int32_t m_channelKit;
    /* 001C */ int32_t m_hasMissile;
    /* 0020 */ int32_t m_missileModel;
    /* 0024 */ int32_t m_missilePathType;
    /* 0028 */ int32_t m_missileDestinationAttachment;
    /* 002C */ int32_t m_missileSound;
    /* 0030 */ int32_t m_animEventSoundID;
    /* 0034 */ int32_t m_flags;
    /* 0038 */ int32_t m_casterImpactKit;
    /* 003C */ int32_t m_targetImpactKit;
    /* 0040 */ int32_t m_missileAttachment;
    /* 0044 */ int32_t m_missileFollowGroundHeight;
    /* 0048 */ int32_t m_missileFollowGroundDropSpeed;
    /* 004C */ int32_t m_missileFollowGroundApproach;
    /* 0050 */ int32_t m_missileFollowGroundFlags;
    /* 0054 */ int32_t m_missileMotion;
    /* 0058 */ int32_t m_missileTargetingKit;
    /* 005C */ int32_t m_instantAreaKit;
    /* 0060 */ int32_t m_impactAreaKit;
    /* 0064 */ int32_t m_persistentAreaKit;
    /* 0068 */ float m_missileCastOffset [3];
    /* 0074 */ float m_missileImpactOffset [3];

// VERIFY: ca9f07c5 auto
    // OFFSET: 0x008B93E0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellVisual.dbc";
    }

// VERIFY: 2893355a auto
    // OFFSET: 0x008B93F0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t precastKitRead = SFileReadFile(file, &this->m_precastKit, sizeof(this->m_precastKit), nullptr, nullptr, 0);
        int32_t castKitRead = SFileReadFile(file, &this->m_castKit, sizeof(this->m_castKit), nullptr, nullptr, 0);
        int32_t impactKitRead = SFileReadFile(file, &this->m_impactKit, sizeof(this->m_impactKit), nullptr, nullptr, 0);
        int32_t stateKitRead = SFileReadFile(file, &this->m_stateKit, sizeof(this->m_stateKit), nullptr, nullptr, 0);
        int32_t stateDoneKitRead = SFileReadFile(file, &this->m_stateDoneKit, sizeof(this->m_stateDoneKit), nullptr, nullptr, 0);
        int32_t channelKitRead = SFileReadFile(file, &this->m_channelKit, sizeof(this->m_channelKit), nullptr, nullptr, 0);
        int32_t hasMissileRead = SFileReadFile(file, &this->m_hasMissile, sizeof(this->m_hasMissile), nullptr, nullptr, 0);
        int32_t missileModelRead = SFileReadFile(file, &this->m_missileModel, sizeof(this->m_missileModel), nullptr, nullptr, 0);
        int32_t missilePathTypeRead = SFileReadFile(file, &this->m_missilePathType, sizeof(this->m_missilePathType), nullptr, nullptr, 0);
        int32_t missileDestinationAttachmentRead = SFileReadFile(file, &this->m_missileDestinationAttachment, sizeof(this->m_missileDestinationAttachment), nullptr, nullptr, 0);
        int32_t missileSoundRead = SFileReadFile(file, &this->m_missileSound, sizeof(this->m_missileSound), nullptr, nullptr, 0);
        int32_t animEventSoundIDRead = SFileReadFile(file, &this->m_animEventSoundID, sizeof(this->m_animEventSoundID), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t casterImpactKitRead = SFileReadFile(file, &this->m_casterImpactKit, sizeof(this->m_casterImpactKit), nullptr, nullptr, 0);
        int32_t targetImpactKitRead = SFileReadFile(file, &this->m_targetImpactKit, sizeof(this->m_targetImpactKit), nullptr, nullptr, 0);
        int32_t missileAttachmentRead = SFileReadFile(file, &this->m_missileAttachment, sizeof(this->m_missileAttachment), nullptr, nullptr, 0);
        int32_t missileFollowGroundHeightRead = SFileReadFile(file, &this->m_missileFollowGroundHeight, sizeof(this->m_missileFollowGroundHeight), nullptr, nullptr, 0);
        int32_t missileFollowGroundDropSpeedRead = SFileReadFile(file, &this->m_missileFollowGroundDropSpeed, sizeof(this->m_missileFollowGroundDropSpeed), nullptr, nullptr, 0);
        int32_t missileFollowGroundApproachRead = SFileReadFile(file, &this->m_missileFollowGroundApproach, sizeof(this->m_missileFollowGroundApproach), nullptr, nullptr, 0);
        int32_t missileFollowGroundFlagsRead = SFileReadFile(file, &this->m_missileFollowGroundFlags, sizeof(this->m_missileFollowGroundFlags), nullptr, nullptr, 0);
        int32_t missileMotionRead = SFileReadFile(file, &this->m_missileMotion, sizeof(this->m_missileMotion), nullptr, nullptr, 0);
        int32_t missileTargetingKitRead = SFileReadFile(file, &this->m_missileTargetingKit, sizeof(this->m_missileTargetingKit), nullptr, nullptr, 0);
        int32_t instantAreaKitRead = SFileReadFile(file, &this->m_instantAreaKit, sizeof(this->m_instantAreaKit), nullptr, nullptr, 0);
        int32_t impactAreaKitRead = SFileReadFile(file, &this->m_impactAreaKit, sizeof(this->m_impactAreaKit), nullptr, nullptr, 0);
        int32_t persistentAreaKitRead = SFileReadFile(file, &this->m_persistentAreaKit, sizeof(this->m_persistentAreaKit), nullptr, nullptr, 0);
        int32_t missileCastRead0 = SFileReadFile(file, &this->m_missileCastOffset[0], sizeof(this->m_missileCastOffset[0]), nullptr, nullptr, 0);
        int32_t missileCastRead1 = SFileReadFile(file, &this->m_missileCastOffset[1], sizeof(this->m_missileCastOffset[1]), nullptr, nullptr, 0);
        int32_t missileCastRead2 = SFileReadFile(file, &this->m_missileCastOffset[2], sizeof(this->m_missileCastOffset[2]), nullptr, nullptr, 0);
        int32_t missileImpactRead0 = SFileReadFile(file, &this->m_missileImpactOffset[0], sizeof(this->m_missileImpactOffset[0]), nullptr, nullptr, 0);
        int32_t missileImpactRead1 = SFileReadFile(file, &this->m_missileImpactOffset[1], sizeof(this->m_missileImpactOffset[1]), nullptr, nullptr, 0);
        int32_t missileImpactRead2 = SFileReadFile(file, &this->m_missileImpactOffset[2], sizeof(this->m_missileImpactOffset[2]), nullptr, nullptr, 0);

        if (!idRead
            | !precastKitRead
            | !castKitRead
            | !impactKitRead
            | !stateKitRead
            | !stateDoneKitRead
            | !channelKitRead
            | !hasMissileRead
            | !missileModelRead
            | !missilePathTypeRead
            | !missileDestinationAttachmentRead
            | !missileSoundRead
            | !animEventSoundIDRead
            | !flagsRead
            | !casterImpactKitRead
            | !targetImpactKitRead
            | !missileAttachmentRead
            | !missileFollowGroundHeightRead
            | !missileFollowGroundDropSpeedRead
            | !missileFollowGroundApproachRead
            | !missileFollowGroundFlagsRead
            | !missileMotionRead
            | !missileTargetingKitRead
            | !instantAreaKitRead
            | !impactAreaKitRead
            | !persistentAreaKitRead
            | !missileCastRead0
            | !missileCastRead1
            | !missileCastRead2
            | !missileImpactRead0
            | !missileImpactRead1
            | !missileImpactRead2) {
            return false;
        }

        return true;
    }
};

#endif
