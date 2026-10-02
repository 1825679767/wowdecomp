#include "WTOBJECT.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Model2/M2Model.h"
#include <typeinfo>

// STATIC: 0x00D38074
static uint32_t s_count;

// STATIC: 0x00D38078
static CDataAllocator s_freeSwings(sizeof(SWING), 8);

// STATIC: 0x00D3808C
static CDataAllocator s_freeTrails(sizeof(WTOBJECT), 128);

// INLINED
WTOBJECT::~WTOBJECT() {
    if (this->m_model) {
        CM2Model* model = this->m_model;
        model->m_fFlags |= 0x20;
        model->m_drawCallback = nullptr;
        model->m_drawArg = nullptr;
        this->m_model->Release();
    }

    if (this->m_swing) {
        s_freeSwings.PutData(this->m_swing, nullptr, 0);
    }
}

// VERIFY: c3c7b136 auto
// OFFSET: 0x007E4BC0
void WeaponTrailsShutdown() {
    s_freeTrails.Clear(typeid(WTOBJECT).raw_name(), SLOG_OBJECT, 1);
    s_freeSwings.Clear(typeid(SWING).raw_name(), SLOG_OBJECT, 1);
}

// VERIFY: 38b83c0d auto
// OFFSET: 0x007E4BF0
WTOBJECT* WeaponTrailCreate(CM2Model* model) {
    s_count++;
    WTOBJECT* trail = static_cast<WTOBJECT*>(s_freeTrails.GetData(0, typeid(WTOBJECT).raw_name(), SLOG_OBJECT));

    if (trail) {
        trail->m_swing = nullptr;
        trail->m_model = nullptr;
    }

    trail->m_model = model;
    model->m_refCount++;
    return trail;
}

// VERIFY: 254754a7 auto
// OFFSET: 0x007E4CC0
void WeaponTrailClose(WTOBJECT* trail) {
    s_count--;
    s_freeTrails.Delete(trail);
}
