#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1A9C
int32_t CSimplePathAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499E4
int32_t CSimplePathAnim::s_objectType;

// STATIC: 0x00AC13A0
const char* CSimplePathAnim::s_objectTypeName = "Path";

// INLINED
int32_t CSimplePathAnim::GetObjectType() {
    if (!CSimplePathAnim::s_objectType) {
        CSimplePathAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimplePathAnim::s_objectType;
}

// VERIFY: efbd8198 auto
// OFFSET: 0x00499940
bool CSimplePathAnim::IsA(int32_t type) {
    return type == CSimplePathAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 7c5c147c auto
// OFFSET: 0x00499970
bool CSimplePathAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimplePathAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 932c67c7 auto
// OFFSET: 0x004999D0
const char* CSimplePathAnim::GetObjectTypeName() {
    return CSimplePathAnim::s_objectTypeName;
}

// VERIFY: 62e4eeb6 auto
// OFFSET: 0x004999E0
int32_t CSimplePathAnim::GetScriptMetaTable() {
    return CSimplePathAnim::s_metatable;
}
