#include "Effect_C.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

// STATIC: 0x00ADA764
static const char* s_specialUnitEffectNames[] = {
    "HARDCODED Footstep Water Run Spray",
    "HARDCODED Footstep Water Walk Spray",
    "HARDCODED Breath Underwater",
    "HARDCODED Breath Cold",
    "HARDCODED Loot Art",
    "HARDCODED Unit Level Up",
    "HARDCODED Mount Poof",
    "HARDCODED Inebriated Bubbles",
    "HARDCODED Meeting Stone Join",
    "HARDCODED Reputation",
    "HARDCODED Resist Spell",
    "HARDCODED Achievement Base",
};

// STATIC: 0x00CA0504
static CEffect* s_orphanedEffects;

// STATIC: 0x00CA0508
static CEffect* s_destroyedEffects;

// STATIC: 0x00CA050C
static int32_t s_specialUnitEffectIDs[sizeof(s_specialUnitEffectNames) / sizeof(s_specialUnitEffectNames[0])];

// VERIFY: 8db32f34 auto
// OFFSET: 0x006F7520
static int32_t BuildSpecialUnitEffectTable() {
    memset(s_specialUnitEffectIDs, 0, sizeof(s_specialUnitEffectIDs));

    for (int32_t i = 0; i < g_spellVisualEffectNameDB.m_numRecords; i++) {
        SpellVisualEffectNameRec* rec = (i < 0 || i >= g_spellVisualEffectNameDB.m_numRecords) ? nullptr : &g_spellVisualEffectNameDB.m_records[i];

        for (uint32_t effect = 0; effect < sizeof(s_specialUnitEffectNames) / sizeof(s_specialUnitEffectNames[0]); effect++) {
            if (!SStrCmp(s_specialUnitEffectNames[effect], rec->m_name, STORM_MAX_STR)) {
                s_specialUnitEffectIDs[effect] = rec->m_id;
                break;
            }
        }
    }

    return 1;
}

// VERIFY: d8c1c6a0 auto
// OFFSET: 0x006F75E0
int32_t SpecialUnitEffectsInitialize() {
    return BuildSpecialUnitEffectTable();
}

// VERIFY: 00dde46e manual
// OFFSET: 0x006FA3C0
void CEffect::Destroy() {
    while (s_destroyedEffects) {
        CEffect* effect = s_destroyedEffects;

        if (--effect->m_refCount == 0) {
            delete effect;
        }
    }

    while (s_orphanedEffects) {
        CEffect* effect = s_orphanedEffects;

        if (--effect->m_refCount == 0) {
            delete effect;
        }
    }
}

// VERIFY: 2ef1b8ec manual
// OFFSET: 0x006F74B0
void CEffect::UnLinkSelf() {
    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_prevLink = nullptr;
    this->m_next = nullptr;
}

// VERIFY: 694be16d auto
// OFFSET: 0x006F75B0
const SpellVisualEffectNameRec* GetSpecialSpellVisualEffectNameRec(int32_t effect) {
    return g_spellVisualEffectNameDB.GetRecord(s_specialUnitEffectIDs[effect]);
}

// VERIFY: 582bd84a auto
// OFFSET: 0x006F75F0
void CEffect::SetEmissionRateIgnoresDistance(int32_t ignore) {
    if (!this->m_model || !this->m_model->IsLoaded(0, 0)) {
        return;
    }

    for (uint32_t i = 0; ; i++) {
        CM2Model* model = this->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(nullptr);
        }

        if (i >= model->m_shared->m_data->m_particles.m_count) {
            break;
        }

        model = this->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(nullptr);
        }

        CParticleEmitter2* emitter = model->m_particleEmitters[i];

        if (ignore) {
            emitter->m_flags |= 0x400000;
        } else {
            emitter->m_flags &= ~0x400000;
        }
    }
}

// VERIFY: be80d20e manual
// OFFSET: 0x006F76C0
void CEffect::LinkTo(CEffect** head) {
    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = nullptr;
    this->m_prevLink = head;
    this->m_next = *head;
    *head = this;

    if (this->m_next) {
        this->m_next->m_prevLink = &this->m_next;
    }
}

// VERIFY: 166255dc manual
// OFFSET: 0x006F7850
void CEffect::DetachFromParent() {
    if (this->m_model && this->m_model->m_attachParent) {
        this->m_model->DetachFromParent();
    }
}

// VERIFY: e8248e58 auto
// OFFSET: 0x006F7870
void CEffect::SetEnabled(int32_t enabled) {
    if (!this->m_model) {
        return;
    }

    this->m_model->unk_017C = enabled ? 1.0f : 0.0f;
    this->m_model->SetEmittersEnabled(enabled);
    this->m_model->SetRibbonsEnabled(enabled);
}

// VERIFY: af80ebbc auto
// OFFSET: 0x006F78B0
void CEffect::InitLightningEndpoint() {
    this->m_lightningState = 0;

    if (!this->m_model) {
        return;
    }

    uint32_t time = this->m_model->m_scene->m_time;

    if (!time) {
        time = 1;
    }

    this->m_model->m_lastAnimTime = time;
    this->SetEnabled(0);
}

// VERIFY: 71fcd0d5 auto
// OFFSET: 0x006F7900
void CEffect::ConnectToLightningThisFrame() {
    if (!this->m_lightningState && this->m_model) {
        this->m_model->m_lastAnimTime = 0;
        this->SetEnabled(1);
    }

    this->m_lightningState = 2;
}

// VERIFY: 43ed07f0 auto
// OFFSET: 0x006F97D0
void CEffect::AddFishingLine(CGObject_C* object, const WOWGUID& target) {
    CM2Model* model = object->m_worldModel;

    if (!model) {
        return;
    }

    CM2Model* line = model->m_attachList;

    while (line && line->m_attachmentId != 1) {
        line = line->m_attachNext;
    }

    this->m_lineModel = line;
    line->m_refCount++;
    this->m_lineModel->m_fFlags |= 0x20;
    this->m_lineModel->m_drawCallback = &CEffect::DrawFishingString;
    this->m_lineModel->m_drawArg = this;
    this->m_sourceGuid = object->m_obj->m_guid;
    this->m_targetGuid = target;
}

// VERIFY: e993f4c6 manual
// OFFSET: 0x006F9D70
CEffect::CEffect() {
    this->m_attachmentId = -1;
    this->m_model = nullptr;
    this->m_lineModel = nullptr;
    this->m_sourceGuid = WOWGUID();
    this->m_targetGuid = WOWGUID();
    this->m_spellId = 0;
    this->m_kitRec = nullptr;
    this->m_effectNameRec = nullptr;
    this->m_flags = 0;
    this->unk_004C = 0;
    this->unk_0050 = 0;
    this->unk_0054 = 0;
    this->unk_0058 = 0;
    this->unk_005C = 0;
    this->unk_0060 = 0;
    this->unk_0064 = 0;
    this->unk_0068 = 0;
    this->unk_006C = 0;
    this->unk_0070 = 0;
    this->unk_0074 = 0;
    this->unk_0078 = 0;
    this->unk_0080 = 0;
    this->unk_0084 = 0;
    this->unk_0088 = 0.0f;
    this->unk_008C = -1;
    this->unk_0090 = 0;
    this->unk_0094 = 0;
    this->unk_0098 = 0;
    this->unk_009C = 12;
    this->unk_00A0 = 0;
    this->unk_00C0 = 0;
    this->unk_00C4 = 0;
    this->unk_00C8 = 0;
    this->unk_00CC = 0;
    this->unk_00D0 = 0;
    this->unk_00D4 = 0;
    this->unk_00D8 = 0;
    this->unk_00DC = 0;
    this->unk_00E0 = 0.0f;
    this->unk_00E4 = 0.0f;
    this->unk_00E8 = 0.0f;
    this->unk_00F0 = 0;
    this->m_refCount = 1;
    this->unk_00F8 = 0;
    this->m_lightningState = 0;
    this->m_attachRec = nullptr;
    this->m_prevLink = nullptr;
    this->m_next = nullptr;
}

// VERIFY: c30a71c8 auto
// OFFSET: 0x006F7A00
void CEffect::MoveToOrphanList() {
    this->LinkTo(&s_orphanedEffects);
}

// VERIFY: 1b5e24a5 manual
// OFFSET: 0x006FA390
void CEffect::Release() {
    if (this->m_refCount-- == 1) {
        delete this;
    }
}
