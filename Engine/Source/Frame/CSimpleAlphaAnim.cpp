#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1AB0
int32_t CSimpleAlphaAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499E0
int32_t CSimpleAlphaAnim::s_objectType;

// STATIC: 0x00AC13A4
const char* CSimpleAlphaAnim::s_objectTypeName = "Alpha";

// INLINED
int32_t CSimpleAlphaAnim::GetObjectType() {
    if (!CSimpleAlphaAnim::s_objectType) {
        CSimpleAlphaAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAlphaAnim::s_objectType;
}

// VERIFY: 5e0d6557 auto
// OFFSET: 0x00499670
bool CSimpleAlphaAnim::IsA(int32_t type) {
    return type == CSimpleAlphaAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: b32b8c67 auto
// OFFSET: 0x004996A0
bool CSimpleAlphaAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAlphaAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 76fd86c5 auto
// OFFSET: 0x00499700
const char* CSimpleAlphaAnim::GetObjectTypeName() {
    return CSimpleAlphaAnim::s_objectTypeName;
}

// VERIFY: c0bf94aa auto
// OFFSET: 0x00499710
int32_t CSimpleAlphaAnim::GetScriptMetaTable() {
    return CSimpleAlphaAnim::s_metatable;
}
