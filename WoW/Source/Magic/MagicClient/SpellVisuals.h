#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLVISUALS_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLVISUALS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Object/WGUID.h"

struct CVar;
class CEffect;
class LightningObject;
class SpellChainEffectsRec;
class CTexture;
class CGObject_C;
class SpellVisualKitModelAttachRec;
class C44Matrix;
class SpellRec;
class CGUnit_C;

class BlizzardObject : public TSLinkedNode<BlizzardObject> {
    public:
    /* 0008 */ uint8_t unk_0008[256];
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint32_t unk_0114;
    /* 0118 */ uint32_t unk_0118;
    /* 011C */ uint32_t unk_011C;
    /* 0120 */ uint32_t unk_0120;
    /* 0124 */ float unk_0124;
    /* 0128 */ uint8_t unk_0128[16];
    /* 0138 */ uint32_t unk_0138;
    /* 013C */ uint32_t unk_013C;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint8_t unk_0144[64];
    /* 0184 */ uint32_t unk_0184;

    ~BlizzardObject();
    void SetFrameOfReference(const C44Matrix* matrix);
    bool Update();
};

class EclipseState {
    public:
    /* 0000 */ CImVector m_color;
    /* 0004 */ uint32_t m_start;
    /* 0008 */ uint32_t m_fadeInEnd;
    /* 000C */ uint32_t m_holdEnd;
    /* 0010 */ uint32_t m_end;

    void Update(uint32_t time);
};

class Shard {
    public:
    /* 0000 */ uint8_t unk_0000[32];
};

class LightningObject : public TSLinkedNode<LightningObject> {
    public:
    struct Bolt {
        /* 0000 */ uint16_t m_source;
        /* 0002 */ uint16_t m_target;
        /* 0004 */ uint32_t m_startTime;
        /* 0008 */ uint32_t m_endTime;
        /* 000C */ uint32_t m_lightning;
        /* 0010 */ int32_t m_attachment;
    };

    static void CreateFromSpell(CGObject_C* caster, CEffect* effect, uint32_t chainEffectsId, const WOWGUID* targets, uint32_t count, uint32_t spellId, bool persist, bool reverse, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment);
    static C3Vector GetCasterPosition(CGUnit_C* unit, uint32_t attachment, const C3Vector& offset, int32_t raw);
    static C3Vector GetImpactPosition(CGUnit_C* unit, uint32_t spellId);

    /* 0008 */ TSGrowableArray<uint64_t> m_targets;
    /* 0018 */ TSGrowableArray<Bolt> m_bolts;
    /* 0028 */ uint32_t m_endTime;
    /* 002C */ CTexture* m_texture;
    /* 0030 */ uint32_t m_spellId;
    /* 0034 */ uint32_t m_sourceAttachment;
    /* 0038 */ C3Vector m_sourceOffset;
    /* 0044 */ C3Vector m_targetOffset;
    /* 0050 */ uint32_t m_flags;
    /* 0054 */ CEffect* m_effect;
    /* 0058 */ SpellChainEffectsRec* m_chainEffects;
    /* 005C */ int32_t m_refCount;

    LightningObject();
    ~LightningObject();
    void InitializeBolts(CGObject_C* caster, const WOWGUID* targets, uint32_t count, bool reverse, int32_t attachment);
    bool Tick(uint32_t time);
};

void LightningObjectReleaseRef(LightningObject* object);

class MountTransitionObject : public TSLinkedNode<MountTransitionObject> {
    public:
    /* 0008 */ WOWGUID m_unitGuid;
    /* 0010 */ float m_height;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ float unk_0020;
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ C3Vector m_jumpVector;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ C3Vector m_position;
    /* 0054 */ C3Vector m_groundNormal;
    /* 0060 */ uint32_t m_startTime;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ float m_facing;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ float m_alpha;
    /* 0080 */ uint32_t unk_0080;

    bool Tick();
};

extern CVar* s_spellEffectLevelVar;

void SpellVisualsBlizzardStop(BlizzardObject** object);
void SpellVisualsInitialize();
void SpellVisualsInitializeModelAttachLookup();
void SpellVisualsRender();
void SpellVisualsTick();
void SpellVisualsClear();
void SpellVisualsShutdown();
void SpellVisualsShutdownModelAttachLookup();
int32_t SpellVisualsIsMountTransitionFinished(const MountTransitionObject* transition);
float SpellVisualsGetMountTransitionHeight(const MountTransitionObject* transition);
void SpellVisualsGetMountTransitionTransform(const MountTransitionObject* transition, C3Vector* position, C3Vector* up, float* facing);
float SpellVisualsGetMountTransitionAlpha(const MountTransitionObject* transition);
void SpellVisualsFlagMountTransitionFinished(MountTransitionObject* transition);
void SpellVisualsPlayCameraShakeID(int32_t id, const C3Vector* position);
void SpellVisualsBlizzardSetFrameOfReference(BlizzardObject* object, const C44Matrix* matrix);
void SpellVisualsComputeMountTransitionJumpVec(const MountTransitionObject* transition, C3Vector* jump);
uint32_t SpellVisualsGetModelAttachRecCount(uint32_t kitId);
const SpellVisualKitModelAttachRec* SpellVisualsGetModelAttachRec(uint32_t kitId, uint32_t index);
MountTransitionObject* SpellVisualsStartMountTransition(CGUnit_C* unit);
void SpellVisualsCancelMountTransition(MountTransitionObject* transition);
void SpellVisualsCreateChainLightning(CGUnit_C* caster, CEffect* effect, uint32_t chainEffectsId, bool persist, bool reverse, uint32_t spellId, const WOWGUID* targets, uint32_t count, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment);
void SpellVisualsHandleEclipse(const CImVector* color, float fadeIn, const SpellRec* spell);

#endif
