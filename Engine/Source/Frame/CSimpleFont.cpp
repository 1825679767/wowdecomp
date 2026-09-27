#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00B499B4
static TSHashTable<SIMPLEFONT, HASHKEY_STRI> s_fontList;

// STATIC: 0x00AC18D8
int32_t CSimpleFont::s_metatable = LUA_NOREF;

// STATIC: 0x00B499B0
int32_t CSimpleFont::s_objectType;

// STATIC: 0x00AC1330
const char* CSimpleFont::s_objectTypeName = "Font";

// INLINED
int32_t CSimpleFont::GetObjectType() {
    if (!CSimpleFont::s_objectType) {
        CSimpleFont::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFont::s_objectType;
}

// VERIFY: a2632992 auto
// OFFSET: 0x004965C0
const char* CSimpleFont::GetName() {
    return this->m_name;
}

// VERIFY: eddc0268 auto
// OFFSET: 0x00496610
bool CSimpleFont::IsA(int32_t type) {
    return type == CSimpleFont::s_objectType;
}

// VERIFY: c1008cf1 manual
// OFFSET: 0x00496630
bool CSimpleFont::IsA(const char* typeName) {
    return SStrCmpI(typeName, CSimpleFont::s_objectTypeName, STORM_MAX_STR) == 0;
}

// VERIFY: 3d0bc1be auto
// OFFSET: 0x00496660
const char* CSimpleFont::GetObjectTypeName() {
    return CSimpleFont::s_objectTypeName;
}

// VERIFY: 4925c45f auto
// OFFSET: 0x00496670
int32_t CSimpleFont::GetScriptMetaTable() {
    return CSimpleFont::s_metatable;
}

// VERIFY: dd9273cb manual
// OFFSET: 0x00496F70
CSimpleFont* CSimpleFont::GetFont(const char* name, int32_t create) {
    SIMPLEFONT* node = s_fontList.Ptr(name);

    if (!node) {
        if (!create) {
            return nullptr;
        }

        node = s_fontList.New(name, 0, 0);

        const char* fontName = node->m_key.m_str;
        CSimpleFont* font = node->m_font;

        if (font->m_name) {
            font->UnregisterScriptObject(font->m_name);
        }

        font->m_name = fontName;

        if (fontName) {
            font->RegisterScriptObject(fontName);
        }
    }

    return node->m_font;
}
