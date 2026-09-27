#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D414
int32_t CSimpleCheckbox::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE458
int32_t CSimpleCheckbox::s_objectType;

// STATIC: 0x00B2CBD8
const char* CSimpleCheckbox::s_objectTypeName = "CheckButton";

// INLINED
int32_t CSimpleCheckbox::GetObjectType() {
    if (!CSimpleCheckbox::s_objectType) {
        CSimpleCheckbox::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleCheckbox::s_objectType;
}

// VERIFY: bb4c9a26 auto
// OFFSET: 0x00962180
const char* CSimpleCheckbox::GetObjectTypeName() {
    return CSimpleCheckbox::s_objectTypeName;
}

// VERIFY: d075bc3c auto
// OFFSET: 0x00962190
int32_t CSimpleCheckbox::GetScriptMetaTable() {
    return CSimpleCheckbox::s_metatable;
}

// OFFSET: 0x00962260
bool CSimpleCheckbox::IsA(int32_t type) {
    return type == CSimpleCheckbox::s_objectType || type == CSimpleButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x009622A0
bool CSimpleCheckbox::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleCheckbox::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
