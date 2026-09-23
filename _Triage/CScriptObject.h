#ifndef _TRIAGE_CSCRIPTOBJECT_H
#define _TRIAGE_CSCRIPTOBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Frame/CSimpleAnim.h"

class CScriptObject : public FrameScript_Object {
    public:
    /* 0014 */ RCString m_name;
};

#endif
