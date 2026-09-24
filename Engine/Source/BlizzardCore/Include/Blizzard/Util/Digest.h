#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H

#include <cstdint>
#include <cstring>

namespace Blizzard {
namespace Util {

template <int N>
struct Digest {
    /* 0000 */ uint8_t m_data[N];

    bool operator<(const Digest& other) const;
    bool operator==(const Digest& other) const;
    bool operator!=(const Digest& other) const;
};

}
}

// VERIFY: 182eff5b manual
// OFFSET: 0x00429540
template <int N>
bool Blizzard::Util::Digest<N>::operator<(const Digest& other) const {
    for (int32_t i = 0; i < N; i++) {
        if (this->m_data[i] < other.m_data[i]) {
            return true;
        }

        if (this->m_data[i] > other.m_data[i]) {
            return false;
        }
    }

    return false;
}

// VERIFY: 06cb648f manual
// OFFSET: 0x00668750
template <int N>
bool Blizzard::Util::Digest<N>::operator==(const Digest& other) const {
    return memcmp(this->m_data, other.m_data, N) == 0;
}

// VERIFY: 951b079f manual
// OFFSET: 0x004221A0
template <int N>
bool Blizzard::Util::Digest<N>::operator!=(const Digest& other) const {
    return memcmp(this->m_data, other.m_data, N) != 0;
}

#endif
