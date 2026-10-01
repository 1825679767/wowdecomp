#include "SpellVisuals.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/Lightning.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/DB/Rec/SpellRec.h"

// STATIC: 0x00D39750
static CLightningManager* s_lightningManager;

// STATIC: 0x00AF5090
static int32_t s_unk_AF5090 = -1;

// STATIC: 0x00AF5094
static TSList<BlizzardObject> s_blizzardObjects;

// STATIC: 0x00AF50A0
static TSList<LightningObject> s_lightningObjects;

// STATIC: 0x00D3975C
static CDataAllocator s_blizzardObjectHeap(sizeof(BlizzardObject), 8);

// STATIC: 0x00D39770
static CDataAllocator s_shardHeap(sizeof(Shard), 32);

// STATIC: 0x00D39798
static EclipseState s_eclipse;

// STATIC: 0x00AF50AC
static TSList<MountTransitionObject> s_mountTransitions;

// STATIC: 0x00D39758
static TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>* s_modelAttachLookup;

// VERIFY: 1522f2db manual
// OFFSET: 0x007FBB80
void SpellVisualsShutdownModelAttachLookup() {
    if (!s_modelAttachLookup) {
        return;
    }

    uint32_t count = s_modelAttachLookup->Count();

    for (uint32_t i = 0; i < count; i++) {
        if ((*s_modelAttachLookup)[i]) {
            delete (*s_modelAttachLookup)[i];
        }
    }

    delete s_modelAttachLookup;
    s_modelAttachLookup = nullptr;
}

// VERIFY: f470cb1c manual
// OFFSET: 0x007FBE90
void SpellVisualsInitializeModelAttachLookup() {
    SpellVisualsShutdownModelAttachLookup();

    uint32_t maxKitID = g_spellVisualKitDB.m_maxID <= 0 ? 0 : g_spellVisualKitDB.m_maxID;
    uint32_t maxAttachID = g_spellVisualKitModelAttachDB.m_maxID <= 0 ? 0 : g_spellVisualKitModelAttachDB.m_maxID;

    s_modelAttachLookup = new (SMemAlloc(sizeof(TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>), __FILE__, __LINE__, 0)) TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>();
    s_modelAttachLookup->GrowToFit(maxKitID, 1);

    for (uint32_t id = 1; id <= maxAttachID; id++) {
        const SpellVisualKitModelAttachRec* rec = g_spellVisualKitModelAttachDB.GetRecord(id);

        if (!rec || !rec->m_parentSpellVisualKitID || static_cast<uint32_t>(rec->m_parentSpellVisualKitID) > maxKitID) {
            continue;
        }

        TSGrowableArray<const SpellVisualKitModelAttachRec*>*& kitAttachments = (*s_modelAttachLookup)[rec->m_parentSpellVisualKitID];

        if (!kitAttachments) {
            kitAttachments = new (SMemAlloc(sizeof(TSGrowableArray<const SpellVisualKitModelAttachRec*>), __FILE__, __LINE__, 0)) TSGrowableArray<const SpellVisualKitModelAttachRec*>();
        }

        kitAttachments->New(rec);
    }
}

// VERIFY: 95aa500d manual
// OFFSET: 0x007FC5A0
void SpellVisualsInitialize() {
    s_lightningManager = new (SMemAlloc(sizeof(CLightningManager), __FILE__, __LINE__, 0)) CLightningManager();
    s_spellEffectLevelVar = CVar::Lookup("spellEffectLevel");
    SpellVisualsInitializeModelAttachLookup();
}

// VERIFY: 1702458f manual
// OFFSET: 0x007FC9F0
void SpellVisualsClear() {
    s_lightningObjects.Clear();

    BlizzardObject* object;

    while ((object = s_blizzardObjects.Head()) != nullptr) {
        object->~BlizzardObject();
        s_blizzardObjectHeap.PutData(object, nullptr, 0);
    }
}

// VERIFY: 9c3b0607 auto
// OFFSET: 0x007FCBC0
void SpellVisualsShutdown() {
    SpellVisualsClear();

    if (s_lightningManager) {
        delete s_lightningManager;
    }

    s_unk_AF5090 = -1;
    s_blizzardObjectHeap.Clear(typeid(BlizzardObject).raw_name(), -2, 1);
    s_shardHeap.Clear(typeid(Shard).raw_name(), -2, 1);
    s_spellEffectLevelVar = nullptr;
    SpellVisualsShutdownModelAttachLookup();
}

// VERIFY: c2183fd9 auto
// OFFSET: 0x007F9EE0
void SpellVisualsBlizzardStop(BlizzardObject** object) {
    (*object)->unk_0124 = 0.0f;
    *object = nullptr;
}

// VERIFY: cf521745 auto
// OFFSET: 0x007F9F70
int32_t SpellVisualsIsMountTransitionFinished(const MountTransitionObject* transition) {
    return (transition->unk_0078 >> 1) & 1;
}

// VERIFY: bea8214c auto
// OFFSET: 0x007F9EC0
void SpellVisualsRender() {
    s_lightningManager->Render(CGWorldFrame::GetActiveCamera()->m_position);
}

// VERIFY: 6a5e8212 auto
// OFFSET: 0x007F9F00
float SpellVisualsGetMountTransitionHeight(const MountTransitionObject* transition) {
    return transition->m_height;
}

// VERIFY: 21a8f719 auto
// OFFSET: 0x007F9F60
void SpellVisualsFlagMountTransitionFinished(MountTransitionObject* transition) {
    transition->unk_0078 |= 0x2;
}

// VERIFY: 985816b3 auto
// OFFSET: 0x007FA620
void SpellVisualsPlayCameraShakeID(int32_t id, const C3Vector* position) {
    SpellEffectCameraShakesRec* rec = g_spellEffectCameraShakesDB.GetRecord(id);

    if (!rec) {
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (rec->m_cameraShake[i]) {
            CGWorldFrame::GetActiveCamera()->AddShakeById(rec->m_cameraShake[i], position);
        }
    }
}

// VERIFY: 3532766c auto
// OFFSET: 0x007FA680
void SpellVisualsBlizzardSetFrameOfReference(BlizzardObject* object, const C44Matrix* matrix) {
    object->SetFrameOfReference(matrix);
}

// VERIFY: eeefbc3b manual
// OFFSET: 0x007FA930
void SpellVisualsComputeMountTransitionJumpVec(const MountTransitionObject* transition, C3Vector* jump) {
    *jump = transition->m_jumpVector;

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transition->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_mountTransitionEffect) {
        return;
    }

    CM2Model* model = unit->m_mountTransitionEffect->m_model;

    if (!model) {
        return;
    }

    C3Vector world = model->GetWorldPosition();
    C3Vector attach = model->GetAttachmentPosition(0);
    jump->x = attach.x - world.x;
    jump->y = attach.y - world.y;
    jump->z = attach.z - world.z;

    float height = transition->m_height;
    jump->x = jump->x * height;
    jump->y = jump->y * height;
    jump->z = height * jump->z;
}

// VERIFY: 32467c5c manual
// OFFSET: 0x007FA9F0
uint32_t SpellVisualsGetModelAttachRecCount(uint32_t kitId) {
    if (kitId < s_modelAttachLookup->m_count && s_modelAttachLookup->m_data[kitId]) {
        return s_modelAttachLookup->m_data[kitId]->m_count;
    }

    return 0;
}

// VERIFY: 1fcb7f6f auto
// OFFSET: 0x007FBE00
MountTransitionObject* SpellVisualsStartMountTransition(CGUnit_C* unit) {
    MountTransitionObject* transition = s_mountTransitions.NewNode(2, 0, 0);

    if (!transition) {
        return nullptr;
    }

    transition->unk_0078 = 0;
    transition->m_unitGuid = unit->m_obj->m_guid;
    transition->m_groundNormal = unit->m_movement->GetGroundNormal();
    transition->m_startTime = OsGetAsyncTimeMs();
    transition->unk_0020 = 0.0f;
    transition->unk_0024 = 0.0f;
    transition->unk_0028 = 0.0f;
    transition->m_alpha = 0.0f;
    transition->unk_0080 = OsGetAsyncTimeMs();
    return transition;
}

// VERIFY: c6399436 auto
// OFFSET: 0x007FC9D0
void SpellVisualsCancelMountTransition(MountTransitionObject* transition) {
    if (transition) {
        s_mountTransitions.DeleteNode(transition);
    }
}

// VERIFY: 1bb4ee05 auto
// OFFSET: 0x007FAA20
const SpellVisualKitModelAttachRec* SpellVisualsGetModelAttachRec(uint32_t kitId, uint32_t index) {
    return s_modelAttachLookup->m_data[kitId]->m_data[index];
}

// VERIFY: 4bfbe4ed manual
// OFFSET: 0x007F9E10
void EclipseState::Update(uint32_t time) {
    if (this->m_start == this->m_end) {
        return;
    }

    float blend = 0.0f;

    if (time < this->m_fadeInEnd) {
        blend = static_cast<float>(time - this->m_start) / static_cast<float>(this->m_fadeInEnd - this->m_start);
    } else if (time < this->m_holdEnd) {
        blend = 1.0f;
    } else if (time < this->m_end) {
        blend = 1.0f - static_cast<float>(time - this->m_holdEnd) / static_cast<float>(this->m_end - this->m_holdEnd);
    } else {
        this->m_end = 0;
        this->m_start = 0;
    }

    DayNight::SetFlashColor(this->m_color, blend);
}

// VERIFY: 21c0a07b manual
// OFFSET: 0x007FA450
void SpellVisualsHandleEclipse(const CImVector* color, float fadeIn, const SpellRec* spell) {
    uint32_t castTime = SpellRec::GetCastTime(spell, 0, 0, 0);
    s_eclipse.m_color = *color;
    uint32_t now = OsGetAsyncTimeMs();
    s_eclipse.m_start = now;
    s_eclipse.m_holdEnd = now + castTime;
    s_eclipse.m_fadeInEnd = static_cast<int32_t>(castTime * fadeIn) + now;
    s_eclipse.m_end = now + castTime + 100;
}

// VERIFY: b5145a50 auto
// OFFSET: 0x007F9F10
float SpellVisualsGetMountTransitionAlpha(const MountTransitionObject* transition) {
    return transition->m_alpha;
}

// VERIFY: 85b7ab4a auto
// OFFSET: 0x007F9F20
void SpellVisualsGetMountTransitionTransform(const MountTransitionObject* transition, C3Vector* position, C3Vector* up, float* facing) {
    *position = transition->m_position;
    *up = transition->m_groundNormal;
    *facing = transition->m_facing;
}

// VERIFY: 77642498 manual
// OFFSET: 0x007FCA30
void SpellVisualsTick() {
    uint32_t time = OsGetAsyncTimeMs();
    LightningObject* lightning = s_lightningObjects.Head();

    while (!s_lightningObjects.IsEnd(lightning)) {
        LightningObject* next = s_lightningObjects.RawNext(lightning);

        if (!lightning->Tick(time)) {
            s_lightningObjects.UnlinkNode(lightning);

            if (--lightning->m_refCount == 0) {
                delete lightning;
            }
        }

        lightning = next;
    }

    float elapsed = 0.0f;

    if (s_unk_AF5090 != -1) {
        elapsed = static_cast<float>((time - s_unk_AF5090) * 0.001);
    }

    s_unk_AF5090 = time;
    s_lightningManager->Update(elapsed);
    s_eclipse.Update(time);
    BlizzardObject* blizzard = s_blizzardObjects.Head();

    while (!s_blizzardObjects.IsEnd(blizzard)) {
        BlizzardObject* next = s_blizzardObjects.RawNext(blizzard);

        if (!blizzard->Update()) {
            blizzard->~BlizzardObject();
            s_blizzardObjectHeap.PutData(blizzard, nullptr, 0);
        }

        blizzard = next;
    }

    MountTransitionObject* transition = s_mountTransitions.Head();

    while (!s_mountTransitions.IsEnd(transition)) {
        MountTransitionObject* next = s_mountTransitions.RawNext(transition);

        if (!transition->Tick()) {
            s_mountTransitions.DeleteNode(transition);
        }

        transition = next;
    }
}
