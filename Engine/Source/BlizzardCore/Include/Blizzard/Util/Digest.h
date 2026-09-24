#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H

#include <cstdint>

namespace Blizzard {
namespace Util {

template <int N>
struct Digest {
    /* 0000 */ uint8_t m_data[N];

    bool operator<(const Digest& other) const;
};

}
}

#endif
