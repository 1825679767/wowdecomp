#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/DynamicObject.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

class BlizzardObject;
class CClientObjCreate;

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:
    static void Initialize();
    /* 00D8 */ CPassenger m_passenger;
    /* 0108 */ TSList<void> m_dynamicObjectMirrorLists [6];
    /* 0150 */ uint32_t unk_0150;
    /* 0154 */ float unk_0154;
    /* 0158 */ BlizzardObject* m_blizzardVisual;
    /* 015C */ SE2SoundKitObject m_soundKit;

    ~CGDynamicObject_C() override;
    virtual C3Vector GetPosition() const;
    virtual C3Vector GetRawPosition() const;
    virtual float GetFacing() const;
    virtual C44Matrix GetMatrix() const;
    virtual void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3);

    CGDynamicObject_C(uint32_t time, const CClientObjCreate& create);
    void HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time);
    void ObjectVisKitProc();
    void PostInit(uint32_t time, const CClientObjCreate& create, bool a3);
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void UpdateDisplay(uint32_t time);
};

#endif
