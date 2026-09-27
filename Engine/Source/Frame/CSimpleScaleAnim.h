#ifndef ENGINE_SOURCE_FRAME_CSIMPLESCALEANIM_H
#define ENGINE_SOURCE_FRAME_CSIMPLESCALEANIM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"

class CSimpleScaleAnim : public CSimpleAnim {
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
