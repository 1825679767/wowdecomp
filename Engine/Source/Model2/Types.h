#ifndef ENGINE_SOURCE_MODEL2_TYPES_H
#define ENGINE_SOURCE_MODEL2_TYPES_H

#include <cstddef>
#include <cstdint>

enum M2TEXTURETYPE : uint32_t {
    M2TEX_SKIN = 1,
    M2TEX_OBJECT_SKIN = 2,
    M2TEX_HAIR = 6,
    M2TEX_EXTRA_SKIN = 8,
    M2TEX_MONSTER_1 = 11,
    M2TEX_MONSTER_2 = 12,
    M2TEX_MONSTER_3 = 13,
    M2TEX_ITEM_ICON = 14,
};

enum M2TRACKTYPE : uint16_t {
    M2TRACK_NONE = 0,
    M2TRACK_LINEAR = 1,
    M2TRACK_BEZIER = 2,
    M2TRACK_HERMITE = 3,
};

enum M2BLEND : uint16_t {
    M2BLEND_OPAQUE = 0,
    M2BLEND_ALPHA_KEY = 1,
    M2BLEND_ALPHA = 2,
    M2BLEND_NO_ALPHA_ADD = 3,
    M2BLEND_ADD = 4,
    M2BLEND_MOD = 5,
    M2BLEND_MOD2X = 6,
    M2BLEND_COUNT = 7,
};

enum M2PARTICLEBLENDMODE : uint8_t {
    M2PARTICLEBLEND_OPAQUE = 0,
    M2PARTICLEBLEND_ALPHA_KEY = 1,
    M2PARTICLEBLEND_ALPHA = 2,
    M2PARTICLEBLEND_NO_ALPHA_ADD = 3,
    M2PARTICLEBLEND_ADD = 4,
    M2PARTICLEBLEND_MOD = 5,
    M2PARTICLEBLEND_MOD2X = 6,
};

enum M2PASS {
    M2PASS_0 = 0,
    M2PASS_1 = 1,
    M2PASS_2 = 2,
};

enum M2LIGHTTYPE {
    M2LIGHT_DIRECT = 0,
    M2LIGHT_POINT = 1,
};

class CM2ModelCall;

class CM2ModelCall {
    public:
    /* 0000 */ uint32_t m_type;
    /* 0004 */ CM2ModelCall* m_next;
    /* 0008 */ uint32_t m_time;
    /* 000C */ uintptr_t m_args [17];

    CM2ModelCall();
};

#endif
