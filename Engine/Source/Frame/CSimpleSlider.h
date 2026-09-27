#ifndef ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H
#define ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CSimpleSlider : public CSimpleFrame {
    public:
    /* 029C */ uint32_t m_sliderFlags;
    /* 02A0 */ C2Vector m_minMax;
    /* 02A8 */ uint32_t m_value;
    /* 02AC */ uint32_t m_valueStep;
    /* 02B0 */ uint32_t m_simpleSliderUkn6;
    /* 02B4 */ uint32_t m_simpleSliderUkn7;
    /* 02B8 */ uint32_t m_simpleSliderUkn8;
    /* 02BC */ uint32_t m_simpleSliderUkn9;
    /* 02C0 */ uint32_t m_simpleSliderUkn10;
    /* 02C4 */ uint32_t m_simpleSliderUkn11;

    static CDataAllocator s_allocator;

    CSimpleSlider(CSimpleFrame* parent);

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
