#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC19EC
int32_t CSimpleTranslationAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00B499F4
int32_t CSimpleTranslationAnim::s_objectType;

// STATIC: 0x00AC1390
const char* CSimpleTranslationAnim::s_objectTypeName = "Translation";

// INLINED
int32_t CSimpleTranslationAnim::GetObjectType() {
    if (!CSimpleTranslationAnim::s_objectType) {
        CSimpleTranslationAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleTranslationAnim::s_objectType;
}

// VERIFY: 2d763647 auto
// OFFSET: 0x00498E10
bool CSimpleTranslationAnim::IsA(int32_t type) {
    return type == CSimpleTranslationAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 700dd37a auto
// OFFSET: 0x00498E40
bool CSimpleTranslationAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleTranslationAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: d1d66ef7 auto
// OFFSET: 0x00498EA0
const char* CSimpleTranslationAnim::GetObjectTypeName() {
    return CSimpleTranslationAnim::s_objectTypeName;
}

// VERIFY: c14e6432 auto
// OFFSET: 0x00498EB0
int32_t CSimpleTranslationAnim::GetScriptMetaTable() {
    return CSimpleTranslationAnim::s_metatable;
}
