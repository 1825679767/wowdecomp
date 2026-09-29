#ifndef WOW_SOURCE_UI_ITEMTEXTFRAME_H
#define WOW_SOURCE_UI_ITEMTEXTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGItemText {
    public:
    static WOWGUID m_itemGUID;

    static void SetItem(const WOWGUID& guid, int32_t a2);
};

#endif
