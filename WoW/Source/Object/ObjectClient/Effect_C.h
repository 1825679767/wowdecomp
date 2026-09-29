#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

class CGObject_C;
class CM2Lighting;
class CM2Model;
class SpellVisualEffectNameRec;
class SpellVisualKitRec;

class CEffect {
    public:
    void ApplyItemVisual();
    static void CleanUpStaticLists();
    static void Destroy();
    static void DrawFishingString(CM2Model* model, const CM2Lighting* lighting, void* arg);

    /* 0000 */ CM2Model* m_model;
    /* 0004 */ CM2Model* m_lineModel;
    /* 0008 */ WOWGUID m_sourceGuid;
    /* 0010 */ WOWGUID m_targetGuid;
    /* 0018 */ uint32_t m_spellId;
    /* 001C */ const SpellVisualKitRec* m_kitRec;
    /* 0020 */ const SpellVisualEffectNameRec* m_effectNameRec;
    /* 0024 */ int32_t m_attachmentId;
    /* 0028 */ C3Vector m_offset;
    /* 0034 */ SE2SoundKitObject m_soundKitObject;
    /* 0048 */ uint32_t m_flags;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ float unk_0088;
    /* 008C */ int32_t unk_008C;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ C3Vector m_position;
    /* 00B0 */ C3Vector unk_00B0;
    /* 00BC */ uint32_t unk_00BC;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ uint32_t unk_00DC;
    /* 00E0 */ float unk_00E0;
    /* 00E4 */ float unk_00E4;
    /* 00E8 */ float unk_00E8;
    /* 00EC */ CImVector m_color;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ int32_t m_refCount;
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ int32_t m_lightningState;
    /* 0100 */ void* m_attachRec;
    /* 0104 */ CEffect** m_prevLink;
    /* 0108 */ CEffect* m_next;
    /* 010C */ uint32_t unk_010C;

    CEffect();
    ~CEffect();
    void AddFishingLine(CGObject_C* object, const WOWGUID& target);
    void AddSpecialEffect(int32_t type, const WOWGUID& target, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param));
    void ConnectToLightningThisFrame();
    void DetachFromParent();
    int32_t UpdateAttachment();
    void InitLightningEndpoint();
    void LinkTo(CEffect** head);
    void MoveToDestroyList();
    void EffectUpdate();
    void MoveToOrphanList();
    void Release();
    void SetEmissionRateIgnoresDistance(int32_t ignore);
    void SetEnabled(int32_t enabled);
    void UnLinkSelf();
};

const SpellVisualEffectNameRec* GetSpecialSpellVisualEffectNameRec(int32_t effect);
int32_t SpecialUnitEffectsInitialize();

#endif
