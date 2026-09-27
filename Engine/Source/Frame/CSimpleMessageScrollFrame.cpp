#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D060
int32_t CSimpleMessageScrollFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE4A4
int32_t CSimpleMessageScrollFrame::s_objectType;

// STATIC: 0x00B2CC64
const char* CSimpleMessageScrollFrame::s_objectTypeName = "ScrollingMessageFrame";

// INLINED
int32_t CSimpleMessageScrollFrame::GetObjectType() {
    if (!CSimpleMessageScrollFrame::s_objectType) {
        CSimpleMessageScrollFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMessageScrollFrame::s_objectType;
}

// VERIFY: f536be9b auto
// OFFSET: 0x00969DB0
const char* CSimpleMessageScrollFrame::GetObjectTypeName() {
    return CSimpleMessageScrollFrame::s_objectTypeName;
}

// VERIFY: 8d535b01 auto
// OFFSET: 0x00969DC0
int32_t CSimpleMessageScrollFrame::GetScriptMetaTable() {
    return CSimpleMessageScrollFrame::s_metatable;
}

// VERIFY: f7a01043 auto
// OFFSET: 0x00969DE0
bool CSimpleMessageScrollFrame::IsA(int32_t type) {
    return type == CSimpleMessageScrollFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 651a3515 auto
// OFFSET: 0x00969E20
bool CSimpleMessageScrollFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleMessageScrollFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
