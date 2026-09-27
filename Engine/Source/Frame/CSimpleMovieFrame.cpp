#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B2CD48
int32_t CSimpleMovieFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DCE3D8
int32_t CSimpleMovieFrame::s_objectType;

// STATIC: 0x00B2CAEC
const char* CSimpleMovieFrame::s_objectTypeName = "MovieFrame";

// INLINED
int32_t CSimpleMovieFrame::GetObjectType() {
    if (!CSimpleMovieFrame::s_objectType) {
        CSimpleMovieFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMovieFrame::s_objectType;
}

// VERIFY: b35f404b auto
// OFFSET: 0x0095E7D0
const char* CSimpleMovieFrame::GetObjectTypeName() {
    return CSimpleMovieFrame::s_objectTypeName;
}

// VERIFY: 8e847d4e auto
// OFFSET: 0x0095E7E0
int32_t CSimpleMovieFrame::GetScriptMetaTable() {
    return CSimpleMovieFrame::s_metatable;
}

// VERIFY: e8219013 auto
// OFFSET: 0x0095E800
bool CSimpleMovieFrame::IsA(int32_t type) {
    return type == CSimpleMovieFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 68c5960a auto
// OFFSET: 0x0095E840
bool CSimpleMovieFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleMovieFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}
