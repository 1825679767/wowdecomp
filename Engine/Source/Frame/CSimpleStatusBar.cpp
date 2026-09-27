#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CE18
int32_t CSimpleStatusBar::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE440
int32_t CSimpleStatusBar::s_objectType;

// STATIC: 0x00B2CBD4
const char* CSimpleStatusBar::s_objectTypeName = "StatusBar";

// INLINED
int32_t CSimpleStatusBar::GetObjectType() {
    if (!CSimpleStatusBar::s_objectType) {
        CSimpleStatusBar::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleStatusBar::s_objectType;
}

// VERIFY: 1dccb1ab auto
// OFFSET: 0x00961610
const char* CSimpleStatusBar::GetObjectTypeName() {
    return CSimpleStatusBar::s_objectTypeName;
}

// VERIFY: 84bf541f auto
// OFFSET: 0x00961620
int32_t CSimpleStatusBar::GetScriptMetaTable() {
    return CSimpleStatusBar::s_metatable;
}

// VERIFY: 74ee586d auto
// OFFSET: 0x00961980
bool CSimpleStatusBar::IsA(int32_t type) {
    return type == CSimpleStatusBar::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: eb1c4628 auto
// OFFSET: 0x009619C0
bool CSimpleStatusBar::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleStatusBar::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
