#include "DynamicObject_C.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "_Triage/CClientObjCreate.h"

// VERIFY: 711b9f81 manual
// OFFSET: 0x00704F20
static void AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGDynamicObject_C* object = static_cast<CGDynamicObject_C*>(ClntObjMgrObjectPtr(WOWGUID{ param }, TYPEMASK_DYNAMICOBJECT, __FILE__, __LINE__));

    if (object) {
        object->HandleAnimEvent(eventId, data, position, time);
    }
}

// VERIFY: e474faa1 manual
// OFFSET: 0x00704AF0
static void AnimFinishedCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (interrupted) {
        return;
    }

    CGDynamicObject_C* object = static_cast<CGDynamicObject_C*>(ClntObjMgrObjectPtr(WOWGUID{ param }, TYPEMASK_DYNAMICOBJECT, __FILE__, __LINE__));

    if (!object) {
        return;
    }

    if (object->unk_0150 & 0x1) {
        object->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 158, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else {
        object->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
}

// VERIFY: e898266b manual
// OFFSET: 0x007053A0
CGDynamicObject_C::CGDynamicObject_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_move.m_status.m_transportGuid, create.m_move.m_status.m_transportPosition, this->m_obj->m_guid), unk_0154(1.0f), m_blizzardVisual(nullptr) {
    this->unk_0150 &= ~0x3;
    this->m_passenger.m_facing = create.m_move.m_status.m_transportFacing;
}

// VERIFY: b6793ae9 manual
// OFFSET: 0x007054E0
CGDynamicObject_C::~CGDynamicObject_C() {
    if (this->m_blizzardVisual) {
        SpellVisualsBlizzardStop(&this->m_blizzardVisual);
    }

    SI2::StopSound(&this->m_soundKit, 0, 3.0f, 1);
}

// VERIFY: 2fd08b4c manual
// OFFSET: 0x00705230
void CGDynamicObject_C::PostInit(uint32_t time, const CClientObjCreate& create, bool a3) {
    this->m_passenger.PostInit(create.m_move.m_status.m_position);

    if (!this->GetTransportGUID()) {
        this->m_passenger.m_facing = create.m_move.m_status.m_facing;
    }

    this->CGObject_C::PostInit(time, create, a3);

    int32_t inTransit = Spell_C_IsSpellInTransit(this->m_dynamicObject->m_caster, this->m_dynamicObject->m_spellID, this->m_dynamicObject->m_castTime);
    this->unk_0150 ^= (this->unk_0150 ^ (inTransit * 2)) & 0x2;

    if (this->GetObjectModel()) {
        this->GetObjectModel()->SetEventCallback(reinterpret_cast<void*>(&AnimEventCallback), this->m_obj->m_guid.m_guid);
        this->GetObjectModel()->SetSequenceCallback(reinterpret_cast<void*>(&AnimFinishedCallback), this->m_obj->m_guid.m_guid);

        if (this->unk_0150 & 0x2) {
            this->GetObjectModel()->SetEmittersEnabled(0);
            this->GetObjectModel()->SetRibbonsEnabled(0);
        }
    }

    this->ObjectVisKitProc();

    if (this->m_dynamicObject->m_bytes[0] != 2 || this->m_dynamicObject->m_caster != ClntObjMgrGetActivePlayer()) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && this->m_obj->m_guid.m_guid == player->GetFarSightGuid()) {
        player->SetFarSightFocus(this);
    }
}

// VERIFY: fe375946 manual
// OFFSET: 0x00704CC0
C44Matrix CGDynamicObject_C::GetMatrix() const {
    C44Matrix matrix;
    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
    matrix.RotateAroundZ(this->GetRawFacing());

    if (this->GetTransportGUID()) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ this->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (transport) {
            matrix *= transport->GetMatrix();
        }
    }

    return matrix;
}

// VERIFY: 35b0255d auto
// OFFSET: 0x00706500
C3Vector CGDynamicObject_C::GetRawPosition() const {
    return this->m_passenger.m_position;
}

// VERIFY: 74221d0d auto
// OFFSET: 0x007049D0
void CGDynamicObject_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);

    if (this->unk_0150 & 2) {
        a3 = 1;
    }
}

// VERIFY: 7501382d manual
// OFFSET: 0x007064E0
C3Vector CGDynamicObject_C::GetPosition() const {
    return this->m_passenger.GetPosition(this->m_passenger.m_position);
}

// VERIFY: 57b1635b auto
// OFFSET: 0x0070CBA0
void CGDynamicObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_dynamicObject = reinterpret_cast<CGDynamicObjectData*>(storage + OBJECT_END);
    this->m_dynamicObjectMirror = saved + CGObject::TotalFieldsSaved();
}
