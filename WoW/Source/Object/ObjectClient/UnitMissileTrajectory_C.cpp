#include "UnitMissileTrajectory_C.h"
#include <new>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/WorldClient/Map.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include <malloc.h>
#include <cmath>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

// STATIC: 0x00CA0A98
CMapEntity* CGUnit_C::s_trajectoryWorldObjects[2];

// STATIC: 0x00CA0AA0
CM2Model* CGUnit_C::s_trajectoryModels[2];

// STATIC: 0x00CA0AA8
static WOWGUID s_trajectoryTargetGuid;

// STATIC: 0x00CA0AB8
static float s_trajectoryAlpha;

// STATIC: 0x00CA0ABC
static uint32_t s_trajectoryStartTime;

// STATIC: 0x00CA0AC0
static uint32_t s_trajectoryFadeStart;

// STATIC: 0x00CA0AC4
static int32_t s_trajectoryMode;

// STATIC: 0x00CA0AC8
C3Vector* CGUnit_C::s_trajectoryPoints;

// STATIC: 0x00CA0ACC
static uint32_t s_trajectoryPointCount;

// STATIC: 0x00CA0AD0
static CTexture* s_trajectoryTexture0;

// STATIC: 0x00CA0AD4
static CTexture* s_trajectoryTexture1;

// STATIC: 0x00CA0AEC
static C3Vector s_trajectoryLaunchPosition;

// STATIC: 0x00CA0AD8
CMissileCollisionHeap* CMissileCollision::s_pool;

// STATIC: 0x00CA0ADC
CMissileCollision* CMissileCollision::s_head;

// STATIC: 0x00CA0AE0
CMissileCollision* CMissileCollision::s_tail;

// INLINED
inline CMissileCollision::CMissileCollision() {
    for (int32_t i = 0; i < 8; i++) {
        this->unk_0044[i] = 0.0f;
    }
}

// VERIFY: dba3ae87 manual
// OFFSET: 0x006FC0F0
void CMissileCollision::RemoveUnitFromSystem(CGUnit_C* unit) {
    for (CMissileCollision* collision = CMissileCollision::s_head; collision; collision = collision->m_next) {
        uint32_t i = 0;

        while (collision->m_units[i]) {
            if (collision->m_units[i] == unit) {
                do {
                    collision->m_units[i] = collision->m_units[i + 1];
                    i++;
                } while (collision->m_units[i]);

                break;
            }

            i++;
        }
    }
}

// INLINED
inline void CMissileCollision::InitSystem() {
    delete CMissileCollision::s_pool;

    CMissileCollisionHeap* pool = static_cast<CMissileCollisionHeap*>(SMemAlloc(sizeof(CMissileCollisionHeap), __FILE__, __LINE__, 0));

    if (pool) {
        pool->m_heapId = ObjectAllocAddHeap(sizeof(CMissileCollision), 8, "Missile Collision", true);
        CMissileCollision::s_tail = nullptr;
        CMissileCollision::s_head = nullptr;
        CMissileCollision::s_pool = pool;
    } else {
        CMissileCollision::s_pool = nullptr;
        CMissileCollision::s_tail = nullptr;
        CMissileCollision::s_head = nullptr;
    }
}

// VERIFY: 9a79f0ab manual
// OFFSET: 0x006FC770
CMissileCollision* CMissileCollisionHeap::New() {
    uint32_t memHandle;
    void* memory = nullptr;

    if (!ObjectAlloc(this->m_heapId, &memHandle, &memory, false)) {
        return nullptr;
    }

    CMissileCollision* collision = memory ? new (memory) CMissileCollision() : nullptr;
    collision->m_memHandle = memHandle;

    return collision;
}

// VERIFY: 4b422f42 manual
// OFFSET: 0x006FC8E0
void CGUnit_C::InitMissileTrajectorySystem() {
    C3Vector* points = static_cast<C3Vector*>(SMemAlloc(2400, __FILE__, __LINE__, 0));
    CGUnit_C::s_trajectoryModels[0] = nullptr;
    CGUnit_C::s_trajectoryWorldObjects[0] = nullptr;
    CGUnit_C::s_trajectoryModels[1] = nullptr;
    CGUnit_C::s_trajectoryWorldObjects[1] = nullptr;
    CGUnit_C::s_trajectoryPoints = points;
    CMissileCollision::InitSystem();
}

// VERIFY: 23f03795 auto
// OFFSET: 0x006FC950
void CMissileCollision::Free() {
    if (this == CMissileCollision::s_head) {
        CMissileCollision::s_head = this->m_next;

        if (CMissileCollision::s_head) {
            CMissileCollision::s_head->m_prev = nullptr;
        }
    } else if (this == CMissileCollision::s_tail) {
        CMissileCollision::s_tail = this->m_prev;

        if (this->m_prev) {
            this->m_prev->m_next = nullptr;
        }
    } else {
        this->m_prev->m_next = this->m_next;
        this->m_next->m_prev = this->m_prev;
    }

    ObjectFree(CMissileCollision::s_pool->m_heapId, this->m_memHandle);
}

// VERIFY: 0abf6409 auto
// OFFSET: 0x006FC920
CMissileCollision* CMissileCollision::Alloc() {
    CMissileCollision* collision = CMissileCollision::s_pool->New();
    CMissileCollision* head = CMissileCollision::s_head;
    collision->m_prev = nullptr;
    CMissileCollision::s_head = collision;

    if (head) {
        collision->m_next = head;
        head->m_prev = collision;
    } else {
        collision->m_next = nullptr;
        CMissileCollision::s_tail = collision;
    }

    return collision;
}

// VERIFY: 3780b5c1 auto
// OFFSET: 0x006FCA40
void CMissileCollision::MaybeAddUnitToSystem(CGUnit_C* unit) {
    for (CMissileCollision* collision = CMissileCollision::s_head; collision; collision = collision->m_next) {
        if (collision->ComputeUnitRank(unit) == -1.0f) {
            continue;
        }

        int32_t i = 0;

        while (collision->m_units[i]) {
            i++;
        }

        if (i < 15) {
            collision->m_units[i] = unit;
            collision->m_units[i + 1] = nullptr;
        }
    }
}

// VERIFY: d983a829 manual
// OFFSET: 0x006FBF00
void CGUnit_C::StopMissileTrajectoryTargeting() {
    if (s_trajectoryTexture0) {
        HandleClose(reinterpret_cast<HOBJECT>(s_trajectoryTexture0));
        s_trajectoryTexture0 = nullptr;
    }

    if (s_trajectoryTexture1) {
        HandleClose(reinterpret_cast<HOBJECT>(s_trajectoryTexture1));
        s_trajectoryTexture1 = nullptr;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (CGUnit_C::s_trajectoryModels[i]) {
            CGUnit_C::s_trajectoryModels[i]->Release();
            CGUnit_C::s_trajectoryModels[i] = nullptr;
        }

        if (CGUnit_C::s_trajectoryWorldObjects[i]) {
            World::ObjectDestroy(CGUnit_C::s_trajectoryWorldObjects[i]);
            CGUnit_C::s_trajectoryWorldObjects[i] = nullptr;
        }
    }

    s_trajectoryTargetGuid = WOWGUID();
}

// VERIFY: 865fca67 auto
// OFFSET: 0x006FE7E0
void CGUnit_C::UpdateMissileTrajectory() {
    if (!this->m_missileTrajectoryCastId || !this->m_missileTrajectoryStart || this->m_missileTrajectoryReleased) {
        return;
    }

    int32_t spellId = this->m_missileTrajectorySpell;

    if (static_cast<int32_t>(FrameTime::s_curTimeMs - this->m_missileTrajectoryEnd - this->m_missileTrajectoryCastId) < 0) {
        return;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return;
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual = this->GetAppropriateSpellVisual(&spell, visualRec, 0, 0);
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual ? visual->m_castKit : 0);
    WOWGUID guid = this->m_obj->m_guid;
    PendingSpellCast* cast = GetPendingSpellCast(this->m_castCount, this->m_castSpellId, guid);

    if (cast && !cast->unk_0139 && visual && visual->m_hasMissile && kit && kit->m_animID > 0 && !(visual->m_flags & 0x4)) {
        cast->m_data.m_targetFlags |= 0x40;
        cast->m_data.m_elevation = 0.0f;
        cast->m_data.m_destPos = C3Vector(0.0f, 0.0f, 0.0f);
        cast->m_data.unk_0120 = 1.0f;
        int32_t visuals = 0;
        Spell_C_PlayMissileTrajectoryCastVisuals(this, cast, &spell, visual, kit, &visuals, 1.0f);
        this->m_missileTrajectoryReleased = cast->m_data.m_castCount;
        cast->unk_0139 = 1;
    } else {
        this->SendFinalMissileTrajectoryUpdate();
    }
}

// VERIFY: 890fca3b manual
// OFFSET: 0x006FDA20
void CGUnit_C::UpdateMissileTrajectoryTargeting() {
    s_trajectoryAlpha = 0.0f;
    uint32_t pitchNextFrame = CGUnit_C::s_pitchMissileTrajectoryTargetingNextFrame;
    CGUnit_C::s_pitchMissileTrajectoryTargetingNextFrame = 0;

    if (!s_trajectoryTargetGuid) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_trajectoryTargetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        s_trajectoryTargetGuid = WOWGUID();
        return;
    }

    VehicleRec* vehicleRec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : nullptr;

    if (!vehicleRec || !(vehicleRec->m_flags & 0xF800000)) {
        return;
    }

    int32_t spellId = 0;
    bool hasMissile = false;
    SpellRec spell;
    int32_t castSpell;

    if (unit->m_castSpellId) {
        castSpell = unit->m_castSpellId;
    } else if (Spell_C_GetDelayedTrajectoryCast()) {
        castSpell = Spell_C_GetDelayedTrajectoryCast()->m_data.m_spellId;
    } else {
        castSpell = 0;
    }

    if (g_spellDB.GetRecord(castSpell, &spell) && spell.m_spellMissileID && g_spellMissileDB.GetRecord(spell.m_spellMissileID)) {
        hasMissile = true;
        spellId = castSpell;
    }

    CGInputControl* input = CGInputControl::GetActive();

    if (!input) {
        return;
    }

    uint32_t flags = vehicleRec->m_flags;
    int32_t mode;
    bool active;

    if (flags & 0x800000) {
        mode = 0;
        active = true;
        s_trajectoryMode = 0;
    } else if ((flags & 0x1000000) && input->IsMouseDragging()) {
        mode = 1;
        active = true;
        s_trajectoryMode = 1;
    } else if ((flags & 0x2000000) && (pitchNextFrame || (input->m_flags & 0xC00))) {
        mode = 2;
        active = true;
        s_trajectoryMode = 2;
    } else if ((flags & 0x4000000) && input->CanSyncFreeLookFacing(unit)) {
        mode = 3;
        active = true;
        s_trajectoryMode = 3;
    } else if ((vehicleRec->m_flags & 0x8000000) && hasMissile) {
        mode = 0;
        active = true;
        s_trajectoryMode = 0;
    } else {
        mode = s_trajectoryMode;
        active = false;
    }

    bool fullAlpha = true;
    int32_t pitchArg = static_cast<int32_t>(pitchNextFrame);

    if (active) {
        s_trajectoryFadeStart = 0;
    } else {
        if (!mode) {
            s_trajectoryFadeStart = 0;
            s_trajectoryMode = 0;
            return;
        }

        if (!s_trajectoryFadeStart) {
            if (static_cast<int32_t>(CGUnit_C::s_missileTrajectoryTargetingSpell) <= 0) {
                s_trajectoryFadeStart = 0;
                s_trajectoryMode = 0;
                return;
            }

            s_trajectoryFadeStart = 1;

            if (FrameTime::s_curTimeMs) {
                s_trajectoryFadeStart = FrameTime::s_curTimeMs;
            }
        } else {
            float linger;

            if (mode == 1) {
                linger = vehicleRec->m_msslTrgtTurnLingering;
            } else if (mode == 2) {
                linger = vehicleRec->m_msslTrgtPitchLingering;
            } else {
                linger = vehicleRec->m_msslTrgtMouseLingering;
            }

            float fade = 0.5f * linger;

            if (fade > 0.3f) {
                fade = 0.3f;
            }

            int32_t hold = CMath::fint_((linger - fade) * 1000.0f);
            pitchArg = hold;
            uint32_t fadeMs = static_cast<uint32_t>(CMath::fint_(fade * 1000.0f));
            uint32_t elapsed = FrameTime::s_curTimeMs - (s_trajectoryFadeStart + hold);

            if (static_cast<int32_t>(elapsed) >= 0) {
                if (!fadeMs) {
                    s_trajectoryAlpha = 0.0f;
                    s_trajectoryFadeStart = 0;
                    s_trajectoryMode = 0;
                    return;
                }

                s_trajectoryAlpha = 1.0f - static_cast<float>(elapsed) / static_cast<float>(fadeMs);

                if (s_trajectoryAlpha <= 0.0f) {
                    s_trajectoryFadeStart = 0;
                    s_trajectoryMode = 0;
                    return;
                }

                fullAlpha = false;
            }
        }
    }

    if (fullAlpha) {
        s_trajectoryAlpha = 1.0f;
    }

    spell.m_id = 0;
    spell.m_spellMissileID = 0;

    if (!spellId) {
        spellId = CGUnit_C::s_missileTrajectoryTargetingSpell;
    }

    if (spellId <= 0 || (g_spellDB.GetRecord(spellId, &spell), !spell.m_id)) {
        for (uint32_t i = 0; i < 10; i++) {
            uint32_t* action = GetPetAction(i);

            if (!action) {
                continue;
            }

            uint32_t type = (*action >> 24) & 0x3F;

            if (type != 1 && (type <= 7 || type > 17)) {
                continue;
            }

            spellId = *action & 0xFFFFFF;

            if (g_spellDB.GetRecord(spellId, &spell) && spell.m_spellMissileID) {
                break;
            }
        }

        if (!spell.m_id) {
            s_trajectoryAlpha = 0.0f;
            return;
        }
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    s_trajectoryPointCount = 0;
    CGUnit_C* target = unit;
    float facing;

    if (!unit->ComputeMissileTrajectory(&spell, &pitchArg, &target, &s_trajectoryLaunchPosition, &position, &facing, CGUnit_C::s_trajectoryPoints, &s_trajectoryPointCount, 1)) {
        s_trajectoryAlpha = 0.0f;
        return;
    }

    facing = unit->GetFacing();

    for (uint32_t i = 0; i < 2; i++) {
        CM2Model* model = CGUnit_C::s_trajectoryModels[i];

        if (!model) {
            continue;
        }

        CAaSphere sphere = model->GetBoundingSphere();
        float scale = sphere.r <= 0.0f ? vehicleRec->m_msslTrgtImpactRadius[i] : vehicleRec->m_msslTrgtImpactRadius[i] / sphere.r;
        CGUnit_C::s_trajectoryModels[i]->SetWorldTransform(position, facing, scale);
        CGUnit_C::s_trajectoryModels[i]->m_fadeAlpha = s_trajectoryAlpha;
        CGUnit_C::s_trajectoryModels[i]->ForceAnimate();

        if (!CGUnit_C::s_trajectoryWorldObjects[i]) {
            C44Matrix transform = CGUnit_C::s_trajectoryModels[i]->m_worldTransform;
            CGUnit_C::s_trajectoryWorldObjects[i] = CMap::ObjectCreateStaticEx(CGUnit_C::s_trajectoryModels[i], transform, MissileTrajectoryObjectCallback, nullptr, 0, i, __FILE__, __LINE__);
        }

        if (CGUnit_C::s_trajectoryModels[i]->IsLoaded(0, 0)) {
            CAaBox box = CGUnit_C::s_trajectoryModels[i]->GetBoundingBox();
            C44Matrix transform = CGUnit_C::s_trajectoryModels[i]->m_worldTransform;
            CMap::ObjectUpdate(CGUnit_C::s_trajectoryWorldObjects[i], transform, box, sphere, sphere.c, false, 0xFFFFFFFF);
        }
    }

    CGUnit_C::s_missileTrajectoryTargetingSpell = spellId;
}

// OFFSET: 0x006FC190
void CGUnit_C::ShutdownMissileTrajectorySystem() {
    SMemFree(CGUnit_C::s_trajectoryPoints, __FILE__, __LINE__, 0);
    CGUnit_C::StopMissileTrajectoryTargeting();
    delete CMissileCollision::s_pool;
    CMissileCollision::s_pool = nullptr;
    CMissileCollision::s_tail = nullptr;
    CMissileCollision::s_head = nullptr;
}

// VERIFY: 2d5a7665 manual
// OFFSET: 0x006FDFB0
void CGUnit_C::RenderMissileTrajectoryTargetingTexture() {
    if (!(s_trajectoryAlpha > 0.0f) || !s_trajectoryTargetGuid) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_trajectoryTargetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        s_trajectoryTargetGuid = WOWGUID();
        return;
    }

    float facing = unit->GetFacing();
    VehicleRec* vehicle = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : nullptr;

    if (!vehicle) {
        return;
    }

    CGxTex* arcTex;

    if (s_trajectoryTexture0 && (arcTex = TextureGetGxTex(s_trajectoryTexture0, 0, nullptr)) != nullptr) {
        g_theGxDevicePtr->RsPush();
        GxXformPush(GxXform_World);
        GxXformPush(GxXform_Tex0);
        GxXformTranslate(GxXform_World, -CGWorldFrame::GetActiveCamera()->m_position);
        GxRsSet(GxRs_Culling, 0);
        GxRsSet(GxRs_Fog, 0);
        GxRsSet(GxRs_ColorOp0, 0);
        GxRsSet(GxRs_AlphaOp0, 0);
        GxRsSet(GxRs_ColorWrite, 15);
        GxRsSet(GxRs_Lighting, 0);
        GxRsSet(GxRs_DepthWrite, 0);
        GxRsSet(GxRs_BlendingMode, 3);
        int32_t vertexCount = 2 * s_trajectoryPointCount + 2;
        C3Vector* positions = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * vertexCount));
        CImVector* colors = static_cast<CImVector*>(alloca(sizeof(CImVector) * vertexCount));
        C2Vector* texCoords = static_cast<C2Vector*>(alloca(sizeof(C2Vector) * vertexCount));
        uint16_t* indices = static_cast<uint16_t*>(alloca(sizeof(uint16_t) * vertexCount));

        for (int32_t i = 0; i < vertexCount; i++) {
            indices[i] = i;
        }

        float halfWidth = vehicle->m_msslTrgtArcWidth * 0.5f;
        C3Vector side;
        side.x = -(CMath::sin_(facing) * halfWidth);
        side.y = CMath::cos_(facing) * halfWidth;
        uint32_t elapsed = FrameTime::s_curTimeMs - s_trajectoryStartTime;
        float alpha = s_trajectoryAlpha * 255.0f;
        float alphaStep = (255.0f * (s_trajectoryAlpha * vehicle->m_msslTrgtEndOpacity) - alpha) * (1.0f / s_trajectoryPointCount);
        float texStart = 1.0f - fmodf(static_cast<float>(elapsed) * 0.001f * vehicle->m_msslTrgtArcSpeed, 1.0f);
        positions[0].x = s_trajectoryLaunchPosition.x + side.x;
        positions[0].y = s_trajectoryLaunchPosition.y + side.y;
        positions[0].z = s_trajectoryLaunchPosition.z;
        positions[1].x = s_trajectoryLaunchPosition.x - side.x;
        positions[1].y = s_trajectoryLaunchPosition.y - side.y;
        positions[1].z = s_trajectoryLaunchPosition.z;
        colors[0].a = 0;
        colors[0].r = 0xFF;
        colors[0].g = 0xFF;
        colors[0].b = 0xFF;
        colors[1] = colors[0];
        texCoords[0].x = texStart;
        texCoords[0].y = 0.0f;
        texCoords[1].x = texStart;
        texCoords[1].y = 1.0f;
        float pointAlpha = alphaStep + alpha;
        float texCoord = texStart + vehicle->m_msslTrgtArcRepeat;

        for (uint32_t i = 0; i < s_trajectoryPointCount; i++) {
            const C3Vector& point = CGUnit_C::s_trajectoryPoints[i];
            positions[2 * i + 2].x = point.x + side.x;
            positions[2 * i + 2].y = point.y + side.y;
            positions[2 * i + 2].z = point.z;
            positions[2 * i + 3].x = point.x - side.x;
            positions[2 * i + 3].y = point.y - side.y;
            positions[2 * i + 3].z = point.z;

            if (pointAlpha < 0.0f) {
                pointAlpha = 0.0f;
            }

            colors[2 * i + 2].b = 0xFF;
            colors[2 * i + 2].g = 0xFF;
            colors[2 * i + 2].r = 0xFF;
            colors[2 * i + 2].a = static_cast<uint8_t>(pointAlpha + 0.5f);
            colors[2 * i + 3] = colors[2 * i + 2];
            pointAlpha += alphaStep;
            texCoords[2 * i + 2].x = texCoord;
            texCoords[2 * i + 2].y = 0.0f;
            texCoords[2 * i + 3].x = texCoord;
            texCoords[2 * i + 3].y = 1.0f;
            texCoord += vehicle->m_msslTrgtArcRepeat;
        }

        CGxMatrixStack& texStack = g_theGxDevicePtr->m_xforms[GxXform_Tex0];
        texStack.m_dirty = 1;
        texStack.m_flags[texStack.m_level] &= ~CGxMatrixStack::F_Identity;
        texStack.m_mtx[texStack.m_level] = C44Matrix();
        g_theGxDevicePtr->RsSet(GxRs_Texture0, arcTex);
        C3Vector normal(0.0f, 0.0f, 1.0f);
        GxRsSet(GxRs_TextureShader0, 1);
        GxPrimLockVertexPtrs(vertexCount, positions, sizeof(C3Vector), &normal, 0, colors, sizeof(CImVector), nullptr, 0, texCoords, sizeof(C2Vector), nullptr, 0);
        GxDrawLockedElements(GxPrim_TriangleStrip, vertexCount, indices);
        GxPrimUnlockVertexPtrs();
        GxXformPop(GxXform_World);
        GxXformPop(GxXform_Tex0);
        g_theGxDevicePtr->RsPop();
    }

    if (!s_trajectoryTexture1) {
        return;
    }

    CGxTex* impactTex = TextureGetGxTex(s_trajectoryTexture1, 0, nullptr);

    if (!impactTex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 3);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, impactTex);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, ProjectTex2dGetAddFadeTex());
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    const C3Vector& impact = CGUnit_C::s_trajectoryPoints[s_trajectoryPointCount - 1];
    float radius = vehicle->m_msslTrgtImpactTexRadius;
    CAaBox box;
    box.b.x = impact.x - radius;
    box.b.y = impact.y - radius;
    box.b.z = impact.z - radius;
    box.t.x = radius + impact.x;
    box.t.y = radius + impact.y;
    box.t.z = radius + impact.z;
    CImVector color;
    color.b = 0xFF;
    color.g = 0xFF;
    color.r = 0xFF;
    color.a = static_cast<uint8_t>(s_trajectoryAlpha * 255.0f + 0.5f);
    C44Matrix rotation;
    rotation.RotateAroundZ(-facing);
    ProjectTex::Render2d(box, color, &rotation, 0.5f, 0x200122, 5, 0.039999999f);
    g_theGxDevicePtr->RsPop();
}
