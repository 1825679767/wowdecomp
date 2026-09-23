#ifndef WOW_SOURCE_OBJECT_CONTAINER_H
#define WOW_SOURCE_OBJECT_CONTAINER_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGContainerData;

class CGContainer {
    public:
    /* 0000 */ CGContainerData* m_container;
    /* 0004 */ void* m_containerMirror;
};

class CGContainerData {
    public:
    /* 0000 */ uint32_t m_containerFieldNumSlots;
    /* 0004 */ uint8_t m_containerAlignPad [4];
    /* 0008 */ WGUID m_containerFieldSlot1 [36];
};

#endif
