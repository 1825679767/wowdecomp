#ifndef ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_H
#define ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

class CScriptObject : public FrameScript_Object {
    public:
    /* 0014 */ RCString m_name;

    static CScriptObject* GetScriptObjectByName(const char* name, int32_t type);

    virtual const char* GetName();
    virtual FrameScript_Object* GetScriptObjectParent();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
};

#endif
