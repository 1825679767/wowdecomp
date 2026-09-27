#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D148
int32_t CSimpleMessageFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE48C
int32_t CSimpleMessageFrame::s_objectType;

// STATIC: 0x00B2CC1C
const char* CSimpleMessageFrame::s_objectTypeName = "MessageFrame";

// INLINED
int32_t CSimpleMessageFrame::GetObjectType() {
    if (!CSimpleMessageFrame::s_objectType) {
        CSimpleMessageFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMessageFrame::s_objectType;
}

// VERIFY: 6c8073c0 auto
// OFFSET: 0x00968610
const char* CSimpleMessageFrame::GetObjectTypeName() {
    return CSimpleMessageFrame::s_objectTypeName;
}

// VERIFY: e51cd05e auto
// OFFSET: 0x00968620
int32_t CSimpleMessageFrame::GetScriptMetaTable() {
    return CSimpleMessageFrame::s_metatable;
}

// VERIFY: 2b059924 auto
// OFFSET: 0x00968640
bool CSimpleMessageFrame::IsA(int32_t type) {
    return type == CSimpleMessageFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: e420b1cc auto
// OFFSET: 0x00968680
bool CSimpleMessageFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleMessageFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
