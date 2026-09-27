#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleMessageScrollFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    CSimpleMessageScrollFrame(CSimpleFrame* parent, int32_t maxLines);

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

class CSimpleMessageScrollFrameDisplayNode {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003[4];
};

#endif
