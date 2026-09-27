#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONT_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFONT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFontStringAttributes.h"
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleFont;
struct CSimpleFontable;
class CStatus;
class XMLNode;

struct SIMPLEFONT : public TSHashObject<SIMPLEFONT, HASHKEY_STRI> {
    /* 0018 */ CSimpleFont* m_font;

    SIMPLEFONT();
    ~SIMPLEFONT();
};

class CSimpleFont : public FrameScript_Object {
    public:
    /* 0014 */ CSimpleFontable m_baseSimpleFontable;
    /* 0028 */ CSimpleFontStringAttributes m_attributes;
    /* 0058 */ const char* m_name;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ TSList<CSimpleFontable> m_fontableList;

    static CSimpleFont* GetFont(const char* name, int32_t create);

    void LoadXML(XMLNode* node, CStatus* status);

    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    virtual const char* GetName();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual void UpdateObjects();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
};

#endif
