#ifndef ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_H
#define ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CSimpleFrame;
class CGxTex;
class CStatus;
struct CTexture;
class XMLNode;

class CSimpleTexture : public CSimpleRegion {
    public:
    /* 00D4 */ CTexture* m_texture;
    /* 00D8 */ uint32_t m_alphaMode;
    /* 00DC */ uint32_t* m_shader;
    /* 00E0 */ C3Vector m_positions [4];
    /* 0110 */ uint32_t m_textureUkn17;
    /* 0114 */ uint32_t m_textureUkn18;
    /* 0118 */ uint32_t m_textureUkn19;
    /* 011C */ uint32_t m_textureUkn20;
    /* 0120 */ uint32_t m_textureUkn21;
    /* 0124 */ uint32_t m_textureUkn22;
    /* 0128 */ uint32_t m_textureUkn23;
    /* 012C */ uint32_t m_textureUkn24;
    /* 0130 */ uint32_t m_textureUkn25;
    /* 0134 */ uint32_t m_textureUkn26;
    /* 0138 */ uint32_t m_textureUkn27;
    /* 013C */ uint32_t m_textureUkn28;
    /* 0140 */ C2Vector m_texCoords [4];
    /* 0160 */ uint32_t m_textureFlags;

    static CDataAllocator s_allocator;

    CSimpleTexture(CSimpleFrame* frame, uint32_t drawLayer, int32_t show);
    CGxTex* GetGxTex();
    int32_t GetTexture(char* buffer, uint32_t bufferLen);
    void PostLoadXML(XMLNode* node, CStatus* status);

    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
};

#endif
