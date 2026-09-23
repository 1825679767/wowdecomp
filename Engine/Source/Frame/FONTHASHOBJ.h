#ifndef ENGINE_SOURCE_FRAME_FONTHASHOBJ_H
#define ENGINE_SOURCE_FRAME_FONTHASHOBJ_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"

struct CGxFont;
struct FONTHASHOBJ;
struct HASHKEY_STR;

class CHandleObject {
    public:
    /* 0000 */ int32_t m_refcount;
};

struct FONTHASHOBJ : public CHandleObject {
    /* 0004 */ TSHashObject<FONTHASHOBJ, HASHKEY_STR> m_hashObject;
    /* 001C */ CGxFont* m_font;
};

#endif
