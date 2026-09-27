#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC154C
int32_t CSimpleFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00B49984
int32_t CSimpleFrame::s_objectType;

// STATIC: 0x00AC1260
const char* CSimpleFrame::s_objectTypeName = "Frame";

// VERIFY: f47c2198 auto
// OFFSET: 0x0049CAB0
int32_t CSimpleFrame::GetObjectType() {
    if (!CSimpleFrame::s_objectType) {
        CSimpleFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFrame::s_objectType;
}

// VERIFY: bfa0f3ed auto
// OFFSET: 0x004926C0
bool CSimpleFrame::IsA(int32_t type) {
    return type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: bb492e3f auto
// OFFSET: 0x004926F0
bool CSimpleFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleFrame::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 734071a8 auto
// OFFSET: 0x00492750
const char* CSimpleFrame::GetObjectTypeName() {
    return CSimpleFrame::s_objectTypeName;
}

// VERIFY: 3fde1784 auto
// OFFSET: 0x00492760
int32_t CSimpleFrame::GetScriptMetaTable() {
    return CSimpleFrame::s_metatable;
}

// VERIFY: 1e528e71 auto
// OFFSET: 0x0048E970
void CSimpleFrame::SetFrameFlag(uint32_t flag, bool set) {
    if (set) {
        this->m_frameFlags |= flag;
    } else {
        this->m_frameFlags &= ~flag;
    }
}

// VERIFY: c5f65169 auto
// OFFSET: 0x0048E9A0
void CSimpleFrame::Raise() {
    this->m_simpleTop->RaiseFrame(this, 1);
}

// VERIFY: b74e77a5 auto
// OFFSET: 0x0048EAC0
void CSimpleFrame::GetHitRectInsets(float& left, float& right, float& top, float& bottom) {
    left = this->m_hitRectInsets.minX;
    right = this->m_hitRectInsets.maxX;
    top = this->m_hitRectInsets.maxY;
    bottom = this->m_hitRectInsets.minY;
}

// VERIFY: 7a64124f auto
// OFFSET: 0x0048EB00
void CSimpleFrame::SetClampRectInsets(float left, float right, float top, float bottom) {
    this->m_clampRectInsets.minX = left;
    this->m_clampRectInsets.maxX = right;
    this->m_clampRectInsets.maxY = top;
    this->m_clampRectInsets.minY = bottom;
}

// VERIFY: 31db8368 auto
// OFFSET: 0x0048EB70
bool CSimpleFrame::IsDragging() {
    return this->m_isMouseDragging || (this->m_parent && this->m_parent->IsDragging());
}

// VERIFY: 0a1a8e02 manual
// OFFSET: 0x0048ECF0
void CSimpleFrame::AllowAttributeChanges(int32_t allow) {
    if (allow) {
        this->m_frameFlags |= 0x8000;
        this->m_simpleTop->RegisterForCleanup(this);
    } else {
        this->m_frameFlags &= ~0x8000u;
    }
}

// VERIFY: 0b620746 manual
// OFFSET: 0x0048ED30
int32_t CSimpleFrame::AttributeChangesAllowed() {
    for (CSimpleFrame* frame = this; frame; frame = frame->m_parent) {
        if (frame->m_frameFlags & 0x8000) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 8faa6b6f manual
// OFFSET: 0x00490600
void CSimpleFrame::EnableDrawLayer(uint32_t drawLayer) {
    this->m_drawLayerEnabled[drawLayer] = 1;
    this->NotifyDrawLayerChanged(drawLayer);
}

// VERIFY: 65896940 manual
// OFFSET: 0x00490620
void CSimpleFrame::DisableDrawLayer(uint32_t drawLayer) {
    this->m_drawLayerEnabled[drawLayer] = 0;
    this->NotifyDrawLayerChanged(drawLayer);
}
