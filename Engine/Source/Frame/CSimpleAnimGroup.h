#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_H
#define ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/stpl.h"

class CScriptRegion;
class CStatus;
class XMLNode;

class CSimpleAnimGroup : public CScriptObject {
    public:
    /* 0020 */ TSLink<CSimpleAnimGroup> m_regionLink;
    /* 0028 */ TSLink<CSimpleAnimGroup> m_playingLink;
    /* 0030 */ uint32_t unk_0030[30];

    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual CScriptObject* GetScriptObjectParent();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();

    CSimpleAnimGroup(CScriptRegion* owner);
    void LoadXML(XMLNode* node, CStatus* status);
    void OnUpdate(float elapsedSec);
    void PostLoadXML(XMLNode* node, CStatus* status);
    void Stop(bool finish);
};

#endif
