#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CBD0
int32_t CSimpleModel::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE428
int32_t CSimpleModel::s_objectType;

// STATIC: 0x00B2CB0C
const char* CSimpleModel::s_objectTypeName = "Model";

// INLINED
int32_t CSimpleModel::GetObjectType() {
    if (!CSimpleModel::s_objectType) {
        CSimpleModel::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleModel::s_objectType;
}

// VERIFY: 9bb9dd69 auto
// OFFSET: 0x004E60D0
bool CSimpleModel::IsA(int32_t type) {
    return type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 3d38f994 auto
// OFFSET: 0x004E6110
bool CSimpleModel::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleModel::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// VERIFY: 2b17add5 auto
// OFFSET: 0x004E6150
const char* CSimpleModel::GetObjectTypeName() {
    return CSimpleModel::s_objectTypeName;
}

// VERIFY: 8bffeeff auto
// OFFSET: 0x0095F410
int32_t CSimpleModel::GetScriptMetaTable() {
    return CSimpleModel::s_metatable;
}
