#ifndef WOW_SOURCE_OBJECT_DYNAMICOBJECT_H
#define WOW_SOURCE_OBJECT_DYNAMICOBJECT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGDynamicObjectData;

class CGDynamicObject {
    public:
    /* 0000 */ CGDynamicObjectData* m_dynamicObject;
    /* 0004 */ void* m_dynamicObjectMirror;
};

class CGDynamicObjectData {
    public:
    /* 0000 */ WGUID m_dynamicobjectCaster;
    /* 0008 */ uint8_t m_dynamicobjectBytes [4];
    /* 000C */ uint32_t m_dynamicobjectSpellid;
    /* 0010 */ float m_dynamicobjectRadius;
    /* 0014 */ uint32_t m_dynamicobjectCasttime;
};

#endif
