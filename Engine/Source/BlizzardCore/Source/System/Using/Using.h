#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_H

#include <cstddef>
#include <cstdint>

namespace System_Using { struct Depends; }

namespace System_Using {
struct Depends {
    /* 0000 */ uint16_t m_id;
    /* 0002 */ uint8_t unk_0002[2];
    /* 0004 */ System_Using::Depends* m_next;
};
}

namespace System_Using {
struct UsingRec {
    /* 0000 */ char* m_name;
    /* 0004 */ uint16_t m_id;
    /* 0006 */ uint16_t m_refCount;
    /* 0008 */ void (__stdcall* m_shutdownproc )();
    /* 000C */ uint16_t m_dependCount;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ System_Using::Depends* m_depends;
};
}

#endif
