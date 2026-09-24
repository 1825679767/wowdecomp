#ifndef ENGINE_SOURCE_SERVICES_PARTICLESYSTEM2_H
#define ENGINE_SOURCE_SERVICES_PARTICLESYSTEM2_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Storm/H/stpl.h"

class CM2Model;
class CParticle2;
class CParticle2_Model;

class CParticle2 {
    public:
    /* 0000 */ float m_age;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C3Vector m_velocity;
    /* 001C */ int16_t m_lifeSeed;
    /* 001E */ uint16_t m_randomSeed;
};

class CParticle2_Model {
    public:
    /* 0000 */ CParticle2 m_base;
    /* 0020 */ C4Quaternion m_orientation;
    /* 0030 */ C3Vector m_angularVelocity;
    /* 003C */ CM2Model* m_model;
};

class CParticleEmitter2 {
    public:
    /* 0004 */ int32_t m_refCount;
    /* 0008 */ uint32_t m_emitAccumulator;
    /* 000C */ uint32_t m_textureCellShift;
    /* 0010 */ uint32_t m_textureCellWidth;
    /* 0014 */ uint32_t m_textureCellHeight;
    /* 0018 */ uint32_t m_priorityPlane;
    /* 001C */ uint32_t m_renderedParticles;
    /* 0020 */ int32_t m_emitterType;
    /* 0024 */ uint32_t m_random;
    /* 0028 */ uint32_t m_random2;
    /* 002C */ TSGrowableArray<CParticle2> m_particles;
    /* 003C */ TSGrowableArray<CParticle2_Model> m_particleModels;
    /* 004C */ TSGrowableArray<uint32_t> m_liveList;
    /* 005C */ TSGrowableArray<uint32_t> m_freeList;
    /* 006C */ uint32_t m_childEmitterCount;
    /* 0070 */ uint32_t m_childEmitters [4];
    /* 0080 */ uint32_t m_model;
    /* 0084 */ uint32_t m_scene;
    /* 0088 */ uint32_t m_childModel;
    /* 008C */ uint32_t m_vertsPerParticle;
    /* 0090 */ uint32_t m_indicesPerParticle;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t m_hasModel;
    /* 009C */ float m_emissionRate;
    /* 00A0 */ float m_emissionRateVariation;
    /* 00A4 */ float m_life;
    /* 00A8 */ float m_lifeVariation;
    /* 00AC */ float m_tailLength;
    /* 00B0 */ float m_speed;
    /* 00B4 */ float m_gravity;
    /* 00B8 */ float m_variation;
    /* 00BC */ float m_zsource;
    /* 00C0 */ float m_initialSpin;
    /* 00C4 */ float m_initialSpinVariation;
    /* 00C8 */ float m_spinVariation;
    /* 00CC */ float m_spin;
    /* 00D0 */ uint32_t m_materialBlend;
    /* 00D4 */ uint32_t m_materialFlags;
    /* 00D8 */ uint32_t m_colorTrack;
    /* 00DC */ uint32_t m_alphaTrack;
    /* 00E0 */ uint32_t m_scaleTrack;
    /* 00E4 */ float m_scaleVariationX;
    /* 00E8 */ float m_scaleVariationY;
    /* 00EC */ uint32_t m_headCellTrack;
    /* 00F0 */ uint32_t m_tailCellTrack;
    /* 00F4 */ uint32_t m_replacementColors;
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ uint32_t unk_00FC;
    /* 0100 */ uint32_t unk_0100;
    /* 0104 */ uint32_t unk_0104;
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint32_t unk_0114;
    /* 0118 */ uint32_t m_simpleMidTime;
    /* 011C */ uint32_t m_simpleKeys;
    /* 0120 */ uint32_t m_textureCols;
    /* 0124 */ uint32_t m_textureRows;
    /* 0128 */ uint32_t m_texture;
    /* 012C */ uint32_t unk_012C;
    /* 0130 */ uint32_t m_alphaScale;
    /* 0134 */ uint32_t m_flags;
    /* 0138 */ uint32_t m_batchFlags;
    /* 013C */ float m_twinkleFPS;
    /* 0140 */ float m_twinkleOnOff;
    /* 0144 */ float m_twinkleScaleBase;
    /* 0148 */ float m_twinkleScaleSpan;
    /* 014C */ float m_ivelScale;
    /* 0150 */ float m_tumbleBaseX;
    /* 0154 */ float m_tumbleSpanX;
    /* 0158 */ float m_tumbleBaseY;
    /* 015C */ float m_tumbleSpanY;
    /* 0160 */ float m_tumbleBaseZ;
    /* 0164 */ float m_tumbleSpanZ;
    /* 0168 */ float m_drag;
    /* 016C */ float m_windVectorX;
    /* 0170 */ float m_windVectorY;
    /* 0174 */ float m_windVectorZ;
    /* 0178 */ float m_windTime;
    /* 017C */ float m_followIntercept;
    /* 0180 */ float m_followSlope;
    /* 0184 */ C44Matrix m_xform;
    /* 01C4 */ uint32_t m_cameraPos;
    /* 01C8 */ uint32_t unk_01C8;
    /* 01CC */ uint32_t unk_01CC;
    /* 01D0 */ C3Vector m_parentPosition;
    /* 01DC */ uint32_t m_ivelTimer;
    /* 01E0 */ C3Vector m_parentVelocity;
    /* 01EC */ uint32_t m_xformScale;
    /* 01F0 */ uint32_t unk_01F0;
    /* 01F4 */ uint32_t m_followOffset;
    /* 01F8 */ uint32_t unk_01F8;
    /* 01FC */ uint32_t unk_01FC;
    /* 0200 */ C3Vector m_burstOffset;
    /* 020C */ uint32_t m_billboardAxis;
    /* 0210 */ uint32_t unk_0210;
    /* 0214 */ uint32_t unk_0214;
    /* 0218 */ CAaBox m_worldBounds;
    /* 0230 */ uint32_t unk_0230;

    virtual ~CParticleEmitter2();
};

#endif
