#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1A70
int32_t CSimpleControlPoint::s_metatable = LUA_NOREF;

// STATIC: 0x00B499E8
int32_t CSimpleControlPoint::s_objectType;

// STATIC: 0x00AC139C
const char* CSimpleControlPoint::s_objectTypeName = "ControlPoint";

// INLINED
int32_t CSimpleControlPoint::GetObjectType() {
    if (!CSimpleControlPoint::s_objectType) {
        CSimpleControlPoint::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleControlPoint::s_objectType;
}

// VERIFY: 387d7ed2 auto
// OFFSET: 0x00498740
bool CSimpleControlPoint::IsA(int32_t type) {
    return type == CSimpleControlPoint::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: c865c60a auto
// OFFSET: 0x00498770
bool CSimpleControlPoint::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleControlPoint::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 70589995 auto
// OFFSET: 0x004987C0
const char* CSimpleControlPoint::GetObjectTypeName() {
    return CSimpleControlPoint::s_objectTypeName;
}

// VERIFY: 70e114c4 auto
// OFFSET: 0x004987D0
int32_t CSimpleControlPoint::GetScriptMetaTable() {
    return CSimpleControlPoint::s_metatable;
}
