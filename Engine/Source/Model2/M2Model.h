#ifndef ENGINE_SOURCE_MODEL2_M2MODEL_H
#define ENGINE_SOURCE_MODEL2_M2MODEL_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/World.h"

struct C3Vector;
struct C44Matrix;
struct C4Quaternion;
class CM2Model;
struct CM2ModelCall;
struct CM2Scene;
struct CM2Shared;
struct CTexture;
class M2ModelBone;
struct M2ModelColor;
struct M2ModelLight;
struct M2ModelParticle;
struct M2ModelRibbon;
struct M2ModelTextureTransform;
struct M2Track;

class CM2Model {
    public:
    /* 0000 */ uint32_t m_refCount;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ CM2Model** m_scenePrev;
    /* 000C */ CM2Model* m_sceneNext;
    /* 0010 */ uint32_t m_fFlags;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ CM2Model** m_callbackPrev;
    /* 001C */ CM2Model* m_callbackNext;
    /* 0020 */ uint32_t m_loadedCallback;
    /* 0024 */ uint32_t m_loadedArg;
    /* 0028 */ CM2Scene* m_scene;
    /* 002C */ CM2Shared* m_shared;
    /* 0030 */ CM2Model* m_model30;
    /* 0034 */ CM2ModelCall* m_modelCallList;
    /* 0038 */ CM2ModelCall** m_modelCallTail;
    /* 003C */ uint32_t m_frameStamp;
    /* 0040 */ CM2Model** m_animatePrev;
    /* 0044 */ CM2Model* m_animateNext;
    /* 0048 */ CM2Model* m_attachParent;
    /* 004C */ uint32_t m_attachments;
    /* 0050 */ uint32_t m_attachmentId;
    /* 0054 */ uint32_t m_attachmentIndex;
    /* 0058 */ CM2Model* m_attachList;
    /* 005C */ CM2Model** m_attachPrev;
    /* 0060 */ CM2Model* m_attachNext;
    /* 0064 */ uint32_t m_lastAnimTime;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ int32_t* m_loops;
    /* 0074 */ uint32_t m_loopOrigin;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t m_sequenceCallbackParam1;
    /* 0084 */ uint32_t m_sequenceCallbackParam2;
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t m_lastEmitterTime;
    /* 0090 */ uint32_t m_uint90;
    /* 0094 */ M2ModelBone* m_bones;
    /* 0098 */ C44Matrix* m_boneMatrices;
    /* 009C */ uint32_t* m_skinSections;
    /* 00A0 */ M2ModelColor* m_colors;
    /* 00A4 */ CTexture* m_textures;
    /* 00A8 */ uint32_t m_textureWeights;
    /* 00AC */ M2ModelTextureTransform* m_textureTransforms;
    /* 00B0 */ C44Matrix* m_textureMatrices;
    /* 00B4 */ C44Matrix m_worldTransform;
    /* 00F4 */ C44Matrix m_matrixF4;
    /* 0134 */ C3Vector unk_0134;
    /* 0140 */ C3Vector unk_0140;
    /* 014C */ C3Vector unk_014C;
    /* 0158 */ C3Vector unk_0158;
    /* 0164 */ C3Vector unk_0164;
    /* 0170 */ C44Matrix unk_0170;
    /* 01B0 */ float unk_01B0;
    /* 01B4 */ float unk_01B4;
    /* 01B8 */ C3Vector unk_01B8;
    /* 01C4 */ C3Vector unk_01C4;
    /* 01D0 */ M2ModelLight* m_lights;
    /* 01D4 */ CM2Lighting m_lighting;
    /* 02A8 */ uint32_t m_uint2A8;
    /* 02AC */ uint32_t m_lightingCallback;
    /* 02B0 */ uint32_t m_lightingArg;
    /* 02B4 */ uint32_t m_cameras;
    /* 02B8 */ M2ModelRibbon* m_ribbons;
    /* 02BC */ uint32_t m_ribbonEmitters;
    /* 02C0 */ M2ModelParticle* m_particles;
    /* 02C4 */ uint32_t m_particleEmitters;
    /* 02C8 */ uint32_t m_particlePrev;
    /* 02CC */ uint32_t m_particleNext;
    /* 02D0 */ uint32_t unk_02D0;
    /* 02D4 */ uint32_t m_hitTestMode;
    /* 02D8 */ uint32_t m_hitTestPrev;
    /* 02DC */ uint32_t m_hitTestNext;
    /* 02E0 */ uint32_t m_hitTestOwner;
    /* 02E4 */ uint32_t m_hitTestGroup;
    /* 02E8 */ uint32_t m_handle;
};

class CM2SequencePlayBack {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
};

struct M2ModelBoneSeq {
    /* 0000 */ uint32_t m_currentTime;
    /* 0004 */ uint16_t m_animIndex;
    /* 0006 */ uint16_t m_sourceBoneIndex;
    /* 0008 */ uint16_t m_sequenceIndex;
    /* 000A */ uint8_t m_finished;
    /* 000B */ uint8_t m_pickRandomVariation;
    /* 000C */ uint32_t m_startTime;
    /* 0010 */ uint32_t m_endTime;
    /* 0014 */ float m_speed;
    /* 0018 */ float m_invSpeed;
    /* 001C */ uint32_t m_startOffset;
    /* 0020 */ uint32_t m_repeatCount;
};

template <class T>
struct M2ModelTrack {
    /* 0000 */ uint32_t m_currentKey;
    /* 0004 */ M2Track* m_sourceTrack;
    /* 0008 */ T m_currentValue;
};

class M2ModelBone {
    public:
    /* 0000 */ M2ModelTrack<C3Vector> m_translation;
    /* 0014 */ M2ModelTrack<C4Quaternion> m_rotation;
    /* 002C */ M2ModelTrack<C3Vector> m_scale;
    /* 0040 */ M2ModelBoneSeq m_sequence;
    /* 0064 */ M2ModelBoneSeq m_secondarySequence;
    /* 0088 */ C44Matrix* m_proceduralTransform;
    /* 008C */ uint32_t m_flags;
    /* 0090 */ uint32_t m_sequenceId;
    /* 0094 */ uint16_t m_variationIndex;
    /* 0096 */ uint16_t m_animListNext;
    /* 0098 */ uint16_t* m_animListPrev;
    /* 009C */ uint32_t m_blendEndTime;
    /* 00A0 */ float m_invBlendDuration;
    /* 00A4 */ float m_blendWeightMax;
    /* 00A8 */ float m_blendFactor;
};

struct M2ModelColor {
    /* 0000 */ M2ModelTrack<C3Vector> m_colorTrack;
    /* 0014 */ M2ModelTrack<float> m_alphaTrack;
};

struct M2ModelParticle {
    /* 0000 */ M2ModelTrack<float> m_speedTrack;
    /* 000C */ M2ModelTrack<float> m_variationTrack;
    /* 0018 */ M2ModelTrack<float> m_latitudeTrack;
    /* 0024 */ M2ModelTrack<float> m_longitudeTrack;
    /* 0030 */ M2ModelTrack<float> m_gravityTrack;
    /* 003C */ M2ModelTrack<float> m_lifeTrack;
    /* 0048 */ M2ModelTrack<float> m_emissionRateTrack;
    /* 0054 */ M2ModelTrack<float> m_widthTrack;
    /* 0060 */ M2ModelTrack<float> m_lengthTrack;
    /* 006C */ M2ModelTrack<float> m_zsourceTrack;
    /* 0078 */ M2ModelTrack<uint8_t> m_visibilityTrack;
    /* 0084 */ uint8_t m_emitting;
    /* 0085 */ uint8_t m_active;
    /* 0086 */ uint8_t m_burstFired;
    /* 0087 */ uint8_t unk_0087;
};

struct M2ModelRibbon {
    /* 0000 */ M2ModelTrack<C3Vector> m_colorTrack;
    /* 0014 */ M2ModelTrack<float> m_alphaTrack;
    /* 0020 */ M2ModelTrack<float> m_heightAboveTrack;
    /* 002C */ M2ModelTrack<float> m_heightBelowTrack;
    /* 0038 */ M2ModelTrack<uint16_t> m_textureSlotTrack;
    /* 0044 */ M2ModelTrack<uint8_t> m_visibilityTrack;
};

struct M2ModelTextureTransform {
    /* 0000 */ M2Track m_translation;
    /* 0014 */ M2Track m_rotation;
    /* 0028 */ M2Track m_scaling;
};

struct M2PartTrack {
    /* 0000 */ M2Array m_times;
    /* 0008 */ M2Array m_values;
};

struct M2Particle {
    /* 0000 */ uint32_t m_particleId;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ uint16_t m_boneIndex;
    /* 0016 */ uint16_t m_textureIndex;
    /* 0018 */ M2Array m_geometryMdl;
    /* 0020 */ M2Array m_recursionMdl;
    /* 0028 */ uint8_t m_blendMode;
    /* 0029 */ uint8_t m_emitterType;
    /* 002A */ uint16_t m_colorIndex;
    /* 002C */ uint16_t unk_002C;
    /* 002E */ int16_t m_priorityPlane;
    /* 0030 */ uint16_t m_rows;
    /* 0032 */ uint16_t m_cols;
    /* 0034 */ M2Track m_speedTrack;
    /* 0048 */ M2Track m_variationTrack;
    /* 005C */ M2Track m_latitudeTrack;
    /* 0070 */ M2Track m_longitudeTrack;
    /* 0084 */ M2Track m_gravityTrack;
    /* 0098 */ M2Track m_lifeTrack;
    /* 00AC */ float m_lifeVariation;
    /* 00B0 */ M2Track m_emissionRateTrack;
    /* 00C4 */ float m_emissionRateVariation;
    /* 00C8 */ M2Track m_widthTrack;
    /* 00DC */ M2Track m_lengthTrack;
    /* 00F0 */ M2Track m_zsourceTrack;
    /* 0104 */ M2PartTrack m_colorTrack;
    /* 0114 */ M2PartTrack m_alphaTrack;
    /* 0124 */ M2PartTrack m_scaleTrack;
    /* 0134 */ C2Vector m_scaleVariation;
    /* 013C */ M2PartTrack m_headCellTrack;
    /* 014C */ M2PartTrack m_tailCellTrack;
    /* 015C */ float m_tailLength;
    /* 0160 */ float m_twinkleFPS;
    /* 0164 */ float m_twinkleOnOff;
    /* 0168 */ CRange m_twinkleScale;
    /* 0170 */ float m_ivelScale;
    /* 0174 */ float m_drag;
    /* 0178 */ float m_initialSpin;
    /* 017C */ float m_initialSpinVariation;
    /* 0180 */ float m_spin;
    /* 0184 */ float m_spinVariation;
    /* 0188 */ CAaBox m_tumble;
    /* 01A0 */ C3Vector m_windVector;
    /* 01AC */ float m_windTime;
    /* 01B0 */ float m_followSpeed1;
    /* 01B4 */ float m_followScale1;
    /* 01B8 */ float m_followSpeed2;
    /* 01BC */ float m_followScale2;
    /* 01C0 */ M2Array m_spline;
    /* 01C8 */ M2Track m_visibilityTrack;
};

#endif
