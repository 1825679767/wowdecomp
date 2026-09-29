#include "DressUpModelFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"

// STATIC: 0x00ACF54C
const char* CGDressUpModelFrame::s_objectTypeName = "DressUpModel";

// STATIC: 0x00ACF568
int32_t CGDressUpModelFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00C0E4F4
int32_t CGDressUpModelFrame::s_objectType;

// STATIC: 0x00C0E500
CDataAllocator CGDressUpModelFrame::s_dressUpModelHeap(sizeof(CGDressUpModelFrame), 5);

// INLINED
void CGDressUpModelFrame::operator delete(void* ptr) {
    CGDressUpModelFrame::s_dressUpModelHeap.PutData(ptr, nullptr, 0);
}

// INLINED
int32_t CGDressUpModelFrame::GetObjectType() {
    if (!CGDressUpModelFrame::s_objectType) {
        CGDressUpModelFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGDressUpModelFrame::s_objectType;
}

// VERIFY: 3888121b auto
// OFFSET: 0x00514300
CSimpleFrame* CGDressUpModelFrame::Create(CSimpleFrame* parent) {
    if (CGDressUpModelFrame::s_dressUpModelHeap.m_bytesPerData < sizeof(CGDressUpModelFrame)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGDressUpModelFrame::s_dressUpModelHeap.GetData(0, typeid(CGDressUpModelFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGDressUpModelFrame(parent) : nullptr;
}

// VERIFY: 62525612 auto
// OFFSET: 0x00597B50
CGDressUpModelFrame::CGDressUpModelFrame(CSimpleFrame* parent) : CGCharacterModelBase(parent) {
    this->m_component = nullptr;
}

// VERIFY: cc335fc2 auto
// OFFSET: 0x00597B80
const char* CGDressUpModelFrame::GetObjectTypeName() {
    return CGDressUpModelFrame::s_objectTypeName;
}

// VERIFY: 44b27d0d auto
// OFFSET: 0x00597B90
int32_t CGDressUpModelFrame::GetScriptMetaTable() {
    return CGDressUpModelFrame::s_metatable;
}

// VERIFY: 76453306 manual
// OFFSET: [0x00597D10, 0x00597DB0]
CGDressUpModelFrame::~CGDressUpModelFrame() {
    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = nullptr;
    }
}

// OFFSET: 0x00597D20
bool CGDressUpModelFrame::IsA(int32_t type) {
    return type == CGDressUpModelFrame::s_objectType || type == CGCharacterModelBase::s_objectType || type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00597D70
bool CGDressUpModelFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGDressUpModelFrame::s_objectTypeName, STORM_MAX_STR) || CGCharacterModelBase::IsA(typeName);
}

// VERIFY: 79d529ac manual
// OFFSET: 0x00597F40
void CGDressUpModelFrame::UpdateModel() {
    CGCharacterModelBase::UpdateModel();

    if (!this->m_component) {
        return;
    }

    uint32_t ready = this->m_component->RenderPrep(0) & 1;
    CM2Model* model = this->m_component->m_data.m_model;

    if (model->m_attachParent) {
        model->m_fFlags = (model->m_fFlags & ~0x80u) | (ready << 7);
        model->m_fFlags = (model->m_fFlags & ~0x20000u) | (ready << 17);
    } else {
        model->m_fFlags = (model->m_fFlags & ~0x8u) | (ready << 3);
        model->m_fFlags = (model->m_fFlags & ~0x10000u) | (ready << 16);
    }
}
