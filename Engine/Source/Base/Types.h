#ifndef ENGINE_SOURCE_BASE_TYPES_H
#define ENGINE_SOURCE_BASE_TYPES_H

#include <cstddef>
#include <cstdint>

enum STATUS_TYPE {
    STATUS_INFO = 0,
    STATUS_WARNING = 1,
    STATUS_ERROR = 2,
    STATUS_FATAL = 3,
    STATUS_NUMTYPES = 4,
};

#endif
