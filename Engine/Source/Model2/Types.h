#ifndef ENGINE_SOURCE_MODEL2_TYPES_H
#define ENGINE_SOURCE_MODEL2_TYPES_H

#include <cstddef>
#include <cstdint>

enum M2PASS {
    M2PASS_0 = 0,
    M2PASS_1 = 1,
    M2PASS_2 = 2,
};

enum M2LIGHTTYPE {
    M2LIGHT_DIRECT = 0,
    M2LIGHT_POINT = 1,
};

// Deferred M2 calls; MDL command indices are not compatible.
enum EModelModQ : uint32_t {
    MODEL_REPLACE_TEXTURE = 0,
    MODEL_SET_GEOMETRY_VISIBLE = 1,
    MODEL_SET_GEOSET_VISIBILITY_RANGE = 2,
    MODEL_OPTIMIZE_VISIBLE_GEOSETS = 3,
    MODEL_SET_BONE_FLAGS = 4,
    MODEL_SET_BONE_SEQUENCE = 5,
    MODEL_UNSET_BONE_SEQUENCE = 6,
    MODEL_SET_BONE_SEQUENCE_TIME = 7,
    MODEL_SET_BONE_SEQUENCE_SPEED = 8,
    MODEL_SET_BONE_PROCEDURAL_TRANSFORM = 9,
    MODEL_SET_LIGHT_ENABLED = 10,
    MODEL_REPLACE_PARTICLE_COLOR = 11,
    MODEL_SET_EMITTERS_ENABLED = 12,
    MODEL_SET_RIBBONS_ENABLED = 13,
    MODEL_LOAD_SEQUENCE = 14,
    MODEL_NUM_COMMANDS = 15,
    MODEL_COMMAND_NOT_QUEUED = 0xFFFFFFFF,
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
