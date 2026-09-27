#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHTML_H
#define ENGINE_SOURCE_FRAME_CSIMPLEHTML_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"

class CSimpleHTML : public CSimpleHyperlinkedFrame {
    public:
    /* 02B8 */ uint32_t m_simpleHtmlUkn1;
    /* 02BC */ uint32_t m_simpleHtmlUkn2;
    /* 02C0 */ uint32_t m_simpleHtmlUkn3;
    /* 02C4 */ uint32_t m_simpleHtmlUkn4;
    /* 02C8 */ uint32_t m_simpleHtmlUkn5;
    /* 02CC */ uint32_t m_simpleHtmlUkn6;
    /* 02D0 */ uint32_t m_simpleHtmlUkn7;
    /* 02D4 */ uint32_t m_simpleHtmlUkn8;
    /* 02D8 */ uint32_t m_simpleHtmlUkn9;
    /* 02DC */ uint32_t m_simpleHtmlUkn10;
    /* 02E0 */ uint32_t m_simpleHtmlUkn12;
    /* 02E4 */ uint32_t m_simpleHtmlUkn13;
    /* 02E8 */ uint32_t m_simpleHtmlUkn14;
    /* 02EC */ uint32_t m_simpleHtmlUkn15;
    /* 02F0 */ uint32_t m_simpleHtmlUkn16;
    /* 02F4 */ uint32_t m_simpleHtmlUkn17;

    static CDataAllocator s_allocator;

    CSimpleHTML(CSimpleFrame* parent);

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
