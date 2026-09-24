#ifndef ENGINE_SOURCE_SERVICES_TEXTBLOCK_H
#define ENGINE_SOURCE_SERVICES_TEXTBLOCK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Handle.h"

class CGxString;

class TEXTBLOCK : public CHandleObject {
    public:
    /* 0008 */ CGxString* m_string;
};

#endif
