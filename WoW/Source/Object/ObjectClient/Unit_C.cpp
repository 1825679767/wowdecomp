#include "Unit_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/AreaGroupRec.h"
#include "WoW/Source/DB/Rec/EmotesRec.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"
#include "_Triage/CClientObjCreate.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Ui/NamePlateFrame.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/UnitMissileTrajectory_C.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/DB/Rec/AnimationDataRec.h"
#include "_Triage/CMoveSpline.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Services/SysMessage.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA1238
WOWGUID CGUnit_C::s_activeMover;

// STATIC: 0x00CA11F4
uint32_t CGUnit_C::s_trackingType;

// STATIC: 0x00CA11D8
float CGUnit_C::s_trackingFacing;

// STATIC: 0x00CA1208
uint32_t CGUnit_C::s_namePlateFlags;

// STATIC: 0x00CA0AB0
uint32_t CGUnit_C::s_missileTrajectoryTargetingSpell;

// STATIC: 0x00CA0AB4
uint32_t CGUnit_C::s_pitchMissileTrajectoryTargetingNextFrame;

// STATIC: 0x00A37170
const uint32_t CGUnit_C::s_impactTypes[4] = { 0, 8, 7, 9 };

// STATIC: 0x00CA1264
C3Vector CGUnit_C::s_trackingPosition;

// STATIC: 0x00CA11EC
float CGUnit_C::s_trackingDirection;

// STATIC: 0x00CA1370
uint32_t CGUnit_C::s_vehiclePassengerCameraState[3];

// STATIC: 0x00CA12CC
int32_t CGUnit_C::s_dismountSoundKitId;

// STATIC: 0x00CA1198
WOWGUID CGUnit_C::s_lastNPCSoundGUID;

// STATIC: 0x00CA1248
WOWGUID CGUnit_C::s_deferredClientControlUpdateGUID;

// STATIC: 0x00CA1250
uint8_t CGUnit_C::s_deferredClientControlUpdateState;

// STATIC: 0x00CA1240
uint32_t CGUnit_C::s_flightStatusTime;

// STATIC: 0x00CA11D0
float CGUnit_C::s_unk_CA11D0;

// STATIC: 0x00CA11D4
uint32_t CGUnit_C::s_unk_CA11D4;

// STATIC: 0x00CA11F8
WOWGUID CGUnit_C::s_trackingGUID;

// STATIC: 0x00CA1200
uint32_t CGUnit_C::s_trackingFlags;

// STATIC: 0x00CA1204
CGNamePlateFrame* CGUnit_C::s_namePlateFocus;

// STATIC: 0x00ADAA8C
static TSExplicitList<CGNamePlateFrame, offsetof(CGNamePlateFrame, m_link)> s_unusedNamePlates;

// STATIC: 0x00ADAAB8
static ClickToMoveActionInfo s_clickToMoveActions[12] = {
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 900.0f, 0, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 1, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 0, { 0, 0 } },
};

// STATIC: 0x00ADAA20
static const uint32_t s_effectAttachmentIds[16] = { 20, 34, 19, 21, 22, 17, 23, 24, 25, 15, 16, 37, 38, 22, 47, 48 };

// STATIC: 0x00ADB630
static const float s_attachmentHeights[50] = { 1.0f, 1.0f, 1.0f, 1.5f, 1.5f, 1.8f, 1.8f, 0.5f, 0.5f, 1.0f, 1.0f, 2.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.5f, 0.0f, 2.0f, 1.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, 1.5f, 1.5f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 2.5f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

// VERIFY: fa026898 auto
// OFFSET: 0x00715970
const char* CGUnit_C::GetDisplayRaceNameFromRecord(const ChrRacesRec* rec, int32_t sex, int32_t* displaySex) {
    if (displaySex) {
        *displaySex = sex;
    }

    if (!rec) {
        return nullptr;
    }

    if (sex == 0) {
        if (*rec->m_nameMale) {
            return rec->m_nameMale;
        }

        if (*rec->m_nameFemale) {
            if (displaySex) {
                *displaySex = 1;
            }

            return rec->m_nameFemale;
        }
    } else if (sex == 1) {
        if (*rec->m_nameFemale) {
            return rec->m_nameFemale;
        }

        if (*rec->m_nameMale) {
            if (displaySex) {
                *displaySex = 0;
            }

            return rec->m_nameMale;
        }
    }

    return rec->m_name;
}

// VERIFY: a01b044c auto
// OFFSET: 0x007159E0
const char* CGUnit_C::GetDisplayClassNameFromRecord(const ChrClassesRec* rec, int32_t sex, int32_t* displaySex) {
    if (displaySex) {
        *displaySex = sex;
    }

    if (!rec) {
        return nullptr;
    }

    if (sex == 0) {
        if (*rec->m_nameMale) {
            return rec->m_nameMale;
        }

        if (*rec->m_nameFemale) {
            if (displaySex) {
                *displaySex = 1;
            }

            return rec->m_nameFemale;
        }
    } else if (sex == 1) {
        if (*rec->m_nameFemale) {
            return rec->m_nameFemale;
        }

        if (*rec->m_nameMale) {
            if (displaySex) {
                *displaySex = 0;
            }

            return rec->m_nameMale;
        }
    }

    return rec->m_name;
}

// VERIFY: 39368271 auto
// OFFSET: 0x004D43C0
int32_t CGUnit_C::IsActiveMover() const {
    return this->m_obj->m_guid == s_activeMover;
}

// VERIFY: f85d0af8 auto
// OFFSET: 0x004F61D0
float CGUnit_C::GetDistanceToPos(const C3Vector& pos) const {
    C3Vector position = this->GetPosition();
    float dy = position.y - pos.y;
    float dz = position.z - pos.z;
    float dx = position.x - pos.x;
    return dx * dx + dz * dz + dy * dy;
}

// VERIFY: 21b1c818 auto
// OFFSET: 0x004F6210
VehicleRec* CGUnit_C::GetVehicleRec() const {
    if (this->m_vehicle) {
        return this->m_vehicle->m_vehicleRec;
    }

    return nullptr;
}

// VERIFY: c3019c7e auto
// OFFSET: 0x004F8850
uint32_t CGUnit_C::GetAuraCount() const {
    if (this->m_auras.m_count == 0xFFFFFFFF) {
        return this->m_auras.m_dynamic.m_count;
    }

    return this->m_auras.m_count;
}

// VERIFY: 540bfe58 manual
// OFFSET: 0x004F8870
int32_t CGUnit_C::GetAuraSpellId(uint32_t index) {
    return this->m_auras[index].m_spellId;
}

// VERIFY: 43e20507 auto
// OFFSET: 0x0051A230
int32_t CGUnit_C::IsMounted() const {
    return this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000);
}

// VERIFY: cb916759 manual
// OFFSET: 0x00556E10
AuraData* CGUnit_C::GetAura(uint32_t index) {
    if (index >= this->GetAuraCount()) {
        return nullptr;
    }

    return &this->m_auras[index];
}

// VERIFY: ee76539f manual
// OFFSET: 0x00565510
uint8_t CGUnit_C::GetAuraFlags(uint32_t index) {
    return this->m_auras[index].m_flags;
}

// VERIFY: 649f7610 manual
// OFFSET: 0x005D3310
int32_t CGUnit_C::IsControllable() const {
    uint32_t flags = this->m_unit->m_unitFieldFlags;
    return !(flags & 0x40000) && !(flags & 0x800000) && !(flags & 0x400000);
}

// VERIFY: 0d046ee1 auto
// OFFSET: 0x005D3340
VehicleSeatRec* CGUnit_C::GetVehicleSeatRec(uint8_t seat) const {
    if (this->m_vehicle) {
        return this->m_vehicle->GetVehicleSeatRec(seat);
    }

    return nullptr;
}

// VERIFY: af7909ae manual
// OFFSET: 0x006D1DE0
int32_t __stdcall CGUnit_C::UnitCanGossip(CGUnit_C* unit) {
    return unit->IsQuestGiver() && unit->m_questStatus >= 2;
}

// VERIFY: c178f90d manual
// OFFSET: 0x006E6EC0
int32_t CGUnit_C::CanBeTargetted() const {
    return this->CanHighlight();
}

// VERIFY: ebdc15d6 auto
// OFFSET: 0x006E6ED0
int32_t CGUnit_C::IsHighlightSuppressed() const {
    return (this->m_clientFlags >> 25) & 1;
}

// VERIFY: 48845495 auto
// OFFSET: 0x006E6F10
C3Vector CGUnit_C::GetRawPosition() const {
    return this->m_movement->m_position;
}

// VERIFY: bfc83e4b auto
// OFFSET: 0x006E6F60
float CGUnit_C::GetRawFacing() const {
    return this->m_movement->m_facing;
}

// VERIFY: 4aabb319 auto
// OFFSET: 0x006E6F70
uint64_t CGUnit_C::GetTransportGUID() const {
    return this->m_movementData.m_transportGuid.m_guid;
}

// VERIFY: 38a0654c auto
// OFFSET: 0x006E6F80
CM2Model* CGUnit_C::GetObjectModel() const {
    if (this->m_mountModel) {
        return this->m_mountModel;
    }

    return this->m_worldModel;
}

// VERIFY: 6cda1b12 auto
// OFFSET: 0x006E6FC0
float CGUnit_C::GetPitch() const {
    return this->m_movementData.m_pitch;
}

// VERIFY: c74ca830 auto
// OFFSET: 0x006FBE30
void CGUnit_C::PreStartUpdatingMissileTrajectory(int32_t spellId, uint32_t castId) {
    this->m_missileTrajectorySpell = spellId;
    this->m_missileTrajectoryCastId = castId;
}

// VERIFY: cae7a4bc auto
// OFFSET: 0x006FBE50
void CGUnit_C::StartUpdatingMissileTrajectory(int32_t spellId, uint32_t start, uint32_t end) {
    if (spellId == this->m_missileTrajectorySpell) {
        this->m_missileTrajectoryStart = start;
        this->m_missileTrajectoryEnd = end;
    }
}

// VERIFY: e1295041 auto
// OFFSET: 0x006FBE80
void CGUnit_C::StopUpdatingMissileTrajectory(int32_t spellId) {
    if (spellId == this->m_missileTrajectorySpell) {
        this->m_missileTrajectorySpell = 0;
        this->m_missileTrajectoryCastId = 0;
        this->m_missileTrajectoryStart = 0;
        this->m_missileTrajectoryReleased = 0;
    }
}

// VERIFY: 9b961405 auto
// OFFSET: 0x006FBEB0
int32_t CGUnit_C::IsUpdatingMissileTrajectory() const {
    return this->m_missileTrajectoryCastId && this->m_missileTrajectoryStart;
}

// VERIFY: fb60f5d5 auto
// OFFSET: 0x006FBED0
int32_t CGUnit_C::IsMissileTrajectoryUpdateWaitingForMissileRelease() const {
    return this->m_missileTrajectoryCastId && this->m_missileTrajectoryStart && this->m_missileTrajectoryReleased;
}

// VERIFY: 156171df auto
// OFFSET: 0x006FBF80
void CGUnit_C::PitchMissileTrajectoryTargetingNextFrame() {
    s_pitchMissileTrajectoryTargetingNextFrame = 1;
}

// VERIFY: 3eb050df auto
// OFFSET: 0x006FBF90
void CGUnit_C::SetMissileTrajectoryTargetingSpell(uint32_t spellId) {
    s_missileTrajectoryTargetingSpell = spellId;
}

// VERIFY: f0ab1eb4 auto
// OFFSET: 0x00714AC0
bool CGUnit_C::IsLocalClientControlled() const {
    return (this->m_clientFlags >> 10) & 1;
}

// VERIFY: ca03fd67 auto
// OFFSET: 0x00714BD0
void CGUnit_C::FreeComponent() {
    if (this->m_characterComponent) {
        CCharacterComponent::FreeComponent(this->m_characterComponent);
        this->m_characterComponent = nullptr;
        this->m_clientFlags = (this->m_clientFlags & 0xFFBDFFFF) | 0x400000;
    }
}

// VERIFY: 0e7d75c5 auto
// OFFSET: 0x00714C40
int32_t CGUnit_C::ShouldFadeout() {
    int32_t render = 0;
    int32_t fade = 0;
    this->ShouldRender(1, render, fade);
    return fade == 0;
}

// VERIFY: 8b66139f auto
// OFFSET: 0x00714C80
int32_t __stdcall CGUnit_C::IsTwoHanded(const CGUnitVirtualItem* item) {
    if (!item || item->m_classID != 2) {
        return 0;
    }

    switch (item->m_subclassID) {
        case 1:
        case 5:
        case 6:
        case 8:
        case 10:
        case 12:
        case 17:
        case 20:
            return 1;

        default:
            return 0;
    }
}

// VERIFY: 46787e5a auto
// OFFSET: 0x00714DD0
uint32_t CGUnit_C::GetCombatReadyAnim() const {
    const CGUnitVirtualItem* item = this->GetVirtualItem(0, false);

    if (!item || item->m_classID != 2) {
        item = this->GetVirtualItem(1, false);
    }

    if (!item || item->m_classID != 2) {
        return ANIM_READY_UNARMED;
    }

    uint32_t anim = ANIM_READY_2H - (this->GetVirtualItem(1, false) != nullptr);

    switch (item->m_subclassID) {
        case 0:
        case 4:
        case 7:
        case 11:
        case 13:
        case 14:
        case 15:
        case 20:
            return ANIM_READY_1H;

        case 1:
        case 5:
        case 8:
        case 12:
            return anim;

        case 6:
        case 10:
        case 17:
            return ANIM_READY_2HL;

        default:
            return ANIM_READY_UNARMED;
    }
}

// VERIFY: cfd62b02 auto
// OFFSET: 0x00714F90
int32_t CGUnit_C::ChooseRangedLoadAnim(uint32_t flags, uint32_t* anim) const {
    if (this->m_sheatheState != 2 || !(this->m_animationState & 0x200)) {
        return (flags & 0xFFFFFF00) == 0;
    }

    if (!(flags & 0x80) || (this->m_animationState & 0x4000)) {
        return 1;
    }

    *anim = ANIM_READY_UNARMED;
    const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

    if (!item || item->m_classID != 2) {
        return 1;
    }

    switch (item->m_subclassID) {
        case 2: {
            *anim = 105;
            return 1;
        }

        case 3:
        case 18: {
            *anim = 106;
            return 1;
        }

        case 16: {
            *anim = 112;
            return 1;
        }

        case 19: {
            *anim = 111;
            return 1;
        }

        default: {
            return 1;
        }
    }
}

// VERIFY: 293f222c auto
// OFFSET: 0x007151A0
void CGUnit_C::RemoveMountModel() {
    if (this->m_mountModel) {
        this->m_mountModel->SetLoadedCallback(nullptr, nullptr);
        this->m_mountModel->SetSequenceCallback(nullptr, 0);
        this->m_mountModel->SetEventCallback(nullptr, 0);
        this->m_mountModel->Release();
        this->m_mountModel = nullptr;
    }
}

// VERIFY: 1ea7f73c auto
// OFFSET: 0x00715270
void CGUnit_C::RotateFacing(float delta) {
    this->m_targetFacing = CMath::normalizeangle0to2pi_(delta + this->m_targetFacing);
    this->m_renderFacing = CMath::normalizeangle0to2pi_(delta + this->m_renderFacing);
}

// VERIFY: 9077e5e5 auto
// OFFSET: 0x00715380
void CGUnit_C::NamePlateSetResortAll(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x1;
    } else {
        s_namePlateFlags &= ~0x1;
    }
}

// VERIFY: 384acd97 auto
// OFFSET: 0x007153A0
void CGUnit_C::NamePlateUpdateLevels(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x2;
    } else {
        s_namePlateFlags &= ~0x2;
    }
}

// VERIFY: fb6389d8 auto
// OFFSET: 0x007153C0
void CGUnit_C::NamePlateUpdateColor(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x4;
    } else {
        s_namePlateFlags &= ~0x4;
    }
}

// VERIFY: 0cc4b6e6 auto
// OFFSET: 0x007153E0
uint32_t CGUnit_C::GetCreatureFamily() const {
    if (this->m_creatureCacheEntry) {
        return this->m_creatureCacheEntry->m_family;
    }

    return 0;
}

// VERIFY: 8fb86460 manual
// OFFSET: 0x00715400
void CGUnit_C::SaveChainSpellLocation(const C3Vector* location) {
    if (location) {
        this->m_chainSpellLocation = *location;
        this->m_clientFlags |= 0x800000;
    } else {
        this->m_clientFlags &= ~0x800000;
    }
}

// VERIFY: a3c8b445 manual
// OFFSET: 0x00715440
int32_t FactionTemplateReaction(const FactionTemplateRec* faction, const FactionTemplateRec* other) {
    int32_t otherGroup = other->m_factionGroup;

    if (otherGroup & faction->m_enemyGroup) {
        return 1;
    }

    for (uint32_t i = 0; i < 4 && faction->m_enemies[i]; i++) {
        if (faction->m_enemies[i] == other->m_faction) {
            return 1;
        }
    }

    if (otherGroup & faction->m_friendGroup) {
        return 4;
    }

    for (uint32_t i = 0; i < 4 && faction->m_friend[i]; i++) {
        if (faction->m_friend[i] == other->m_faction) {
            return 4;
        }
    }

    if (faction->m_factionGroup & other->m_friendGroup) {
        return 4;
    }

    for (uint32_t i = 0; i < 4 && other->m_friend[i]; i++) {
        if (other->m_friend[i] == faction->m_faction) {
            return 4;
        }
    }

    return (~(static_cast<uint32_t>(faction->m_flags) >> 12) & 2) | 1;
}

// VERIFY: 1d445e4d auto
// OFFSET: 0x00715670
void CGUnit_C::StartMountTransition(const MountTransitionObject* transition, CEffect* effect) {
    this->m_mountTransition = transition;
    this->m_mountTransitionEffect = effect;
}

// VERIFY: 3a7849d2 auto
// OFFSET: 0x00715690
void CGUnit_C::StopMountTransition() {
    this->m_mountTransition = nullptr;
    this->m_mountTransitionEffect = nullptr;
}

// VERIFY: d86295f0 auto
// OFFSET: 0x00715880
bool CGUnit_C::IsStandStateSplitBody() const {
    bool result = false;

    switch (this->GetClientStandState()) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 6:
            result = true;
            break;
    }

    return result;
}

// VERIFY: 8bfb7b9e auto
// OFFSET: 0x00715930
const char* CGUnit_C::GetDisplayBakeName() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_bakeName;
    }

    return nullptr;
}

// VERIFY: 613e51c7 auto
// OFFSET: 0x00715950
const char* CGUnit_C::GetPortraitTextureName() const {
    if (this->m_displayInfo) {
        return this->m_displayInfo->m_portraitTextureName;
    }

    return nullptr;
}

// VERIFY: 24d7a4c0 auto
// OFFSET: 0x00715A50
void CGUnit_C::SetAnimEventPosition(const C3Vector* position) {
    this->m_clientFlags |= 0x80000000;
    this->m_animEventPosition = *position;
}

// VERIFY: 888d0b7d auto
// OFFSET: 0x00715A80
void CGUnit_C::SetCastingSpell(uint8_t castCount, int32_t spellId) {
    this->m_castCount = castCount;
    this->m_castSpellId = spellId;
}

// VERIFY: 39ee7f23 auto
// OFFSET: 0x00715AA0
void CGUnit_C::SetCastSchoolImmunities(int32_t immunities) {
    this->m_castSchoolImmunities = immunities;
}

// VERIFY: 82aea5d5 auto
// OFFSET: 0x00715AB0
void CGUnit_C::SetCastImmunities(int32_t immunities) {
    this->m_castImmunities = immunities;
}

// VERIFY: ed54196e auto
// OFFSET: 0x00715AC0
void CGUnit_C::SetCastingAutoRepeat(bool enable) {
    if (enable) {
        this->m_animationState |= 0x200;
    } else {
        this->m_animationState &= ~0x200;
    }
}

// VERIFY: 160a8121 auto
// OFFSET: 0x00715AF0
void CGUnit_C::SetCastingAnim(bool enable) {
    if (enable) {
        this->m_animationState |= 0x400;
    } else {
        this->m_animationState &= ~0x400;
    }
}

// VERIFY: e7dd87c8 auto
// OFFSET: 0x00715B50
float CGUnit_C::GetBaseAlpha() const {
    if (this->m_displayInfo) {
        return this->m_displayInfo->m_creatureModelAlpha * (1.0f / 255.0f);
    }

    return 1.0f;
}

// VERIFY: 5c5ddc60 auto
// OFFSET: 0x00715C60
uint32_t CGUnit_C::GetTrackingType() {
    return s_trackingType;
}

// VERIFY: 89710c35 auto
// OFFSET: 0x00715CF0
float CGUnit_C::GetTrackingTurn() {
    return s_trackingFacing;
}

// VERIFY: 77188ed2 auto
// OFFSET: 0x00715D70
bool CGUnit_C::IsBossMob() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 2) & 1;
    }

    return false;
}

// VERIFY: 627af4b4 auto
// OFFSET: 0x00715D90
bool CGUnit_C::IsLinkAll() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 22) & 1;
    }

    return false;
}

// VERIFY: 6082cbf5 auto
// OFFSET: 0x00715DB0
bool CGUnit_C::HasNoShadowBlob() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 25) & 1;
    }

    return false;
}

// VERIFY: 472a80ea manual
// OFFSET: 0x00715DD0
bool CGUnit_C::ShowFactionTooltip() const {
    return !this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags & 0x10);
}

// VERIFY: 2a450047 auto
// OFFSET: 0x00715DF0
bool CGUnit_C::IsTreatAsRaidUnit() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 26) & 1;
    }

    return false;
}

// VERIFY: 1e582e23 auto
// OFFSET: 0x00715E10
bool CGUnit_C::IsForceGossip() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 27) & 1;
    }

    return false;
}

// VERIFY: 43b9bada auto
// OFFSET: 0x00715E30
bool CGUnit_C::CanInteractWhileDead() const {
    return this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags & 0x80);
}

// VERIFY: 568ed656 manual
// OFFSET: 0x00715E50
uint32_t CGUnit_C::GetSkinningType() const {
    if (!this->m_creatureCacheEntry) {
        return 0;
    }

    if (this->m_creatureCacheEntry->m_typeFlags & 0x100) {
        return 1;
    }

    if (this->m_creatureCacheEntry->m_typeFlags & 0x200) {
        return 2;
    }

    if (this->m_creatureCacheEntry->m_typeFlags & 0x8000) {
        return 3;
    }

    return 0;
}

// VERIFY: 79224dcb auto
// OFFSET: 0x00715F20
void CGUnit_C::GetCursorFile(int32_t unable, char* buffer, uint32_t size) const {
    if (unable) {
        SStrPrintf(buffer, size, "Interface\\Cursor\\Unable%s.blp", this->m_creatureCacheEntry->m_iconName);
    } else {
        SStrPrintf(buffer, size, "Interface\\Cursor\\%s.blp", this->m_creatureCacheEntry->m_iconName);
    }
}

// VERIFY: c4fa9258 auto
// OFFSET: 0x00715F70
bool CGUnit_C::MountedCombatAllowed() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 11) & 1;
    }

    return false;
}

// VERIFY: a73b6e27 auto
// OFFSET: 0x00715F90
bool CGUnit_C::CanAidPlayers() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 12) & 1;
    }

    return false;
}

// VERIFY: 00017622 auto
// OFFSET: 0x00715FB0
uint64_t CGUnit_C::GetLooter() const {
    return this->m_looter.m_guid;
}

// VERIFY: 1900c06b auto
// OFFSET: 0x00715FC0
uint64_t CGUnit_C::GetMasterLooter() const {
    return this->m_masterLooter.m_guid;
}

// VERIFY: d8d121aa auto
// OFFSET: 0x00715FD0
void CGUnit_C::InitWheels() {
    this->m_wheelMask = 0;
    CM2Model* model = this->GetObjectModel();

    for (uint32_t bone = 27; bone <= 34; bone++) {
        if (model->HasBone(bone)) {
            this->m_wheelMask |= 1 << (bone - 27);
            this->m_wheelAngles[bone - 27] = 0.0f;
        }
    }
}

// VERIFY: 3f9e9d5b auto
// OFFSET: 0x00716050
void CGUnit_C::SetPredictedHealth(int32_t health) {
    this->m_predictedHealth = health;
}

// VERIFY: aff444a7 manual
// OFFSET: 0x00718080
float CGUnit_C::GetMaxCameraHeight() const {
    float height = this->m_movementData.m_collisionHeight;

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) || height <= 2.0277777f) {
        return height - 0.16666667f;
    }

    return 2.0277777f - 0.16666667f;
}

// VERIFY: 5cc27cbd auto
// OFFSET: 0x00718A00
uint32_t CGUnit_C::GetCreatureClassification() const {
    if (!this->m_creatureCacheEntry || this->m_unit->m_unitFieldPetnumber) {
        return 0;
    }

    return this->m_creatureCacheEntry->m_rank;
}

// VERIFY: d337d178 auto
// OFFSET: 0x00718A20
int32_t CGUnit_C::IsDeepInLiquid() const {
    float depth = 0.0f;
    int32_t liquidType;
    float height;
    int32_t flags;

    if (World::QueryObjectLiquid(this->m_worldObject, &liquidType, &height, &flags)) {
        depth = height - this->GetPosition().z;
    }

    return this->m_movementData.m_collisionHeight * 0.5f < depth;
}

// VERIFY: b255f639 manual
// OFFSET: 0x00718A90
bool CGUnit_C::DoNotLogDeath() const {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return false;
    }

    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 10) & 1;
    }

    return true;
}

// VERIFY: a3b6dbb4 auto
// OFFSET: 0x00718B10
uint32_t CGUnit_C::GetVirtualItemDisplayID(uint32_t slot) const {
    return this->m_virtualItemDisplayId[slot];
}

// VERIFY: 349e3c57 auto
// OFFSET: 0x00718B30
bool CGUnit_C::FactionHasReputation(int32_t factionId) {
    FactionRec* faction = g_factionDB.GetRecord(factionId);
    return faction && faction->m_reputationIndex >= 0;
}

// VERIFY: 60955254 manual
// OFFSET: 0x00718B70
CGUnit_C* CGUnit_C::GetControllingPlayer() {
    CGUnit_C* unit = this;
    const WOWGUID* controller = this->GetCharmerOrCreatorGuid();

    if (*controller) {
        controller = this->GetCharmerOrCreatorGuid();
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (!unit) {
        return nullptr;
    }

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        controller = unit->GetCharmerOrCreatorGuid();
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return nullptr;
        }
    }

    return unit;
}

// VERIFY: 8777f146 auto
// OFFSET: 0x00718FC0
int32_t CGUnit_C::IsDisarmed(int32_t hand) const {
    if (hand == 2) {
        return 0;
    }

    if (this->m_unit->m_unitFieldFlags & 0x200000) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(0, true);

        if (!hand) {
            return item && item->m_classID == 2;
        }

        if (!this->IsDisarmed(0)) {
            const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, true);

            if (offhand && offhand->m_classID == 2) {
                return 1;
            }
        }
    }

    return hand == 1 && (this->m_unit->m_unitFieldFlags2 & 0x80);
}

// VERIFY: d1a9830c auto
// OFFSET: 0x007192C0
int32_t CGUnit_C::IsQuestGiver() const {
    return (this->m_unit->m_unitNpcFlags >> 1) & 1;
}

// VERIFY: 28dbc451 auto
// OFFSET: 0x007192E0
void CGUnit_C::UpdateInteractIcon() {
    if (this->m_unit->m_unitFieldHealth > 0 || (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags & 0x80))) {
        CGObject_C::UpdateInteractIcon();
    }
}

// VERIFY: 773add64 manual
// OFFSET: 0x00719310
void CGUnit_C::UpdateInteractIconAttach() {
    if (!this->m_interactIcon || !this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0)) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    uint32_t attachment;

    if (this->m_mountModel && this->m_worldModel->HasAttachment(29)) {
        attachment = 29;
    } else {
        if (!this->m_worldModel->HasAttachment(18)) {
            return;
        }

        attachment = 18;
    }

    this->m_interactIcon->AttachToParent(this->m_worldModel, attachment, nullptr, 0);
    this->UpdateInteractIconScale();
    this->ObjectNameVisibilityChanged();
}

// VERIFY: c0742b82 auto
// OFFSET: 0x007193C0
void CGUnit_C::UpdateInteractIconScale() {
    if (!this->m_mountModel || this->m_mountModel->IsLoaded(0, 0)) {
        CGObject_C::UpdateInteractIconScale();
    }
}

// VERIFY: 6ba1cbf2 auto
// OFFSET: 0x007195D0
CreatureSoundDataRec* CGUnit_C::GetMountSoundDataRec() const {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

    if (!displayInfo) {
        return nullptr;
    }

    CreatureSoundDataRec* soundData = g_creatureSoundDataDB.GetRecord(displayInfo->m_soundID);

    if (soundData) {
        return soundData;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);
    return g_creatureSoundDataDB.GetRecord(modelData->m_soundID);
}

// VERIFY: 7bdb435b manual
// OFFSET: 0x00719900
int32_t __stdcall CGUnit_C::GetRecordForTitle(int32_t maskId) {
    for (int32_t i = 0; i < g_charTitlesDB.m_numRecords; i++) {
        CharTitlesRec* rec = (i < 0 || i >= g_charTitlesDB.m_numRecords) ? nullptr : &g_charTitlesDB.m_records[i];

        if (rec && rec->m_maskID == maskId) {
            return rec->m_id;
        }
    }

    return 0;
}

// VERIFY: 4b0cd521 manual
// OFFSET: 0x00719950
const char* CGUnit_C::GetUnitTitle() const {
    if (!this->m_creatureCacheEntry) {
        return nullptr;
    }

    const char* title = this->m_creatureCacheEntry->m_subName;

    if (!*title || this->m_unit->m_unitFieldPetnumber) {
        return nullptr;
    }

    return title;
}

// VERIFY: 54b589a7 manual
// OFFSET: 0x0071A030
int32_t CGUnit_C::IsSplashing(const C3Vector& position) const {
    int32_t liquidType;
    float height;
    int32_t flags;

    if (World::QueryObjectLiquid(this->m_worldObject, &liquidType, &height, &flags)) {
        int32_t flag8;
        int32_t flag9;
        World::QueryObjectLiquidFlags(this->m_worldObject, &flag8, &flag9);

        if (flag9 && height + 0.0099999998f > position.z && !(this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING)) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 066b72ae auto
// OFFSET: 0x0071A380
uint32_t CGUnit_C::GetClientStandState() const {
    return this->m_unit->m_unitFieldBytes1[0];
}

// VERIFY: 232c855f manual
// OFFSET: 0x0071A430
int32_t CGUnit_C::HasBakedNpcTexture() const {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    return 0;
}

// VERIFY: c01a2903 auto
// OFFSET: 0x0071A470
uint32_t CGUnit_C::GetDisplayRace() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displayRaceID;
    }

    return this->m_unit->m_unitFieldBytes0[0];
}

// VERIFY: f436709c auto
// OFFSET: 0x0071A490
uint32_t CGUnit_C::GetDisplaySex() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displaySexID;
    }

    return this->m_unit->m_unitFieldBytes0[2];
}

// VERIFY: d86707be auto
// OFFSET: 0x0071AA70
int32_t CGUnit_C::GetSpellRank(const SpellRec* spell) const {
    int32_t rank = 5 * this->m_unit->m_unitFieldLevel;

    if (spell->m_maxLevel > 0 && rank >= 5 * spell->m_maxLevel) {
        rank = 5 * spell->m_maxLevel;
    }

    if (rank < 0) {
        return 0;
    }

    return rank;
}

// VERIFY: d8b95a7c manual
// OFFSET: 0x0071AC30
void CGUnit_C::ApplyAlpha(uint32_t time) {
    CM2Model* model = this->GetObjectModel();

    if (this->m_forceWorldModelAlpha) {
        model = this->m_worldModel;
    }

    if (!model) {
        return;
    }

    if (this->m_fadeDuration) {
        if (static_cast<int32_t>(time - this->m_fadeStartTime - this->m_fadeDuration) < 0) {
            this->m_alpha = this->m_fadeStartAlpha + static_cast<int32_t>((time - this->m_fadeStartTime) * (this->m_fadeEndAlpha - this->m_fadeStartAlpha)) / static_cast<int32_t>(this->m_fadeDuration);
        } else {
            this->m_alpha = this->m_fadeEndAlpha;
            this->m_fadeDuration = 0;
        }
    }

    model->m_fadeAlpha = static_cast<float>(this->m_alphaScale) * this->m_alpha * (1.0f / 65025.0f);
}

// VERIFY: b788c577 auto
// OFFSET: 0x0071AD20
int32_t CGUnit_C::GetSpellCastingTime(const SpellRec* spell) const {
    if (!spell) {
        return 0;
    }

    SpellCastTimesRec* castTime = g_spellCastTimesDB.GetRecord(spell->m_castingTimeIndex);

    if (!castTime) {
        return 0;
    }

    int32_t time = castTime->m_base + castTime->m_perLevel * static_cast<int32_t>(static_cast<uint32_t>(this->GetSpellRank(spell)) / 5 - spell->m_baseLevel);

    if (time < castTime->m_minimum) {
        time = castTime->m_minimum;
    }

    if (!(spell->m_attributes & 0x30) && !(spell->m_attributesExC & 0x20000000) && time > 0) {
        CGUnitData* unit = this->m_unit;

        if (fnotequal(unit->m_unitModCastSpeed, 1.0f)) {
            time = static_cast<int32_t>(time * unit->m_unitModCastSpeed);
        }
    }

    if (spell->m_attributes & 0x2) {
        time = 0x7FFFFFFF;
    }

    return time <= 0 ? 0 : time;
}

// VERIFY: f55ea921 auto
// OFFSET: 0x0071AF70
uint32_t CGUnit_C::GetShapeshiftForm() const {
    if (this->m_shapeshiftSuppressed) {
        return 0;
    }

    return this->m_unit->m_unitFieldBytes2[3];
}

// VERIFY: ae4e25f4 auto
// OFFSET: 0x0071AF90
int32_t CGUnit_C::IsInMelee() const {
    return static_cast<bool>(this->m_combat.m_attacking);
}

// VERIFY: 7ef7ac88 manual
// OFFSET: 0x0071AFB0
int32_t CGUnit_C::IsInCombat() const {
    if (this->m_unit->m_unitFieldFlags & 0x800) {
        return 1;
    }

    return static_cast<bool>(this->m_combat.m_attacking);
}

// VERIFY: b236545b auto
// OFFSET: 0x0071AFE0
void CGUnit_C::UnRootEffects() {
    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        effect->m_flags &= ~0x4000;
    }
}

// VERIFY: e72ba487 auto
// OFFSET: 0x0071B600
uint64_t CGUnit_C::GetCreatorGuid() const {
    CGUnitData* unit = this->m_unit;

    if (unit->m_unitFieldCharmedby || unit->m_unitFieldSummonedby) {
        return 0;
    }

    return unit->m_unitFieldCreatedby.m_guid;
}

// VERIFY: b6dbb791 manual
// OFFSET: 0x0071B630
bool CGUnit_C::CanGainXP() const {
    CGUnitData* unit = this->m_unit;

    if (!unit->m_unitFieldPetnumber) {
        return false;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));
    return owner && owner->m_unit->m_unitFieldBytes0[1] == 3;
}

// VERIFY: 6c86f70b manual
// OFFSET: 0x0071B680
int32_t CGUnit_C::IsPVPLeader() const {
    if (!(this->m_unit->m_unitFieldBytes2[1] & 0x1)) {
        return 0;
    }

    if (!this->m_creatureCacheEntry || !this->m_creatureCacheEntry->m_racialLeader) {
        return 0;
    }

    return 1;
}

// VERIFY: 514b93b6 auto
// OFFSET: 0x0071B770
uint32_t CGUnit_C::GetBasePowerForType(int32_t powerType) const {
    switch (powerType) {
        case -2:
            return this->m_unit->m_unitFieldBaseHealth;

        case 0:
            return this->m_unit->m_unitFieldBaseMana;

        case 1:
        case 6:
            return 1000;

        case 2:
        case 3:
            return 100;

        case 5:
            return 1;

        default:
            return 0;
    }
}

// VERIFY: b148d15a auto
// OFFSET: 0x0071B810
uint32_t CGUnit_C::GetCanFly() const {
    return this->m_movementData.m_flags & MOVEMENTFLAG_CAN_FLY;
}

// VERIFY: 54d048fe manual
// OFFSET: 0x0071B820
int32_t CGUnit_C::IsInMeleeRange(CGUnit_C* target) const {
    CGUnit* targetUnit = target ? static_cast<CGUnit*>(target) : nullptr;
    float range = this->m_unit->m_unitFieldCombatreach + targetUnit->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        range = 5.0f;
    }

    C3Vector targetPosition = target->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = targetPosition.x - position.x;
    float dy = targetPosition.y - position.y;
    float dz = targetPosition.z - position.z;
    return range * range > dx * dx + dz * dz + dy * dy;
}

// VERIFY: fd381439 manual
// OFFSET: 0x00716710
bool CGUnit_C::IsClientControlled() const {
    if (!(this->m_unit->m_unitFieldFlags & 0x2) && (this->m_unit->m_unitFieldFlags & 0xC00004)) {
        return false;
    }

    CGUnitData* unit = this->m_unit;
    uint32_t flags = unit->m_unitFieldFlags;

    if (flags & 0x1000000) {
        const WOWGUID* controller = this->GetCharmerOrCreatorGuid();
        CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!owner || !(owner->m_obj->m_type & TYPEMASK_PLAYER)) {
            return false;
        }

        return !(owner->m_unit->m_unitFieldFlags & 0x1);
    }

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || unit->m_unitFieldCharmedby) {
        return false;
    }

    return !(flags & 0x1);
}

// VERIFY: a355861f manual
// OFFSET: 0x00716F10
void CGUnit_C::RequestMirrorImageData() {
    this->m_clientFlags |= 0x20000;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GET_MIRROR_IMAGE_DATA));
    msg.Put(this->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: a2e758f2 auto
// OFFSET: 0x00716FD0
int32_t CGUnit_C::ChooseSpecialIdleAnim(uint32_t flags, uint32_t* anim) const {
    if (!(this->m_animationState & 0x40) || this->m_mountModel || this->unk_09F8 != 10) {
        return (flags & 0xFFFFF800) == 0;
    }

    if (this->m_clientFlags & 0x40000000) {
        return 0;
    }

    if (!this->m_worldModel->HasSequence(127)) {
        return (flags & 0xFFFFFFFC) == 0;
    }

    *anim = 127;
    return 1;
}

// VERIFY: 5f734b90 auto
// OFFSET: 0x007171C0
int32_t CGUnit_C::ChooseEmoteAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t state = this->m_animationState;

    if ((!(state & 0x40) || this->m_mountModel) && !(state & 0x20)) {
        return (flags & 0x200) == 0;
    }

    int32_t emoteState = this->m_unit->m_unitNpcEmotestate;

    if (!emoteState) {
        return (flags & 0x200) == 0;
    }

    EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

    if (!emote || (CGGameUI::m_interactTarget == this->m_obj->m_guid && (emote->m_emoteFlags & 0x2000))) {
        return (flags & 0x200) == 0;
    }

    if (flags & 0x200) {
        *anim = emote->m_animID;
    }

    return 1;
}

// VERIFY: a913b882 auto
// OFFSET: 0x00717260
uint32_t CGUnit_C::GetCurrentTorsoAnimId() const {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0)) {
        return 0xFFFFFFFF;
    }

    if (this->m_torsoKeyBone == -1) {
        return this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    uint32_t anim = this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone);

    if (anim == 0xFFFFFFFF) {
        return this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    return anim;
}

// VERIFY: 98a0fb8b auto
// OFFSET: 0x007172B0
uint32_t CGUnit_C::GetCurrentBaseAnimId() const {
    if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) {
        return this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    return 0xFFFFFFFF;
}

// VERIFY: 6b5cc13c auto
// OFFSET: 0x007172E0
uint32_t CGUnit_C::GetCurrentMountAnimId() const {
    if (this->m_mountModel && this->m_mountModel->IsLoaded(0, 0)) {
        return this->m_mountModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    return 0xFFFFFFFF;
}

// VERIFY: ab38fe5b manual
// OFFSET: 0x007173F0
void CGUnit_C::GetCurrentAnimationInfo(M2BoneSequenceInfo* mount, M2BoneSequenceInfo* base, M2BoneSequenceInfo* torso, int32_t keepFinished) const {
    mount->m_sequenceId = 0;
    mount->m_variationIndex = 0;
    mount->m_time = 0;
    mount->m_speed = 0.0f;
    mount->m_startTime = 0;
    mount->m_endTime = 0;
    mount->m_finished = 0;
    mount->m_complete = 0;

    if (this->m_mountModel && this->m_mountModel->IsLoaded(0, 0)) {
        this->m_mountModel->GetBoneSequenceInfo(0xFFFFFFFF, *mount);
    } else {
        mount->m_finished = 1;
    }

    if (mount->m_complete) {
        mount->m_finished = 1;
    }

    if (mount->m_finished) {
        mount->m_sequenceId = 0xFFFFFFFF;
        mount->m_speed = 1.0f;
        mount->m_variationIndex = 0xFFFFFFFF;
        mount->m_time = 0;
    }

    base->m_sequenceId = 0;
    base->m_variationIndex = 0;
    base->m_time = 0;
    base->m_speed = 0.0f;
    base->m_startTime = 0;
    base->m_endTime = 0;
    base->m_finished = 0;
    base->m_complete = 0;
    this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, *base);

    if (base->m_complete) {
        base->m_finished = 1;
    }

    if (base->m_finished && !keepFinished) {
        base->m_sequenceId = 0xFFFFFFFF;
        base->m_speed = 1.0f;
        base->m_variationIndex = 0xFFFFFFFF;
        base->m_time = 0;
    }

    torso->m_sequenceId = 0;
    torso->m_variationIndex = 0;
    torso->m_time = 0;
    torso->m_speed = 0.0f;
    torso->m_startTime = 0;
    torso->m_endTime = 0;
    torso->m_finished = 0;
    torso->m_complete = 0;

    if (this->m_torsoKeyBone == -1) {
        torso->m_finished = 1;
    } else {
        this->m_worldModel->GetBoneSequenceInfo(this->m_torsoKeyBone, *torso);
    }

    if (torso->m_complete) {
        torso->m_finished = 1;
    }

    if (torso->m_finished && !keepFinished) {
        torso->m_sequenceId = 0xFFFFFFFF;
        torso->m_speed = 1.0f;
        torso->m_variationIndex = 0xFFFFFFFF;
        torso->m_time = 0;
    }
}

// VERIFY: 71fe5a98 manual
// OFFSET: 0x00717600
int32_t CGUnit_C::FindDefaultAnimForTier(int32_t tier, uint32_t* anim) const {
    if (g_animationDataDB.m_numRecords <= 0) {
        return 0;
    }

    AnimationDataRec* rec = g_animationDataDB.m_records;
    uint32_t id;

    if (!tier && rec && !rec->m_behaviorID && !rec->m_behaviorTier) {
        id = rec->m_id;
    } else {
        AnimationDataRec* stand = g_animationDataDB.GetRecord(0);

        if (stand && stand->m_behaviorTier) {
            id = 0;
        } else {
            int32_t i = 0;

            while (true) {
                AnimationDataRec* candidate = (i < 0 || i >= g_animationDataDB.m_numRecords) ? nullptr : rec;

                if (!candidate->m_behaviorID && candidate->m_behaviorTier == tier) {
                    id = candidate->m_id;
                    break;
                }

                i++;
                rec++;

                if (i >= g_animationDataDB.m_numRecords) {
                    return 0;
                }
            }
        }
    }

    if (!this->m_worldModel->HasSequence(id)) {
        return 0;
    }

    *anim = id;
    return 1;
}

// VERIFY: 272cc5b1 manual
// OFFSET: 0x00717A20
CreatureModelDataRec* CGUnit_C::GetModelData() const {
    int32_t displayId = this->m_displayId;

    if (!displayId || this->m_unit->m_unitFieldNativedisplayid != this->m_unit->m_unitFieldDisplayid) {
        displayId = this->m_unit->m_unitFieldDisplayid;
    }

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOCREATUREDISPLAYIDFOUND|%d", displayId);
        return nullptr;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (!modelData) {
        SysMsgPrintf(STATUS_WARNING, 16, "INVALIDDISPLAYMODELRECORD|%d|%d", displayInfo->m_modelID, displayInfo->m_id);
        return nullptr;
    }

    return modelData;
}

// VERIFY: 2fde84be auto
// OFFSET: 0x00717B60
int32_t CGUnit_C::CanBeLooted(uint32_t time) const {
    CGUnitData* unit = this->m_unit;
    return unit->m_unitFieldHealth <= 0 && (!this->m_lootableTime || static_cast<int32_t>(time - this->m_lootableTime) >= 0) && (unit->m_unitDynamicFlags & 0x1);
}

// VERIFY: e9e8e2f9 auto
// OFFSET: 0x0071C0E0
float CGUnit_C::GetTrueScale() const {
    float scale = this->m_modelScale * this->m_scaleModifier * this->m_scale;

    if (this->m_mountModel) {
        return scale * this->m_mountScale;
    }

    return scale;
}

// VERIFY: bbc73873 manual
// OFFSET: 0x0071C260
void CGUnit_C::ModifyPredictedHealth(int32_t delta) {
    if (delta < 0 && (this->m_unit->m_unitFieldFlags2 & 0x100)) {
        return;
    }

    int32_t health = delta + this->m_predictedHealth;

    if (health < 1) {
        this->m_predictedHealth = 1;
    } else {
        if (health > static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth)) {
            health = this->m_unit->m_unitFieldMaxhealth;
        }

        this->m_predictedHealth = health;
    }
}

// VERIFY: bf8a5563 auto
// OFFSET: 0x0071C320
void CGUnit_C::InitPredictedPower() {
    this->m_predictedPower[0] = this->m_unit->m_unitFieldPower[0];
    this->m_predictedPower[1] = this->m_unit->m_unitFieldPower[1];
    this->m_predictedPower[2] = this->m_unit->m_unitFieldPower[2];
    this->m_predictedPower[3] = this->m_unit->m_unitFieldPower[3];
    this->m_predictedPower[4] = this->m_unit->m_unitFieldPower[4];
    this->m_predictedPower[5] = this->m_unit->m_unitFieldPower[5];
    this->m_predictedPower[6] = this->m_unit->m_unitFieldPower[6];
}

// VERIFY: 61272ca6 auto
// OFFSET: 0x0071C390
void CGUnit_C::SetManaRegenInterrupt() {
    this->m_manaRegenInterruptTime = FrameTime::s_curTimeMs + 5000;
}

// VERIFY: 7e46098c manual
// OFFSET: 0x0071C500
int32_t CGUnit_C::UsesNpcBakedTexture() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    if (!this->m_displayInfoExtra && (this->m_obj->m_type & TYPEMASK_PLAYER) && (this->m_modelData->m_flags & 0x4)) {
        return 1;
    }

    return 0;
}

// VERIFY: 97e6d037 manual
// OFFSET: 0x0071C600
int32_t __stdcall CGUnit_C::MaybeDeferTeleport(const WOWGUID* transport) {
    return *transport && !ClntObjMgrObjectPtr(*transport, TYPEMASK_OBJECT, __FILE__, __LINE__);
}

// VERIFY: 013dc073 auto
// OFFSET: 0x0071DF30
int32_t CGUnit_C::OverrideAnimForFlying(uint32_t* anim) const {
    if (anim[0] == 0xFFFFFFFF) {
        return 0;
    }

    M2SequenceInfo info = {};
    this->m_worldModel->GetSequenceInfo(anim[0], anim[1], info);

    if (info.m_animId && info.m_animId != 8 && info.m_animId != 25) {
        return 0;
    }

    anim[0] = 193;
    anim[1] = 0xFFFFFFFF;
    return 1;
}

// VERIFY: eab41c40 auto
// OFFSET: 0x0071F300
int32_t CGUnit_C::GetCreatureType() const {
    uint32_t form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];
    SpellShapeshiftFormRec* shapeshift = g_spellShapeshiftFormDB.GetRecord(form);

    if (shapeshift && shapeshift->m_creatureType > 0) {
        return shapeshift->m_creatureType;
    }

    if (this->m_creatureCacheEntry) {
        return this->m_creatureCacheEntry->m_type;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (!race || race->m_creatureType <= 0) {
        return 0;
    }

    return race->m_creatureType;
}

// VERIFY: b7deec71 manual
// OFFSET: 0x0071F390
PetPersonalityRec* CGUnit_C::GetPetPersonality() const {
    CGUnitData* unit = this->m_unit;

    if (!unit->m_unitFieldPetnumber) {
        return nullptr;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!owner || owner->m_unit->m_unitFieldBytes0[1] != 3) {
        return nullptr;
    }

    return g_petPersonalityDB.GetRecord(1);
}

// VERIFY: 1beb8441 auto
// OFFSET: 0x0071FB60
void CGUnit_C::GetFootprintInfo(int32_t* textureId, float* size) const {
    float length;
    float width;

    if (this->m_mountDisplayId <= 0 || (this->m_clientFlags & 0x10000000) || !this->m_mountModel) {
        *textureId = this->m_footprintTextureId;
        float scale = this->GetTrueScale();
        length = this->m_footprintLength * scale;
        width = scale * this->m_footprintWidth;
    } else {
        *textureId = this->m_mountFootprintTextureId;
        float scale = this->GetTrueScale();
        length = this->m_mountFootprintLength * scale;
        width = scale * this->m_mountFootprintWidth;
    }

    size[1] = width;
    size[0] = length;
}

// VERIFY: 99ea3d4f auto
// OFFSET: 0x0071FFC0
int32_t CGUnit_C::IsControlledByViewer() const {
    return this->m_obj->m_guid == ClntObjMgrGetActivePlayer() || this->m_obj->m_guid == s_activeMover || (this->m_clientFlags & 0x1000);
}

// VERIFY: e21be286 manual
// OFFSET: 0x007202C0
int32_t CGUnit_C::CanEquipItemsInThisForm() const {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    if (this->m_shapeshiftSuppressed) {
        return 1;
    }

    CGUnitData* unit = this->m_unit;

    if (this->m_displayId) {
        return this->m_displayId == unit->m_unitFieldNativedisplayid;
    }

    return unit->m_unitFieldNativedisplayid == unit->m_unitFieldDisplayid;
}

// VERIFY: 8b19ad28 manual
// OFFSET: 0x00720AF0
uint32_t CGUnit_C::GetVoiceSex() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displaySexID;
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return static_cast<const CGPlayer_C*>(this)->m_player->m_playerBytes3[0];
    }

    return this->m_unit->m_unitFieldBytes0[2];
}

// VERIFY: bc5ad5e9 auto
// OFFSET: 0x00721CA0
bool CGUnit_C::IsShapeShifted() const {
    if (this->m_shapeshiftSuppressed) {
        return false;
    }

    uint32_t form = this->m_unit->m_unitFieldBytes2[3];

    if (!form) {
        return false;
    }

    return !(g_spellShapeshiftFormDB.GetRecord(form)->m_flags & 0x1);
}

// VERIFY: faccdd0e manual
// OFFSET: 0x00721CF0
void CGUnit_C::PreemptivelyCancelShapeshift() {
    if (!(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        return;
    }

    uint32_t form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];
    SpellShapeshiftFormRec* shapeshift = g_spellShapeshiftFormDB.GetRecord(form);

    if (shapeshift) {
        CGUnitData* unit = this->m_unit;

        for (uint32_t i = 0; i < 4; i++) {
            if (unit->m_unitFieldDisplayid == shapeshift->m_creatureDisplayID[i]) {
                this->m_displayId = unit->m_unitFieldNativedisplayid;
                this->UpdateDisplayInfo(0);
                break;
            }
        }
    }

    this->m_shapeshiftSuppressed = 1;
    FrameScript_SignalEvent(377, nullptr);
}

// VERIFY: 0919a411 manual
// OFFSET: 0x00721E70
bool CGUnit_C::IsCancelShapeshiftAtFlightMaster() const {
    if (this->m_shapeshiftSuppressed) {
        return false;
    }

    uint32_t form = this->m_unit->m_unitFieldBytes2[3];

    if (!form) {
        return false;
    }

    int32_t flags = g_spellShapeshiftFormDB.GetRecord(form)->m_flags;
    return !(flags & 0x1) || (flags & 0x800);
}

// VERIFY: 1c8b9c45 auto
// OFFSET: 0x00721F90
int32_t CGUnit_C::IsAutoTracking() const {
    return this->m_obj->m_guid == s_activeMover && s_trackingType != 13;
}

// VERIFY: d88b4789 auto
// OFFSET: 0x00721FC0
bool CGUnit_C::IsActiveMoverTracking(uint32_t typeMask) const {
    if (s_trackingType == 13) {
        return false;
    }

    return this->m_obj->m_guid == s_activeMover && ((1 << s_trackingType) & typeMask);
}

// VERIFY: 33aad9dd auto
// OFFSET: 0x00722010
void CGUnit_C::TrackingMakeRelativeTo(const C44Matrix* transform, float facing) {
    if (this->m_obj->m_guid != s_activeMover) {
        return;
    }

    uint32_t type = s_trackingType;

    if (type == 13) {
        return;
    }

    if (type == 4) {
        s_trackingPosition *= *transform;
        type = s_trackingType;
    }

    if (type == 2 || type == 8) {
        s_trackingDirection = CMath::normalizeangle0to2pi_(s_trackingDirection + facing);
    }
}

// VERIFY: c2e94952 manual
// OFFSET: 0x007225E0
const char* CGUnit_C::GetFrameScriptText(const char* token, int32_t count) const {
    uint32_t sex;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        sex = static_cast<const CGPlayer_C*>(this)->m_player->m_playerBytes3[0];
    } else {
        sex = this->m_unit->m_unitFieldBytes0[2];
    }

    return FrameScript_GetText(token, count, sex == 1 ? GENDER_FEMALE : GENDER_MALE);
}

// VERIFY: 2b9830ba manual
// OFFSET: 0x00722B50
C44Matrix CGUnit_C::GetMatrix() const {
    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        return this->m_vehicle->m_worldMatrix;
    }

    C44Matrix matrix;
    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
    matrix.RotateAroundZ(this->GetRawFacing());

    if (this->GetTransportGUID()) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);
        matrix *= transport->GetMatrix();
    }

    return matrix;
}

// VERIFY: d769e18c manual
// OFFSET: 0x00723490
uint64_t CGUnit_C::GetAuraCaster(uint32_t index) {
    const WOWGUID& creator = this->m_auras[index].m_creator;
    return creator.m_guid;
}

// VERIFY: 12476da9 auto
// OFFSET: 0x00746340
void CGUnit_C::KillCreatureLoopSound() {
    SI2::StopSound(&this->m_loopSound, 0, 0.5f, 1);
}

// VERIFY: f678b1e1 auto
// OFFSET: 0x00746360
bool CGUnit_C::GetWeaponSwingType(bool offhand, int32_t* swingType) const {
    if (!this->GetVirtualItemDisplayID(offhand == false)) {
        *swingType = 0;
        return true;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(offhand == false, false);

    if (!item || item->m_classID != 2) {
        return false;
    }

    const ItemSubClassRec* subClass = SDBItemSubclassGetSubClassRec(2, item->m_subclassID);

    if (subClass) {
        *swingType = subClass->m_weaponSwingSize;
    }

    return subClass != nullptr;
}

// VERIFY: b66560c6 auto
// OFFSET: 0x007463E0
uint32_t CGUnit_C::GetImpactType() const {
    if (this->m_soundData && this->m_soundData->m_creatureImpactType < 4) {
        return s_impactTypes[this->m_soundData->m_creatureImpactType];
    }

    return 0;
}

// VERIFY: 22593312 auto
// OFFSET: 0x007464D0
void CGUnit_C::PlayFoleySound() {
    static CVar* s_enableArmorFoley = CVar::Lookup("Sound_EnableArmorFoleySoundForOthers");

    if (s_enableArmorFoley && s_enableArmorFoley->m_intValue) {
        C3Vector position = this->GetPosition();
        SI2::PlayFoleySound(this->m_modelData->m_foleyMaterialID, &position, 0);
    }
}

// VERIFY: f4e40cf7 auto
// OFFSET: 0x007465E0
void CGUnit_C::PlayStandSound() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        this->PlayUnitSound(5, 0);
    }
}

// VERIFY: 5cdf8669 auto
// OFFSET: 0x00746900
int32_t CGUnit_C::GetNewNPCSoundCount(int32_t count, int32_t* soundKitId, int32_t* soundIndex) const {
    if (!this->m_displayInfo) {
        return -1;
    }

    NPCSoundsRec* sounds = g_npcSoundsDB.GetRecord(this->m_displayInfo->m_npcsoundID);

    if (!sounds || !sounds->m_soundID[0]) {
        return -1;
    }

    if (count < 5) {
        *soundKitId = sounds->m_soundID[0];
        *soundIndex = -1;
        return count + 1;
    }

    if (count - 5 < SI2::GetNumSoundsInSoundKit(sounds->m_soundID[2])) {
        *soundIndex = count - 5;
        *soundKitId = sounds->m_soundID[2];
        return count + 1;
    }

    *soundKitId = sounds->m_soundID[0];
    *soundIndex = -1;
    return 0;
}

// VERIFY: 055965ad auto
// OFFSET: 0x00746AD0
void CGUnit_C::PlayEmoteStateSound(const C3Vector* position) {
    EmotesRec* emote = g_emotesDB.GetRecord(this->m_unit->m_unitNpcEmotestate);

    if (emote && emote->m_emoteSpecProc == 2) {
        SI2::PlaySoundKit(emote->m_eventSoundID, position, nullptr, nullptr, 0, nullptr, 1, 0);
    }
}

// VERIFY: c9c84a2c manual
// OFFSET: 0x00746B30
int32_t CGUnit_C::GetUnitSoundKitIndex(uint32_t soundType) const {
    switch (soundType) {
        case 0:
        case 1:
            return (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags & 0x20)) ? 15 : 9;

        case 2:
        case 3:
        case 9:
            return (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags & 0x20)) ? 16 : 11;

        default:
            return 0;
    }
}

// VERIFY: dfd4b1ae auto
// OFFSET: 0x0074B880
void CGUnit_C::VehiclePassengerInitWorldCameraState() {
    s_vehiclePassengerCameraState[2] = 0;
    s_vehiclePassengerCameraState[1] = 0;
    s_vehiclePassengerCameraState[0] = 0;
}

// VERIFY: 352da6a6 auto
// OFFSET: 0x0074B8A0
void CGUnit_C::VehiclePassengerOnUninit() {
    this->m_vehiclePassenger = nullptr;
}

// VERIFY: 014f995c auto
// OFFSET: 0x0074B8B0
int32_t CGUnit_C::HasVehicleTransport() const {
    uint32_t high = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid >> 32);
    return (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid)));
}

// VERIFY: c6051623 auto
// OFFSET: 0x0074B9A0
uint32_t CGUnit_C::IsStrafingDisabled() const {
    return this->m_movementData.m_flags2 & MOVEMENTFLAG2_NO_STRAFE;
}

// VERIFY: d8212e45 auto
// OFFSET: 0x0074BB90
uint32_t CGUnit_C::IsVehicleCurrentlyUnstoppable() const {
    VehicleRec* vehicle = this->m_vehicle ? this->m_vehicle->m_vehicleRec : nullptr;

    if (!vehicle) {
        return 0;
    }

    return vehicle->m_flags & ((this->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) ? 0x100 : 0x80);
}

// VERIFY: 64e4513d auto
// OFFSET: 0x00754EF0
const CGUnitVirtualItem* CGUnit_C::GetParryingItem(bool offhand) const {
    const CGUnitVirtualItem* item = this->GetVirtualItem(0, false);

    if (offhand || (item && item->m_classID != 2)) {
        item = this->GetVirtualItem(1, false);

        if (item && item->m_classID != 2 && item->m_classID != 4) {
            return nullptr;
        }
    }

    return item;
}

// VERIFY: 7a0f5b4d auto
// OFFSET: 0x00754F40
int32_t CGUnit_C::GetUnitSize() const {
    int32_t size = this->m_displayInfo->m_sizeClass;

    if (size == -1) {
        return this->m_modelData->m_sizeClass;
    }

    return size;
}

// VERIFY: 94a622f4 auto
// OFFSET: 0x00755540
const CGUnitVirtualItem* CGUnit_C::GetAttackingWeapon(int32_t hand, bool requireLoadedModel) const {
    if (!this->m_sheatheState) {
        return nullptr;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(hand != 0, false);

    if (!item || item->m_classID != 2) {
        return nullptr;
    }

    if (!requireLoadedModel) {
        return item;
    }

    CM2Model* model = this->m_worldModel;

    if (!model || !model->IsLoaded(0, 0)) {
        return nullptr;
    }

    CM2Model* attached = model->m_attachList;

    if (!attached) {
        return nullptr;
    }

    while (attached->m_attachmentId != (hand != 0) + 1) {
        attached = attached->m_attachNext;

        if (!attached) {
            return nullptr;
        }
    }

    if (attached->IsLoaded(0, 0)) {
        return item;
    }

    return nullptr;
}

// VERIFY: 2e85141a manual
// OFFSET: 0x007559E0
void CGUnit_C::StopAttack() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ATTACK_STOP));
    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_stopAttackSent = 1;
}

// VERIFY: 5a947930 auto
// OFFSET: 0x0056C220
bool CGUnit_C::IsInVehicle() const {
    return this->m_vehiclePassenger && this->m_vehiclePassenger->m_state;
}

// VERIFY: 2230d2c7 auto
// OFFSET: 0x00714AD0
int32_t Packet_SMSG_MULTIPLE_PACKETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    while (!msg->IsRead() && msg->m_read <= msg->m_size) {
        ClientServices::Connection()->ProcessMessage(time, msg, 1);
    }

    return 1;
}

// VERIFY: 3b3b64a0 auto
// OFFSET: 0x00714B20
int32_t Packet_SMSG_UNKNOWN_1240(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    char text[512];
    msg->Get(guid.m_guid);
    msg->GetString(text, sizeof(text));
    return 1;
}

// VERIFY: e07f113e auto
// OFFSET: 0x00714C10
float CopySign(float value, float sign) {
    uint32_t bits = (*reinterpret_cast<uint32_t*>(&sign) ^ *reinterpret_cast<uint32_t*>(&value)) & 0x7FFFFFFF;
    bits ^= *reinterpret_cast<uint32_t*>(&sign);
    return *reinterpret_cast<float*>(&bits);
}

// VERIFY: af507daf manual
// OFFSET: 0x007167C0
int32_t AnimTierChangeHandler(WOWGUID guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        int32_t tier = unit->m_unit->m_unitFieldBytes1[3];

        if (tier != unit->m_animTier) {
            unit->m_animTier = tier;
        }
    }

    return 1;
}

// VERIFY: 70795edd manual
// OFFSET: 0x00716900
int32_t TargetChangeHandler(WOWGUID guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_targetChangeTime = OsGetAsyncTimeMs();
    }

    return 1;
}

// VERIFY: 88a563c1 auto
// OFFSET: 0x00716C00
int32_t Packet_SMSG_PET_DISMISS_SOUND(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t modelId;
    msg->Get(modelId);

    C3Vector position;
    msg->Get(position.x);
    msg->Get(position.y);
    msg->Get(position.z);

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(modelId);

    if (modelData) {
        CreatureSoundDataRec* soundData = g_creatureSoundDataDB.GetRecord(modelData->m_soundID);

        if (soundData && soundData->m_soundPetDismissID) {
            position.z = position.z + 1.0f;
            SI2::PlaySoundKit(soundData->m_soundPetDismissID, &position, nullptr, nullptr, 0, nullptr, 1, 0);
        }
    }

    return 1;
}

// VERIFY: 3607660a manual
// OFFSET: 0x00717310
float CGUnit_C::GetTorsoAnimProgress() const {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0)) {
        return 0.0f;
    }

    M2BoneSequenceInfo info;
    info.m_sequenceId = 0;
    info.m_variationIndex = 0;
    info.m_time = 0;
    info.m_speed = 0.0f;
    info.m_startTime = 0;
    info.m_finished = 0;
    info.m_complete = 0;
    info.m_endTime = 0;

    if (this->m_torsoKeyBone != -1) {
        this->m_worldModel->GetBoneSequenceInfo(this->m_torsoKeyBone, info);
    }

    if (info.m_sequenceId == 0xFFFFFFFF) {
        this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, info);
    }

    float duration = static_cast<float>(static_cast<int32_t>(info.m_endTime - info.m_startTime));

    if (duration < 0.00000011920929f) {
        return 0.0f;
    }

    float progress = info.m_time / duration;

    if (progress < 0.0f) {
        return 0.0f;
    }

    if (progress >= 1.0f) {
        return 1.0f;
    }

    return progress;
}

// VERIFY: 5ee0ebb3 manual
// OFFSET: 0x00717EC0
void CGUnit_C::BuildSmoothMatrix(C44Matrix& matrix) const {
    matrix = C44Matrix::RotationAroundZ(this->m_targetFacing);

    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;

    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (transport) {
        C44Matrix transportMatrix;

        if (transport->m_obj->m_type & TYPEMASK_UNIT) {
            static_cast<CGUnit_C*>(transport)->BuildSmoothMatrix(transportMatrix);
        } else {
            transportMatrix = transport->GetMatrix();
        }

        matrix *= transportMatrix;
    }
}

// VERIFY: 73623b2d manual
// OFFSET: 0x00719980
bool __stdcall CGUnit_C::IsSpellAllowedInArena(const SpellRec* spell) {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && map->m_instanceType == 4) {
        int32_t attributes = spell->m_attributesExD;

        if (!(attributes & 0x20000) && (spell->m_recoveryTime > 900000 || spell->m_categoryRecoveryTime > 900000 || (attributes & 0x10000))) {
            return false;
        }
    }

    return true;
}

// VERIFY: bba780f3 manual
// OFFSET: 0x00719A90
void CGUnit_C::GetSwimMatrix(C44Matrix& matrix) const {
    C33Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    rotation.FromEulerAnglesZYX(-(6.2831855f - this->GetRenderFacing()), 6.2831855f - this->m_movementData.m_pitch, 0.0f);

    matrix.a0 = rotation.a0;
    matrix.a1 = rotation.a1;
    matrix.a2 = rotation.a2;
    matrix.a3 = 0.0f;
    matrix.b0 = rotation.b0;
    matrix.b1 = rotation.b1;
    matrix.b2 = rotation.b2;
    matrix.b3 = 0.0f;
    matrix.c0 = rotation.c0;
    matrix.c1 = rotation.c1;
    matrix.c2 = rotation.c2;
    matrix.c3 = 0.0f;
    matrix.d0 = 0.0f;
    matrix.d1 = 0.0f;
    matrix.d2 = 0.0f;
    matrix.d3 = 1.0f;
    matrix.Scale(this->GetTrueScale());

    C3Vector position = this->GetPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
}

// VERIFY: 54b43bec manual
// OFFSET: 0x0071B6B0
int32_t CGUnit_C::IsLooting() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        const WOWGUID& target = static_cast<const CGPlayer_C*>(this)->m_lootTarget;
        return static_cast<bool>(target);
    }

    return (this->m_unit->m_unitFieldFlags >> 10) & 1;
}

// VERIFY: 51b8f049 auto
// OFFSET: 0x0071BFF0
void CGUnit_C::ClearPrecastMissileModel(uint8_t castCount) {
    if (castCount && castCount != this->m_precastCastCount) {
        return;
    }

    CM2Model* model = this->m_precastMissileModel;

    if (model) {
        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = nullptr;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;
}

// VERIFY: 991ba226 manual
// OFFSET: 0x0071C4D0
uint64_t CGUnit_C::GetCameraRelativeTo() const {
    if (this->m_vehicleCamera && this->m_vehicleCamera->m_state) {
        return this->m_vehicleCamera->GetRelativeTo();
    }

    return this->GetTransportGUID();
}

// VERIFY: 881c67fa manual
// OFFSET: 0x0071CA10
int32_t HoverHeightChangeHandler(WOWGUID guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.m_hoverHeight = unit->m_unit->m_unitFieldHoverheight;
    }

    return 1;
}

// VERIFY: 79327c77 manual
// OFFSET: 0x0071DB20
int32_t IsBirthAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior == 127 || (behavior > 200 && behavior <= 202)) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 923e44ce auto
// OFFSET: 0x0071DB70
int32_t IsSubmergedDeathAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 466 && (behavior <= 468 || behavior == 472)) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 7173da99 manual
// OFFSET: 0x0071DBC0
int32_t IsCorpseAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior > 468) {
        return behavior == 472;
    }

    if (behavior >= 467) {
        return 1;
    }

    return behavior == 6 || behavior == 132;
}

// VERIFY: 1b5d614f auto
// OFFSET: 0x0071DC20
int32_t IsAirborneAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 37 && (behavior <= 40 || behavior == 467)) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 2a3c94c3 manual
// OFFSET: 0x0071DD80
int32_t IsDyingAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior > 131) {
        if (behavior < 466 || behavior > 467) {
            return 0;
        }
    } else if (behavior != 131 && behavior != 1) {
        return 0;
    }

    return 1;
}

// VERIFY: 03ed6220 manual
// OFFSET: 0x0071DDE0
int32_t IsDeadOrDyingAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior > 132) {
        if (behavior < 466 || (behavior > 468 && behavior != 472)) {
            return 0;
        }
    } else if (behavior < 131 && behavior != 1 && behavior != 6) {
        return 0;
    }

    return 1;
}

// VERIFY: e05af72f auto
// OFFSET: 0x007176B0
int32_t GetAnimBehaviorId(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        return rec->m_behaviorID;
    }

    return 506;
}

// VERIFY: 211bd0bd auto
// OFFSET: 0x00734F70
void CGUnit_C::GetDefenseSkillRank(int32_t& rank, int32_t& bonus) const {
    rank = 5 * this->m_unit->m_unitFieldLevel;
    bonus = 0;
}

// VERIFY: fdf5655e auto
// OFFSET: 0x00734FA0
void CGUnit_C::GetAttackSkillRank(int32_t hand, int32_t& rank, int32_t& bonus) const {
    rank = 5 * this->m_unit->m_unitFieldLevel;
    bonus = 0;
}

// VERIFY: 1d778d00 manual
// OFFSET: 0x00737390
void CGUnit_C::SetMaxAlpha(float alpha) {
    CGUnit_C* transport;

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state == 3 && (transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        CM2Model* model = this->GetObjectModel();

        if (model && model->m_attachParent == transport->GetObjectModel()) {
            this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(255.0f));
        } else {
            this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(alpha * 255.0f));
        }
    } else {
        this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(alpha * 255.0f));
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec) {
        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                unit->SetMaxAlpha(alpha);
            }
        }
    }
}

// VERIFY: d0b7eaae manual
// OFFSET: 0x00746720
void CGUnit_C::PlaySplashSound(const C3Vector* position) {
    static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

    int32_t activeMover;
    int32_t listenerAtCharacter;

    if (s_activeMover != this->m_obj->m_guid) {
        activeMover = 0;
        listenerAtCharacter = 0;
    } else {
        activeMover = 1;
        listenerAtCharacter = s_listenerAtCharacter->m_intValue ? 1 : 0;
    }

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();

    if (activeMover) {
        properties.m_priority = 110;

        if (listenerAtCharacter) {
            properties.m_volumeScale = 0.64999998f;
        }
    }

    properties.m_group = 8;
    SI2::PlaySoundKit(this->m_splashSoundKitId, listenerAtCharacter ? nullptr : position, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: e49e815c auto
// OFFSET: 0x007469C0
void CGUnit_C::PlayAISound(int32_t soundKitId) {
    SI2::StopSound(&this->m_npcSound, 1, -1.0f, 1);

    static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

    if (s_activeMover == this->m_obj->m_guid && s_listenerAtCharacter->m_intValue) {
        SE2SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_volumeScale = 0.64999998f;
        properties.m_priority = 110;
        SI2::PlaySoundKit(soundKitId, nullptr, &this->m_aiSound, &properties, 0, nullptr, 1, 0);
    } else {
        C3Vector position = this->GetPosition();
        SI2::PlaySoundKit(soundKitId, &position, &this->m_aiSound, nullptr, 0, nullptr, 1, 0);
        SI2::AttachToObject(&this->m_aiSound, this->m_obj->m_guid);
    }
}

// VERIFY: b56823a7 manual
// OFFSET: 0x007470D0
void CGUnit_C::PlayDismountSound() {
    static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

    int32_t activeMover;
    int32_t listenerAtCharacter;

    if (s_activeMover != this->m_obj->m_guid) {
        activeMover = 0;
        listenerAtCharacter = 0;
    } else {
        activeMover = 1;
        listenerAtCharacter = s_listenerAtCharacter->m_intValue ? 1 : 0;
    }

    C3Vector position = this->GetPosition();

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();

    if (activeMover) {
        properties.m_priority = 110;

        if (listenerAtCharacter) {
            properties.m_volumeScale = 0.64999998f;
        }
    }

    SI2::PlaySoundKit(s_dismountSoundKitId, listenerAtCharacter ? nullptr : &position, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: c0baa56a auto
// OFFSET: 0x0074B840
int32_t CGUnit_C::VehiclePassengerIsTransitionAllowed(const VehicleSeatRec* seat, int32_t checkSeat) const {
    return (this->m_clientFlags & 0x20000000) || (checkSeat && seat && (seat->m_flagsB & 0x1000000));
}

// VERIFY: 561780c9 auto
// OFFSET: 0x0074BB60
int32_t CGUnit_C::ShouldUseVehicleSeatExitAnimation(const VehicleSeatRec* seat) const {
    return seat->m_flags & ((this->m_movementData.m_flags2 & 0x40) ? 0x8 : 0x8000);
}

// VERIFY: 71f105c7 manual
// OFFSET: 0x00716650
int32_t CGUnit_C::ShouldFadeIn() const {
    CGUnitData* unit = this->m_unit;

    if ((unit->m_unitFieldFlags2 & 0x20) || (unit->m_unitFieldFlags & 0x2) || (unit->m_unitFieldBytes1[2] & 0x2)) {
        return 0;
    }

    if (!this->HasVehicleTransport()) {
        return 1;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport) {
        return 0;
    }

    VehicleSeatRec* seat = transport->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return (seat && seat->m_attachmentID < 0) || transport->m_fadeDuration == 0;
}

// VERIFY: 785dd0e1 manual
// OFFSET: 0x00716FA0
int32_t CGUnit_C::IsRunning() const {
    return this->m_movementData.m_walkSpeed + this->m_movementData.m_walkSpeed >= this->m_movementData.GetBaseSpeed(0);
}

// VERIFY: 2be9606e manual
// OFFSET: 0x00717050
int32_t CGUnit_C::ChooseMovementAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t moveFlags = this->m_movementData.m_flags;

    if (!(moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))) {
        return (flags & 0xFFFFFFF8) == 0;
    }

    uint32_t state = this->m_animationState;

    if (!(state & 0x70) || !(flags & 0x4) || (state & 0x800000) || (state & 0x2000000)) {
        return 1;
    }

    if (moveFlags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        if (moveFlags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            *anim = ANIM_SWIM_RIGHT - ((moveFlags & MOVEMENTFLAG_STRAFE_LEFT) != 0);
            return 1;
        }

        *anim = (moveFlags & MOVEMENTFLAG_BACKWARD) ? ANIM_SWIM_BACKWARDS : ANIM_SWIM;
        return 1;
    }

    if (this->IsOnFlyingSpline()) {
        *anim = ANIM_FLY;
        return 1;
    }

    if (moveFlags & MOVEMENTFLAG_BACKWARD) {
        *anim = ANIM_WALKBACKWARDS;
        return 1;
    }

    if (this->m_unit->m_unitFieldBytes1[2] & 0x2) {
        *anim = ANIM_STEALTH_WALK;
        return 1;
    }

    float speed = this->m_movementData.GetBaseSpeed(0);

    if (speed >= 11.0f) {
        *anim = ANIM_SPRINT;
        return 1;
    }

    if (this->m_movementData.m_walkSpeed + this->m_movementData.m_walkSpeed < speed) {
        *anim = ANIM_RUN;
        return 1;
    }

    *anim = ANIM_WALK;
    return 1;
}

// VERIFY: 2b271f9b manual
// OFFSET: 0x00717AD0
float CGUnit_C::GetStandHeight() const {
    CreatureModelDataRec* modelData = this->m_modelData;

    if (!modelData) {
        modelData = this->GetModelData();

        if (!modelData) {
            return 0.0f;
        }
    }

    float height = modelData->m_geoBoxMaxZ - modelData->m_geoBoxMinZ;
    return this->GetTrueScale() * height;
}

// VERIFY: 566bccef manual
// OFFSET: 0x00717B20
int32_t CGUnit_C::GetModelFileName(const char** name) const {
    CreatureModelDataRec* modelData = this->GetModelData();

    if (modelData) {
        *name = modelData->m_modelName;
        return *name != nullptr;
    }

    *name = "Spells\\ErrorCube.mdx";
    return 1;
}

// VERIFY: b60fda18 manual
// OFFSET: 0x00717FC0
C3Vector CGUnit_C::BuildSmoothPosition() const {
    C3Vector position = this->GetRawPosition();
    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (transport) {
        C44Matrix transportMatrix;

        if (transport->m_obj->m_type & TYPEMASK_UNIT) {
            static_cast<CGUnit_C*>(transport)->BuildSmoothMatrix(transportMatrix);
        } else {
            transportMatrix = transport->GetMatrix();
        }

        position *= transportMatrix;
    }

    return position;
}

// VERIFY: 24e3b6ca manual
// OFFSET: 0x0071A720
C3Vector CGUnit_C::GetMissileFirePosition() const {
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded(0, 0)) {
        if (model->HasEvent(0x4C534324)) {
            return model->GetEventPosition(0x4C534324);
        }

        if (model->HasEvent(0x52534324)) {
            return model->GetEventPosition(0x52534324);
        }

        if (model->HasEvent(0x54534324)) {
            return model->GetEventPosition(0x54534324);
        }

        if (this->m_vehiclePassenger) {
            return const_cast<CGUnit_C*>(this)->GetModelPosition();
        }
    }

    return this->GetPosition();
}

// VERIFY: 460736e3 auto
// OFFSET: 0x0071A940
void CGUnit_C::StartProceduralFade(uint8_t alpha, uint32_t type, uint32_t time, uint32_t hold, uint32_t endAlpha) {
    if (type < 12) {
        this->m_proceduralFadeAlpha = alpha;
        this->m_proceduralFadeType = type;
        this->m_proceduralFadeStart = FrameTime::s_curTimeMs;
        this->m_proceduralFadeDuration = time + hold;
        this->m_proceduralFadeEndAlpha = endAlpha;
        this->DoFade(alpha, time);
    }
}

// VERIFY: 98c8bae1 auto
// OFFSET: 0x0071B000
bool CGUnit_C::AnimSuppressesMovement() const {
    CM2Model* model = this->m_worldModel;

    if (!model || !model->IsLoaded(0, 0)) {
        return false;
    }

    uint32_t anim;

    if (this->m_torsoKeyBone == -1 || (anim = this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone)) == 0xFFFFFFFF) {
        anim = this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    if (anim > 505) {
        return false;
    }

    return (g_animationDataDB.GetRecord(this->GetCurrentTorsoAnimId())->m_bodyflags >> 7) & 1;
}

// VERIFY: 75c2d34b manual
// OFFSET: 0x0071CB30
int32_t Packet_SMSG_MOUNTSPECIAL_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && !unit->IsDeepInLiquid() && guid != ClntObjMgrGetActivePlayer()) {
        unit->OnSpecialMountAnim();
    }

    return 1;
}

// VERIFY: 486abade manual
// OFFSET: 0x0071DFF0
int32_t CGUnit_C::ChooseSubmergeAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t state = this->m_animationState;

    if (state & 0x400008) {
        return 1;
    }

    if (!(state & 0x40) || this->m_mountModel) {
        return (flags & 0xFFFFFFFC) == 0;
    }

    uint32_t submerge;

    if (this->GetClientStandState() == 9) {
        submerge = (this->unk_09F8 == 9) + ANIM_SUBMERGE;
    } else {
        if (this->unk_09F8 != 9) {
            return (flags & 0xFFFFFFFC) == 0;
        }

        submerge = ANIM_EMERGE;

        if (!this->m_worldModel->HasSequence(ANIM_EMERGE)) {
            submerge = ANIM_BIRTH;
        }
    }

    if (!this->m_worldModel->HasSequence(submerge)) {
        return (flags & 0xFFFFFFFC) == 0;
    }

    if (this->GetCurrentTorsoAnimId() != submerge && (flags & 0x2)) {
        *anim = submerge;
    }

    return 1;
}

// VERIFY: 1313ac12 manual
// OFFSET: 0x0071ED80
CAaBox CGUnit_C::GetStandBox() const {
    CAaBox box;
    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.b.z = 0.0f;
    box.t.x = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;

    CreatureModelDataRec* modelData = this->m_modelData;

    if (modelData || (modelData = this->GetModelData())) {
        box = *reinterpret_cast<const CAaBox*>(&modelData->m_geoBoxMinX);
    }

    if (this->m_unit->m_unitFieldMountdisplayid > 0) {
        CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

        if (displayInfo) {
            CreatureModelDataRec* mountModel = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

            if (mountModel) {
                float offset = mountModel->m_mountHeight - (box.t.z - box.b.z) * 0.5f;
                box.b.z = box.b.z + offset;
                box.t.z = offset + box.t.z;
                box |= *reinterpret_cast<const CAaBox*>(&mountModel->m_geoBoxMinX);
            }
        }
    }

    return box;
}

// VERIFY: fa1de831 manual
// OFFSET: 0x0071F400
uint32_t CGUnit_C::GetHappinessIndex() const {
    PetPersonalityRec* personality = this->GetPetPersonality();

    if (!personality) {
        return 0;
    }

    uint32_t index = 1;

    while (this->m_unit->m_unitFieldPower[4] >= static_cast<uint32_t>(personality->m_happinessThreshold[index])) {
        if (++index >= 3) {
            break;
        }
    }

    return index - 1;
}

// VERIFY: e993503e auto
// OFFSET: 0x0071F440
const CGUnitVirtualItem* CGUnit_C::GetVirtualItem(uint32_t slot, bool ignoreDisarm) const {
    if (!this->GetVirtualItemDisplayID(slot)) {
        return nullptr;
    }

    if (ignoreDisarm) {
        return &this->m_virtualItem[slot];
    }

    if (slot == 0) {
        if (this->m_unit->m_unitFieldFlags & 0x200000) {
            const CGUnitVirtualItem* item = this->GetVirtualItem(0, true);

            if (item) {
                if (item->m_classID != 2) {
                    return &this->m_virtualItem[0];
                }

                return nullptr;
            }
        }
    } else if (slot == 1) {
        if (this->m_unit->m_unitFieldFlags & 0x200000) {
            this->GetVirtualItem(0, true);

            if (!this->IsDisarmed(0)) {
                const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, true);

                if (offhand && offhand->m_classID == 2) {
                    return nullptr;
                }
            }
        }

        if (this->m_unit->m_unitFieldFlags2 & 0x80) {
            return nullptr;
        }
    } else if (slot == 2) {
        if (this->m_unit->m_unitFieldFlags2 & 0x400) {
            return nullptr;
        }
    }

    return &this->m_virtualItem[slot];
}

// VERIFY: 07176a35 auto
// OFFSET: 0x0071FEF0
C3Vector CGUnit_C::GetNamePosition() const {
    if (this->m_worldModel->IsLoaded(0, 0)) {
        if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && this->m_worldModel->HasAttachment(29)) {
            return this->m_worldModel->GetAttachmentPosition(29);
        }

        if (this->m_worldModel->HasAttachment(18)) {
            return this->m_worldModel->GetAttachmentPosition(18);
        }
    }

    C3Vector position = this->m_vehiclePassenger ? const_cast<CGUnit_C*>(this)->GetModelPosition() : this->GetPosition();
    position.z = this->m_height * this->m_scale * 1.25f + position.z;
    return position;
}

// VERIFY: e6787c39 auto
// OFFSET: 0x00722180
bool CGUnit_C::DoNotPlayWoundAnim() const {
    if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_vehicle->HasHighPriorityAnim()) {
        return true;
    }

    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags >> 3) & 1;
    }

    return this->GetCurrentTorsoAnimId() == 121;
}

// VERIFY: 54fb09a7 manual
// OFFSET: 0x007221D0
void CGUnit_C::CreateFishingLineEffect() {
    CGObject_C* bobber = ClntObjMgrObjectPtr(this->m_unit->m_unitFieldChannelObject, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!bobber || !this->m_unit->m_unitChannelSpell || static_cast<CGGameObject_C*>(bobber)->m_gameObject->m_typeID != 17) {
        return;
    }

    for (CM2Model* attached = this->m_worldModel->m_attachList; attached; attached = attached->m_attachNext) {
        if (attached->m_attachmentId == 1) {
            if (!this->m_fishingLineEffect) {
                void* memory = SMemAlloc(sizeof(CEffect), __FILE__, __LINE__, 0);
                this->m_fishingLineEffect = memory ? new (memory) CEffect() : nullptr;
                WOWGUID target = this->m_unit->m_unitFieldChannelObject;
                this->m_fishingLineEffect->AddFishingLine(this, target);
            }

            return;
        }
    }
}

// VERIFY: ef43f122 manual
// OFFSET: 0x00724200
int32_t CGUnit_C::ChooseFallAnim(uint32_t* anim) const {
    int32_t falling = this->m_movementData.IsFalling();

    if (!falling) {
        return falling;
    }

    uint32_t current = (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) ? this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF) : 0xFFFFFFFF;
    *anim = IsAirborneAnim(current) ? ANIM_COUNT : ANIM_FALL;
    return 1;
}

// VERIFY: e5ee87a2 manual
// OFFSET: 0x00724F50
void CGUnit_C::SaveChainSpellTargets(const TSGrowableArray<WOWGUID>& targets) {
    this->m_chainSpellTargets.m_count = 0;
    uint32_t i = targets.m_count;

    while (i) {
        i--;
        const WOWGUID& target = targets.m_data[i];

        if (target != this->m_obj->m_guid) {
            this->m_chainSpellTargets.New(target);
        }
    }
}

// VERIFY: 5984d3a5 manual
// OFFSET: 0x007282A0
bool CGUnit_C::HasAuraBySpellId(int32_t spellId) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_auras[i].m_spellId == spellId) {
            return true;
        }
    }

    return false;
}

// VERIFY: c93008c2 manual
// OFFSET: 0x00728300
bool CGUnit_C::HasAuraActive(int32_t spellId) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        AuraData* aura = this->GetAura(i);

        if (aura->m_spellId == spellId) {
            for (uint32_t effect = 0; effect < 3; effect++) {
                if ((1 << effect) & aura->m_flags) {
                    return true;
                }
            }
        }
    }

    return false;
}

// VERIFY: b1ef5b63 manual
// OFFSET: 0x007289C0
int32_t CGUnit_C::GetAuraIndex(int32_t spellId) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_auras[i].m_spellId == spellId) {
            return i;
        }
    }

    return -1;
}

// VERIFY: f3ee2cc8 manual
// OFFSET: 0x007226B0
bool CGUnit_C::IsPetBarUsed() const {
    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags & 0x2000)) {
        return false;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->GetTransportGUID() != this->m_obj->m_guid.m_guid || !this->m_vehicle) {
        return true;
    }

    VehicleSeatRec* seat = this->m_vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);
    return !seat || seat->m_vehicleAbilityDisplay != 0;
}

// VERIFY: 5d3d4eef auto
// OFFSET: 0x006E6F40
float CGUnit_C::GetFacing() const {
    return this->m_movement->GetFacing(this->m_movement->m_facing);
}

// VERIFY: 003a9d67 auto
// OFFSET: 0x007156A0
float CGUnit_C::GetRenderFacing() const {
    return this->m_movementData.GetFacing(this->m_renderFacing);
}

// VERIFY: cd088da4 auto
// OFFSET: 0x00715810
void CGUnit_C::ResetSmoothFacing() {
    this->unk_08D4 = 0.0f;
    this->unk_0A34 &= ~0x7u;

    float facing = this->m_movementData.GetFacing(this->m_renderFacing);
    this->m_smoothFacing = facing;
    this->m_smoothFacingTarget = facing;
    this->unk_08D8 = 0.0f;
}

// VERIFY: 0ce666a6 manual
// OFFSET: 0x00717E50
float CGUnit_C::GetSmoothFacing() const {
    float facing = 0.0f;
    const CGUnit_C* unit = this;

    while (unit) {
        facing += unit->m_targetFacing;

        if (!unit->HasVehicleTransport()) {
            break;
        }

        unit = static_cast<const CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ unit->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return unit->m_movement->GetFacing(facing);
}

// VERIFY: 7a726f02 manual
// OFFSET: 0x0071E340
void CGUnit_C::ChooseDefaultAnim(uint32_t& anim, int32_t force) const {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state == 3 && passenger->GetLowerAnim(passenger->m_seatRec) != 506) {
        return;
    }

    if (this->m_animationState & 0x800004) {
        return;
    }

    uint32_t torso = this->GetCurrentTorsoAnimId();

    if (!force && GetAnimBehaviorId(torso) == 464) {
        anim = torso;
        return;
    }

    if (this->m_movementData.m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        anim = ANIM_SWIM_IDLE;
        return;
    }

    if (this->m_unit->m_unitFieldBytes1[2] & 0x2) {
        anim = ANIM_STEALTH_STAND;
        return;
    }

    anim = this->m_movement->IsSplineFlyerOrHovering() ? ANIM_HOVER : ANIM_STAND;
}

// VERIFY: 3787d22a auto
// OFFSET: 0x00720330
void CGUnit_C::UpdateSelectionRadius() {
    CAaBox box = this->GetStandBox();
    float dx = box.t.x - box.b.x;
    float dy = box.t.y - box.b.y;

    if (fabs(dx) < 0.00000023841858f && fabs(dy) < 0.00000023841858f) {
        this->m_selectionRadius = 1.2f;
        return;
    }

    this->m_selectionRadius = sqrt(dx * dx + dy * dy) * 0.5f;
    float scaled = this->GetTrueScale() * this->m_selectionRadius;
    this->m_selectionRadius = sqrt(scaled);

    if (scaled > 5.0f) {
        this->m_selectionRadius = this->m_selectionRadius + (scaled - 5.0f) * (scaled - 5.0f) * 0.059999999f;
    }

    if (this->m_selectionRadius > 10.0f) {
        this->m_selectionRadius = 10.0f;
    }
}

// VERIFY: 9da62ce8 auto
// OFFSET: 0x0074B810
int32_t CGUnit_C::AddPassenger(CPassenger* passenger, int32_t a2) {
    if (!this->m_vehicle) {
        return 0;
    }

    return this->m_vehicle->AddPassenger(passenger);
}

// VERIFY: 9082d6a5 auto
// OFFSET: 0x006FC290
void CGUnit_C::OnRemoteMissileTrajectoryCollision(MissileCollisionData* data) {
    for (CMissile* missile = this->m_missiles; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == data->m_casterGuid && missile->m_castID == data->m_castID) {
            missile->SetRemoteCollisionPos(data->m_position);
        }
    }
}

// VERIFY: 97ef0b7c auto
// OFFSET: 0x0071F540
int32_t CGUnit_C::GetVirtualItemDisplayRec(uint32_t slot, ItemDisplayInfoRec& rec) const {
    return g_itemDisplayInfoDB.GetRecord(this->m_virtualItemDisplayId[slot], &rec);
}

// VERIFY: 068ad525 manual
// OFFSET: 0x00718C20
int32_t CGUnit_C::UnitReaction(const CGCorpse_C* corpse) const {
    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);
    FactionTemplateRec* other = g_factionTemplateDB.GetRecord(corpse->GetFactionTemplate());

    if (faction && other) {
        return FactionTemplateReaction(faction, other);
    }

    return 3;
}

// VERIFY: d74ba9e7 auto
// OFFSET: 0x00716490
int32_t CGUnit_C::IsDeadOrGhost() const {
    return this->m_unit->m_unitFieldHealth <= 0;
}

// VERIFY: 32c13bf7 auto
// OFFSET: 0x0071A3F0
CreatureSoundDataRec* CGUnit_C::GetSoundData() const {
    if (this->m_soundDataOverride) {
        return this->m_soundDataOverride;
    }

    CreatureSoundDataRec* soundData = this->m_soundData;

    if (soundData && this->m_unit->m_unitFieldPetnumber && soundData->m_creatureSoundDataIdpet > 0) {
        return g_creatureSoundDataDB.GetRecord(soundData->m_creatureSoundDataIdpet);
    }

    return soundData;
}

// VERIFY: 0a47e594 manual
// OFFSET: 0x00720860
void CGUnit_C::PlayNPCSound(bool greeting) {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return;
    }

    CGUnit_C::s_lastNPCSoundGUID = WOWGUID();

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->IsDeadOrGhost() || SI2::IsPlaying(&this->m_npcInteractSound)) {
        return;
    }

    if (this->m_unit->m_unitFieldHealth <= 0 || (this->m_unit->m_unitDynamicFlags & 0x20) || !this->m_displayInfo) {
        return;
    }

    NPCSoundsRec* sounds = g_npcSoundsDB.GetRecord(this->m_displayInfo->m_npcsoundID);

    if (!sounds) {
        return;
    }

    C3Vector position = this->GetPosition();
    SI2::PlaySoundKit(greeting ? sounds->m_soundID[0] : sounds->m_soundID[1], &position, &this->m_npcInteractSound, nullptr, 0, nullptr, 1, 0);
}

// VERIFY: a3ea8ac7 manual
// OFFSET: 0x00721DA0
bool CGUnit_C::CanCurrentFormMount() const {
    if (this->IsShapeShifted()) {
        return false;
    }

    if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        return true;
    }

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldDisplayid);
    CreatureDisplayInfoExtraRec* extra = displayInfo ? g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID) : nullptr;

    if (!extra) {
        return false;
    }

    if (displayInfo) {
        CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

        if (modelData && (modelData->m_flags & 0x80)) {
            return true;
        }
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(extra->m_displayRaceID);
    return race && (race->m_flags & 0x4);
}

// VERIFY: f5fb6fd8 manual
// OFFSET: 0x0071D1F0
SummonPropertiesRec* CGUnit_C::GetSummonProperties() const {
    SpellRec spell;

    if (g_spellDB.GetRecord(this->m_unit->m_unitCreatedBySpell, &spell)) {
        for (int32_t i = 0; i < 3; i++) {
            if (spell.m_effect[i] == 28) {
                return g_summonPropertiesDB.GetRecord(spell.m_effectMiscValueB[i]);
            }
        }
    }

    return nullptr;
}

// VERIFY: cbaf6b39 auto
// OFFSET: 0x00722640
bool CGUnit_C::CanTurnWhileCasting() const {
    if (this->m_castSpellId) {
        SpellRec spell;

        if (g_spellDB.GetRecord(this->m_castSpellId, &spell) && (spell.m_attributesExE & 0x80000)) {
            return false;
        }
    }

    return true;
}

// VERIFY: 5ae50009 manual
// OFFSET: 0x007206A0
void CGUnit_C::UpdateAmmoModel(uint32_t displayId, uint32_t invType) {
    if (this->m_ammoDisplayId == displayId && this->m_ammoModel) {
        return;
    }

    int32_t noAttach = 0;

    if (this->m_ammoModel) {
        if (this->m_ammoModel->m_attachParent) {
            this->m_ammoModel->DetachFromParent();
        }

        this->m_ammoModel->Release();
        this->m_ammoModel = nullptr;
    }

    this->m_ammoDisplayId = displayId;

    if (!displayId) {
        return;
    }

    uint32_t ammoType = 24;
    const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, false);

    if (invType == 30) {
        if (ranged) {
            ammoType = (ranged->m_inventoryType == 25) + 24;
        }
    } else {
        ammoType = invType;
    }

    if (ranged && ranged->m_classID == 2 && (ranged->m_subclassID == 3 || ranged->m_subclassID == 18)) {
        noAttach = 1;
    }

    ItemDisplayInfoRec rec;

    if (g_itemDisplayInfoDB.GetRecord(this->m_ammoDisplayId, &rec)) {
        this->m_ammoModel = CCharacterComponent::MakeAmmoModel(this->m_worldModel, &rec, ammoType);
    }

    if (this->m_ammoModel && !noAttach && (this->m_animationState & 0x4000) && ammoType == 24) {
        this->m_ammoModel->AttachToParent(this->m_worldModel, 0x23, nullptr, 0);
    }
}

// VERIFY: 2abd4122 auto
// OFFSET: 0x006E6EF0
C3Vector CGUnit_C::GetPosition() const {
    return this->m_movement->GetPosition(this->m_movement->m_position);
}

// VERIFY: 66196daa manual
// OFFSET: 0x00718860
void CGUnit_C::OnFlightLocal(uint32_t eventTime, int32_t flying) {
    this->m_movementData.OnFlightLocal(eventTime, flying);

    if (flying) {
        CGUnit_C::s_flightStatusTime = eventTime;
    }
}

// VERIFY: a7d1e04b auto
// OFFSET: 0x0071AE50
void CGUnit_C::ToggleRunModeLocal(uint32_t eventTime) {
    this->m_movementData.OnSetRunModeLocal(eventTime, this->m_movementData.m_flags & MOVEMENTFLAG_WALKING);
}

// VERIFY: cf000702 manual
// OFFSET: 0x0071F890
bool CGUnit_C::CanAssist(const CGCorpse_C* corpse) const {
    return this->UnitReaction(corpse) >= 4;
}

// VERIFY: 034a2c24 auto
// OFFSET: 0x007467F0
void CGUnit_C::CheckLoopSound() {
    CreatureSoundDataRec* soundData = this->GetSoundData();

    if (this->m_unit->m_unitFieldHealth > 0 && !(this->m_unit->m_unitFieldFlags2 & 0x1) && soundData && soundData->m_loopSoundID && !SI2::IsPlaying(&this->m_loopSound)) {
        C3Vector position = this->GetPosition();

        if (SI2::IsPlaying(&this->m_loopSound) || SI2::IsTooCloseToExistingSoundKit(soundData->m_loopSoundID, &position)) {
            return;
        }

        SE2SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_loopMode = 1;
        properties.m_maxAudibleBehavior = 0;
        properties.m_streaming = true;
        SI2::PlaySoundKit(soundData->m_loopSoundID, &position, &this->m_loopSound, &properties, 0, nullptr, 1, 0);
        SI2::AttachToObject(&this->m_loopSound, this->m_obj->m_guid);
        return;
    }

    SI2::StopSound(&this->m_loopSound, 0, 0.5f, 1);
}

// VERIFY: 13e3e5f1 auto
// OFFSET: 0x00746BD0
C3Vector CGUnit_C::GetAttachmentPosition(uint32_t attachId, const C3Vector* offset) {
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded(0, 0) && model->HasAttachment(attachId)) {
        if (!offset || (offset->x == 0.0f && offset->y == 0.0f && offset->z == 0.0f)) {
            return model->GetAttachmentPosition(attachId);
        }

        return *offset * model->GetAttachmentWorldTransform(attachId);
    }

    C3Vector position(0.0f, 0.0f, 0.0f);

    if (offset) {
        position = *offset;
    }

    if (attachId <= 49) {
        position.z = s_attachmentHeights[attachId] + position.z;
    }

    if (!offset || (offset->x == 0.0f && offset->y == 0.0f && offset->z == 0.0f)) {
        C3Vector base = this->m_vehiclePassenger ? this->GetModelPosition() : this->GetPosition();
        position.x = base.x + position.x;
        position.y = base.y + position.y;
        position.z = base.z + position.z;
    } else {
        position *= this->m_vehiclePassenger ? this->GetModelMatrix() : this->GetMatrix();
    }

    return position;
}

// VERIFY: 2fefd292 manual
// OFFSET: 0x00727E70
void CGUnit_C::UpdateAuraEffectMask() {
    memset(this->m_auraEffectMask, 0, sizeof(this->m_auraEffectMask));

    SpellRec spell;

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = this->GetAura(i);

        if (!g_spellDB.GetRecord(aura->m_spellId, &spell)) {
            continue;
        }

        for (int32_t effect = 0; effect < 3; effect++) {
            if (aura->m_flags & (1 << effect)) {
                this->m_auraEffectMask[spell.m_effectAura[effect] / 8] |= 1 << (spell.m_effectAura[effect] % 8);
            }
        }
    }
}

// VERIFY: 3c460348 manual
// OFFSET: 0x00716E50
void CGUnit_C::ApplyGuildTabard(int32_t guildId, const WOWGUID& guid, int32_t a3, bool apply) {
    if (!guildId || !apply) {
        return;
    }

    WOWGUID cacheGuid = WOWGUID();

    if (!g_guildInfoCache.GetRecord(guildId, cacheGuid, nullptr, nullptr, false)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(guid, guildId, nullptr, nullptr, emblemStyle, emblemColor, borderStyle, borderColor, background) && unit->m_characterComponent) {
        unit->m_characterComponent->ApplyGuildColor(emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// VERIFY: 97aaae3a manual
// OFFSET: 0x0071E5B0
void CGUnit_C::UpdateObjectEffectAnimationStates() {
    if (!this->m_effectManager) {
        return;
    }

    M2BoneSequenceInfo sequences[3];
    this->GetCurrentAnimationInfo(&sequences[0], &sequences[1], &sequences[2], 1);

    if (this->m_mountModel) {
        if (!this->m_mountModel->IsLoaded(0, 0)) {
            return;
        }

        if (sequences[0].m_sequenceId < 506) {
            sequences[0].m_sequenceId = this->m_mountModel->ResolveSequenceFallback(sequences[0].m_sequenceId);
        }
    }

    if (this->m_worldModel) {
        if (!this->m_worldModel->IsLoaded(0, 0)) {
            return;
        }

        if (sequences[1].m_sequenceId < 506) {
            sequences[1].m_sequenceId = this->m_worldModel->ResolveSequenceFallback(sequences[1].m_sequenceId);
        }

        if (sequences[2].m_sequenceId < 506) {
            sequences[2].m_sequenceId = this->m_worldModel->ResolveSequenceFallback(sequences[2].m_sequenceId);
        }
    }

    struct {
        uint32_t m_state;
        int32_t m_slot;
    } states[3];

    int32_t count = 0;

    for (int32_t slot = 0; slot < 3; slot++) {
        if (sequences[slot].m_sequenceId != 0xFFFFFFFF) {
            states[count].m_state = FindObjectEffectStateFromAnimID(sequences[slot].m_sequenceId, sequences[slot].m_variationIndex);
            states[count].m_slot = slot;
            count++;
        }
    }

    uint32_t previous[3];
    previous[0] = this->m_animEffectStates[0];
    previous[1] = this->m_animEffectStates[1];
    previous[2] = this->m_animEffectStates[2];

    for (int32_t i = 0; i < 3; i++) {
        if (this->m_animEffectStates[i]) {
            int32_t j;

            for (j = 0; j < count; j++) {
                if (this->m_animEffectStates[i] == states[j].m_state) {
                    break;
                }
            }

            if (j == count) {
                this->m_animEffectStates[i] = 0;
            }
        }
    }

    for (int32_t i = 0; i < count; i++) {
        uint32_t state = states[i].m_state;
        int32_t j;

        for (j = 0; j < 3; j++) {
            if (this->m_animEffectStates[j] == state) {
                break;
            }
        }

        if (j != 3) {
            continue;
        }

        if (state) {
            this->m_effectManager->EnterState(state, 1, 0);

            if (state == 15 || state == 81) {
                this->m_effectManager->ExitState(37, 1);
                this->m_effectManager->ExitState(38, 1);
                this->m_effectManager->ExitState(6, 1);
                this->m_effectManager->ExitState(7, 1);
                this->m_effectManager->ExitState(47, 1);
                this->m_effectManager->ExitState(48, 1);
                this->m_effectManager->ExitState(49, 1);
                this->m_effectManager->ExitState(50, 1);
                this->m_effectManager->ExitState(51, 1);
                this->m_effectManager->ExitState(52, 1);
                this->m_effectManager->ExitState(53, 1);
                this->m_effectManager->ExitState(54, 1);
                this->m_effectManager->ExitState(55, 1);
                this->m_effectManager->ExitState(56, 1);
                this->m_effectManager->ExitState(57, 1);
                this->m_effectManager->ExitState(58, 1);
                this->m_effectManager->ExitState(59, 1);
                this->m_effectManager->ExitState(60, 1);
                this->m_effectManager->ExitState(61, 1);
                this->m_effectManager->ExitState(62, 1);
                this->m_effectManager->ExitState(63, 1);
                this->m_effectManager->ExitState(64, 1);
                this->m_effectManager->ExitState(65, 1);
                this->m_effectManager->ExitState(66, 1);
            }
        }

        this->m_animEffectStates[states[i].m_slot] = state;
    }

    for (int32_t i = 0; i < 3; i++) {
        if (previous[i]) {
            int32_t j;

            for (j = 0; j < count; j++) {
                if (previous[i] == states[j].m_state) {
                    break;
                }
            }

            if (j == count) {
                this->m_effectManager->ExitState(previous[i], 1);
            }
        }
    }
}

// VERIFY: 91123c63 manual
// OFFSET: 0x005A1120
bool CGUnit_C::HasAuraEffect(int32_t auraType) const {
    return (this->m_auraEffectMask[auraType / 8] & (1 << (auraType % 8))) != 0;
}

// VERIFY: 6e8940a0 auto
// OFFSET: 0x00715D00
int32_t __stdcall CGUnit_C::Unk715D00(int32_t hand, const CGUnitVirtualItem* mainHand, const CGUnitVirtualItem* offHand) {
    if (hand == 1) {
        if (!mainHand && (!offHand || offHand->m_inventoryType == 23)) {
            return 0;
        }
    } else if (hand == 0) {
        if ((offHand && offHand->m_inventoryType == 14) || (mainHand && mainHand->m_classID == 2) || (offHand && offHand->m_classID == 2)) {
            return 1;
        }
    }

    return hand;
}

// VERIFY: 3fcb3ed9 auto
// OFFSET: 0x00716040
void CGUnit_C::Unk716040() {
    this->m_clientFlags |= 0x100000;
}

// VERIFY: 99f1723a manual
// OFFSET: 0x00717870
C3Spline_CatmullRom* CGUnit_C::GetSpline() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? &spline->m_spline : nullptr;
}

// VERIFY: 1e31eb25 auto
// OFFSET: 0x00717890
float CGUnit_C::GetSplineElapsedTime() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? static_cast<float>(static_cast<int32_t>(spline->m_start)) : 0.0f;
}

// VERIFY: 80a0af13 manual
// OFFSET: 0x007178B0
float CGUnit_C::GetSplineDuration() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? static_cast<float>(spline->m_duration) : 0.0f;
}

// VERIFY: ace397fc auto
// OFFSET: 0x007199E0
bool __stdcall CGUnit_C::IsAreaInAreaGroup(int32_t areaId, int32_t areaGroupId, int32_t* checked) {
    if (checked) {
        *checked = 0;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (!area) {
        return false;
    }

    while (areaGroupId) {
        AreaGroupRec* group = g_areaGroupDB.GetRecord(areaGroupId);

        if (!group) {
            return false;
        }

        for (uint32_t i = 0; i < 6; i++) {
            if (!group->m_areaID[i]) {
                break;
            }

            if (checked) {
                (*checked)++;
            }

            if (area->m_id == group->m_areaID[i] || area->m_parentAreaID == group->m_areaID[i]) {
                return true;
            }
        }

        areaGroupId = group->m_nextAreaID;
    }

    return false;
}

// VERIFY: b35207d3 manual
// OFFSET: 0x0071A7F0
int32_t CGUnit_C::HasAttachmentForEffect(uint32_t id, int32_t raw) const {
    CM2Model* model = this->m_worldModel;

    if (!model || !model->IsLoaded(0, 0)) {
        return 0;
    }

    if (raw) {
        return model->HasAttachment(id);
    }

    if (id == 13) {
        return model->HasAttachment(s_effectAttachmentIds[4]);
    }

    return model->HasAttachment(s_effectAttachmentIds[id]);
}

// VERIFY: 88f55858 manual
// OFFSET: 0x0071A860
C3Vector CGUnit_C::GetAttachmentPositionForEffect(uint32_t id, const C3Vector& offset, int32_t raw) {
    if (raw) {
        return this->GetAttachmentPosition(id, &offset);
    }

    if (id != 13) {
        return this->GetAttachmentPosition(s_effectAttachmentIds[id], &offset);
    }

    for (CM2Model* model = this->m_worldModel->m_attachList; model; model = model->m_attachNext) {
        if (model->m_attachmentId == 1) {
            for (int32_t attach = 4; attach >= 0; attach--) {
                if (model->HasAttachment(attach)) {
                    return offset * model->GetAttachmentWorldTransform(attach);
                }
            }

            break;
        }
    }

    return this->GetAttachmentPosition(s_effectAttachmentIds[4], &offset);
}

// VERIFY: f8803a2c manual
// OFFSET: 0x0071A9A0
bool CGUnit_C::Unk71A9A0(uint32_t time, CImVector& color) {
    if (!this->m_flashStartTime) {
        return false;
    }

    uint32_t start = this->m_flashStartTime + this->m_flashDelay;
    uint32_t elapsed = time - start;

    if (static_cast<int32_t>(elapsed) < 0) {
        color = this->m_flashColor;
        return true;
    }

    uint32_t duration = this->m_flashDuration;

    if (static_cast<int32_t>(time - duration - start) >= 0) {
        this->m_flashStartTime = 0;
        return false;
    }

    int32_t t = static_cast<int32_t>((1.0f - static_cast<float>(elapsed) / static_cast<float>(duration)) * 255.0f);
    CImVector flash = this->m_flashColor;

    if (t) {
        color.InterpColor(t, &flash);
    }

    return true;
}

// VERIFY: 26728f29 auto
// OFFSET: 0x0071AE80
int32_t CGUnit_C::Unk71AE80() const {
    if (fabs(this->GetRawFacing() - this->m_lastSentFacing) < 0.1f) {
        if (!(this->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && !(this->m_movement->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
            return 1;
        }

        if (fabs(this->GetPitch() - this->m_lastSentPitch) < 0.1f) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 3cad216e auto
// OFFSET: 0x0071B580
float __fastcall CGUnit_C::ComputeTrackingElevationAngle(const C3Vector& position, const C3Vector& target) {
    float dx = position.x - target.x;
    float dy = position.y - target.y;
    float dz = position.z - target.z;
    float scale = 1.0f / sqrt(dz * dz + dy * dy + dx * dx);
    dx *= scale;
    dy *= scale;
    dz *= scale;

    if (fabs(dx) < 0.001f && fabs(dy) < 0.001f) {
        return target.z >= position.z ? -1.5707964f : 1.5707964f;
    }

    return asin(dz);
}

// VERIFY: cb7d9a59 manual
// OFFSET: 0x007222A0
bool CGUnit_C::ShouldKneelForLoot() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return !(this->m_unit->m_unitFieldFlags & 0x10000000);
    }

    const WOWGUID& lootTarget = static_cast<const CGPlayer_C*>(this)->m_lootTarget;
    CGObject_C* target = ClntObjMgrObjectPtr(lootTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target) {
        return false;
    }

    if ((target->m_obj->m_type & TYPEMASK_GAMEOBJECT) && static_cast<CGGameObject_C*>(target)->m_gameObject->m_typeID == 17) {
        return false;
    }

    if ((target->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(target)->m_unit->m_unitFieldHealth > 0) {
        return false;
    }

    return !(target->m_obj->m_type & TYPEMASK_ITEM);
}

// VERIFY: dcad4d49 auto
// OFFSET: 0x0074B9B0
void CGUnit_C::SetVehicleExitVoluntary(int32_t enable) {
    this->m_movementData.SetVehicleExitVoluntary(enable);
}

// VERIFY: 5d120117 auto
// OFFSET: 0x0074BB20
void CGUnit_C::OnDismissControlledVehicleLocal(uint32_t eventTime) {
    this->m_movementData.OnDismissControlledVehicleLocal(eventTime);
}

// VERIFY: 8a7ea592 manual
// OFFSET: 0x0071A260
void CGUnit_C::SetEmoteQueue(const QUESTGIVEREMOTENODE* nodes, uint32_t count) {
    this->m_emoteQueue[0].m_emote = -1;

    if (!count) {
        return;
    }

    uint32_t queued = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (nodes[i].m_emote) {
            EmotesRec* emote = g_emotesDB.GetRecord(nodes[i].m_emote);

            if (emote && !emote->m_emoteSpecProc) {
                this->m_emoteQueue[queued].m_delay = nodes[i].m_delay;
                this->m_emoteQueue[queued].m_emote = nodes[i].m_emote;
                queued++;
            }
        }

        EmotesRec* current = g_emotesDB.GetRecord(this->m_emoteQueue[i].m_emote);

        if (current && current->m_animID) {
            this->m_animationState &= ~0x8000u;
            this->unk_0B58 = this->m_sheatheState;
        }
    }

    if (queued < 4) {
        this->m_emoteQueue[queued].m_emote = 0;
    }

    this->m_emoteQueue[0].m_delay += OsGetAsyncTimeMs();
}

// VERIFY: f3ca94ed auto
// OFFSET: 0x00728050
DelayedSpellVisualKit* CGUnit_C::DelaySpellVisualKit(int32_t spellId, int32_t kitId, int32_t kitType, const C3Vector* position, WOWGUID targetGuid, int32_t a7, int32_t a8, int32_t delay, int32_t a10, int32_t a11) {
    uint32_t index;

    for (index = 0; index < this->m_delayedSpellVisualKits.m_count; index++) {
        if (!(this->m_delayedSpellVisualKits.m_data[index].m_flags & 0x8)) {
            break;
        }
    }

    if (index == this->m_delayedSpellVisualKits.m_count) {
        this->m_delayedSpellVisualKits.SetCount(this->m_delayedSpellVisualKits.m_count + 1);
    }

    DelayedSpellVisualKit* kit = &this->m_delayedSpellVisualKits.m_data[index];
    kit->m_spellId = spellId;
    kit->m_kitType = kitType;
    kit->m_kitId = kitId;

    if (position) {
        kit->m_position = *position;
        kit->m_flags |= 0x1;
        kit->m_targetGuid = targetGuid;
    } else {
        kit->m_position.x = 0.0f;
        kit->m_position.y = 0.0f;
        kit->m_position.z = 0.0f;
        kit->m_flags &= ~0x1u;
        kit->m_targetGuid = WOWGUID();
    }

    kit->m_delay = delay;
    kit->m_flags = (kit->m_flags & ~0x6u) | (((a7 != 0) | (((a8 != 0) | 2) << 1)) << 1);
    kit->unk_0028 = a10;
    kit->unk_002C = a11;
    return kit;
}

// VERIFY: 6d9103d1 auto
// OFFSET: 0x00716470
C3Vector CGUnit_C::GetGroundNormal() const {
    return this->m_movement->GetGroundNormal();
}

// VERIFY: 6dfad482 manual
// OFFSET: 0x0071BC50
bool CGUnit_C::Unk71BC50(CGObject_C* target) const {
    C3Vector position = this->GetPosition();
    C3Vector targetPosition = target->GetPosition();
    C3Vector delta(targetPosition.x - position.x, targetPosition.y - position.y, targetPosition.z - position.z);
    C3Vector direction = this->m_movement->GetDirection();
    return direction.x * delta.x + direction.y * delta.y + direction.z * delta.z >= 0.0f;
}

// VERIFY: fbe09df2 auto
// OFFSET: 0x00724280
int32_t CGUnit_C::ChooseLootAnim(uint32_t flags, uint32_t* animId) {
    if (this->m_mountModel || !(this->m_animationState & 0x40) || !this->ShouldKneelForLoot()) {
        return (flags & 0xFFFFFFF0) == 0;
    }

    uint32_t torso = this->GetCurrentTorsoAnimId();

    if (!this->IsLooting()) {
        if (torso != 188) {
            return (flags & 0xFFFFFFF0) == 0;
        }

        if (flags & 0x8) {
            *animId = 189;
        }

        return 1;
    }

    if (!(flags & 0x8) || torso == 188 || torso == 50) {
        return 1;
    }

    *animId = 50;
    return 1;
}

// VERIFY: 26b35972 auto
// OFFSET: 0x0074BB30
void CGUnit_C::OnChangeSeatsOnControlledVehicleLocal(uint32_t eventTime, WOWGUID transport, uint8_t seat) {
    this->m_movementData.OnChangeSeatsOnControlledVehicleLocal(eventTime, transport, seat);
}

// VERIFY: ef8795db manual
// OFFSET: 0x0071BCE0
int32_t CGUnit_C::IsInFrontOfTargetWithGuid(const WOWGUID& guid) const {
    CGObject_C* target = ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);
    return target && this->Unk71BC50(target);
}

// VERIFY: c4de07e2 manual
// OFFSET: 0x007197D0
void CGUnit_C::UpdateWorldTransform(float elapsed) {
    C3Vector normal = this->m_movement->GetGroundNormal();

    if (normal.z >= 0.35721236f) {
        C3Vector delta(this->m_groundNormal.x - normal.x, this->m_groundNormal.y - normal.y, this->m_groundNormal.z - normal.z);
        float decay = pow(0.0018f, elapsed);
        this->m_groundNormal.x = delta.x * decay + normal.x;
        this->m_groundNormal.y = delta.y * decay + normal.y;
        this->m_groundNormal.z = delta.z * decay + normal.z;
    }

    CM2Model* model = this->GetObjectModel();

    if (model) {
        float scale = this->GetTrueScale();
        float facing = this->GetRenderFacing();
        model->SetWorldTransform(this->GetPosition(), facing, scale, this->m_groundNormal, 0xFFFFFFFF);
    }
}

// VERIFY: 315f8bea manual
// OFFSET: 0x00735CC0
void CGUnit_C::SetKeyBoneSequenceSpeed(CM2Model* model, uint32_t boneId, float speed, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation()) {
        return;
    }

    model->SetBoneSequenceSpeed(boneId, speed);

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
            unit->SetKeyBoneSequenceSpeed(unit->GetObjectModel(), boneId, speed, 1);
        }
    }
}

// VERIFY: 4cfce839 auto
// OFFSET: 0x007178E0
void CGUnit_C::ReleaseEffectsBySpell(int32_t spellId) {
    CEffect* effect = this->m_procEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_spellId == spellId) {
            effect->Release();
        }

        effect = next;
    }
}

// VERIFY: e1b2b150 manual
// OFFSET: 0x00735820
void CGUnit_C::SetKeyBoneSequence(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, float speed, int32_t blend, int32_t primary, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation()) {
        return;
    }

    if (this->m_animationState & 0x8000000) {
        blend = 0;
    }

    model->SetBoneSequence(boneId, sequenceId, variation, time, speed, blend, primary);

    M2BoneSequenceInfo info;

    if (model->IsLoaded(0, 0) && model->HasBone(boneId) && (model->GetBoneSequenceInfo(boneId, info), info.m_sequenceId == sequenceId) && time && fabs(speed) > 0.001f && info.m_finished) {
        if (boneId == 0xFFFFFFFF || boneId == 26) {
            model->SetBoneSequence(boneId, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
        } else {
            model->UnsetBoneSequence(boneId, blend, primary);
        }

        return;
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    uint32_t mirrorVariation;

    if (model->IsLoaded(0, 0) && model->HasBone(boneId)) {
        model->GetBoneSequenceInfo(boneId, info);
        mirrorVariation = info.m_variationIndex;
    } else {
        mirrorVariation = 0;
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
            CM2Model* unitModel = unit->GetObjectModel();

            if (unitModel) {
                uint32_t unitVariation = unitModel->IsLoaded(0, 0) && mirrorVariation < unitModel->GetSequenceVariationCount(sequenceId) ? mirrorVariation : 0xFFFFFFFF;
                unit->SetKeyBoneSequence(unitModel, boneId, sequenceId, unitVariation, time, speed, blend, primary, 1);
            }
        }
    }
}

// VERIFY: a814c576 manual
// OFFSET: 0x00735A60
int32_t CGUnit_C::UnsetKeyBoneSequence(CM2Model* model, uint32_t boneId, int32_t blend, int32_t primary, int32_t force) {
    if (!model) {
        return 0;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation()) {
        return 0;
    }

    int32_t result;

    if (model->IsLoaded(0, 0) && model->BoneHasParent(boneId)) {
        result = 1;
        model->UnsetBoneSequence(boneId, blend, primary);
    } else {
        result = 0;
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers() && model == this->GetObjectModel()) {
        for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
                unit->UnsetKeyBoneSequence(unit->GetObjectModel(), boneId, blend, primary, 1);
            }
        }
    }

    return result;
}

// VERIFY: 3f921c2a manual
// OFFSET: 0x00735BB0
void CGUnit_C::SetBoneSequenceTimeRecursive(CM2Model* model, uint32_t boneId, uint32_t time, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation()) {
        return;
    }

    model->SetBoneSequenceTime(boneId, time);

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
            unit->SetBoneSequenceTimeRecursive(unit->GetObjectModel(), boneId, time, 1);
        }
    }
}

// VERIFY: 2a100620 manual
// OFFSET: 0x00735DD0
void CGUnit_C::UpdateBoneSequenceFlags(CM2Model* model, int32_t freeze, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation()) {
        return;
    }

    if (freeze) {
        uint32_t time = model->m_scene->m_time;
        model->m_lastAnimTime = time ? time : 1;
    } else {
        model->m_lastAnimTime = 0;
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
            unit->UpdateBoneSequenceFlags(unit->GetObjectModel(), freeze, 1);
        }
    }
}

// VERIFY: 91f8c2f1 manual
// OFFSET: 0x00738440
void CGUnit_C::Unk738440() {
    CM2Model* model = this->m_worldModel;

    if (model) {
        if (this->m_animationState & 0x80) {
            uint32_t torsoBone = this->m_torsoKeyBone;

            if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->OverridesModelAnimation()) {
                if (model->IsLoaded(0, 0) && model->BoneHasParent(torsoBone)) {
                    model->UnsetBoneSequence(torsoBone, 1, 1);
                }

                CVehicle_C* vehicle = this->m_vehicle;

                if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers() && model == this->GetObjectModel()) {
                    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
                        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->OverridesModelAnimation()) {
                            unit->UnsetKeyBoneSequence(unit->GetObjectModel(), torsoBone, 1, 1, 1);
                        }
                    }
                }
            }
        }

        this->SetKeyBoneSequence(model, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0);
    }

    if (this->m_mountModel) {
        this->SetKeyBoneSequence(this->m_mountModel, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0);
    }
}

// VERIFY: 4d921ef3 auto
// OFFSET: 0x00653A10
void CGUnit_C::GetAFKText(char* buffer, uint32_t size) const {
}

// VERIFY: 4f552ebd auto
// OFFSET: 0x00653A10
void CGUnit_C::GetDNDText(char* buffer, uint32_t size) const {
}

// VERIFY: 98c5ab58 auto
// OFFSET: 0x00653A10
void CGUnit_C::GetGMText(char* buffer, uint32_t size) const {
}

// VERIFY: 391de47a auto
// OFFSET: 0x00653A10
void CGUnit_C::GetDevText(char* buffer, uint32_t size) const {
}

// VERIFY: dbd6dbc3 auto
// OFFSET: 0x008E5250
const CGUnitVirtualItem* CGUnit_C::GetDefendingItem() const {
    return nullptr;
}

// VERIFY: a623d951 auto
// OFFSET: 0x007161C0
UnitData0B98::UnitData0B98() {
    for (int32_t i = 0; i < 21; i++) {
        this->unk_0000[i] = 0;
    }

    for (int32_t i = 0; i < 6; i++) {
        this->unk_0054[i] = 0;
    }

    for (int32_t i = 0; i < 8; i++) {
        this->unk_006C[i] = 0.0f;
    }

    for (int32_t i = 0; i < 5; i++) {
        this->unk_008C[i] = 0;
    }
}

// VERIFY: 18e39639 manual
// OFFSET: 0x0073F660
CGUnit_C::CGUnit_C(uint32_t time, const CClientObjCreate& create)
    : CGObject_C(time, create),
      unk_0784(0.0f),
      m_movementData(create.m_move.m_status.m_position, create.m_move.m_status.m_facing, this->m_obj->m_guid, this),
      m_smoothFacing(0.0f),
      unk_08D4(0.0f),
      unk_08D8(0.0f),
      m_smoothFacingTarget(0.0f),
      unk_08E0(-1),
      unk_08E4(),
      m_splashSoundKitId(0),
      unk_0930(0),
      unk_0948(),
      m_creatureCacheEntry(nullptr),
      m_displayInfo(nullptr),
      m_displayInfoExtra(nullptr),
      m_modelData(nullptr),
      m_soundData(nullptr),
      m_soundDataOverride(nullptr),
      m_bloodlevels(nullptr),
      m_fishingLineEffect(nullptr),
      m_auraEffects(nullptr),
      unk_0988(nullptr),
      m_mountModel(nullptr),
      m_mountScale(1.0f),
      m_forceWorldModelAlpha(0),
      m_mountTransition(nullptr),
      m_mountTransitionEffect(nullptr),
      unk_09CC(0),
      unk_09D0(0),
      m_displayId(0),
      m_groundNormal(0.0f, 0.0f, 1.0f),
      unk_09E8(0),
      m_missiles(nullptr),
      m_pendingMissiles(nullptr),
      m_shapeshiftSuppressed(0),
      unk_09F8(0),
      unk_0A1C(0),
      m_combat(),
      m_stopAttackSent(0),
      m_animationState(0x70),
      m_footprintTextureId(-1),
      m_terrainTypeId(-1),
      m_footprintLength(0.27777779f),
      m_footprintWidth(0.27777779f),
      unk_0A4C(1.0f),
      m_lastSentFacing(-10.0f),
      m_lastSentPitch(3.4028235e38f),
      unk_0A58(0),
      m_castCount(0),
      m_castSpellId(0),
      m_castSchoolImmunities(0),
      m_castImmunities(0),
      unk_0A6C(0),
      unk_0A78(),
      m_renderFacing(m_movementData.m_facing),
      m_facingVelocity(0.0f),
      m_facingBlend(1.0f),
      m_lootableTime(0),
      m_mountFootprintLength(0.0f),
      m_mountFootprintWidth(0.0f),
      m_animEventPosition(0.0f, 0.0f, 0.0f),
      m_procEffects(nullptr),
      unk_0B00(),
      m_selectionRadius(0.0f),
      m_flashStartTime(0),
      m_proceduralFadeAlpha(0xFF),
      m_proceduralFadeType(0),
      m_proceduralFadeStart(0),
      m_proceduralFadeDuration(0),
      m_proceduralFadeEndAlpha(0),
      unk_0B34(),
      m_modelScale(1.0f),
      unk_0B40(nullptr),
      m_ammoModel(nullptr),
      m_ammoDisplayId(0),
      m_characterComponent(nullptr),
      unk_0B58(this->m_unit->m_unitFieldBytes2[0]),
      m_sheatheState(this->m_unit->m_unitFieldBytes2[0]),
      m_chainSpellLocation(0.0f, 0.0f, 0.0f),
      m_mountAnimBehaviorId(91),
      m_animTier(0),
      m_torsoKeyBone(-1),
      m_meleeAttackAnimId(-1),
      m_pendingAnimId(-1),
      m_awaitingEndAnimId(-1),
      m_namePlateFrame(nullptr),
      m_chatBubble(nullptr),
      m_masterLooter(),
      m_looter(),
      unk_0F58(0),
      m_vehicle(nullptr),
      m_vehiclePassenger(nullptr),
      m_vehicleCamera(nullptr),
      m_missileTrajectorySpell(0),
      m_missileTrajectoryCastId(0),
      m_missileTrajectoryStart(0),
      m_missileTrajectoryEnd(0),
      m_missileTrajectoryReleased(0),
      m_wheelMask(0),
      unk_0FA0(0),
      unk_0FA4(-1),
      m_precastMissileModel(nullptr),
      m_predictedHealth(this->m_unit->m_unitFieldHealth),
      unk_0FD0(0),
      m_manaRegenInterruptTime(0) {
    this->m_movement = &this->m_movementData;
    this->unk_0A34 = 0;
    this->m_clientFlags = 0x400000;

    this->SetClientInitData(create, false);

    if (static_cast<float>(this->m_unit->m_unitFieldHealth) / static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth) < 0.2f && this->m_unit->m_unitFieldHealth > 0 && this->m_bloodlevels) {
        this->m_clientFlags |= 0x2;
    }

    this->m_weaponTrails[0] = 0;
    this->m_weaponTrails[1] = 0;

    this->m_emoteQueue[0].m_delay = 0;
    this->m_emoteQueue[1].m_delay = 0;
    this->m_emoteQueue[1].m_emote = 0;
    this->m_emoteQueue[2].m_delay = 0;
    this->m_emoteQueue[2].m_emote = 0;
    this->m_emoteQueue[3].m_delay = 0;
    this->m_emoteQueue[3].m_emote = 0;
    this->m_emoteQueue[0].m_emote = -1;

    this->m_targetChangeTime = FrameTime::s_curTimeMs - 1000;

    this->InitPredictedPower();

    for (int32_t i = 0; i < 3; i++) {
        ItemRec* rec = g_itemDB.GetRecord(this->m_unit->m_unitVirtualItemSlotId[i]);
        CGUnitVirtualItem item;

        if (rec) {
            this->m_virtualItemDisplayId[i] = rec->m_displayInfoID;
            item.m_classID = rec->m_classID;
            item.m_subclassID = rec->m_subclassID;
            item.m_soundOverrideSubclassID = rec->m_soundOverrideSubclassID;
            item.m_material = rec->m_material;
            item.m_inventoryType = rec->m_inventoryType;
            item.m_sheatheType = rec->m_sheatheType;
        } else {
            item.m_classID = 0;
            item.m_subclassID = 0;
            item.m_soundOverrideSubclassID = 0;
            item.m_material = 0;
            item.m_inventoryType = 0;
            item.m_sheatheType = 0;
            this->m_virtualItemDisplayId[i] = 0;
        }

        item.m_pad[0] = 0;
        item.m_pad[1] = 0;
        this->m_virtualItem[i] = item;
    }

    this->RefreshDataPointers();

    if (create.m_updateFlags & 0x1) {
        this->InitializeActivePlayerComponent();
    }
}

// VERIFY: a4a460eb manual
// OFFSET: 0x0073C260
void CGUnit_C::SetClientInitData(const CClientObjCreate& create, bool reenable) {
    this->m_combat.SetClientInitData(create);

    if (create.m_updateFlags & 0x80) {
        this->CreateVehicleData(&create, create.m_vehicleId);
    }

    if (reenable) {
        return;
    }

    int32_t activePlayer = create.m_updateFlags & 0x1;
    this->m_movementData.SetUpdateInfo(OsGetAsyncTimeMs(), &create.m_move, activePlayer);

    if (this->m_movementData.m_flags & MOVEMENTFLAG_FALLING_FAR) {
        this->OnCollideFalling();
    }

    if (create.m_updateFlags & 0x1) {
        this->m_clientFlags |= 0x80;
        Weather* weather = World::s_weather;
        C3Vector position = this->GetPosition();
        weather->unk_017C = position.x;
        weather->unk_0180 = position.y;
        weather->unk_0184 = position.z;
    }

    if (create.m_updateFlags & 0x400) {
        this->m_clientFlags |= 0x40000000;
    }
}

// VERIFY: 62962b3e auto
// OFFSET: 0x0073AD00
void CGUnit_C::OnCollideFalling() {
    if (!(this->m_movementData.m_flags & MOVEMENTFLAG_FALLING_FAR) || this->m_unit->m_unitFieldHealth <= 0) {
        return;
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_vehicle->HasHighPriorityAnim()) {
        return;
    }

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state) {
        return;
    }

    this->PlayBaseAnimation(ANIM_FALL, 0);
}

// VERIFY: 51ad0765 auto
// OFFSET: 0x0074C750
void CGUnit_C::CreateVehicleData(const CClientObjCreate* create, int32_t vehicleId) {
    if (this->m_vehicle) {
        this->m_vehicle->ReceiveVehicleRecIDFromServer(vehicleId);
        return;
    }

    this->m_vehicle = CVehicle_C::Create(create, this, vehicleId);

    if ((this->m_modelFlags & 0x40000) && this->IsActivePlayer()) {
        this->SignalPlayerGainsVehicleDataEvent();
    }
}

// VERIFY: 8cfa7612 auto
// OFFSET: 0x0072D940
void CGUnit_C::RefreshDataPointers() {
    int32_t displayId = this->m_displayId;

    if (!displayId || this->m_unit->m_unitFieldNativedisplayid != this->m_unit->m_unitFieldDisplayid) {
        displayId = this->m_unit->m_unitFieldDisplayid;
    }

    this->m_displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!this->m_displayInfo) {
        SysMsgPrintf(STATUS_ERROR, 2, "NOUNITDISPLAYID|%d|%s", displayId, this->GetUnitName(nullptr, 1));
        this->m_displayInfo = g_creatureDisplayInfoDB.m_numRecords > 0 ? g_creatureDisplayInfoDB.m_records : nullptr;
    }

    CreatureDisplayInfoRec* displayInfo = this->m_displayInfo;
    this->m_displayInfoExtra = g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID);

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);
    this->m_modelData = modelData;

    this->m_soundData = g_creatureSoundDataDB.GetRecord(displayInfo->m_soundID);

    if (!this->m_soundData) {
        this->m_soundData = g_creatureSoundDataDB.GetRecord(modelData->m_soundID);
    }

    this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(displayInfo->m_bloodID);

    if (!this->m_bloodlevels) {
        this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(modelData->m_bloodID);

        if (!this->m_bloodlevels) {
            this->m_bloodlevels = g_unitBloodLevelsDB.m_numRecords > 0 ? g_unitBloodLevelsDB.m_records : nullptr;
        }
    }

    if (this->m_obj->m_type == (TYPEMASK_UNIT | TYPEMASK_OBJECT)) {
        this->m_creatureCacheEntry = g_creatureDBCache.GetRecord(this->m_obj->m_entryID, this->m_obj->m_guid, &CreatureQueryCallback, nullptr, false);
    }

    if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        this->m_clientFlags |= 0x100;
    } else {
        this->m_clientFlags &= ~0x100u;
    }

    if (this->m_modelData->m_flags & 0x8) {
        this->m_animationState |= 0x20000;
    } else {
        this->m_animationState &= ~0x20000u;
    }

    if (this->m_modelData->m_flags & 0x40) {
        this->m_clientFlags |= 0x2000000;
    } else {
        this->m_clientFlags &= ~0x2000000u;
    }
}

// VERIFY: 6c3e7cce auto
// OFFSET: 0x0071CFA0
void CGUnit_C::InitializeActivePlayerComponent() {
    CCharacterComponent* component = CCharacterComponent::m_activePlayerComponent;

    if (!component) {
        return;
    }

    if (this->m_modelData && (this->m_modelData->m_flags & 0x4) && CGPlayer_C::s_displayId == this->m_unit->m_unitFieldDisplayid) {
        this->SetCharacterModel(CCharacterComponent::m_activePlayerComponent->m_data.m_model);
        this->m_characterComponent = component;
        CCharacterComponent::m_activePlayerComponent = nullptr;
    } else {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = nullptr;
    }
}

// VERIFY: 6e2e4920 manual
// OFFSET: 0x0073FCC0
void CGUnit_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_movementData.UpdateTransportStatus(create);
    this->m_modelScale = this->GetModelScale(this->m_unit->m_unitFieldDisplayid);

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        this->OnMoveUpdate(time, 1, 1);
    }

    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
    CGObject_C::PostInit(time, create, reenable);
    this->VehiclePassengerInit();
    MovementUpdateCameraYaw(this->m_obj->m_guid, this->m_movementData.m_transportGuid);

    CM2Model* model = this->m_worldModel;
    model->SetSequenceCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEndHandler), this->m_obj->m_guid.m_guid);
    model->SetEventCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEventCallback), this->m_obj->m_guid.m_guid);

    if (this->m_displayInfo) {
        CCharacterComponent::ApplyMonsterGeosets(this->m_worldModel, this->m_displayInfo);
        CCharacterComponent::ReplaceMonsterSkin(this->m_worldModel, this->m_displayInfo, this->m_modelData);

        if (this->m_modelData) {
            this->m_worldModel->m_fFlags ^= (this->m_worldModel->m_fFlags ^ (this->m_modelData->m_flags >> 7)) & 0x4;
        }
    }

    this->m_mountDisplayId = this->m_unit->m_unitFieldMountdisplayid;

    CreatureModelDataRec* modelData = this->m_modelData;
    this->m_footprintTextureId = modelData->m_footprintTextureID;
    this->m_footprintLength = modelData->m_footprintTextureWidth * 0.027777778f;
    this->m_footprintWidth = modelData->m_footprintTextureLength * 0.027777778f;
    this->unk_0A4C = modelData->m_footprintParticleScale;

    this->CheckLoopSound();

    if ((create.m_updateFlags & 0x1) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && CGPlayer_C::s_displayId == this->m_unit->m_unitFieldDisplayid && this->m_characterComponent) {
        this->m_clientFlags &= 0xFFBDFFFF;
        static_cast<CGPlayer_C*>(this)->RefreshVisibleItems();
    } else {
        if (this->m_characterComponent) {
            CCharacterComponent::FreeComponent(this->m_characterComponent);
            this->m_characterComponent = nullptr;
            this->m_clientFlags = (this->m_clientFlags & 0xFFFDFFFF) | 0x400000;
        }

        this->m_clientFlags = (this->m_clientFlags & 0xFFFDFFFF) | 0x400000;
    }

    this->UpdateUnitCollisionBox(1, 1);

    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (race) {
        this->m_splashSoundKitId = race->m_splashSoundID;
    }

    this->m_targetFacing = CMath::normalizeangle0to2pi_(this->GetRawFacing());
    this->unk_0AA4 = 0.0f;
    this->m_turnDelta[0] = 0.0f;
    this->m_turnDelta[1] = 0.0f;
    this->m_turnDelta[2] = 0.0f;
    this->m_turnDelta[3] = 0.0f;
    this->Animate(0.0f);

    if (this->m_unit->m_unitFieldHealth > 0) {
        if (static_cast<int32_t>(this->m_unit->m_unitFieldMountdisplayid) > 0) {
            this->CreateUnitMount(1, 1);
        }
    } else {
        this->m_lootableTime = time;
        this->ClearEffectList(0);
        this->Unk717BA0();
    }

    int32_t groundType;

    if (!ClntObjMgrGetPlayerType() && World::QueryGroundType(this->m_worldObject, &groundType)) {
        this->m_terrainTypeId = groundType;
    }

    this->UpdateScriptRegistration();

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    WOWGUID guid = this->m_obj->m_guid;
    this->m_nameDesc = PlayerNameCreate(guid);

    this->UpdateBreathState(FrameTime::s_curTimeMs);
    this->UpdateChannelEffects();

    if (!this->ShouldFadeIn()) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    this->m_animTier = this->m_unit->m_unitFieldBytes1[3];
    this->m_movementData.UpdateHoverState(this->m_animTier == 2, 0);
    this->m_movementData.m_hoverHeight = this->m_unit->m_unitFieldHoverheight;
    this->CreateOrDestroyObjectEffectManager();

    if (reenable && !(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        this->unk_09F8 = 10;
    }

    this->UpdateBaseAnimation(false, -1);

    uint32_t sequenceId;

    if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) {
        if (this->m_torsoKeyBone == -1 || (sequenceId = this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone)) == 0xFFFFFFFF) {
            sequenceId = this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
        }
    } else {
        sequenceId = 0xFFFFFFFF;
    }

    AnimationDataRec* anim = g_animationDataDB.GetRecord(sequenceId);

    if (anim && anim->m_behaviorID == 127) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    const WOWGUID& charmedBy = this->m_unit->m_unitFieldCharmedby;

    if (charmedBy) {
        Script_SendUnitSignal(charmedBy, 0);
    }

    const WOWGUID& summonedBy = this->m_unit->m_unitFieldSummonedby;

    if (summonedBy) {
        Script_SendUnitSignal(summonedBy, 2);
    }

    this->UpdatePartyMemberPetState();

    TotemInfo* totem = CGGameUI::GetTotemInfo(this->m_obj->m_guid);

    if (totem) {
        totem->m_name = this->GetUnitName(nullptr, 1);
        FrameScript_SignalEvent(165, "%d", totem->m_slot + 1);
    }

    this->m_movementData.SnapToGroundIfCloseEnough();

    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : nullptr;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID vehicleGuid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddVehicle(vehicleGuid);
    }

    this->m_clientFlags |= 0x80000;

    if (CGUnit_C::s_deferredClientControlUpdateGUID == this->m_obj->m_guid) {
        this->ExecuteClientControlUpdate(CGUnit_C::s_deferredClientControlUpdateGUID, CGUnit_C::s_deferredClientControlUpdateState);
        CGUnit_C::s_deferredClientControlUpdateGUID = WOWGUID();
    }

    CMissileCollision::MaybeAddUnitToSystem(this);

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_unit->m_unitFieldPetnumber && player && player->UnitReaction(this) <= 1) {
        WOWGUID petGuid = this->m_obj->m_guid;
        const WOWGUID* owner = this->m_unit->m_unitFieldCharmedby ? &this->m_unit->m_unitFieldCharmedby : &this->m_unit->m_unitFieldCreatedby;
        CGBattlefieldInfo::AddArenaOpponentPet(petGuid, *owner);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->SignalPlayerGainsVehicleDataEvent();
    }
}

// VERIFY: ab6c33e7 manual
// OFFSET: 0x00722AE0
float CGUnit_C::GetModelScale(uint32_t displayId) {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        return 1.0f;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (!modelData) {
        return 1.0f;
    }

    return this->GetModelScale(displayInfo, modelData);
}

// VERIFY: 7ba41095 manual
// OFFSET: 0x0073AB20
void CGUnit_C::OnMoveUpdate(uint32_t time, int32_t a3, int32_t a4) {
    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateWorldMatrix();
    }

    this->UpdateWorldObject(0);

    if (!this->m_worldObject || !World::QueryGroundType(this->m_worldObject, &this->m_terrainTypeId)) {
        this->m_terrainTypeId = -1;
    }

    this->UpdateFlightStatus(time);
    this->UpdateSwimmingStatus(time, a3);
}

// VERIFY: ac360cae auto
// OFFSET: 0x0074D070
void CGUnit_C::VehiclePassengerInit() {
    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->Free();
        this->m_vehiclePassenger = nullptr;
    }

    CVehiclePassenger_C::Init(this);

    if (!this->m_vehicleCamera) {
        this->OnVehicleCameraPossiblyNeeded();
        return;
    }

    this->m_vehicleCamera->BeginTransition(FrameTime::s_curTimeMs);

    if (!this->m_vehicleCamera->m_state) {
        this->DestroyVehicleCamera();
    }
}

// VERIFY: 3c56a72f auto
// OFFSET: 0x007179D0
void CGUnit_C::SetCharacterModel(CM2Model* model) {
    if (this->m_worldModel == model) {
        return;
    }

    if (!this->m_mountModel) {
        this->CleanUpVehicleBoneAnimsBeforeObjectModelChange();
    }

    CM2Model* oldModel = this->SetModelStart(model);
    CMissile::UpdateAttachments(this->m_obj->m_guid);
    this->SetModelFinish(oldModel);
}

// VERIFY: c1bf7317 manual
// OFFSET: 0x00754FF0
void CGUnit_C::SetAttacking(WOWGUID guid) {
    this->m_combat.SetAttacking(guid);
    this->m_combat.m_attackSent = 0;
}

// VERIFY: d0695a99 manual
// OFFSET: 0x007237F0
void CGUnit_C::Reenable() {
    CGObject_C::Reenable();

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    WOWGUID guid = this->m_obj->m_guid;
    this->m_nameDesc = PlayerNameCreate(guid);

    float facing = this->m_movementData.m_facing;
    this->m_targetFacing = CMath::normalizeangle0to2pi_(facing);
    this->unk_0AA4 = 0.0f;
    this->m_turnDelta[0] = 0.0f;
    this->m_turnDelta[1] = 0.0f;
    this->m_turnDelta[2] = 0.0f;
    this->m_turnDelta[3] = 0.0f;
    this->m_renderFacing = facing;
    this->m_facingVelocity = 0.0f;
    this->m_facingBlend = 1.0f;

    this->m_predictedHealth = this->m_unit->m_unitFieldHealth;
    this->InitPredictedPower();
    this->unk_0FD0 = 0;
    this->m_manaRegenInterruptTime = 0;

    if (static_cast<float>(this->m_unit->m_unitFieldHealth) / static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth) < 0.2f && !this->IsDeadOrGhost() && this->m_bloodlevels) {
        this->m_clientFlags |= 0x2;
    }

    this->m_animationState &= 0xFFFFDBFF;

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(3);

    if (this->m_worldModel) {
        this->m_worldModel->m_lastAnimTime = 0;
    }

    this->Unk717BA0();

    for (int32_t i = 0; i < 3; i++) {
        ItemRec* rec = g_itemDB.GetRecord(this->m_unit->m_unitVirtualItemSlotId[i]);
        CGUnitVirtualItem item;

        if (rec) {
            this->m_virtualItemDisplayId[i] = rec->m_displayInfoID;
            item.m_classID = rec->m_classID;
            item.m_subclassID = rec->m_subclassID;
            item.m_soundOverrideSubclassID = rec->m_soundOverrideSubclassID;
            item.m_material = rec->m_material;
            item.m_inventoryType = rec->m_inventoryType;
            item.m_sheatheType = rec->m_sheatheType;
        } else {
            item.m_classID = 0;
            item.m_subclassID = 0;
            item.m_soundOverrideSubclassID = 0;
            item.m_material = 0;
            item.m_inventoryType = 0;
            item.m_sheatheType = 0;
            this->m_virtualItemDisplayId[i] = 0;
        }

        item.m_pad[0] = 0;
        item.m_pad[1] = 0;
        this->m_virtualItem[i] = item;
    }

    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : nullptr;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID vehicleGuid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddVehicle(vehicleGuid);
    }

    this->unk_0B58 = this->m_sheatheState;
    this->UpdateObjectEffectMovementStates();
}

// VERIFY: 10fdac8a manual
// OFFSET: 0x007402B0
void CGUnit_C::PostReenable() {
    CGObject_C::PostReenable();
    this->CheckLoopSound();
    this->UpdateChannelEffects();

    if (!this->ShouldFadeIn()) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    this->UpdateBaseAnimation(false, -1);
    this->DestroyUnitMount(1);
    this->UpdateDisplayInfo(0);

    if (static_cast<int32_t>(this->m_unit->m_unitFieldMountdisplayid) > 0) {
        this->CreateUnitMount(1, 1);
    }

    this->UpdateScriptRegistration();
    this->UpdatePartyMemberPetState();
    this->UpdateBreathState(FrameTime::s_curTimeMs);

    if (this->m_unit->m_unitFieldHealth <= 0 && !this->m_lootableTime) {
        this->m_lootableTime = FrameTime::s_curTimeMs;
        this->Unk717BA0();
    }

    this->m_movementData.SnapToGroundIfCloseEnough();
    this->VehiclePassengerInit();

    if (CGUnit_C::s_deferredClientControlUpdateGUID == this->m_obj->m_guid) {
        this->ExecuteClientControlUpdate(CGUnit_C::s_deferredClientControlUpdateGUID, CGUnit_C::s_deferredClientControlUpdateState);
        CGUnit_C::s_deferredClientControlUpdateGUID = WOWGUID();
    }

    this->unk_0948[2] = 0;
    this->unk_0948[3] = 0;

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_unit->m_unitFieldPetnumber && player && player->UnitReaction(this) <= 1) {
        WOWGUID petGuid = this->m_obj->m_guid;
        const WOWGUID* owner = this->m_unit->m_unitFieldCharmedby ? &this->m_unit->m_unitFieldCharmedby : &this->m_unit->m_unitFieldCreatedby;
        CGBattlefieldInfo::AddArenaOpponentPet(petGuid, *owner);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateFreeSeatMask();
    }
}

// VERIFY: 1062337a auto
// OFFSET: 0x0070CBA0
void CGUnit_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_unit = reinterpret_cast<CGUnitData*>(storage + OBJECT_END);
    this->m_unitMirror = saved + CGObject::TotalFieldsSaved();
}

// VERIFY: 8d51be48 manual
// OFFSET: 0x00729220
void CGUnit_C::OnDeath() {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->OnDeath();
    }

    this->m_combat.m_attacking = WOWGUID();
    this->m_combat.m_attackSent = 0;
    this->m_stopAttackSent = 0;

    if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) {
        this->m_worldModel->m_lastAnimTime = 0;
    }

    if (!this->m_vehiclePassenger || this->m_vehiclePassenger->m_state != 3 || (this->m_unit->m_unitFieldFlags2 & 0x20000)) {
        this->OnDeathAnimate();
    }

    WOWGUID guid;

    if (!(this->m_unit->m_unitDynamicFlags & 0x20) || (guid = this->m_obj->m_guid, !CGGameUI::IsRaidMemberOrPet(guid))) {
        guid = this->m_obj->m_guid;
        Spell_C_CancelSpellByTarget(guid, 109);
    }

    this->Unk71EE70();

    if (!this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags & 0x80)) {
        this->RemoveInteractIcon();
    }

    PlayerNameTriggerColorUpdate(this->m_nameDesc);

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(0);
    SI2::StopSound(&this->m_npcInteractSound, 1, -1.0f, 1);
    SI2::StopSound(&this->unk_0934, 1, -1.0f, 1);
    this->ClearTrackingTarget(false, true);

    guid = this->m_obj->m_guid;
    CGGameUI::ClearInteractTarget(guid, 1);
}

// VERIFY: 35cb1629 manual
// OFFSET: 0x007272C0
void CGUnit_C::ClearTrackingTarget(bool a2, bool a3) {
    if (this->m_obj->m_guid != CGUnit_C::s_activeMover || CGUnit_C::s_trackingType == 13) {
        return;
    }

    if (CGUnit_C::s_trackingType == 3) {
        FrameScript_SignalEvent(358, nullptr);
    }

    uint32_t time = OsGetAsyncTimeMs();

    if (a2) {
        CGObject_C* target = ClntObjMgrObjectPtr(CGUnit_C::s_trackingGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (target) {
            C3Vector targetPosition = target->GetPosition();
            C3Vector position = this->GetPosition();
            CGUnit_C::s_trackingFacing = CalculateFacingTo(&position, &targetPosition);
            CGInputControl::GetActive()->ToggleControlBit(0x1000000, 1, time, 0);
        }
    }

    if (a3) {
        if (s_clickToMoveActions[CGUnit_C::s_trackingType].unk_0000) {
            this->Unk71AEE0(time);
        }

        CGInputControl::GetActive()->ToggleControlBit(0x1000000, 0, time, 0);
        CGInputControl::GetActive()->ToggleControlBit(0x400000, 0, time, 0);
    }

    CGUnit_C::s_trackingGUID = WOWGUID();
    CGUnit_C::s_unk_CA11D0 = 0.0f;
    CGUnit_C::s_trackingFlags = 0;
    CGUnit_C::s_unk_CA11D4 = 0;
    CGUnit_C::s_trackingType = 13;
}

// VERIFY: 916e508f manual
// OFFSET: 0x00734FD0
void CGUnit_C::Disable() {
    CGObject_C::Disable();
    CMissileCollision::RemoveUnitFromSystem(this);

    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : nullptr;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::RemoveVehicle(guid);
    }

    this->DestroyVehicleCamera();

    if (this->m_vehicle) {
        this->m_vehicle->RemoveAllPassengersLocally();
    }

    this->m_chainSpellTargets.Clear();
    this->KillCreatureLoopSound();

    if (this->m_effectManager) {
        for (int32_t i = 1; i < 82; i++) {
            this->m_effectManager->ExitState(i, 0);
        }
    }

    this->m_clientFlags &= ~0x2;
    this->ClearThreatList();

    if (this->m_namePlateFrame) {
        if (CGUnit_C::s_namePlateFocus == this->m_namePlateFrame) {
            CGUnit_C::s_namePlateFocus = nullptr;
        }

        s_unusedNamePlates.LinkToTail(this->m_namePlateFrame);
        this->m_namePlateFrame->Hide();
        this->m_namePlateFrame = nullptr;
        this->ObjectNameVisibilityChanged();
    }

    if (this->m_chatBubble) {
        CGChatBubbleFrame::Remove(this->m_chatBubble);
        this->m_chatBubble = nullptr;
    }

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    this->m_nameDesc = nullptr;

    WOWGUID guid = this->m_obj->m_guid;

    if (!CGGameUI::IsRaidMemberOrPet(guid) || this->IsDuelingWith(static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)))) {
        CGGameUI::BreakTarget(this->m_obj->m_guid);
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera->m_targetGUID == this->m_obj->m_guid) {
        camera->MakeRelativeTo(WOWGUID());
    }

    this->m_movementData.Disable();

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        if (CGUnit_C::s_trackingType != 13) {
            this->ClearTrackingTarget(false, true);
        }

        CGUnit_C::s_activeMover = WOWGUID();
    }

    guid = this->m_obj->m_guid;
    Spell_C_CancelSpellByCaster(guid);

    if (this->m_castSpellId) {
        this->m_castCount = 0;
        this->m_castSpellId = 0;
        this->unk_0A6C = 0;
        this->unk_0A78[0] = 0;
        this->unk_0A78[1] = 0;
        this->UpdateSpellCastBars(FrameTime::s_curTimeMs);

        SpellRec spell;

        if (g_spellDB.GetRecord(this->m_castSpellId, &spell)) {
            guid = this->m_obj->m_guid;
            Script_SendUnitSpellSignal(guid, this->m_castCount, &spell, 325);
        }
    }

    if (this->m_unit->m_unitChannelSpell) {
        uint32_t elapsed = this->unk_0A78[4] - this->unk_0A78[3];
        uint32_t now = OsGetAsyncTimeMs();
        this->unk_0A78[4] = now;
        this->unk_0A78[3] = now - elapsed;
        this->unk_0A6C = 0;
        this->unk_0A78[0] = 0;
        this->unk_0A78[1] = 0;
        this->UpdateSpellCastBars(FrameTime::s_curTimeMs);
        this->unk_0A78[2] = 0;
        this->unk_0A78[3] = 0;
        this->unk_0A78[4] = 0;
        this->UpdateSpellCastBars(FrameTime::s_curTimeMs);

        SpellRec spell;

        if (g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell, &spell)) {
            guid = this->m_obj->m_guid;
            Script_SendUnitSpellSignal(guid, 0, &spell, 333);
        }
    }

    int32_t index;
    guid = this->m_obj->m_guid;

    if (CGPartyInfo::IsMemberPet(guid, &index) && index >= 0) {
        CGPartyMemberState* member = CGPartyInfo::m_members[index] ? &unk_BD0D08[index] : nullptr;
        this->FillPartyMemberPetState(&member->m_pet);

        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(member->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    guid = this->m_obj->m_guid;

    if (CGRaidInfo::IsPetInRaid(guid, &index) && index >= 0) {
        CGRaidMember* raidMember = CGRaidInfo::m_members[index];
        CGPartyMemberState* member = raidMember->m_guid ? &raidMember->m_stats : nullptr;
        this->FillPartyMemberPetState(&member->m_pet);

        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(member->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (this->unk_0A78[6]) {
        this->unk_0A78[6] = 0;
        ScriptEventsUnregisterUnit(this);
    }

    while (this->m_auraEffects) {
        this->m_auraEffects->Release();
    }

    while (this->m_procEffects) {
        this->m_procEffects->Release();
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = nullptr;
    }

    if (this->unk_0988) {
        this->unk_0988->DeleteSelf();
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x8) {
            effect->MoveToDestroyList();
            break;
        }
    }

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(2);

    for (uint32_t i = 0; i < (this->unk_0EDC.m_count == 0xFFFFFFFF ? this->unk_0EDC.m_dynamic.m_count : this->unk_0EDC.m_count); i++) {
        this->unk_0EDC[i] = 0;
    }

    this->m_animationState &= ~0x200;

    for (int32_t i = 0; i < 2; i++) {
        if (this->m_weaponTrails[i]) {
            WeaponTrailClose(this->m_weaponTrails[i]);
            this->m_weaponTrails[i] = 0;
        }
    }

    SI2::StopSound(&this->m_npcInteractSound, 0, 2.0f, 1);
    this->ClearTrackingTarget(false, true);

    this->unk_0B34[1] = 0;
    this->m_emoteQueue[0].m_delay = 0;
    this->m_emoteQueue[1].m_delay = 0;
    this->m_emoteQueue[1].m_emote = 0;
    this->m_emoteQueue[2].m_delay = 0;
    this->m_emoteQueue[2].m_emote = 0;
    this->m_emoteQueue[3].m_delay = 0;
    this->m_emoteQueue[3].m_emote = 0;
    this->m_animationState &= ~0x8000;
    this->m_emoteQueue[0].m_emote = -1;

    TotemInfo* totem = CGGameUI::GetTotemInfo(this->m_obj->m_guid);

    if (totem) {
        CGGameUI::DestroyTotem(totem->m_slot);
    }

    g_nameDBCache.CancelCallback(this->m_obj->m_guid.m_guid, &NameQueryCallback, nullptr);
    g_petNameCache.CancelCallback(this->m_unit->m_unitFieldPetnumber, &NameQueryCallback, nullptr);

    if (this->m_precastMissileModel) {
        if (this->m_precastMissileModel->m_attachParent) {
            this->m_precastMissileModel->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = nullptr;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (this->GetControllingPlayer() == player) {
        Spell_C_RemovePeriodicClientTrigger(this->m_obj->m_guid, 0);
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->Uninit();
        this->m_vehiclePassenger = nullptr;
    }
}
