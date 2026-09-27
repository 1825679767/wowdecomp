#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CE88
int32_t CSimpleSlider::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE4D4
int32_t CSimpleSlider::s_objectType;

// STATIC: 0x00B2CCF4
const char* CSimpleSlider::s_objectTypeName = "Slider";

// INLINED
int32_t CSimpleSlider::GetObjectType() {
    if (!CSimpleSlider::s_objectType) {
        CSimpleSlider::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleSlider::s_objectType;
}

// VERIFY: a4db6b4a auto
// OFFSET: 0x0096BED0
const char* CSimpleSlider::GetObjectTypeName() {
    return CSimpleSlider::s_objectTypeName;
}

// VERIFY: b56e9ccb auto
// OFFSET: 0x0096BEE0
int32_t CSimpleSlider::GetScriptMetaTable() {
    return CSimpleSlider::s_metatable;
}

// VERIFY: d8c76476 auto
// OFFSET: 0x0096BF00
bool CSimpleSlider::IsA(int32_t type) {
    return type == CSimpleSlider::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 6fe163ff auto
// OFFSET: 0x0096BF40
bool CSimpleSlider::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleSlider::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
