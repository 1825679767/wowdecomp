#include "GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_AreaDamage.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_AuraGenerator.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_BarberChair.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Binder.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Button.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Camera.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_CapturePoint.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Chair.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Chest.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Door.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_DuelArbiter.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_DungeonDifficulty.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FishingHole.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FlagDrop.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FlagStand.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Generic.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Goober.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Mailbox.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_MapObj.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_MiniGame.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_QuestGiver.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_SpellCaster.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_SpellFocus.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Text.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Transport.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Trap.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_TrapDoor.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "_Triage/CClientObjCreate.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/UIUtil/Cursor.h"

// STATIC: 0x00A335E8
static const uint32_t s_oneShotAnims[7] = { 0xFFFFFFFF, 0, 8, 9, 10, 11, 12 };

// STATIC: 0x00ADA938
uint32_t CGGameObject_C_TypeAnimated::s_animSequenceIds[13] = { 145, 147, 148, 149, 146, 150, 151, 152, 153, 154, 155, 156, 157 };

// STATIC: 0x00ADA978
const char* CGGameObject_C_TypeAnimated::s_animStateNames[13] = { "Spawn", "Closed", "Opening", "Open", "Closing", "Destroying", "Destroyed", "Rebuilding", "Custom0", "Custom1", "Custom2", "Custom3", "Despawn" };

// VERIFY: 82a5bb1b manual
// OFFSET: 0x00712AE0
static void GameObjectStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));
    WOWGUID requester = WOWGUID();
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(id, requester, nullptr, nullptr, false);

    if (stats) {
        object->LoadBaseObject(stats);
    }
}

// VERIFY: fb01868b manual
// OFFSET: 0x0070CAC0
void CGGameObject_C::AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID{ param }, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        object->m_typeData->HandleAnimEvent(eventId, data, *position, time);
    }
}

// VERIFY: bca2a2a9 manual
// OFFSET: 0x0070CB60
int32_t CGGameObject_C::OnAnimProgressChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object->m_typeData) {
        object->m_typeData->OnUpdateAnimProgress();
    }

    return 1;
}

// VERIFY: 52bb7f99 manual
// OFFSET: 0x0070ED00
int32_t CGGameObject_C::OnFlagsChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object->m_typeData) {
        object->m_typeData->OnUpdateFlags(*static_cast<const uint32_t*>(oldValue) ^ object->m_gameObject->m_flags);
    }

    return 1;
}

// VERIFY: e11279e9 manual
// OFFSET: 0x00711050
int32_t CGGameObject_C::OnStateChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);
    int32_t state = object->m_gameObject->m_state;
    int32_t oldState = object->m_displayState;

    if (state != oldState || object->m_gameObject->m_typeID == 15) {
        object->m_displayState = state;

        if (object->m_typeData) {
            object->m_typeData->UpdateState(oldState, state);
        }
    }

    return 1;
}

// VERIFY: 39a16e63 manual
// OFFSET: 0x0070BE30
int32_t Packet_SMSG_GAMEOBJECT_PAGETEXT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        WOWGUID objectGuid = object->m_obj->m_guid;
        CGItemText::SetItem(objectGuid, 0);
    }

    return 1;
}

// VERIFY: 77bd799e manual
// OFFSET: 0x00711210
void CGGameObject_C::CheckForPassiveHighlight() {
    CGPlayer_C* player;

    if ((this->m_gameObject->m_dynamicFlags & 0x8) || ((player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != nullptr && player->CanTrackObject(this))) {
        this->m_modelFlags |= 0x400000;
        this->ShowLootEffect();
    } else {
        this->m_modelFlags &= ~0x400000;

        if (this->m_lootEffect) {
            if (this->m_lootEffect->m_attachParent) {
                this->m_lootEffect->DetachFromParent();
            }

            this->m_lootEffect->Release();
            this->m_lootEffect = nullptr;
        }
    }
}

// VERIFY: 84ec4568 manual
// OFFSET: 0x00712B40
static int32_t OnUpdateDynamicFlags(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if ((static_cast<uint8_t>(object->m_gameObject->m_dynamicFlags) ^ *static_cast<const uint8_t*>(oldValue)) & 0x8) {
        object->CheckForPassiveHighlight();
    }

    return 1;
}

// VERIFY: de6b9de5 manual
// OFFSET: 0x00713DA0
void ApplyGameObjectArtKit(CM2Model* model, int32_t artKit) {
    GameObjectArtKitRec* rec = g_gameObjectArtKitDB.GetRecord(artKit);

    for (uint32_t i = 0; i < 4; i++) {
        model->DetachAllChildrenById(i);
    }

    if (rec) {
        CStatus status;
        CGxTexFlags flags(GxTex_LinearMipNearest, 1, 1, 0, 0, 0, 1, 0, 0, 0);

        for (uint32_t i = 0; i < 3; i++) {
            if (*rec->m_textureVariation[i]) {
                CTexture* texture = TextureCreate(rec->m_textureVariation[i], flags, &status, 0);

                if (texture) {
                    model->ReplaceTexture(i + 11, texture);
                    HandleClose(reinterpret_cast<HOBJECT>(texture));
                }
            }
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (*rec->m_attachModel[i]) {
                CM2Model* child = model->m_scene->CreateModel(rec->m_attachModel[i], 0);

                if (child) {
                    child->AttachToParent(model, i, nullptr, 0);
                    child->Release();
                }
            }
        }
    }
}

// VERIFY: 6bcc74fd manual
// OFFSET: 0x00713ED0
static int32_t OnUpdateArtKit(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object->IsObjectModelLoaded()) {
        ApplyGameObjectArtKit(object->m_worldModel, object->m_gameObject->m_artKit);
    }

    return 1;
}

// VERIFY: 4d2fc566 auto
// OFFSET: 0x0070D040
void CGGameObject_C::UpdateDisplay(uint32_t time) {
    this->m_typeData->UpdateDisplay(time);

    if (this->m_worldObject) {
        World::TickObject(this->m_worldObject);
    }
}

// VERIFY: 948c52a8 auto
// OFFSET: 0x007140A0
void CGGameObject_C::Initialize() {
    ClientServices::SetMessageHandler(SMSG_PAGE_TEXT, Packet_SMSG_GAMEOBJECT_PAGETEXT, nullptr);
    ClientServices::SetMessageHandler(SMSG_GAME_OBJECT_CUSTOM_ANIM, Packet_SMSG_GAMEOBJECT_CUSTOM_ANIM, nullptr);
    ClientServices::SetMessageHandler(SMSG_GAMEOBJECT_DESPAWN_ANIM, Packet_SMSG_GAMEOBJECT_DESPAWN_ANIM, nullptr);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_state), sizeof(int8_t), CGGameObject_C::OnStateChanged, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_flags), sizeof(uint32_t), CGGameObject_C::OnFlagsChanged, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_artKit), sizeof(int8_t), OnUpdateArtKit, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_pathProgress), sizeof(uint16_t), CGGameObject_C::OnAnimProgressChanged, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_dynamicFlags), sizeof(uint16_t), OnUpdateDynamicFlags, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: 0add9943 auto
// OFFSET: 0x00714150
void CGGameObject_C::Shutdown() {
    ClientServices::ClearMessageHandler(SMSG_PAGE_TEXT);
    ClientServices::ClearMessageHandler(SMSG_GAME_OBJECT_CUSTOM_ANIM);
    ClientServices::ClearMessageHandler(SMSG_GAMEOBJECT_DESPAWN_ANIM);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_state), CGGameObject_C::OnStateChanged, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_flags), CGGameObject_C::OnFlagsChanged, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_artKit), OnUpdateArtKit, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_pathProgress), CGGameObject_C::OnAnimProgressChanged, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_dynamicFlags), OnUpdateDynamicFlags, nullptr);
}

// VERIFY: e9485d9b manual
// OFFSET: 0x0070CB10
void CGGameObject_C::SequenceCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID{ param }, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (interrupted) {
        object->m_typeData->HandleAnimInterrupted();
    } else {
        object->m_typeData->HandleAnimFinished();
    }
}

// VERIFY: 6d6cb557 manual
// OFFSET: 0x0070BA40
void CGGameObject_C::AddToolTipInfo(CGTooltip* tooltip) {
    if (this->m_typeData) {
        this->m_typeData->AddToolTipInfo(tooltip);
    }
}

// VERIFY: 5a089c89 auto
// OFFSET: 0x00712EB0
int32_t CGGameObject_C::AddPassenger(CPassenger* passenger, int32_t a2) {
    this->m_typeData->AddPassenger(passenger, a2);
    return 1;
}

// VERIFY: d2a54236 auto
// OFFSET: 0x0070B930
int32_t CGGameObject_C::Animate(float elapsed) {
    return this->m_typeData->Animate(elapsed) != 0;
}

// VERIFY: 297d76f4 auto
// OFFSET: 0x0070BA00
bool CGGameObject_C::CanUse() const {
    return this->m_typeData->CanUse();
}

// VERIFY: 6c3c1365 auto
// OFFSET: 0x0070BA10
int32_t CGGameObject_C::CanUseNow() const {
    return this->m_typeData->CanUseNow(nullptr, nullptr, nullptr);
}

// VERIFY: fc3ac502 manual
// OFFSET: 0x0070BA30
int32_t CGGameObject_C::CheckRange() const {
    return this->m_typeData->CheckRange();
}

// VERIFY: e2291194 manual
// OFFSET: 0x0070BAC0
void CGGameObject_C::CloseInteraction() {
    this->m_typeData->CloseInteraction();
}

// VERIFY: c0263fe5 auto
// OFFSET: 0x0070B9A0
float CGGameObject_C::GetBaseAlpha() const {
    return this->m_typeData->GetBaseAlpha();
}

// VERIFY: f4df747d auto
// OFFSET: 0x0070BA90
const char* CGGameObject_C::GetCloseText() const {
    if (this->m_stats) {
        return this->m_stats->m_closeText;
    }

    return nullptr;
}

// VERIFY: a6a6a215 auto
// OFFSET: 0x0070CEC0
void CGGameObject_C::GetCursorFile(char* buffer, uint32_t size) const {
    if (!this->m_stats) {
        return;
    }

    if (this->m_typeData->CanUseNow(nullptr, nullptr, nullptr)) {
        SStrPrintf(buffer, size, "Interface\\Cursor\\%s.blp", this->m_stats->m_iconName);
    } else {
        SStrPrintf(buffer, size, "Interface\\Cursor\\Unable%s.blp", this->m_stats->m_iconName);
    }
}

// VERIFY: ee92213d manual
// OFFSET: 0x0070BAD0
float CGGameObject_C::GetInteractDistanceSquared() const {
    if (!this->m_typeData) {
        return 0.0f;
    }

    CGGameObject_C_TypeBase* type = this->m_typeData;
    float distance = type->GetInteractDistance();
    return type->GetInteractDistance() * distance;
}

// VERIFY: e456868d manual
// OFFSET: 0x00712EE0
C44Matrix CGGameObject_C::GetMatrix() const {
    return this->m_matrix;
}

// VERIFY: 728f32b7 manual
// OFFSET: 0x0070EE30
const char* CGGameObject_C::GetModelFileNameInternal() const {
    int32_t displayID = this->m_gameObject->m_displayID;
    GameObjectDisplayInfoRec* rec = g_gameObjectDisplayInfoDB.GetRecord(displayID);

    if (rec) {
        return rec->m_modelName;
    }

    SysMsgPrintf(STATUS_ERROR, 2, "Game object id %d is missing its display record %d", this->m_obj->m_entryID, displayID);
    return nullptr;
}

// VERIFY: 29c8c641 auto
// OFFSET: 0x0070EE80
int32_t CGGameObject_C::GetModelFileName(const char** name) const {
    switch (this->m_gameObject->m_typeID) {
        case 11:
        case 14:
        case 15:
        case 33:
        case 35: {
            return 0;
        }

        default: {
            const char* file = this->GetModelFileNameInternal();
            *name = file;
            return file != nullptr;
        }
    }
}

// VERIFY: c19341d5 auto
// OFFSET: 0x0070CDF0
const char* CGGameObject_C::GetObjectName() const {
    if (this->m_stats) {
        return this->m_stats->m_name[0];
    }

    return "";
}

// VERIFY: 69157b14 auto
// OFFSET: 0x0070BA70
const char* CGGameObject_C::GetOpenText() const {
    if (this->m_stats) {
        return this->m_stats->m_castBarCaption;
    }

    return nullptr;
}

// VERIFY: 15b3b472 auto
// OFFSET: 0x0070B9B0
uint32_t CGGameObject_C::GetPropertyValue(uint32_t index) const {
    if (this->m_stats && index < 24) {
        return this->m_stats->m_propValue[index];
    }

    return 0;
}

// VERIFY: 44b076db auto
// OFFSET: 0x00712D50
C3Vector CGGameObject_C::GetRawPosition() const {
    C3Vector position = this->m_typeData ? this->m_typeData->GetRawPosition() : this->m_passenger.m_position;
    return position;
}

// VERIFY: 9ad4d30c manual
// OFFSET: 0x00712E40
ShipPath* CGGameObject_C::GetShipPath() const {
    if (this->m_typeData) {
        return this->m_typeData->GetShipPath();
    }

    return nullptr;
}

// VERIFY: f6bd5400 manual
// OFFSET: 0x00712F00
float CGGameObject_C::GetSpeed() const {
    if (this->m_typeData) {
        return this->m_typeData->GetSpeed();
    }

    return -1.0f;
}

// VERIFY: e90da76c auto
// OFFSET: 0x00706560
uint64_t CGGameObject_C::GetTransportGUID() const {
    return this->m_passenger.m_transportGuid.m_guid;
}

// VERIFY: 551dd805 manual
// OFFSET: 0x00712E80
int32_t CGGameObject_C::IsHighlightSuppressed() const {
    return this->m_typeData->IsHighlightSuppressed();
}

// VERIFY: 4bfff0e2 manual
// OFFSET: 0x00712E90
int32_t CGGameObject_C::IsPointInside(const C3Vector& point) const {
    return this->m_typeData->IsPointInside(point);
}

// VERIFY: de769fdd auto
// OFFSET: 0x0070F7B0
int32_t CGGameObject_C::IsQuestGiver() const {
    return this->m_gameObject->m_typeID == 2;
}

// VERIFY: 93c8e68a auto
// OFFSET: 0x00712E60
int32_t CGGameObject_C::IsSolidSelectable() const {
    if (this->unk_020C) {
        return 1;
    }

    if (this->CanHighlight()) {
        return 1;
    }

    return 0;
}

// VERIFY: 1492bbb3 auto
// OFFSET: 0x00712F20
bool CGGameObject_C::IsTransport() const {
    return (this->m_gameObject->m_flags >> 3) & 1;
}

// VERIFY: da4967fc manual
// OFFSET: 0x0070EF90
bool CGGameObject_C::IsValidOpenAction(int32_t action) const {
    if (action == 4) {
        return this->m_displayState != 2;
    }

    int32_t state = this->m_displayState;

    if (state == 2) {
        return false;
    }

    if ((action == 0 || action == 1 || action == 3) && state != 1) {
        return false;
    }

    if (action == 0) {
        return !(this->m_gameObject->m_flags & 2);
    }

    if (action == 1) {
        if (!(this->m_gameObject->m_flags & 2)) {
            return false;
        }
    } else if (action == 2 && state) {
        return false;
    }

    return true;
}

// VERIFY: d9a403af manual
// OFFSET: 0x0070BDE0
void CGGameObject_C::PlayCustomAnim(uint32_t anim) {
    uint32_t pending = anim + 2;

    if (this->IsObjectModelLoaded()) {
        this->m_typeData->ActivateOneShotAnim(s_oneShotAnims[pending]);
        this->m_pendingOneShotAnim = 0;
    } else {
        this->m_pendingOneShotAnim = pending;
    }
}

// VERIFY: 5ceaabac auto
// OFFSET: 0x0070F810
void CGGameObject_C::ResetDisplayState() {
    int32_t state = this->m_gameObject->m_state;
    int32_t oldState = this->m_displayState;

    if (state != oldState || this->m_gameObject->m_typeID == 15) {
        this->m_displayState = state;

        if (this->m_typeData) {
            this->m_typeData->UpdateState(oldState, state);
        }
    }
}

// VERIFY: fdc6ecb0 auto
// OFFSET: 0x0070F7D0
void CGGameObject_C::SetDisplayState(int32_t state) {
    int32_t oldState = this->m_displayState;

    if (state != oldState || this->m_gameObject->m_typeID == 15) {
        this->m_displayState = state;

        if (this->m_typeData) {
            this->m_typeData->UpdateState(oldState, state);
        }
    }
}

// VERIFY: eb7fb820 auto
// OFFSET: 0x0070D010
void CGGameObject_C::SetPathProgress(float progress) {
    this->m_gameObject->m_pathProgress = (int32_t)(float)(progress * 65535.0f);
}

// VERIFY: 562579c7 auto
// OFFSET: 0x0070B9E0
int32_t CGGameObject_C::ShouldFadeIn() const {
    return this->m_typeData->ShouldFadeIn();
}

// VERIFY: 9a0a90f2 manual
// OFFSET: 0x0070B960
void CGGameObject_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);
    this->m_typeData->ShouldRender(flags, render, a3);
}

// VERIFY: 981f9d46 manual
// OFFSET: 0x0070BAB0
void CGGameObject_C::StartInteraction() {
    this->m_typeData->StartInteraction();
}

// VERIFY: a2d85110 auto
// OFFSET: 0x0070F550
int32_t CGGameObject_C::Unk40(int32_t a1) const {
    if ((a1 & 0x8000) && !this->m_gameObject->m_typeID) {
        return 0;
    }

    return this->unk_020C;
}

// VERIFY: 08d3c981 auto
// OFFSET: 0x0070CF30
void CGGameObject_C::UpdateInteractIconAttach() {
    if (!this->m_interactIcon) {
        return;
    }

    CM2Model* worldModel = this->m_worldModel;

    if (!worldModel || !worldModel->IsLoaded(0, 0)) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    if (!this->m_worldModel->HasAttachment(18)) {
        return;
    }

    this->m_interactIcon->AttachToParent(this->m_worldModel, 18, nullptr, 0);
    C44Matrix transform = this->m_worldModel->GetAttachmentWorldTransform(18);

    if (transform.c1 < 0.99000001f) {
        C44Matrix inverse = transform.AffineInverse();
        inverse.d0 = 0.0f;
        inverse.d1 = 0.0f;
        inverse.d2 = 0.0f;
        this->m_interactIcon->SetWorldTransform(inverse);
    }

    this->UpdateInteractIconScale();
    this->ObjectNameVisibilityChanged();
}

// INLINED
CGGameObject_C_TypeBase::CGGameObject_C_TypeBase(CGGameObject_C* owner) {
    this->m_owner = owner;
    this->unk_0008 = 234;
    this->m_interactDistance = 5.0f;
}

// INLINED
CGGameObject_C_TypeBase::~CGGameObject_C_TypeBase() {
}

// VERIFY: e4285ce5 auto
// OFFSET: 0x0070BDD0
float CGGameObject_C_TypeBase::GetInteractDistance() const {
    return this->m_interactDistance;
}

// VERIFY: 5624b8db manual
// OFFSET: 0x0070C2E0
C3Vector CGGameObject_C_TypeBase::GetRawPosition() const {
    return this->m_owner->m_passenger.m_position;
}

// INLINED
CGGameObject_C_Type_Null::CGGameObject_C_Type_Null(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner) {
}

// VERIFY: cb78c258 auto
// OFFSET: 0x0070C340
const char* CGGameObject_C_Type_Null::DebugStatus() {
    return "Unknown Object Type";
}

// VERIFY: 66898c90 manual
// OFFSET: 0x007124B0
CGGameObject_C_TypeAnimated::CGGameObject_C_TypeAnimated(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner), m_animState(-1), unk_0014(-1), m_nextAnimTime(-1) {
    this->ResetAnimState();
}

// VERIFY: b69c4cb1 manual
// OFFSET: 0x00713D10
CGGameObject_C_TypeAnimated::~CGGameObject_C_TypeAnimated() {
    SI2::StopSound(&this->m_sound, 0, -1.0f, 1);
}

// VERIFY: 50f95140 auto
// OFFSET: 0x0070C480
void CGGameObject_C_TypeAnimated::ActivateOneShotAnim(uint32_t anim) {
    if (!this->m_owner->m_worldModel->HasSequence(s_animSequenceIds[anim])) {
        return;
    }

    this->UpdateAnimState(anim);

    if (!anim) {
        this->m_owner->DoFade(this->m_owner->GetBaseAlpha(), 0);
    }
}

// VERIFY: 893eef11 auto
// OFFSET: 0x0070B260
const char* CGGameObject_C_TypeAnimated::DebugStatus() {
    return s_animStateNames[this->m_animState];
}

// VERIFY: bf8c5652 auto
// OFFSET: 0x0070C350
void CGGameObject_C_TypeAnimated::Disable() {
    SI2::StopSound(&this->m_sound, 0, -1.0f, 1);
}

// VERIFY: 176bb2cb auto
// OFFSET: 0x0070C3C0
int32_t CGGameObject_C_TypeAnimated::Animate(float elapsed) {
    CGGameObject_C* owner = this->m_owner;

    if (owner->m_modelFlags & 0x100000) {
        CM2Model* model = owner->m_worldModel;

        if (model && model->IsLoaded(0, 0) && !model->m_animatePrev) {
            if (model->m_lastAnimTime || (this->m_nextAnimTime != -1 && (int32_t)(FrameTime::s_curTimeMs - this->m_nextAnimTime) >= 0)) {
                return this->HandleAnimInterrupted();
            }
        }
    }

    return 1;
}

// VERIFY: 0af705ec auto
// OFFSET: 0x0070D160
void CGGameObject_C_TypeAnimated::OnUpdateFlags(uint32_t changed) {
    if (!(changed & 0x80)) {
        return;
    }

    CM2Model* model = this->m_owner->m_worldModel;

    if (!model || !model->IsLoaded(0, 0)) {
        return;
    }

    int32_t state = this->m_animState;

    if (state != 2 && state != 4 && state != 5 && state != 7) {
        return;
    }

    if ((this->m_owner->m_gameObject->m_flags >> 7) & 1) {
        uint32_t time = model->m_scene->m_time;

        if (!time) {
            time = 1;
        }

        model->m_lastAnimTime = time;
    } else {
        model->m_lastAnimTime = 0;
    }
}

// VERIFY: 366e7076 manual
// OFFSET: 0x0070C370
void CGGameObject_C_TypeAnimated::PostInit(bool a1) {
    if (!a1) {
        return;
    }

    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(0);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 1;
    }
}

// VERIFY: 0e70b882 auto
// OFFSET: 0x0070FCE0
void CGGameObject_C_TypeAnimated::PostReenable() {
    CM2Model* model = this->m_owner->m_worldModel;

    if (model && model->IsLoaded(0, 0)) {
        this->ResetAnimState();
    }
}

// VERIFY: 88b0cee4 auto
// OFFSET: 0x0070D600
void CGGameObject_C_TypeAnimated::ResetAnimState() {
    CGGameObject_C* owner = this->m_owner;
    int32_t state = -1;

    switch (owner->m_displayState) {
        case 0: {
            state = (owner->m_gameObject->m_pathProgress == 0xFFFF) + 2;
            break;
        }

        case 1: {
            state = owner->m_gameObject->m_pathProgress != 0xFFFF ? 4 : 1;
            break;
        }

        case 2: {
            state = (owner->m_gameObject->m_pathProgress == 0xFFFF) + 5;
            break;
        }
    }

    this->m_animState = -1;
    this->unk_0014 = -1;
    this->m_nextAnimTime = -1;
    this->UpdateAnimState(state);
}

// VERIFY: 0978367d manual
// OFFSET: 0x00712510
bool CGGameObject_C_TypeAnimated::ShouldFadeIn() const {
    return this->m_animState != 0;
}

// VERIFY: 1c670d12 auto
// OFFSET: 0x0070B230
void CGGameObject_C_TypeAnimated::UpdateDisplay(uint32_t time) {
    C3Vector position = this->GetPosition();
    SI2::Set3DPosition(&this->m_sound, &position);
}

// VERIFY: 2da8644c auto
// OFFSET: 0x0070D690
void CGGameObject_C_TypeAnimated::UpdateState(int32_t oldState, int32_t newState) {
    if (this->m_owner->m_modelFlags & 0x20000) {
        return;
    }

    SI2::StopSound(&this->m_sound, 0, -1.0f, 1);

    switch (newState) {
        case 0: {
            if (oldState == 1 || this->m_owner->m_gameObject->m_pathProgress != 0xFFFF) {
                this->UpdateAnimState(2);
            } else {
                this->UpdateAnimState(3);
            }

            break;
        }

        case 1: {
            int32_t state = this->m_owner->m_gameObject->m_pathProgress == 0xFFFF ? oldState : 0;

            switch (state) {
                case 0: {
                    this->UpdateAnimState(4);
                    break;
                }

                case 2: {
                    this->UpdateAnimState(7);
                    break;
                }

                default: {
                    this->UpdateAnimState(1);
                    break;
                }
            }

            break;
        }

        case 2: {
            if (oldState == 1 || this->m_owner->m_gameObject->m_pathProgress != 0xFFFF) {
                this->UpdateAnimState(5);
            } else {
                this->UpdateAnimState(6);
            }

            break;
        }
    }
}

// VERIFY: c1ad28de manual
// OFFSET: 0x00712030
bool CGGameObject_C_Type_FishingNode::CanUse() const {
    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

    if (player && *(const WOWGUID*)&((CGUnit_C*)player)->m_unit->m_unitFieldChannelObject == this->m_owner->m_obj->m_guid) {
        return CGGameObject_C_TypeBase::CanUse();
    }

    return false;
}

// VERIFY: f61c006c auto
// OFFSET: 0x0070B910
int32_t CGGameObject_C_Type_GuildBank::CanChangeCursor() const {
    return this->CanUseNow(nullptr, nullptr, nullptr) ? 3 : 29;
}

// VERIFY: 1e1b3af8 auto
// OFFSET: 0x0070C540
void CGGameObject_C_Type_TransportBase::Reenable() {
    this->unk_0038 = -1;
    this->unk_0034 = 0;
}

// VERIFY: ed8aebb4 auto
// OFFSET: 0x007126A0
C3Vector CGGameObject_C_Type_TransportBase::GetRawPosition() const {
    return this->m_position;
}

// VERIFY: 3677cf38 auto
// OFFSET: 0x007126C0
float CGGameObject_C_Type_TransportBase::GetSpeed() const {
    return this->m_speed;
}

// VERIFY: 83fbb5b1 auto
// OFFSET: 0x00959D00
uint32_t CGGameObject_C_Type_TransportBase::GetTransportUpdateTime() const {
    return this->m_transportUpdateTime;
}

// VERIFY: db902dce auto
// OFFSET: 0x00711CD0
void CGGameObject_C_Type_MapObjTransport::AddPassenger(CPassenger* passenger, int32_t a2) {
    CGGameObject_C_Type_TransportBase::AddPassenger(passenger, a2);
}

// VERIFY: c14ec6b1 manual
// OFFSET: 0x007141D0
CGGameObject_C_Type_MapObjTransport::CGGameObject_C_Type_MapObjTransport(CGGameObject_C* owner) : CGGameObject_C_Type_TransportBase(owner), unk_0080(0) {
}

// VERIFY: d75d2da1 auto
// OFFSET: 0x00714230
ShipPath* CGGameObject_C_Type_MapObjTransport::GetShipPath() const {
    return (ShipPath*)&this->m_shipPath;
}

// INLINED
CGGameObject_C_Type_WorldObj::CGGameObject_C_Type_WorldObj(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner) {
    this->m_worldObject = nullptr;
}

// VERIFY: fb2d9d67 auto
// OFFSET: 0x0070B2B0
void CGGameObject_C_Type_WorldObj::SequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (interrupted) {
        return;
    }

    uint32_t next;

    switch (sequenceId) {
        case 146: {
            next = 147;
            break;
        }

        case 148: {
            next = 149;
            break;
        }

        case 162: {
            next = 163;
            break;
        }

        case 164: {
            next = 0;
            break;
        }

        default: {
            return;
        }
    }

    model->SetBoneSequence(0xFFFFFFFF, next, 0xFFFFFFFF, 0, 1.0f, 1, 1);
}

// VERIFY: 633095f5 manual
// OFFSET: 0x007128D0
CGGameObject_C_Type_DestructibleBuilding::CGGameObject_C_Type_DestructibleBuilding(CGGameObject_C* owner) : CGGameObject_C_Type_WorldObj(owner) {
    this->unk_0094 = 0.0f;
    this->unk_0098 = 0.0f;
    this->unk_009C = 0.0f;
    this->unk_008C = 0;
    this->unk_0090 = 0;
    memset(this->unk_0014, 0, sizeof(this->unk_0014));
    this->unk_0058 = 0.0f;
    this->unk_006C = 0.0f;
    this->unk_005C = 0;
    this->unk_0060 = 0;
    this->unk_0070 = 0.0f;
    this->unk_00AD = 0;
    this->unk_0064 = 0;
    this->unk_0068 = 0;
    this->unk_00A8 = nullptr;
    this->unk_00AC = 1;
    this->unk_0054 = -1;
}

// VERIFY: 3e8f3f39 auto
// OFFSET: 0x0070B870
void CGGameObject_C_Type_DestructibleBuilding::AmbientSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (!interrupted && sequenceId != 154) {
        model->SetBoneSequence(0xFFFFFFFF, 154, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
}

// VERIFY: 73f5d046 manual
// OFFSET: 0x0070CA40
void CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID{ param }, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        object->m_typeData->HandleAnimEvent(eventId, data, *position, time);
    }
}

// VERIFY: ed94b365 auto
// OFFSET: 0x0070B800
void CGGameObject_C_Type_DestructibleBuilding::DestructSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (!interrupted && sequenceId) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
}

// VERIFY: 850a3d5b auto
// OFFSET: 0x0070B830
void CGGameObject_C_Type_DestructibleBuilding::ImpactEffectSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (interrupted) {
        return;
    }

    if (sequenceId == 153) {
        model->SetBoneSequence(0xFFFFFFFF, 154, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
}

// VERIFY: d0d0873a auto
// OFFSET: 0x0070B8B0
void CGGameObject_C_Type_DestructibleBuilding::MovedDoodadSequenceHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (interrupted) {
        return;
    }

    if (sequenceId == 153 || sequenceId == 154) {
        model->SetBoneSequence(0xFFFFFFFF, 155, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else if (sequenceId == 155) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
}

// VERIFY: 37d18698 manual
// OFFSET: 0x0070CA90
int32_t CGGameObject_C_Type_DestructibleBuilding::IsHighlightSuppressed() const {
    if (this->unk_00A8 && this->unk_00A8[16]) {
        return 1;
    }

    CGGameObject_C* owner = this->m_owner;

    if (!owner || owner->m_displayState == 1) {
        return 0;
    }

    return 1;
}

// VERIFY: 6612f5da manual
// OFFSET: 0x0070C550
float CGGameObject_C_Type_TransportBase::UpdateCurrentVelocity(uint32_t elapsed, const C3Vector& position, C3Vector* direction) {
    float dx = position.x - this->m_position.x;
    float dy = position.y - this->m_position.y;
    float dz = position.z - this->m_position.z;
    float distance = sqrt(dx * dx + dy * dy + dz * dz);
    float speed = distance / ((float)elapsed * 0.001f);

    if (this->m_speed != speed && this->m_owner->m_effectManager) {
        this->m_owner->m_effectManager->PropertyChanged(1);
    }

    this->m_speed = speed;

    if (direction && !(fabs(distance) < 2.3841858e-7f)) {
        float scale = 1.0f / distance;
        direction->x = dx * scale;
        direction->y = dy * scale;
        direction->z = scale * dz;
    }

    return distance;
}

// VERIFY: 1b4606a4 manual
// OFFSET: 0x0070BEF0
int32_t Packet_SMSG_GAMEOBJECT_DESPAWN_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        if (!object->IsObjectModelLoaded()) {
            object->m_pendingOneShotAnim = 6;
            return 1;
        }

        object->m_typeData->ActivateOneShotAnim(12);
        object->m_pendingOneShotAnim = 0;
    }

    return 1;
}

// VERIFY: 222a19b6 auto
// OFFSET: 0x0070B390
void CGGameObject_C_Type_MapObjTransport::UpdateObjectEffectState(uint32_t anim) {
    CObjectEffectManager* manager = this->m_owner->m_effectManager;

    if (!manager) {
        return;
    }

    switch (anim) {
        case 0: {
            manager->EnterState(2, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 162: {
            manager->EnterState(3, 1, 0);
            this->m_owner->m_effectManager->ExitState(2, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 163: {
            manager->EnterState(4, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(2, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 164: {
            manager->EnterState(5, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(2, 1);
            break;
        }
    }
}

// VERIFY: c5cf4c85 auto
// OFFSET: 0x0070D080
void CGGameObject_C::ShowLootEffect() {
    if (this->m_lootEffect) {
        return;
    }

    if (!this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0)) {
        return;
    }

    const SpellVisualEffectNameRec* rec = GetSpecialSpellVisualEffectNameRec(4);
    this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(rec->m_fileName, 0);

    if (!this->m_lootEffect) {
        return;
    }

    this->m_lootEffect->AttachToParent(this->m_worldModel, 19, nullptr, 1);

    float scale = fclamp(this->GetScale() * this->m_height * 1.15f, 1.0f, 5.0f);
    C44Matrix matrix;
    matrix.Scale(scale);

    CM2Model* effect = this->m_lootEffect;
    effect->m_fFlags |= 0x8000;
    effect->m_worldTransform = matrix;
}

// VERIFY: e08bcb80 auto
// OFFSET: 0x0070BB10
void CGGameObject_C::UpdateObjectEffectAnimationStates(uint32_t anim) {
    if (!this->m_effectManager) {
        return;
    }

    uint32_t state = FindObjectEffectStateFromAnimID(anim, 0);

    if (!state || state == this->m_effectAnimState) {
        return;
    }

    this->m_effectManager->ExitState(this->m_effectAnimState, 1);
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

    this->m_effectAnimState = state;
}

// VERIFY: 8e612f17 auto
// OFFSET: 0x00712700
CGGameObject_C_Type_FishingNode::CGGameObject_C_Type_FishingNode(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
    this->m_interactDistance = 100.0f;
}

// VERIFY: b2424cfc auto
// OFFSET: 0x00712AC0
CGGameObject_C_Type_GuildBank::CGGameObject_C_Type_GuildBank(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// VERIFY: a72bb2d6 auto
// OFFSET: 0x00712790
CGGameObject_C_Type_MeetingStone::CGGameObject_C_Type_MeetingStone(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// VERIFY: 9c1d06e1 auto
// OFFSET: 0x00712730
CGGameObject_C_Type_Ritual::CGGameObject_C_Type_Ritual(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// VERIFY: f1457284 manual
// OFFSET: 0x00713250
CGGameObject_C_Type_TransportBase::CGGameObject_C_Type_TransportBase(CGGameObject_C* owner) : CGGameObject_C_Type_WorldObj(owner) {
    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->unk_0030 = 0;
    this->unk_0034 = 0;
    this->m_speed = 0.0f;
    this->m_transportUpdateTime = 0;
    this->unk_0038 = -1;
    this->m_position = owner->m_passenger.GetPosition(owner->m_passenger.m_position);
}

// VERIFY: f74bd918 auto
// OFFSET: 0x0070B330
void CGGameObject_C_Type_TransportBase::PostInitWithStats() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }
}

// VERIFY: f102a3a9 auto
// OFFSET: 0x00710190
void CGGameObject_C_Type_MapObjTransport::PostReenable() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    this->InitializeMovement();
}

// VERIFY: 6e818bac manual
// OFFSET: 0x0070C2B0
C3Vector CGGameObject_C_TypeBase::GetPosition() const {
    return this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
}

// VERIFY: 9c20623b manual
// OFFSET: 0x00712CF0
C3Vector CGGameObject_C::GetPosition() const {
    if (this->m_typeData) {
        return this->m_typeData->GetPosition();
    }

    return this->m_passenger.GetPosition(this->m_passenger.m_position);
}

// VERIFY: ca51d521 manual
// OFFSET: 0x0070BE90
int32_t Packet_SMSG_GAMEOBJECT_CUSTOM_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t anim;
    msg->Get(guid.m_guid);
    msg->Get(anim);

    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object && anim < 4) {
        object->PlayCustomAnim(anim);
    }

    return 1;
}

// VERIFY: 0f64befc auto
// OFFSET: 0x008A1420
bool CGGameObject_C_TypeBase::CanBeSolid() const {
    return true;
}

// VERIFY: c3a2df23 auto
// OFFSET: 0x008A1420
bool CGGameObject_C_TypeBase::ShouldFadeIn() const {
    return true;
}

// VERIFY: 3f057594 auto
// OFFSET: 0x0070BDB0
bool CGGameObject_C_TypeBase::CanHighlight() const {
    return this->CanUse();
}

// VERIFY: 092326ba auto
// OFFSET: 0x008E5250
int32_t CGGameObject_C_TypeBase::IsHighlightSuppressed() const {
    return 0;
}

// VERIFY: 8ee24f6a auto
// OFFSET: 0x00632050
void CGGameObject_C_TypeBase::OnUpdateFlags(uint32_t changed) {
}

// VERIFY: 296b109d auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::OnUpdateAnimProgress() {
}

// VERIFY: 6b1c2c32 auto
// OFFSET: 0x00653A10
void CGGameObject_C_TypeBase::UpdateState(int32_t oldState, int32_t newState) {
}

// VERIFY: 6dd7e229 auto
// OFFSET: 0x00959DE0
int32_t CGGameObject_C_TypeBase::Animate(float elapsed) {
    return 1;
}

// VERIFY: 13be4357 auto
// OFFSET: 0x0047ADD0
void CGGameObject_C_TypeBase::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
}

// VERIFY: 849dbeae auto
// OFFSET: 0x004D5FF0
float CGGameObject_C_TypeBase::GetBaseAlpha() const {
    return 1.0f;
}

// VERIFY: c27bdd8e auto
// OFFSET: 0x006B1B80
void CGGameObject_C_TypeBase::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
}

// VERIFY: 9e8df6c3 auto
// OFFSET: 0x008C8DE0
int32_t CGGameObject_C_TypeBase::HandleAnimInterrupted() {
    return 1;
}

// VERIFY: d522ab25 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::HandleAnimFinished() {
}

// VERIFY: 4d2d7931 auto
// OFFSET: 0x00632050
void CGGameObject_C_TypeBase::ActivateOneShotAnim(uint32_t anim) {
}

// VERIFY: a3b01406 auto
// OFFSET: 0x0070B220
const char* CGGameObject_C_TypeBase::DebugStatus() {
    return "";
}

// VERIFY: 9dd812d6 auto
// OFFSET: 0x006F1700
float CGGameObject_C_TypeBase::GetSpeed() const {
    return -1.0f;
}

// VERIFY: 4c00802d auto
// OFFSET: 0x00653A10
void CGGameObject_C_TypeBase::AddPassenger(CPassenger* passenger, int32_t a2) {
}

// VERIFY: 8d4e2b06 auto
// OFFSET: 0x004899F0
int32_t CGGameObject_C_TypeBase::IsPointInside(const C3Vector& point) const {
    return 0;
}

// VERIFY: d47c3162 auto
// OFFSET: 0x008E5250
CMapBaseObj* CGGameObject_C_TypeBase::GetMapObjectTransportID() const {
    return nullptr;
}

// VERIFY: 980ed0eb auto
// OFFSET: 0x00632050
void CGGameObject_C_TypeBase::PostInit(bool a1) {
}

// VERIFY: 8b5d6ada auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::PostInitWithStats() {
}

// VERIFY: d0d1bd54 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::Reenable() {
}

// VERIFY: 97ab290a auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::Disable() {
}

// VERIFY: 8e070758 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::PostReenable() {
}

// VERIFY: 247ea7ab auto
// OFFSET: 0x008E5250
ShipPath* CGGameObject_C_TypeBase::GetShipPath() const {
    return nullptr;
}

// VERIFY: 06edc0db auto
// OFFSET: 0x00653A10
void CGGameObject_C_TypeBase::UpdateMovement(uint32_t time, uint32_t elapsed) {
}

// VERIFY: a4cd3ea3 auto
// OFFSET: 0x00632050
void CGGameObject_C_TypeBase::UpdateDisplay(uint32_t time) {
}

// VERIFY: 9289b4b5 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::UpdateDisabled() {
}

// VERIFY: 9690f15d auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::StartInteraction() {
}

// VERIFY: abeac73c auto
// OFFSET: 0x005EEB70
void CGGameObject_C_TypeBase::CloseInteraction() {
}

// VERIFY: c9a9e833 auto
// OFFSET: 0x00632050
void CGGameObject_C_TypeBase::AddToolTipInfo(CGTooltip* tooltip) {
}

// VERIFY: 7648e923 auto
// OFFSET: 0x0070BDC0
uint32_t CGGameObject_C_TypeBase::GetSequenceTimeFromRealTime(uint32_t time) const {
    return time;
}

// VERIFY: 0abdada4 auto
// OFFSET: 0x008E5250
uint32_t CGGameObject_C_TypeBase::GetTransportUpdateTime() const {
    return 0;
}

// VERIFY: b57f809b auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_Null::CanUse() const {
    return false;
}

// VERIFY: 1a45ca41 auto
// OFFSET: 0x0070FCD0
void CGGameObject_C_TypeAnimated::UpdateDisabled() {
    this->ResetAnimState();
}

// VERIFY: 8cc82559 auto
// OFFSET: 0x0070CBA0
void CGGameObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_gameObject = reinterpret_cast<CGGameObjectData*>(storage + OBJECT_END);
    this->m_gameObjectMirror = saved + CGObject::TotalFieldsSaved();
}

// VERIFY: d7df8c50 manual
// OFFSET: 0x00714250
CGGameObject_C::CGGameObject_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_move.m_status.m_transportGuid, create.m_move.m_status.m_transportPosition, this->m_obj->m_guid), m_typeData(nullptr), m_stats(nullptr) {
    this->m_worldBounds.b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_worldBounds.t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_pendingOneShotAnim = 0;
    this->unk_020C = 0;
    this->m_lootEffect = nullptr;
    this->m_displayState = this->m_gameObject->m_state;
    *reinterpret_cast<uint64_t*>(&this->m_passenger.m_facing) = create.m_packedRotation;
    this->m_passenger.unk_002C |= 0x2;

    ClntObjMgrHideObject(this->m_obj->m_guid);

    if (create.m_updateFlags & 0x2) {
        this->unk_0200 = create.m_transportTime - time;
    }

    switch (this->m_gameObject->m_typeID) {
        case 0:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Door), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Door(this);
            break;
        case 1:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Button), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Button(this);
            break;
        case 3:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Chest), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Chest(this);
            break;
        case 6:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Trap), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Trap(this);
            break;
        case 12:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_AreaDamage), __FILE__, __LINE__, 0)) CGGameObject_C_Type_AreaDamage(this);
            break;
        case 2:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_QuestGiver), __FILE__, __LINE__, 0)) CGGameObject_C_Type_QuestGiver(this);
            break;
        case 4:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Binder), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Binder(this);
            break;
        case 5:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Generic), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Generic(this);
            break;
        case 7:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Chair), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Chair(this);
            break;
        case 8:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_SpellFocus), __FILE__, __LINE__, 0)) CGGameObject_C_Type_SpellFocus(this);
            break;
        case 9:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Text), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Text(this);
            break;
        case 10:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Goober), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Goober(this);
            break;
        case 11:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Transport), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Transport(this);
            break;
        case 35:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_TrapDoor), __FILE__, __LINE__, 0)) CGGameObject_C_Type_TrapDoor(this);
            break;
        case 13:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Camera), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Camera(this);
            break;
        case 14:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MapObj), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MapObj(this);
            break;
        case 15:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MapObjTransport), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MapObjTransport(this);
            break;
        case 16:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DuelArbiter), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DuelArbiter(this);
            break;
        case 25:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FishingHole), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FishingHole(this);
            break;
        case 17:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FishingNode), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FishingNode(this);
            break;
        case 18:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Ritual), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Ritual(this);
            break;
        case 19:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Mailbox), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Mailbox(this);
            break;
        case 22:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_SpellCaster), __FILE__, __LINE__, 0)) CGGameObject_C_Type_SpellCaster(this);
            break;
        case 23:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MeetingStone), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MeetingStone(this);
            break;
        case 24:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FlagStand), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FlagStand(this);
            break;
        case 26:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FlagDrop), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FlagDrop(this);
            break;
        case 27:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MiniGame), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MiniGame(this);
            break;
        case 29:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_CapturePoint), __FILE__, __LINE__, 0)) CGGameObject_C_Type_CapturePoint(this);
            break;
        case 30:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_AuraGenerator), __FILE__, __LINE__, 0)) CGGameObject_C_Type_AuraGenerator(this);
            break;
        case 31:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DungeonDifficulty), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DungeonDifficulty(this);
            break;
        case 32:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_BarberChair), __FILE__, __LINE__, 0)) CGGameObject_C_Type_BarberChair(this);
            break;
        case 33:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DestructibleBuilding), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DestructibleBuilding(this);
            break;
        case 34:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_GuildBank), __FILE__, __LINE__, 0)) CGGameObject_C_Type_GuildBank(this);
            break;
        default:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Null), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Null(this);
            SysMsgPrintf(STATUS_FATAL, 2, "BADBASEGAMEOBJECT|%d", this->m_gameObject->m_typeID);
            break;
    }

    this->m_effectAnimState = 0;
}

// VERIFY: 77926b2f auto
// OFFSET: 0x00712F30
void CGGameObject_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_passenger.PostInit(create.m_move.m_status.m_position);
    this->CGObject_C::PostInit(time, create, reenable);

    CM2Model* model = this->m_worldModel;

    if (model) {
        ClntObjMgrShowObject(this->m_obj->m_guid);
        model->SetEventCallback(reinterpret_cast<void*>(&CGGameObject_C::AnimEventCallback), this->m_obj->m_guid.m_guid);
        model->SetSequenceCallback(reinterpret_cast<void*>(&CGGameObject_C::SequenceCallback), this->m_obj->m_guid.m_guid);

        if (model->IsLoaded(0, 0)) {
            CM2Model* worldModel = this->m_worldModel;

            if (!(worldModel->m_shared->m_flags & 0x1)) {
                worldModel->WaitForLoad(nullptr);
            }

            CAaBox bounds = worldModel->m_shared->m_data->m_collisionBounds.m_extent;
            CWorldMath::TransformAABox(this->m_matrix, bounds, this->m_worldBounds);

            if (this->m_typeData->CanBeSolid()) {
                this->unk_020C = !this->m_worldBounds.IsEmpty();
            }
        }
    }

    this->m_typeData->PostInit(reenable);

    WOWGUID guid = this->m_obj->m_guid;
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(this->m_obj->m_entryID, guid, &GameObjectStatsCallback, this, false);

    if (stats) {
        this->LoadBaseObject(stats);
    }
}

// VERIFY: 1254baa1 auto
// OFFSET: 0x004268C0
bool CGGameObject_C_Type_Null::CanUseNow(GAME_ERROR_TYPE* error, float* distance) const {
    return false;
}

// VERIFY: 05dc13d2 manual
// OFFSET: 0x0070D900
CGGameObject_C_Type_WorldObj::~CGGameObject_C_Type_WorldObj() {
    if (this->m_worldObject) {
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = nullptr;
    }
}

// VERIFY: 155035c0 auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_WorldObj::CanBeSolid() const {
    return false;
}

// VERIFY: 4e81b6bf auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_WorldObj::CanHighlight() const {
    return false;
}

// VERIFY: 9ea49663 auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_WorldObj::CanUse() const {
    return false;
}

// VERIFY: 3f8abdcd auto
// OFFSET: 0x0095CB40
CMapBaseObj* CGGameObject_C_Type_WorldObj::GetMapObjectTransportID() const {
    return this->m_worldObject;
}

// VERIFY: f4c8758a manual
// OFFSET: 0x0070B270
void CGGameObject_C_Type_WorldObj::PostInitWithStats() {
    this->CreateWorldObj();
}

// VERIFY: dbdf67a5 manual
// OFFSET: 0x0070B280
void CGGameObject_C_Type_WorldObj::Disable() {
    this->DestroyWorldObj();
}

// VERIFY: ed9fece3 manual
// OFFSET: 0x0070B270
void CGGameObject_C_Type_WorldObj::PostReenable() {
    this->CreateWorldObj();
}

// VERIFY: 209f54dd manual
// OFFSET: 0x00712610
CGGameObject_C_Type_TransportBase::~CGGameObject_C_Type_TransportBase() {
    if (this->m_worldObject) {
        MovementRemoveTransport(this->m_owner);
    }
}

// VERIFY: 1e87d942 auto
// OFFSET: 0x007126A0
C3Vector CGGameObject_C_Type_TransportBase::GetPosition() const {
    return this->m_position;
}

// VERIFY: d5e93300 auto
// OFFSET: 0x0070B360
int32_t CGGameObject_C_Type_TransportBase::IsPointInside(const C3Vector& point) const {
    if (!this->m_worldObject) {
        return 1;
    }

    return World::ObjectTestConvexVolume(this->m_worldObject, point);
}

// VERIFY: ff1b15c0 auto
// OFFSET: 0x0070B330
void CGGameObject_C_Type_TransportBase::PostReenable() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }
}

// VERIFY: fab945c9 manual
// OFFSET: 0x007101C0
void CGGameObject_C_Type_MapObjTransport::UpdateState(int32_t oldState, int32_t newState) {
    uint32_t pausePointsIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 128);
    GameObjectStats* pausePointsStats = this->m_owner->m_stats;
    uint32_t pausePoints = (pausePointsStats && pausePointsIndex < 24) ? pausePointsStats->m_propValue[pausePointsIndex] : 0;

    if (!pausePoints) {
        return;
    }

    bool closed = newState == 1;

    if (oldState == newState) {
        this->m_shipPath.WaitAtNextPausePoint(this->unk_0030, !closed);
    }

    this->m_shipPath.WaitAtNextPausePoint(this->unk_0030, closed);
}

// VERIFY: 3805df66 auto
// OFFSET: 0x0070EC80
void CGGameObject_C_Type_DestructibleBuilding::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
    GameObject_C_ProcessAnimEvent(eventId, this->m_owner->m_gameObject->m_displayID, data, position, &this->m_sound);
}

// VERIFY: 427d6342 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_Type_DestructibleBuilding::CreateWorldObj() {
}

// VERIFY: 3411ab45 auto
// OFFSET: 0x005EEB70
void CGGameObject_C_Type_DestructibleBuilding::DestroyWorldObj() {
}

// VERIFY: 273ab916 manual
// OFFSET: 0x0070ECB0
bool CGGameObject_C_Type_GuildBank::Use() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guid = this->m_owner->m_obj->m_guid;
        CGPlayer_C::TalkToGuildBanker(guid);
    }

    return true;
}

// VERIFY: 1f9c8c22 manual
// OFFSET: 0x0070C9F0
void CGGameObject_C_Type_GuildBank::StartInteraction() {
    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(8);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 2;
    }
}

// VERIFY: cf09afa2 auto
// OFFSET: 0x004F58A0
CAaBox CGGameObject_C::GetWorldBounds() const {
    return this->m_worldBounds;
}

// VERIFY: 218fceeb manual
// OFFSET: 0x0070CBE0
void CGGameObject_C::UpdateWorldObject(int32_t a2) {
    this->m_matrix.Identity();
    this->m_matrix.Translate(this->GetPosition());
    this->m_matrix.Rotate(this->GetRotation());
    this->m_matrix.Scale(this->GetScale());
    CM2Model* model = this->m_worldModel;
    CAaBox box(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
    CAaSphere sphere;
    sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    sphere.r = 0.0f;
    C3Vector center(0.0f, 0.0f, 0.0f);

    if (model) {
        if (model->IsLoaded(0, 0)) {
            if (!(model->m_shared->m_flags & 0x1)) {
                model->WaitForLoad(nullptr);
            }

            CAaBox bounds = model->m_shared->m_data->m_collisionBounds.m_extent;
            CWorldMath::TransformAABox(this->m_matrix, bounds, this->m_worldBounds);
            box = model->GetBoundingBox();
            sphere = model->GetBoundingSphere();
            center = C3Vector((bounds.t.x + bounds.b.x) * 0.5f, (bounds.t.y + bounds.b.y) * 0.5f, 0.5f * (bounds.t.z + bounds.b.z));
        }

        model->m_fFlags |= 0x8000;
        model->m_worldTransform = this->m_matrix;
    }

    if (this->m_worldObject) {
        CMap::ObjectUpdate(this->m_worldObject, this->m_matrix, box, sphere, center, a2, 0xFFFFFFFF);
    }
}

// VERIFY: 68b8aeae auto
// OFFSET: 0x00712DB0
float CGGameObject_C::GetFacing() const {
    if (this->m_typeData) {
        return this->m_typeData->GetFacing();
    }

    return this->m_passenger.GetFacing(this->m_passenger.GetRawFacingFromRotation());
}

// VERIFY: e7d3bbca manual
// OFFSET: 0x00712DF0
float CGGameObject_C::GetRawFacing() const {
    if (this->m_typeData) {
        return this->m_typeData->GetRawFacing();
    }

    return this->m_passenger.GetRawFacingFromRotation();
}

// VERIFY: c3c2da56 auto
// OFFSET: 0x00712E20
C4Quaternion CGGameObject_C::GetRotation() const {
    return this->m_passenger.GetRotation();
}

// VERIFY: f3f1026a manual
// OFFSET: 0x00713F50
void CGGameObject_C::ModelLoaded(CM2Model* model) {
    CAaBox box = model->GetBoundingBox();
    this->m_height = box.t.z - box.b.z;

    if ((this->m_modelFlags & 0x40000) && !(this->m_modelFlags & 0x10000)) {
        this->UpdateWorldObject(0);
    }

    if (this->m_typeData->CanBeSolid()) {
        this->unk_020C = !this->m_worldBounds.IsEmpty();
    }

    ApplyGameObjectArtKit(model, this->m_gameObject->m_artKit);
    this->m_typeData->UpdateDisabled();
    this->UpdateInteractIconAttach();

    if (this->m_modelFlags & 0x400000) {
        this->m_modelFlags |= 0x400000;
        this->ShowLootEffect();
    }

    int32_t pending = this->m_pendingOneShotAnim;

    if (pending) {
        if (!this->IsObjectModelLoaded()) {
            this->m_pendingOneShotAnim = pending;
            model->m_fFlags |= 0x4;
            return;
        }

        this->m_typeData->ActivateOneShotAnim(s_oneShotAnims[pending]);
        this->m_pendingOneShotAnim = 0;
    }

    model->m_fFlags |= 0x4;
}

// VERIFY: e5a829d0 manual
// OFFSET: 0x0070C330
float CGGameObject_C_TypeBase::GetRawFacing() const {
    return this->m_owner->m_passenger.GetRawFacingFromRotation();
}

// VERIFY: 4dbf2cb2 auto
// OFFSET: 0x0070C310
float CGGameObject_C_TypeBase::GetFacing() const {
    CPassenger& passenger = this->m_owner->m_passenger;
    return passenger.GetFacing(passenger.GetRawFacingFromRotation());
}

// VERIFY: 9e6b8418 manual
// OFFSET: 0x0070C430
int32_t CGGameObject_C_TypeAnimated::HandleAnimInterrupted() {
    int32_t state = this->unk_0014;

    if (state == -1 || state == 3 || state == 1 || state == 6) {
        this->unk_0014 = -1;
        return 1;
    }

    ClntObjMgrUnlockObject(this->m_owner->m_obj->m_guid, __FILE__, __LINE__);
    return 0;
}

// VERIFY: 4f1c02d7 auto
// OFFSET: 0x0070EF30
LockRec* CGGameObject_C::GetLockRec() const {
    if (!this->m_stats) {
        return nullptr;
    }

    uint32_t lockId = this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 4));
    return lockId ? g_lockDB.GetRecord(lockId) : nullptr;
}

// VERIFY: eef7dd91 manual
// OFFSET: 0x0070F010
bool CGGameObject_C::IsValidTargetForSpell(const WOWGUID& item, int32_t spellId) const {
    LockRec* lock = this->GetLockRec();

    if (!lock) {
        return false;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return false;
    }

    CGItem_C* key = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    for (uint32_t i = 0; i < 3; i++) {
        if (spell.m_effect[i] != 33) {
            continue;
        }

        for (int32_t j = 0; j < 8; j++) {
            switch (lock->m_type[j]) {
                case 1:
                    if (!key || static_cast<int32_t>(key->m_obj->m_entryID) != lock->m_index[j]) {
                        continue;
                    }

                    break;

                case 2:
                    if (spell.m_effectMiscValue[i] != lock->m_index[j]) {
                        continue;
                    }

                    break;

                case 3:
                    if (spell.m_id != lock->m_index[j]) {
                        continue;
                    }

                    break;

                default:
                    continue;
            }

            if (this->IsValidOpenAction(lock->m_action[j])) {
                return true;
            }
        }
    }

    return false;
}

// VERIFY: 81b3c5cd auto
// OFFSET: 0x0070CE10
int32_t CGGameObject_C::CanChangeCursor() const {
    if (!this->m_typeData->CanUse()) {
        return 0;
    }

    if (!this->m_stats || !this->m_stats->m_iconName || !*this->m_stats->m_iconName) {
        return this->m_typeData->CanChangeCursor();
    }

    char name[40];

    if (this->m_typeData->CanUseNow(nullptr, nullptr, nullptr)) {
        SStrCopy(name, this->m_stats->m_iconName, sizeof(name));
    } else {
        SStrPrintf(name, sizeof(name), "Unable%s", this->m_stats->m_iconName);
    }

    int32_t mode = CursorGetModeFromName(name);
    return mode ? mode : 53;
}

// VERIFY: 9f6a62fd manual
// OFFSET: 0x00711AB0
void CGGameObject_C_Type_TransportBase::AddPassenger(CPassenger* passenger, int32_t a2) {
    passenger->m_link.Unlink();
    this->m_passengers.LinkToTail(passenger);

    if (a2 && (passenger->unk_002C & 1) && (!this->m_worldObject || !World::ObjectIsLoaded(this->m_worldObject))) {
        static_cast<CMovementShared*>(passenger)->SetSplineEnabled();
    }

    if (*passenger->m_ownerGuid == CGUnit_C::s_activeMover) {
        MovementSetTransportUpdateTime(this->m_transportUpdateTime);
    }
}
