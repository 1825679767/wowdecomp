#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIM_H
#define ENGINE_SOURCE_FRAME_CSIMPLEANIM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"

class CLCDConnection {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001[4];
    /* 0005 */ uint8_t unk_0005[3];
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
};

class CSimpleAnim : public CScriptObject {
    public:
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
