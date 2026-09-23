#ifndef WOW_SOURCE_OBJECT_GAMEOBJECT_H
#define WOW_SOURCE_OBJECT_GAMEOBJECT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGGameObjectData;

class CGGameObject {
    public:
    /* 0000 */ CGGameObjectData* m_gameObject;
    /* 0004 */ void* m_gameObjectMirror;
};

class CGGameObjectData {
    public:
    /* 0000 */ WGUID m_objectFieldCreatedBy;
    /* 0008 */ uint32_t m_gameobjectDisplayid;
    /* 000C */ uint32_t m_gameobjectFlags;
    /* 0010 */ float m_gameobjectParentrotation [4];
    /* 0020 */ uint16_t m_gameobjectDynamic [2];
    /* 0024 */ uint32_t m_gameobjectFaction;
    /* 0028 */ uint32_t m_gameobjectLevel;
    /* 002C */ uint8_t m_gameobjectBytes1 [4];
};

#endif
