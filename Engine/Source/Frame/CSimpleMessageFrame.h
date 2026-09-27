#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleMessageFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    CSimpleMessageFrame(CSimpleFrame* parent);

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

class CSimpleMessageFrameLineNode {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
};

#endif
