#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC19D8
int32_t CSimpleAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499F8
int32_t CSimpleAnim::s_objectType;

// STATIC: 0x00AC138C
const char* CSimpleAnim::s_objectTypeName = "Animation";

// INLINED
int32_t CSimpleAnim::GetObjectType() {
    if (!CSimpleAnim::s_objectType) {
        CSimpleAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAnim::s_objectType;
}

// VERIFY: 0e40cb2a auto
// OFFSET: 0x00498CB0
bool CSimpleAnim::IsA(int32_t type) {
    return type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 0489e386 auto
// OFFSET: 0x00498CE0
bool CSimpleAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 80ff8422 auto
// OFFSET: 0x00498D30
const char* CSimpleAnim::GetObjectTypeName() {
    return CSimpleAnim::s_objectTypeName;
}

// VERIFY: a447b9f3 auto
// OFFSET: 0x00498D40
int32_t CSimpleAnim::GetScriptMetaTable() {
    return CSimpleAnim::s_metatable;
}
