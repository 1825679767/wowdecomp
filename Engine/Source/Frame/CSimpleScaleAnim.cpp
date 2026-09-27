#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1A44
int32_t CSimpleScaleAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499EC
int32_t CSimpleScaleAnim::s_objectType;

// STATIC: 0x00AC1398
const char* CSimpleScaleAnim::s_objectTypeName = "Scale";

// INLINED
int32_t CSimpleScaleAnim::GetObjectType() {
    if (!CSimpleScaleAnim::s_objectType) {
        CSimpleScaleAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleScaleAnim::s_objectType;
}

// VERIFY: d7142d24 auto
// OFFSET: 0x00498F70
bool CSimpleScaleAnim::IsA(int32_t type) {
    return type == CSimpleScaleAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 0caaa318 auto
// OFFSET: 0x00498FA0
bool CSimpleScaleAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleScaleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: b62e6600 auto
// OFFSET: 0x00499000
const char* CSimpleScaleAnim::GetObjectTypeName() {
    return CSimpleScaleAnim::s_objectTypeName;
}

// VERIFY: f86ff650 auto
// OFFSET: 0x00499010
int32_t CSimpleScaleAnim::GetScriptMetaTable() {
    return CSimpleScaleAnim::s_metatable;
}
