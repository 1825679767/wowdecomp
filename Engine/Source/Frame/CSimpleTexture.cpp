#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00AC1110
int32_t CSimpleTexture::s_metatable = LUA_NOREF;

// STATIC: 0x00B4793C
int32_t CSimpleTexture::s_objectType;

// STATIC: 0x00AC0EF4
const char* CSimpleTexture::s_objectTypeName = "Texture";

// INLINED
int32_t CSimpleTexture::GetObjectType() {
    if (!CSimpleTexture::s_objectType) {
        CSimpleTexture::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleTexture::s_objectType;
}

// VERIFY: bced6855 auto
// OFFSET: 0x00482C60
bool CSimpleTexture::IsA(int32_t type) {
    return type == CSimpleTexture::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 3165fe90 auto
// OFFSET: 0x00482C90
bool CSimpleTexture::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleTexture::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 465f6c7c auto
// OFFSET: 0x00482CF0
const char* CSimpleTexture::GetObjectTypeName() {
    return CSimpleTexture::s_objectTypeName;
}

// VERIFY: 80d103be auto
// OFFSET: 0x00482D00
int32_t CSimpleTexture::GetScriptMetaTable() {
    return CSimpleTexture::s_metatable;
}

// VERIFY: a001c950 auto
// OFFSET: 0x004815E0
int32_t CSimpleTexture::GetTexture(char* buffer, uint32_t bufferLen) {
    if (!this->m_texture) {
        return 0;
    }

    const char* filename = TextureGetFilename(this->m_texture);

    if (!filename || !*filename) {
        return 0;
    }

    SStrCopy(buffer, filename, bufferLen);
    return 1;
}

// VERIFY: 1db02c26 auto
// OFFSET: 0x00481720
CGxTex* CSimpleTexture::GetGxTex() {
    return this->m_texture ? TextureGetGxTex(this->m_texture, 1, nullptr) : nullptr;
}
