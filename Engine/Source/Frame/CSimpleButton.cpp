#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D528
int32_t CSimpleButton::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE650
int32_t CSimpleButton::s_objectType;

// STATIC: 0x00B2CD1C
const char* CSimpleButton::s_objectTypeName = "Button";

// INLINED
int32_t CSimpleButton::GetObjectType() {
    if (!CSimpleButton::s_objectType) {
        CSimpleButton::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleButton::s_objectType;
}

// OFFSET: 0x009621E0
bool CSimpleButton::IsA(int32_t type) {
    return type == CSimpleButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00962220
bool CSimpleButton::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// VERIFY: 6244a24f auto
// OFFSET: 0x0096F1E0
const char* CSimpleButton::GetObjectTypeName() {
    return CSimpleButton::s_objectTypeName;
}

// VERIFY: ea565c62 auto
// OFFSET: 0x0096F1F0
int32_t CSimpleButton::GetScriptMetaTable() {
    return CSimpleButton::s_metatable;
}
