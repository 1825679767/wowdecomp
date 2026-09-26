#ifndef WOW_SOURCE_DB_REC_VEHICLEREC_H
#define WOW_SOURCE_DB_REC_VEHICLEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct VehicleRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ float m_turnSpeed;
    /* 000C */ float m_pitchSpeed;
    /* 0010 */ float m_pitchMin;
    /* 0014 */ float m_pitchMax;
    /* 0018 */ int32_t m_seatID [8];
    /* 0038 */ float m_mouseLookOffsetPitch;
    /* 003C */ float m_cameraFadeDistScalarMin;
    /* 0040 */ float m_cameraFadeDistScalarMax;
    /* 0044 */ float m_cameraPitchOffset;
    /* 0048 */ float m_facingLimitRight;
    /* 004C */ float m_facingLimitLeft;
    /* 0050 */ float m_msslTrgtTurnLingering;
    /* 0054 */ float m_msslTrgtPitchLingering;
    /* 0058 */ float m_msslTrgtMouseLingering;
    /* 005C */ float m_msslTrgtEndOpacity;
    /* 0060 */ float m_msslTrgtArcSpeed;
    /* 0064 */ float m_msslTrgtArcRepeat;
    /* 0068 */ float m_msslTrgtArcWidth;
    /* 006C */ float m_msslTrgtImpactRadius [2];
    /* 0074 */ const char* m_msslTrgtArcTexture;
    /* 0078 */ const char* m_msslTrgtImpactTexture;
    /* 007C */ const char* m_msslTrgtImpactModel [2];
    /* 0084 */ float m_cameraYawOffset;
    /* 0088 */ int32_t m_uiLocomotionType;
    /* 008C */ float m_msslTrgtImpactTexRadius;
    /* 0090 */ int32_t m_vehicleUiindicatorID;
    /* 0094 */ int32_t m_powerDisplayID [3];

// VERIFY: bea912cc auto
    // OFFSET: 0x008BA920
    static const char* GetFilename() {
        return "DBFilesClient\\Vehicle.dbc";
    }

// VERIFY: ad6108e7 auto
    // OFFSET: 0x008BA930
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t msslTrgtArcTextureOffset;
        uint32_t msslTrgtImpactTextureOffset;
        uint32_t msslTrgtImpactModelOffset[2];

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t turnSpeedRead = SFileReadFile(file, &this->m_turnSpeed, sizeof(this->m_turnSpeed), nullptr, nullptr, 0);
        int32_t pitchSpeedRead = SFileReadFile(file, &this->m_pitchSpeed, sizeof(this->m_pitchSpeed), nullptr, nullptr, 0);
        int32_t pitchMinRead = SFileReadFile(file, &this->m_pitchMin, sizeof(this->m_pitchMin), nullptr, nullptr, 0);
        int32_t pitchMaxRead = SFileReadFile(file, &this->m_pitchMax, sizeof(this->m_pitchMax), nullptr, nullptr, 0);
        int32_t seatIDRead = SFileReadFile(file, this->m_seatID, sizeof(this->m_seatID), nullptr, nullptr, 0);
        int32_t mouseLookOffsetPitchRead = SFileReadFile(file, &this->m_mouseLookOffsetPitch, sizeof(this->m_mouseLookOffsetPitch), nullptr, nullptr, 0);
        int32_t cameraFadeDistScalarMinRead = SFileReadFile(file, &this->m_cameraFadeDistScalarMin, sizeof(this->m_cameraFadeDistScalarMin), nullptr, nullptr, 0);
        int32_t cameraFadeDistScalarMaxRead = SFileReadFile(file, &this->m_cameraFadeDistScalarMax, sizeof(this->m_cameraFadeDistScalarMax), nullptr, nullptr, 0);
        int32_t cameraPitchRead = SFileReadFile(file, &this->m_cameraPitchOffset, sizeof(this->m_cameraPitchOffset), nullptr, nullptr, 0);
        int32_t facingLimitRightRead = SFileReadFile(file, &this->m_facingLimitRight, sizeof(this->m_facingLimitRight), nullptr, nullptr, 0);
        int32_t facingLimitLeftRead = SFileReadFile(file, &this->m_facingLimitLeft, sizeof(this->m_facingLimitLeft), nullptr, nullptr, 0);
        int32_t msslTrgtTurnLingeringRead = SFileReadFile(file, &this->m_msslTrgtTurnLingering, sizeof(this->m_msslTrgtTurnLingering), nullptr, nullptr, 0);
        int32_t msslTrgtPitchLingeringRead = SFileReadFile(file, &this->m_msslTrgtPitchLingering, sizeof(this->m_msslTrgtPitchLingering), nullptr, nullptr, 0);
        int32_t msslTrgtMouseLingeringRead = SFileReadFile(file, &this->m_msslTrgtMouseLingering, sizeof(this->m_msslTrgtMouseLingering), nullptr, nullptr, 0);
        int32_t msslTrgtEndOpacityRead = SFileReadFile(file, &this->m_msslTrgtEndOpacity, sizeof(this->m_msslTrgtEndOpacity), nullptr, nullptr, 0);
        int32_t msslTrgtArcSpeedRead = SFileReadFile(file, &this->m_msslTrgtArcSpeed, sizeof(this->m_msslTrgtArcSpeed), nullptr, nullptr, 0);
        int32_t msslTrgtArcRepeatRead = SFileReadFile(file, &this->m_msslTrgtArcRepeat, sizeof(this->m_msslTrgtArcRepeat), nullptr, nullptr, 0);
        int32_t msslTrgtArcWidthRead = SFileReadFile(file, &this->m_msslTrgtArcWidth, sizeof(this->m_msslTrgtArcWidth), nullptr, nullptr, 0);
        int32_t msslTrgtImpactRadiusRead = SFileReadFile(file, this->m_msslTrgtImpactRadius, sizeof(this->m_msslTrgtImpactRadius), nullptr, nullptr, 0);
        int32_t msslTrgtArcTextureRead = SFileReadFile(file, &msslTrgtArcTextureOffset, sizeof(msslTrgtArcTextureOffset), nullptr, nullptr, 0);
        int32_t msslTrgtImpactTextureRead = SFileReadFile(file, &msslTrgtImpactTextureOffset, sizeof(msslTrgtImpactTextureOffset), nullptr, nullptr, 0);
        int32_t msslTrgtImpactModelRead0 = SFileReadFile(file, &msslTrgtImpactModelOffset[0], sizeof(msslTrgtImpactModelOffset[0]), nullptr, nullptr, 0);
        int32_t msslTrgtImpactModelRead1 = SFileReadFile(file, &msslTrgtImpactModelOffset[1], sizeof(msslTrgtImpactModelOffset[1]), nullptr, nullptr, 0);
        int32_t cameraYawRead = SFileReadFile(file, &this->m_cameraYawOffset, sizeof(this->m_cameraYawOffset), nullptr, nullptr, 0);
        int32_t uiLocomotionTypeRead = SFileReadFile(file, &this->m_uiLocomotionType, sizeof(this->m_uiLocomotionType), nullptr, nullptr, 0);
        int32_t msslTrgtImpactTexRadiusRead = SFileReadFile(file, &this->m_msslTrgtImpactTexRadius, sizeof(this->m_msslTrgtImpactTexRadius), nullptr, nullptr, 0);
        int32_t vehicleUiindicatorIDRead = SFileReadFile(file, &this->m_vehicleUiindicatorID, sizeof(this->m_vehicleUiindicatorID), nullptr, nullptr, 0);
        int32_t powerDisplayIDRead = SFileReadFile(file, this->m_powerDisplayID, sizeof(this->m_powerDisplayID), nullptr, nullptr, 0);

        if (!idRead
            | !flagsRead
            | !turnSpeedRead
            | !pitchSpeedRead
            | !pitchMinRead
            | !pitchMaxRead
            | !seatIDRead
            | !mouseLookOffsetPitchRead
            | !cameraFadeDistScalarMinRead
            | !cameraFadeDistScalarMaxRead
            | !cameraPitchRead
            | !facingLimitRightRead
            | !facingLimitLeftRead
            | !msslTrgtTurnLingeringRead
            | !msslTrgtPitchLingeringRead
            | !msslTrgtMouseLingeringRead
            | !msslTrgtEndOpacityRead
            | !msslTrgtArcSpeedRead
            | !msslTrgtArcRepeatRead
            | !msslTrgtArcWidthRead
            | !msslTrgtImpactRadiusRead
            | !msslTrgtArcTextureRead
            | !msslTrgtImpactTextureRead
            | !msslTrgtImpactModelRead0
            | !msslTrgtImpactModelRead1
            | !cameraYawRead
            | !uiLocomotionTypeRead
            | !msslTrgtImpactTexRadiusRead
            | !vehicleUiindicatorIDRead
            | !powerDisplayIDRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_msslTrgtArcTexture = &stringBuffer[msslTrgtArcTextureOffset];
            this->m_msslTrgtImpactTexture = &stringBuffer[msslTrgtImpactTextureOffset];
            this->m_msslTrgtImpactModel[0] = &stringBuffer[msslTrgtImpactModelOffset[0]];
            this->m_msslTrgtImpactModel[1] = &stringBuffer[msslTrgtImpactModelOffset[1]];
        } else {
            this->m_msslTrgtArcTexture = "";
            this->m_msslTrgtImpactTexture = "";
            this->m_msslTrgtImpactModel[0] = "";
            this->m_msslTrgtImpactModel[1] = "";
        }

        return true;
    }
};

#endif
