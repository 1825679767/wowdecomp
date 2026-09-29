#include "TabardModelFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00ACF580
const char* CGTabardModelFrame::s_objectTypeName = "TabardModel";

// STATIC: 0x00ACF584
int32_t CGTabardModelFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00C0E520
int32_t CGTabardModelFrame::s_objectType;

// STATIC: 0x00C0E524
CDataAllocator CGTabardModelFrame::s_tabardModelHeap(sizeof(CGTabardModelFrame), 1);

// INLINED
void CGTabardModelFrame::operator delete(void* ptr) {
    CGTabardModelFrame::s_tabardModelHeap.PutData(ptr, nullptr, 0);
}

// INLINED
int32_t CGTabardModelFrame::GetObjectType() {
    if (!CGTabardModelFrame::s_objectType) {
        CGTabardModelFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGTabardModelFrame::s_objectType;
}

// VERIFY: adbac47f auto
// OFFSET: 0x00514350
CSimpleFrame* CGTabardModelFrame::Create(CSimpleFrame* parent) {
    if (CGTabardModelFrame::s_tabardModelHeap.m_bytesPerData < sizeof(CGTabardModelFrame)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGTabardModelFrame::s_tabardModelHeap.GetData(0, typeid(CGTabardModelFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGTabardModelFrame(parent) : nullptr;
}

// VERIFY: 89cfb882 auto
// OFFSET: 0x00598F50
CGTabardModelFrame::CGTabardModelFrame(CSimpleFrame* parent) : CGCharacterModelBase(parent) {
}

// VERIFY: c940ae6d auto
// OFFSET: 0x00598F80
const char* CGTabardModelFrame::GetObjectTypeName() {
    return CGTabardModelFrame::s_objectTypeName;
}

// VERIFY: d1e0defb auto
// OFFSET: 0x00598F90
int32_t CGTabardModelFrame::GetScriptMetaTable() {
    return CGTabardModelFrame::s_metatable;
}

// VERIFY: 9621214d auto
// OFFSET: [0x005990C0, 0x00599190]
CGTabardModelFrame::~CGTabardModelFrame() {
}

// VERIFY: 0e4ca341 auto
// OFFSET: 0x005990D0
bool CGTabardModelFrame::IsA(int32_t type) {
    return type == CGTabardModelFrame::s_objectType || type == CGCharacterModelBase::s_objectType || type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 887b43f8 auto
// OFFSET: 0x00599120
bool CGTabardModelFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGTabardModelFrame::s_objectTypeName, STORM_MAX_STR) || CGCharacterModelBase::IsA(typeName);
}

// VERIFY: 0dd14137 auto
// OFFSET: 0x005995D0
void CGTabardModelFrame::OnModelLoaded(CM2Model* model) {
    CGCharacterModelBase::OnModelLoaded(model);

    CCamera* camera = this->m_camera;

    if (!camera) {
        return;
    }

    M2SequenceInfo info;
    info.m_radius = 0.0f;
    model->GetSequenceInfo(0, 0, info);

    C3Vector coord(0.0f, (info.m_bounds.t.y - info.m_bounds.b.y) * 0.33000001f, (info.m_bounds.t.z - info.m_bounds.b.z) * 0.5f);
    CameraSetCoord(camera, 8, coord, 0);
}
