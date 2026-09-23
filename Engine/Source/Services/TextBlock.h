#ifndef ENGINE_SOURCE_SERVICES_TEXTBLOCK_H
#define ENGINE_SOURCE_SERVICES_TEXTBLOCK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/FONTHASHOBJ.h"

class CGxString;

class TEXTBLOCK : public CHandleObject {
    public:
    /* 0004 */ CGxString* m_string;
};

#endif
