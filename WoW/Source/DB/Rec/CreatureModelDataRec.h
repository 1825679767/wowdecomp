#ifndef WOW_SOURCE_DB_REC_CREATUREMODELDATAREC_H
#define WOW_SOURCE_DB_REC_CREATUREMODELDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CreatureModelDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ const char* m_modelName;
    /* 000C */ int32_t m_sizeClass;
    /* 0010 */ float m_modelScale;
    /* 0014 */ int32_t m_bloodID;
    /* 0018 */ int32_t m_footprintTextureID;
    /* 001C */ float m_footprintTextureLength;
    /* 0020 */ float m_footprintTextureWidth;
    /* 0024 */ float m_footprintParticleScale;
    /* 0028 */ int32_t m_foleyMaterialID;
    /* 002C */ int32_t m_footstepShakeSize;
    /* 0030 */ int32_t m_deathThudShakeSize;
    /* 0034 */ int32_t m_soundID;
    /* 0038 */ float m_collisionWidth;
    /* 003C */ float m_collisionHeight;
    /* 0040 */ float m_mountHeight;
    /* 0044 */ float m_geoBoxMinX;
    /* 0048 */ float m_geoBoxMinY;
    /* 004C */ float m_geoBoxMinZ;
    /* 0050 */ float m_geoBoxMaxX;
    /* 0054 */ float m_geoBoxMaxY;
    /* 0058 */ float m_geoBoxMaxZ;
    /* 005C */ float m_worldEffectScale;
    /* 0060 */ float m_attachedEffectScale;
    /* 0064 */ float m_missileCollisionRadius;
    /* 0068 */ float m_missileCollisionPush;
    /* 006C */ float m_missileCollisionRaise;

// VERIFY: a12aa492 auto
    // OFFSET: 0x008AD7F0
    static const char* GetFilename() {
        return "DBFilesClient\\CreatureModelData.dbc";
    }

// VERIFY: 78c4a4b3 auto
    // OFFSET: 0x008AD800
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t modelNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t modelNameRead = SFileReadFile(file, &modelNameOffset, sizeof(modelNameOffset), nullptr, nullptr, 0);
        int32_t sizeClassRead = SFileReadFile(file, &this->m_sizeClass, sizeof(this->m_sizeClass), nullptr, nullptr, 0);
        int32_t modelScaleRead = SFileReadFile(file, &this->m_modelScale, sizeof(this->m_modelScale), nullptr, nullptr, 0);
        int32_t bloodIDRead = SFileReadFile(file, &this->m_bloodID, sizeof(this->m_bloodID), nullptr, nullptr, 0);
        int32_t footprintTextureIDRead = SFileReadFile(file, &this->m_footprintTextureID, sizeof(this->m_footprintTextureID), nullptr, nullptr, 0);
        int32_t footprintTextureLengthRead = SFileReadFile(file, &this->m_footprintTextureLength, sizeof(this->m_footprintTextureLength), nullptr, nullptr, 0);
        int32_t footprintTextureWidthRead = SFileReadFile(file, &this->m_footprintTextureWidth, sizeof(this->m_footprintTextureWidth), nullptr, nullptr, 0);
        int32_t footprintParticleScaleRead = SFileReadFile(file, &this->m_footprintParticleScale, sizeof(this->m_footprintParticleScale), nullptr, nullptr, 0);
        int32_t foleyMaterialIDRead = SFileReadFile(file, &this->m_foleyMaterialID, sizeof(this->m_foleyMaterialID), nullptr, nullptr, 0);
        int32_t footstepShakeSizeRead = SFileReadFile(file, &this->m_footstepShakeSize, sizeof(this->m_footstepShakeSize), nullptr, nullptr, 0);
        int32_t deathThudShakeSizeRead = SFileReadFile(file, &this->m_deathThudShakeSize, sizeof(this->m_deathThudShakeSize), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t collisionWidthRead = SFileReadFile(file, &this->m_collisionWidth, sizeof(this->m_collisionWidth), nullptr, nullptr, 0);
        int32_t collisionHeightRead = SFileReadFile(file, &this->m_collisionHeight, sizeof(this->m_collisionHeight), nullptr, nullptr, 0);
        int32_t mountHeightRead = SFileReadFile(file, &this->m_mountHeight, sizeof(this->m_mountHeight), nullptr, nullptr, 0);
        int32_t geoBoxMinXRead = SFileReadFile(file, &this->m_geoBoxMinX, sizeof(this->m_geoBoxMinX), nullptr, nullptr, 0);
        int32_t geoBoxMinYRead = SFileReadFile(file, &this->m_geoBoxMinY, sizeof(this->m_geoBoxMinY), nullptr, nullptr, 0);
        int32_t geoBoxMinZRead = SFileReadFile(file, &this->m_geoBoxMinZ, sizeof(this->m_geoBoxMinZ), nullptr, nullptr, 0);
        int32_t geoBoxMaxXRead = SFileReadFile(file, &this->m_geoBoxMaxX, sizeof(this->m_geoBoxMaxX), nullptr, nullptr, 0);
        int32_t geoBoxMaxYRead = SFileReadFile(file, &this->m_geoBoxMaxY, sizeof(this->m_geoBoxMaxY), nullptr, nullptr, 0);
        int32_t geoBoxMaxZRead = SFileReadFile(file, &this->m_geoBoxMaxZ, sizeof(this->m_geoBoxMaxZ), nullptr, nullptr, 0);
        int32_t worldEffectScaleRead = SFileReadFile(file, &this->m_worldEffectScale, sizeof(this->m_worldEffectScale), nullptr, nullptr, 0);
        int32_t attachedEffectScaleRead = SFileReadFile(file, &this->m_attachedEffectScale, sizeof(this->m_attachedEffectScale), nullptr, nullptr, 0);
        int32_t missileCollisionRadiusRead = SFileReadFile(file, &this->m_missileCollisionRadius, sizeof(this->m_missileCollisionRadius), nullptr, nullptr, 0);
        int32_t missileCollisionPushRead = SFileReadFile(file, &this->m_missileCollisionPush, sizeof(this->m_missileCollisionPush), nullptr, nullptr, 0);
        int32_t missileCollisionRaiseRead = SFileReadFile(file, &this->m_missileCollisionRaise, sizeof(this->m_missileCollisionRaise), nullptr, nullptr, 0);

        if (!idRead
            | !flagsRead
            | !modelNameRead
            | !sizeClassRead
            | !modelScaleRead
            | !bloodIDRead
            | !footprintTextureIDRead
            | !footprintTextureLengthRead
            | !footprintTextureWidthRead
            | !footprintParticleScaleRead
            | !foleyMaterialIDRead
            | !footstepShakeSizeRead
            | !deathThudShakeSizeRead
            | !soundIDRead
            | !collisionWidthRead
            | !collisionHeightRead
            | !mountHeightRead
            | !geoBoxMinXRead
            | !geoBoxMinYRead
            | !geoBoxMinZRead
            | !geoBoxMaxXRead
            | !geoBoxMaxYRead
            | !geoBoxMaxZRead
            | !worldEffectScaleRead
            | !attachedEffectScaleRead
            | !missileCollisionRadiusRead
            | !missileCollisionPushRead
            | !missileCollisionRaiseRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_modelName = &stringBuffer[modelNameOffset];
        } else {
            this->m_modelName = "";
        }

        return true;
    }
};

#endif
