#ifndef ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_H
#define ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

class CScriptObject : public FrameScript_Object {
    public:
    /* 0014 */ RCString m_name;
};

#endif
