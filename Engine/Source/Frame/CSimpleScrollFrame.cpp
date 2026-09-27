#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CED8
int32_t CSimpleScrollFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE4BC
int32_t CSimpleScrollFrame::s_objectType;

// STATIC: 0x00B2CCF0
const char* CSimpleScrollFrame::s_objectTypeName = "ScrollFrame";

// INLINED
int32_t CSimpleScrollFrame::GetObjectType() {
    if (!CSimpleScrollFrame::s_objectType) {
        CSimpleScrollFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleScrollFrame::s_objectType;
}

// VERIFY: 43378d4d auto
// OFFSET: 0x0096B090
const char* CSimpleScrollFrame::GetObjectTypeName() {
    return CSimpleScrollFrame::s_objectTypeName;
}

// VERIFY: 8fad3c2b auto
// OFFSET: 0x0096B0A0
int32_t CSimpleScrollFrame::GetScriptMetaTable() {
    return CSimpleScrollFrame::s_metatable;
}

// VERIFY: 9fdd996a auto
// OFFSET: 0x0096B250
bool CSimpleScrollFrame::IsA(int32_t type) {
    return type == CSimpleScrollFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 8a8d8651 auto
// OFFSET: 0x0096B290
bool CSimpleScrollFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleScrollFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
