#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/DynamicObject.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ uint32_t unk_00DC;
    /* 00E0 */ uint8_t unk_00E0 [8];
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ uint8_t unk_00F4 [92];
    /* 0150 */ uint32_t unk_0150;
    /* 0154 */ uint8_t unk_0154 [4];
    /* 0158 */ uint32_t unk_0158;
    /* 015C */ uint8_t unk_015C [20];
};

#endif
