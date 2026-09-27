#ifndef ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H
#define ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleButton.h"

class CSimpleCheckbox : public CSimpleButton {
    public:
    /* 02F8 */ uint32_t m_isChecked;
    /* 02FC */ uint32_t m_checkedTexture;
    /* 0300 */ uint32_t m_disabledTexture;

    static CDataAllocator s_allocator;

    CSimpleCheckbox(CSimpleFrame* parent);

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
