#ifndef WOW_SOURCE_UI_DRESSUPMODELFRAME_H
#define WOW_SOURCE_UI_DRESSUPMODELFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/Ui/CharacterModelBase.h"

class CCharacterComponent;
class CSimpleFrame;
struct lua_State;

class CGDressUpModelFrame : public CGCharacterModelBase {
    public:
    static CDataAllocator s_dressUpModelHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 0380 */ CCharacterComponent* m_component;
    /* 0384 */ uint32_t unk_0384 [5];

    ~CGDressUpModelFrame() override;
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void UpdateModel() override;

    CGDressUpModelFrame(CSimpleFrame* parent);
};

#endif
