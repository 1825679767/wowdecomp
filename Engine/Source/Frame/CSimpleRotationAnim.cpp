#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1A20
int32_t CSimpleRotationAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499F0
int32_t CSimpleRotationAnim::s_objectType;

// STATIC: 0x00AC1394
const char* CSimpleRotationAnim::s_objectTypeName = "Rotation";

// INLINED
int32_t CSimpleRotationAnim::GetObjectType() {
    if (!CSimpleRotationAnim::s_objectType) {
        CSimpleRotationAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleRotationAnim::s_objectType;
}

// VERIFY: 1314989f auto
// OFFSET: 0x00498EC0
bool CSimpleRotationAnim::IsA(int32_t type) {
    return type == CSimpleRotationAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 2bc3e3cb auto
// OFFSET: 0x00498EF0
bool CSimpleRotationAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleRotationAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: eb41dcd6 auto
// OFFSET: 0x00498F50
const char* CSimpleRotationAnim::GetObjectTypeName() {
    return CSimpleRotationAnim::s_objectTypeName;
}

// VERIFY: 3bdde5e1 auto
// OFFSET: 0x00498F60
int32_t CSimpleRotationAnim::GetScriptMetaTable() {
    return CSimpleRotationAnim::s_metatable;
}
