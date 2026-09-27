#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1114
int32_t CSimpleFontString::s_metatable = LUA_NOREF;

// STATIC: 0x00B4792C
int32_t CSimpleFontString::s_objectType;

// STATIC: 0x00AC0F10
const char* CSimpleFontString::s_objectTypeName = "FontString";

// INLINED
int32_t CSimpleFontString::GetObjectType() {
    if (!CSimpleFontString::s_objectType) {
        CSimpleFontString::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFontString::s_objectType;
}

// VERIFY: a08564ec auto
// OFFSET: 0x00485390
bool CSimpleFontString::IsA(int32_t type) {
    return type == CSimpleFontString::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 9da7e913 auto
// OFFSET: 0x004853C0
bool CSimpleFontString::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleFontString::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 3a930695 auto
// OFFSET: 0x00485420
const char* CSimpleFontString::GetObjectTypeName() {
    return CSimpleFontString::s_objectTypeName;
}

// VERIFY: 210fa960 auto
// OFFSET: 0x00485430
int32_t CSimpleFontString::GetScriptMetaTable() {
    return CSimpleFontString::s_metatable;
}

// VERIFY: c3b240f1 auto
// OFFSET: 0x00482270
const char* CSimpleFontString::GetFontName() {
    return this->m_fontHandle ? TextBlockGetFontName(this->m_fontHandle) : nullptr;
}

// VERIFY: b8a061b3 auto
// OFFSET: 0x00482300
uint32_t CSimpleFontString::GetFontFlags() {
    return this->m_fontHandle ? TextBlockGetFontFlags(this->m_fontHandle) : 0;
}
