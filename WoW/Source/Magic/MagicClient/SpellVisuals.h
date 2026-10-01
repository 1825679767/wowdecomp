#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLVISUALS_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLVISUALS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Object/WGUID.h"

struct CVar;
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
    /* 0008 */ uint8_t unk_0008[3];
    /* 000B */ uint8_t unk_000B[4];
    /* 000F */ uint8_t unk_000F[1];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[1];
    /* 0015 */ uint8_t unk_0015[4];
    /* 0019 */ uint8_t unk_0019[3];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t unk_0024[4];
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint8_t unk_0038[12];
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint8_t unk_0054[4];
    /* 0058 */ uint32_t unk_0058;

    ~LightningObject();
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
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ C3Vector m_groundNormal;
    /* 0060 */ uint32_t m_startTime;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ float unk_007C;
    /* 0080 */ uint32_t unk_0080;
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
void SpellVisualsFlagMountTransitionFinished(MountTransitionObject* transition);
void SpellVisualsPlayCameraShakeID(int32_t id, const C3Vector* position);
void SpellVisualsBlizzardSetFrameOfReference(BlizzardObject* object, const C44Matrix* matrix);
void SpellVisualsComputeMountTransitionJumpVec(const MountTransitionObject* transition, C3Vector* jump);
uint32_t SpellVisualsGetModelAttachRecCount(uint32_t kitId);
const SpellVisualKitModelAttachRec* SpellVisualsGetModelAttachRec(uint32_t kitId, uint32_t index);
MountTransitionObject* SpellVisualsStartMountTransition(CGUnit_C* unit);
void SpellVisualsCancelMountTransition(MountTransitionObject* transition);
void SpellVisualsHandleEclipse(const CImVector* color, float fadeIn, const SpellRec* spell);

#endif
