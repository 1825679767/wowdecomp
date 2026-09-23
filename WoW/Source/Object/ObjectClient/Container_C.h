#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CONTAINER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CONTAINER_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/Container.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

class CGContainer_C : public CGItem_C, public CGContainer {
    public:
    /* 03E8 */ uint8_t unk_03E8 [888];
    /* 0760 */ uint32_t unk_0760;
    /* 0764 */ uint8_t unk_0764 [20];
};

#endif
