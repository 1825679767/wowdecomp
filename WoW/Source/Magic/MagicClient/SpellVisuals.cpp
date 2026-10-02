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
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"

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

// INLINED
LightningObject::LightningObject() : m_refCount(1) {
}

// VERIFY: 67173777 auto
// OFFSET: 0x007FAA40
C3Vector LightningObject::GetCasterPosition(CGUnit_C* unit, uint32_t attachment, const C3Vector& offset, int32_t raw) {
    CM2Model* model = unit->m_worldModel;
    C3Vector base = unit->m_vehiclePassenger ? unit->GetModelPosition() : unit->GetPosition();
    C3Vector position;
    position.x = base.x;
    position.y = base.y;
    position.z = unit->m_scale * unit->m_height * 0.75f + base.z;

    if (model && model->IsLoaded(0, 0) && (model->m_fFlags & 0x8000)) {
        if (attachment != 0xFFFFFFFF && unit->HasAttachmentForEffect(attachment, raw)) {
            return unit->GetAttachmentPositionForEffect(attachment, offset, raw);
        }

        if (model->HasEvent(0x4C534324)) {
            return offset * model->GetEventWorldTransform(0x4C534324);
        }

        if (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f) {
            C3Vector transformed = offset * (unit->m_vehiclePassenger ? unit->GetModelMatrix() : unit->GetMatrix());
            position.x = transformed.x;
            position.y = transformed.y;
            position.z = unit->m_scale * unit->m_height * 0.75f + transformed.z;
        }
    }

    return position;
}

// VERIFY: 3636aab1 manual
// OFFSET: 0x007FABF0
C3Vector LightningObject::GetImpactPosition(CGUnit_C* unit, uint32_t spellId) {
    CM2Model* model = unit->m_worldModel;
    C3Vector position = unit->m_vehiclePassenger ? unit->GetModelPosition() : unit->GetPosition();
    position.z = unit->GetStandHeight() * 0.75f + position.z;
    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        if (model->IsLoaded(0, 0) && (model->m_fFlags & 0x8000) && model->HasAttachment(34)) {
            return model->GetAttachmentPosition(34);
        }

        return position;
    }

    SpellVisualRec buffer;
    const SpellVisualRec* visual = (unit->m_obj->m_type & TYPEMASK_UNIT) ? unit->GetAppropriateSpellVisual(&spell, buffer, 0, 0) : GetSpellVisual(&spell);
    int32_t raw = visual->m_flags & 0x200;

    if (model->IsLoaded(0, 0) && (model->m_fFlags & 0x8000) && unit->HasAttachmentForEffect(visual->m_missileDestinationAttachment, raw)) {
        position.x = visual->m_missileImpactOffset[0];
        position.y = -visual->m_missileImpactOffset[1];
        position.z = visual->m_missileImpactOffset[2];
        return unit->GetAttachmentPositionForEffect(visual->m_missileDestinationAttachment, position, raw);
    }

    if (visual->m_missileImpactOffset[0] != 0.0f || visual->m_missileImpactOffset[1] != 0.0f || visual->m_missileImpactOffset[2] != 0.0f) {
        C44Matrix matrix = unit->m_vehiclePassenger ? unit->GetModelMatrix() : unit->GetMatrix();
        matrix.d0 = 0.0f;
        matrix.d1 = 0.0f;
        matrix.d2 = 0.0f;
        C3Vector offset;
        offset.x = visual->m_missileImpactOffset[0];
        offset.y = -visual->m_missileImpactOffset[1];
        offset.z = visual->m_missileImpactOffset[2];
        C3Vector transformed = offset * matrix;
        position.x = transformed.x + position.x;
        position.y = transformed.y + position.y;
        position.z = transformed.z + position.z;
    }

    return position;
}

// VERIFY: 9da237e2 manual
// OFFSET: 0x007FAE90
bool LightningObject::Tick(uint32_t time) {
    for (uint32_t i = 0; i < this->m_bolts.Count(); i++) {
        Bolt& bolt = this->m_bolts[i];

        if (bolt.m_source == 0xFFFF || bolt.m_target == 0xFFFF) {
            continue;
        }

        CGObject_C* source = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->m_targets[bolt.m_source] }, TYPEMASK_OBJECT, __FILE__, __LINE__));
        CGObject_C* target = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->m_targets[bolt.m_target] }, TYPEMASK_OBJECT, __FILE__, __LINE__));
        CGUnit_C* sourceUnit = source && (source->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(source) : nullptr;
        CGUnit_C* targetUnit = target && (target->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(target) : nullptr;

        if ((this->m_flags & 0x1) || (time >= bolt.m_startTime && time < bolt.m_endTime)) {
            C3Vector sourcePos(0.0f, 0.0f, 0.0f);

            if (sourceUnit) {
                sourcePos = bolt.m_source ? LightningObject::GetImpactPosition(sourceUnit, this->m_spellId) : LightningObject::GetCasterPosition(sourceUnit, this->m_sourceAttachment, this->m_sourceOffset, this->m_flags & 0x4);
            } else if (source) {
                sourcePos = this->m_sourceOffset * source->GetMatrix();
            }

            C3Vector targetPos(0.0f, 0.0f, 0.0f);
            int32_t hidden = 0;

            if (targetUnit) {
                targetPos = LightningObject::GetImpactPosition(targetUnit, this->m_spellId);
            } else if (target) {
                targetPos = this->m_targetOffset * target->GetMatrix();
            } else if (this->m_flags & 0x2) {
                targetPos = this->m_targetOffset;
            } else {
                hidden = 1;
            }

            if (bolt.m_lightning == 0xFFFFFFFF) {
                bolt.m_lightning = s_lightningManager->Add(nullptr, nullptr, this->m_chainEffects);
                CLightning* lightning = s_lightningManager->GetTempPtr(bolt.m_lightning);
                lightning->SetSrcPos(sourcePos);
                lightning->SetDstPos(targetPos);
                lightning->SetTexture(this->m_texture);
            } else {
                s_lightningManager->Move(bolt.m_lightning, &sourcePos, &targetPos);
            }

            s_lightningManager->SetBoltVisible(bolt.m_lightning, source && !hidden);
            CLightning* lightning = s_lightningManager->GetTempPtr(bolt.m_lightning);

            if (source && lightning) {
                if (lightning->m_flags & 0x10) {
                    source->ConnectToLightningThisFrame(this->m_spellId, -1);
                }

                if (target) {
                    if (lightning->m_flags & 0x20) {
                        target->ConnectToLightningThisFrame(this->m_spellId, -1);
                    }
                } else if (lightning->m_flags & 0x20) {
                    source->ConnectToLightningThisFrame(this->m_spellId, bolt.m_attachment);
                }
            }
        }

        if (!(this->m_flags & 0x1) && time >= bolt.m_endTime && bolt.m_lightning != 0xFFFFFFFF) {
            s_lightningManager->Remove(bolt.m_lightning);
            bolt.m_lightning = 0xFFFFFFFF;
        }
    }

    return (this->m_flags & 0x1) || time < this->m_endTime;
}

// VERIFY: a8f49189 manual
// OFFSET: 0x007FB6E0
LightningObject::~LightningObject() {
    for (uint32_t i = 0; i < this->m_bolts.Count(); i++) {
        if (this->m_bolts[i].m_lightning != 0xFFFFFFFF) {
            s_lightningManager->Remove(this->m_bolts[i].m_lightning);
        }
    }

    this->m_bolts.Clear();
    this->m_targets.Clear();

    if (this->m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_texture));
        this->m_texture = nullptr;
    }

    if (this->m_effect) {
        this->m_effect->ReleaseLightningRef();
        this->m_effect->Release();
        this->m_effect = nullptr;
    }
}

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

// VERIFY: 96ae8133 manual
// OFFSET: 0x007FA4D0
void LightningObject::InitializeBolts(CGObject_C* caster, const WOWGUID* targets, uint32_t count, bool reverse, int32_t attachment) {
    uint32_t time = OsGetAsyncTimeMs() + this->m_chainEffects->m_segDelay;
    this->m_endTime = time;

    for (uint32_t i = 0; static_cast<int32_t>(i) < static_cast<int32_t>(count); i++) {
        Bolt& bolt = this->m_bolts[i];
        bolt.m_target = 0xFFFF;
        bolt.m_source = 0xFFFF;
        bolt.m_lightning = 0xFFFFFFFF;
        this->m_targets[i + 1] = targets[count - i - 1].m_guid;
        bolt.m_startTime = time;
        bolt.m_endTime = time + this->m_chainEffects->m_segDuration;
        bolt.m_source = reverse ? 0 : i;
        bolt.m_target = i + 1;

        if (static_cast<int32_t>(bolt.m_endTime - this->m_endTime) >= 0) {
            this->m_endTime = bolt.m_endTime;
        }

        time += static_cast<int64_t>(this->m_chainEffects->m_delayBetweenEffects);
        bolt.m_attachment = i ? -1 : attachment;

        if (!this->m_targets[i + 1]) {
            continue;
        }

        CGObject_C* object = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->m_targets[i + 1] }, TYPEMASK_OBJECT, __FILE__, __LINE__));

        if (!object) {
            continue;
        }

        for (CEffect* effect = object->m_effectList; effect; effect = effect->m_next) {
            if (effect->m_spellId == this->m_spellId && (effect->m_flags & 0x40000)) {
                effect->InitLightningEndpoint();
            }
        }
    }
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

// VERIFY: e8f5ccd3 manual
// OFFSET: 0x007FC5F0
void LightningObject::CreateFromSpell(CGObject_C* caster, CEffect* effect, uint32_t chainEffectsId, const WOWGUID* targets, uint32_t count, uint32_t spellId, bool persist, bool reverse, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment) {
    if (!caster || !spellId) {
        return;
    }

    SpellChainEffectsRec* root = g_spellChainEffectsDB.GetRecord(chainEffectsId);

    if (!root) {
        return;
    }

    SpellChainEffectsRec* chain = root;
    C3Vector sourceOffset(0.0f, 0.0f, 0.0f);
    C3Vector targetOffset(0.0f, 0.0f, 0.0f);
    CStatus status;
    uint32_t sourceAttachment = 0xFFFFFFFF;
    int32_t raw = 0;
    SpellRec spell;

    if (g_spellDB.GetRecord(spellId, &spell)) {
        const SpellVisualRec* visual = GetSpellVisual(&spell);

        if (visual) {
            sourceOffset.x = visual->m_missileCastOffset[0];
            sourceAttachment = visual->m_missileAttachment;
            sourceOffset.y = -visual->m_missileCastOffset[1];
            sourceOffset.z = visual->m_missileCastOffset[2];
            targetOffset.x = visual->m_missileImpactOffset[0];
            targetOffset.y = -visual->m_missileImpactOffset[1];
            targetOffset.z = visual->m_missileImpactOffset[2];
            raw = visual->m_flags & 0x200;
        }
    }

    bool useLocation = false;
    WOWGUID none[1];

    if (!count) {
        if (!location) {
            return;
        }

        count = 1;
        none[0] = WOWGUID();
        targets = none;
        useLocation = true;
    }

    uint32_t created = 0;
    uint32_t offset = 0;

    do {
        LightningObject* object = s_lightningObjects.NewNode(2, 0, 0);
        object->m_targets.SetCount(count + 1);
        object->m_bolts.SetCount(count);
        object->m_targets[0] = caster->m_obj->m_guid.m_guid;
        object->m_flags = 0;
        object->m_texture = TextureCreate(chain->m_texture, CGxTexFlags(GxTex_Linear, 1, 0, 0, 0, 0, 1), &status, 0);
        object->m_sourceOffset.x = sourceOffset.x;
        object->m_spellId = spellId;
        object->m_sourceAttachment = sourceAttachment;
        object->m_sourceOffset.y = sourceOffset.y;
        object->m_sourceOffset.z = sourceOffset.z;
        object->m_chainEffects = chain;

        if (!effect || persist) {
            object->m_effect = nullptr;
        } else {
            object->m_effect = effect;
            effect->m_lightningRefCount++;
            object->m_effect->m_refCount++;
        }

        if (useLocation) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(caster);
            C44Matrix matrix = unit->m_vehiclePassenger ? unit->GetModelMatrix() : unit->GetMatrix();
            matrix.d0 = 0.0f;
            matrix.d1 = 0.0f;
            matrix.d2 = 0.0f;
            C3Vector transformed = targetOffset * matrix;
            object->m_flags |= 0x2;
            object->m_targetOffset.x = transformed.x + location->x;
            object->m_targetOffset.y = transformed.y + location->y;
            object->m_targetOffset.z = transformed.z + location->z;
        } else {
            object->m_targetOffset = targetOffset;
        }

        if (persist) {
            object->m_flags |= 0x1;
        }

        if (raw) {
            object->m_flags |= 0x4;
        }

        object->InitializeBolts(caster, targets, count, reverse, attachment);

        if (persist) {
            object->m_refCount++;

            if (lightning) {
                lightning[created++] = object;
            }
        }

        if (created >= maxLightning) {
            break;
        }

        uint32_t next = CLightning::DecodeChainEffectsID(*reinterpret_cast<const uint32_t*>(root->m_combo + offset));

        if (!next) {
            break;
        }

        chain = g_spellChainEffectsDB.GetRecord(next);

        if (!chain) {
            break;
        }

        offset += 4;
    } while (offset < 0x30);
}

// VERIFY: 65ff87a7 auto
// OFFSET: 0x007FC950
void SpellVisualsCreateChainLightning(CGUnit_C* caster, CEffect* effect, uint32_t chainEffectsId, bool persist, bool reverse, uint32_t spellId, const WOWGUID* targets, uint32_t count, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment) {
    if (caster) {
        LightningObject::CreateFromSpell(caster, effect, chainEffectsId, targets, count, spellId, persist, reverse, lightning, maxLightning, location, attachment);
    }
}

// VERIFY: a30f6eda manual
// OFFSET: 0x007FC990
void LightningObjectReleaseRef(LightningObject* object) {
    uint32_t time = OsGetAsyncTimeMs();
    object->m_flags &= ~0x1;
    object->m_endTime = time;

    if (--object->m_refCount == 0) {
        delete object;
    }
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
