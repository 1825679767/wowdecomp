#ifndef ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_SYSTEM_THREAD_H
#define ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_SYSTEM_THREAD_H

#include <cstddef>
#include <cstdint>

namespace Blizzard {
namespace Thread {
typedef void (__stdcall* TLSDestructor_interface )(void*);
}
}

namespace Blizzard {
namespace Thread {
typedef Blizzard::Thread::TLSDestructor_interface TLSDestructor;
}
}

namespace Blizzard {
namespace Thread {
struct TLSSlot {
    /* 0000 */ uint32_t m_key;
    /* 0004 */ Blizzard::Thread::TLSDestructor m_destructor;
    /* 0008 */ bool m_initialized;
    /* 0009 */ uint8_t unk_0009[3];
};
}
}

#endif
