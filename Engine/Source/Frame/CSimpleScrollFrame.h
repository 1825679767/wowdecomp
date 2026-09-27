#ifndef ENGINE_SOURCE_FRAME_CSIMPLESCROLLFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLESCROLLFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CSimpleScrollFrame : public CSimpleFrame {
    public:
    /* 029C */ uint32_t m_scrollChildUpdate;
    /* 02A0 */ CSimpleFrame* m_scrollChild;
    /* 02A4 */ C2Vector m_scrollRange;
    /* 02AC */ C2Vector m_scrollOffset;
    /* 02B4 */ uint32_t m_simpleScrollFrameUkn7;
    /* 02B8 */ uint32_t m_simpleScrollFrameUkn8;
    /* 02BC */ uint32_t m_simpleScrollFrameUkn9;
    /* 02C0 */ uint32_t m_simpleScrollFrameUkn10;
    /* 02C4 */ uint32_t m_simpleScrollFrameUkn11;
    /* 02C8 */ uint32_t m_simpleScrollFrameUkn12;

    static CDataAllocator s_allocator;

    CSimpleScrollFrame(CSimpleFrame* parent);

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
