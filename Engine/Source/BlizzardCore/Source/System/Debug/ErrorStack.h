#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_ERRORSTACK_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_ERRORSTACK_H

#include <cstddef>
#include <cstdint>

namespace Blizzard { namespace Debug { struct ErrorStackRecord; }}

namespace Blizzard {
namespace Debug {
struct ErrorStackRecord {
    /* 0000 */ Blizzard::Debug::ErrorStackRecord* m_prev;
    /* 0004 */ int32_t m_level;
    /* 0008 */ char* m_flattened;
    /* 000C */ char* m_str;
    /* 0010 */ int32_t m_code;
    /* 0014 */ int32_t m_category;
};
}
}

#endif
