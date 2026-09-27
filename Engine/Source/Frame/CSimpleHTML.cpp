#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D208
int32_t CSimpleHTML::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE4EC
int32_t CSimpleHTML::s_objectType;

// STATIC: 0x00B2CCF8
const char* CSimpleHTML::s_objectTypeName = "SimpleHTML";

// INLINED
int32_t CSimpleHTML::GetObjectType() {
    if (!CSimpleHTML::s_objectType) {
        CSimpleHTML::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleHTML::s_objectType;
}

// VERIFY: 0a33d401 auto
// OFFSET: 0x0096C930
const char* CSimpleHTML::GetObjectTypeName() {
    return CSimpleHTML::s_objectTypeName;
}

// VERIFY: 8ccd312e auto
// OFFSET: 0x0096C940
int32_t CSimpleHTML::GetScriptMetaTable() {
    return CSimpleHTML::s_metatable;
}

// VERIFY: 7cbd791b auto
// OFFSET: 0x0096C960
bool CSimpleHTML::IsA(int32_t type) {
    return type == CSimpleHTML::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 5016a1a3 auto
// OFFSET: 0x0096C9A0
bool CSimpleHTML::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleHTML::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
