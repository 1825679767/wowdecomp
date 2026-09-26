#ifndef WOW_SOURCE_DB_REC_SPELLCHAINEFFECTSREC_H
#define WOW_SOURCE_DB_REC_SPELLCHAINEFFECTSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellChainEffectsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_avgSegLen;
    /* 0008 */ float m_width;
    /* 000C */ float m_noiseScale;
    /* 0010 */ float m_texCoordScale;
    /* 0014 */ int32_t m_segDuration;
    /* 0018 */ int32_t m_segDelay;
    /* 001C */ const char* m_texture;
    /* 0020 */ int32_t m_flags;
    /* 0024 */ int32_t m_jointCount;
    /* 0028 */ float m_jointOffsetRadius;
    /* 002C */ int32_t m_jointsPerMinorJoint;
    /* 0030 */ int32_t m_minorJointsPerMajorJoint;
    /* 0034 */ float m_minorJointScale;
    /* 0038 */ float m_majorJointScale;
    /* 003C */ float m_jointMoveSpeed;
    /* 0040 */ float m_jointSmoothness;
    /* 0044 */ float m_minDurationBetweenJointJumps;
    /* 0048 */ float m_maxDurationBetweenJointJumps;
    /* 004C */ float m_waveHeight;
    /* 0050 */ float m_waveFreq;
    /* 0054 */ float m_waveSpeed;
    /* 0058 */ float m_minWaveAngle;
    /* 005C */ float m_maxWaveAngle;
    /* 0060 */ float m_minWaveSpin;
    /* 0064 */ float m_maxWaveSpin;
    /* 0068 */ float m_arcHeight;
    /* 006C */ float m_minArcAngle;
    /* 0070 */ float m_maxArcAngle;
    /* 0074 */ float m_minArcSpin;
    /* 0078 */ float m_maxArcSpin;
    /* 007C */ float m_delayBetweenEffects;
    /* 0080 */ float m_minFlickerOnDuration;
    /* 0084 */ float m_maxFlickerOnDuration;
    /* 0088 */ float m_minFlickerOffDuration;
    /* 008C */ float m_maxFlickerOffDuration;
    /* 0090 */ float m_pulseSpeed;
    /* 0094 */ float m_pulseOnLength;
    /* 0098 */ float m_pulseFadeLength;
    /* 009C */ int8_t m_alpha;
    /* 009D */ int8_t m_red;
    /* 009E */ int8_t m_green;
    /* 009F */ int8_t m_blue;
    /* 00A0 */ int8_t m_blendMode;
    /* 00A4 */ const char* m_combo;
    /* 00A8 */ int32_t m_renderLayer;
    /* 00AC */ float m_textureLength;
    /* 00B0 */ float m_wavePhase;

// VERIFY: db5aaba1 auto
    // OFFSET: 0x008B73A0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellChainEffects.dbc";
    }

// VERIFY: f5955594 auto
    // OFFSET: 0x008B73B0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textureOffset;
        uint32_t comboOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t avgSegLenRead = SFileReadFile(file, &this->m_avgSegLen, sizeof(this->m_avgSegLen), nullptr, nullptr, 0);
        int32_t widthRead = SFileReadFile(file, &this->m_width, sizeof(this->m_width), nullptr, nullptr, 0);
        int32_t noiseScaleRead = SFileReadFile(file, &this->m_noiseScale, sizeof(this->m_noiseScale), nullptr, nullptr, 0);
        int32_t texCoordScaleRead = SFileReadFile(file, &this->m_texCoordScale, sizeof(this->m_texCoordScale), nullptr, nullptr, 0);
        int32_t segDurationRead = SFileReadFile(file, &this->m_segDuration, sizeof(this->m_segDuration), nullptr, nullptr, 0);
        int32_t segDelayRead = SFileReadFile(file, &this->m_segDelay, sizeof(this->m_segDelay), nullptr, nullptr, 0);
        int32_t textureRead = SFileReadFile(file, &textureOffset, sizeof(textureOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t jointCountRead = SFileReadFile(file, &this->m_jointCount, sizeof(this->m_jointCount), nullptr, nullptr, 0);
        int32_t jointOffsetRadiusRead = SFileReadFile(file, &this->m_jointOffsetRadius, sizeof(this->m_jointOffsetRadius), nullptr, nullptr, 0);
        int32_t jointsPerMinorJointRead = SFileReadFile(file, &this->m_jointsPerMinorJoint, sizeof(this->m_jointsPerMinorJoint), nullptr, nullptr, 0);
        int32_t minorJointsPerMajorJointRead = SFileReadFile(file, &this->m_minorJointsPerMajorJoint, sizeof(this->m_minorJointsPerMajorJoint), nullptr, nullptr, 0);
        int32_t minorJointScaleRead = SFileReadFile(file, &this->m_minorJointScale, sizeof(this->m_minorJointScale), nullptr, nullptr, 0);
        int32_t majorJointScaleRead = SFileReadFile(file, &this->m_majorJointScale, sizeof(this->m_majorJointScale), nullptr, nullptr, 0);
        int32_t jointMoveSpeedRead = SFileReadFile(file, &this->m_jointMoveSpeed, sizeof(this->m_jointMoveSpeed), nullptr, nullptr, 0);
        int32_t jointSmoothnessRead = SFileReadFile(file, &this->m_jointSmoothness, sizeof(this->m_jointSmoothness), nullptr, nullptr, 0);
        int32_t minDurationBetweenJointJumpsRead = SFileReadFile(file, &this->m_minDurationBetweenJointJumps, sizeof(this->m_minDurationBetweenJointJumps), nullptr, nullptr, 0);
        int32_t maxDurationBetweenJointJumpsRead = SFileReadFile(file, &this->m_maxDurationBetweenJointJumps, sizeof(this->m_maxDurationBetweenJointJumps), nullptr, nullptr, 0);
        int32_t waveHeightRead = SFileReadFile(file, &this->m_waveHeight, sizeof(this->m_waveHeight), nullptr, nullptr, 0);
        int32_t waveFreqRead = SFileReadFile(file, &this->m_waveFreq, sizeof(this->m_waveFreq), nullptr, nullptr, 0);
        int32_t waveSpeedRead = SFileReadFile(file, &this->m_waveSpeed, sizeof(this->m_waveSpeed), nullptr, nullptr, 0);
        int32_t minWaveAngleRead = SFileReadFile(file, &this->m_minWaveAngle, sizeof(this->m_minWaveAngle), nullptr, nullptr, 0);
        int32_t maxWaveAngleRead = SFileReadFile(file, &this->m_maxWaveAngle, sizeof(this->m_maxWaveAngle), nullptr, nullptr, 0);
        int32_t minWaveSpinRead = SFileReadFile(file, &this->m_minWaveSpin, sizeof(this->m_minWaveSpin), nullptr, nullptr, 0);
        int32_t maxWaveSpinRead = SFileReadFile(file, &this->m_maxWaveSpin, sizeof(this->m_maxWaveSpin), nullptr, nullptr, 0);
        int32_t arcHeightRead = SFileReadFile(file, &this->m_arcHeight, sizeof(this->m_arcHeight), nullptr, nullptr, 0);
        int32_t minArcAngleRead = SFileReadFile(file, &this->m_minArcAngle, sizeof(this->m_minArcAngle), nullptr, nullptr, 0);
        int32_t maxArcAngleRead = SFileReadFile(file, &this->m_maxArcAngle, sizeof(this->m_maxArcAngle), nullptr, nullptr, 0);
        int32_t minArcSpinRead = SFileReadFile(file, &this->m_minArcSpin, sizeof(this->m_minArcSpin), nullptr, nullptr, 0);
        int32_t maxArcSpinRead = SFileReadFile(file, &this->m_maxArcSpin, sizeof(this->m_maxArcSpin), nullptr, nullptr, 0);
        int32_t delayBetweenEffectsRead = SFileReadFile(file, &this->m_delayBetweenEffects, sizeof(this->m_delayBetweenEffects), nullptr, nullptr, 0);
        int32_t minFlickerOnDurationRead = SFileReadFile(file, &this->m_minFlickerOnDuration, sizeof(this->m_minFlickerOnDuration), nullptr, nullptr, 0);
        int32_t maxFlickerOnDurationRead = SFileReadFile(file, &this->m_maxFlickerOnDuration, sizeof(this->m_maxFlickerOnDuration), nullptr, nullptr, 0);
        int32_t minFlickerOffDurationRead = SFileReadFile(file, &this->m_minFlickerOffDuration, sizeof(this->m_minFlickerOffDuration), nullptr, nullptr, 0);
        int32_t maxFlickerOffDurationRead = SFileReadFile(file, &this->m_maxFlickerOffDuration, sizeof(this->m_maxFlickerOffDuration), nullptr, nullptr, 0);
        int32_t pulseSpeedRead = SFileReadFile(file, &this->m_pulseSpeed, sizeof(this->m_pulseSpeed), nullptr, nullptr, 0);
        int32_t pulseOnLengthRead = SFileReadFile(file, &this->m_pulseOnLength, sizeof(this->m_pulseOnLength), nullptr, nullptr, 0);
        int32_t pulseFadeLengthRead = SFileReadFile(file, &this->m_pulseFadeLength, sizeof(this->m_pulseFadeLength), nullptr, nullptr, 0);
        int32_t alphaRead = SFileReadFile(file, &this->m_alpha, sizeof(this->m_alpha), nullptr, nullptr, 0);
        int32_t redRead = SFileReadFile(file, &this->m_red, sizeof(this->m_red), nullptr, nullptr, 0);
        int32_t greenRead = SFileReadFile(file, &this->m_green, sizeof(this->m_green), nullptr, nullptr, 0);
        int32_t blueRead = SFileReadFile(file, &this->m_blue, sizeof(this->m_blue), nullptr, nullptr, 0);
        int32_t blendModeRead = SFileReadFile(file, &this->m_blendMode, sizeof(this->m_blendMode), nullptr, nullptr, 0);
        int32_t comboRead = SFileReadFile(file, &comboOffset, sizeof(comboOffset), nullptr, nullptr, 0);
        int32_t renderLayerRead = SFileReadFile(file, &this->m_renderLayer, sizeof(this->m_renderLayer), nullptr, nullptr, 0);
        int32_t textureLengthRead = SFileReadFile(file, &this->m_textureLength, sizeof(this->m_textureLength), nullptr, nullptr, 0);
        int32_t wavePhaseRead = SFileReadFile(file, &this->m_wavePhase, sizeof(this->m_wavePhase), nullptr, nullptr, 0);

        if (!idRead
            | !avgSegLenRead
            | !widthRead
            | !noiseScaleRead
            | !texCoordScaleRead
            | !segDurationRead
            | !segDelayRead
            | !textureRead
            | !flagsRead
            | !jointCountRead
            | !jointOffsetRadiusRead
            | !jointsPerMinorJointRead
            | !minorJointsPerMajorJointRead
            | !minorJointScaleRead
            | !majorJointScaleRead
            | !jointMoveSpeedRead
            | !jointSmoothnessRead
            | !minDurationBetweenJointJumpsRead
            | !maxDurationBetweenJointJumpsRead
            | !waveHeightRead
            | !waveFreqRead
            | !waveSpeedRead
            | !minWaveAngleRead
            | !maxWaveAngleRead
            | !minWaveSpinRead
            | !maxWaveSpinRead
            | !arcHeightRead
            | !minArcAngleRead
            | !maxArcAngleRead
            | !minArcSpinRead
            | !maxArcSpinRead
            | !delayBetweenEffectsRead
            | !minFlickerOnDurationRead
            | !maxFlickerOnDurationRead
            | !minFlickerOffDurationRead
            | !maxFlickerOffDurationRead
            | !pulseSpeedRead
            | !pulseOnLengthRead
            | !pulseFadeLengthRead
            | !alphaRead
            | !redRead
            | !greenRead
            | !blueRead
            | !blendModeRead
            | !comboRead
            | !renderLayerRead
            | !textureLengthRead
            | !wavePhaseRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_texture = &stringBuffer[textureOffset];
            this->m_combo = &stringBuffer[comboOffset];
        } else {
            this->m_texture = "";
            this->m_combo = "";
        }

        return true;
    }
};

#endif
