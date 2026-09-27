#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleEditBox.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2D3E0
int32_t CSimpleEditBox::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE474
CSimpleEditBox* CSimpleEditBox::s_currentFocus;

// STATIC: 0x00DCE470
int32_t CSimpleEditBox::s_objectType;

// STATIC: 0x00B2CBDC
const char* CSimpleEditBox::s_objectTypeName = "EditBox";

// INLINED
int32_t CSimpleEditBox::GetObjectType() {
    if (!CSimpleEditBox::s_objectType) {
        CSimpleEditBox::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleEditBox::s_objectType;
}

// VERIFY: 47818eff auto
// OFFSET: 0x00966F20
const char* CSimpleEditBox::GetObjectTypeName() {
    return CSimpleEditBox::s_objectTypeName;
}

// VERIFY: b50cdb9f auto
// OFFSET: 0x00966F30
int32_t CSimpleEditBox::GetScriptMetaTable() {
    return CSimpleEditBox::s_metatable;
}

// VERIFY: 5d8f0f26 auto
// OFFSET: 0x00966F50
bool CSimpleEditBox::IsA(int32_t type) {
    return type == CSimpleEditBox::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 9f181853 auto
// OFFSET: 0x00966F90
bool CSimpleEditBox::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleEditBox::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
