#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1AB4
int32_t CSimpleAnimGroup::s_metatable = LUA_NOREF;

// STATIC: 0x00B499DC
int32_t CSimpleAnimGroup::s_objectType;

// STATIC: 0x00AC13A8
const char* CSimpleAnimGroup::s_objectTypeName = "AnimationGroup";

// INLINED
int32_t CSimpleAnimGroup::GetObjectType() {
    if (!CSimpleAnimGroup::s_objectType) {
        CSimpleAnimGroup::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAnimGroup::s_objectType;
}

// VERIFY: 43ebb9ad auto
// OFFSET: 0x00499FC0
bool CSimpleAnimGroup::IsA(int32_t type) {
    return type == CSimpleAnimGroup::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 66a44196 auto
// OFFSET: 0x00499FF0
bool CSimpleAnimGroup::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAnimGroup::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: dc9c3f61 auto
// OFFSET: 0x0049A040
const char* CSimpleAnimGroup::GetObjectTypeName() {
    return CSimpleAnimGroup::s_objectTypeName;
}

// VERIFY: c5ac6209 auto
// OFFSET: 0x0049A050
int32_t CSimpleAnimGroup::GetScriptMetaTable() {
    return CSimpleAnimGroup::s_metatable;
}
