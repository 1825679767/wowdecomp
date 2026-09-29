#ifndef WOW_SOURCE_UI_TABARDMODELFRAME_H
#define WOW_SOURCE_UI_TABARDMODELFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Gx/Types.h"
#include "WoW/Source/Ui/CharacterModelBase.h"

class CSimpleFrame;
struct lua_State;

class CGTabardModelFrame : public CGCharacterModelBase {
    public:
    static CDataAllocator s_tabardModelHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 0380 */ uint32_t unk_0380 [6];

    ~CGTabardModelFrame() override;
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void OnModelLoaded(CM2Model* model) override;

    CGTabardModelFrame(CSimpleFrame* parent);
};

void EmblemTextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

#endif
