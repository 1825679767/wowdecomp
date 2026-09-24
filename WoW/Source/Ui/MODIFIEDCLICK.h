#ifndef WOW_SOURCE_UI_MODIFIEDCLICK_H
#define WOW_SOURCE_UI_MODIFIEDCLICK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct HASHKEY_STRI;
class MODIFIEDCLICK;

struct MODIFIEDCLICK_DATA {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_modifiers;
    /* 0008 */ const char* m_button;
};

class MODIFIEDCLICK : public TSHashObject<MODIFIEDCLICK, HASHKEY_STRI> {
    public:
    /* 0018 */ MODIFIEDCLICK_DATA m_data [4];
};

#endif
