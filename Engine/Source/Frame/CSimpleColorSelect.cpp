#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CDB0
int32_t CSimpleColorSelect::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE51C
int32_t CSimpleColorSelect::s_objectType;

// STATIC: 0x00B2CD18
const char* CSimpleColorSelect::s_objectTypeName = "ColorSelect";

// INLINED
int32_t CSimpleColorSelect::GetObjectType() {
    if (!CSimpleColorSelect::s_objectType) {
        CSimpleColorSelect::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleColorSelect::s_objectType;
}

// VERIFY: 5165e069 auto
// OFFSET: 0x0096DC30
const char* CSimpleColorSelect::GetObjectTypeName() {
    return CSimpleColorSelect::s_objectTypeName;
}

// VERIFY: 2cca8bdf auto
// OFFSET: 0x0096DC40
int32_t CSimpleColorSelect::GetScriptMetaTable() {
    return CSimpleColorSelect::s_metatable;
}

// VERIFY: 33027447 auto
// OFFSET: 0x0096DE40
bool CSimpleColorSelect::IsA(int32_t type) {
    return type == CSimpleColorSelect::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 6a35e770 auto
// OFFSET: 0x0096DE80
bool CSimpleColorSelect::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleColorSelect::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
