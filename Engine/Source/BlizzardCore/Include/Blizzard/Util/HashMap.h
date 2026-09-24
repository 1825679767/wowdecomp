#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_HASHMAP_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_HASHMAP_H

#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

namespace Blizzard {
namespace Util {

template <class K, class V>
class HashMap {
    public:
    struct Entry {
        /* 0000 */ K m_key;
        /* 0008 */ V m_value;
    };

    /* 0000 */ Entry* m_entries;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_capacity;

    ~HashMap();
    bool Find(const K& key, uint32_t* index);
    bool Get(const K& key, V* value);
};

}
}

// INLINED
template <class K, class V>
Blizzard::Util::HashMap<K, V>::~HashMap() {
    for (uint32_t i = 0; i < this->m_capacity; i++) {
        if (this->m_entries[i].m_key.m_lo) {
            this->m_entries[i].m_key.m_lo = 0;
            this->m_entries[i].m_key.m_hi = 0;
        }
    }

    this->m_count = 0;

    if (this->m_entries) {
        SMemFree(this->m_entries, __FILE__, __LINE__, 0);
        this->m_entries = nullptr;
        this->m_capacity = 0;
    }
}

// VERIFY: 23f50510 manual
// OFFSET: 0x00433BA0
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Find(const K& key, uint32_t* index) {
    if (!this->m_capacity) {
        return false;
    }

    for (*index = key.m_lo & (this->m_capacity - 1); this->m_entries[*index].m_key.m_lo; *index = (*index + 1) & (this->m_capacity - 1)) {
        if (this->m_entries[*index].m_key.m_lo == key.m_lo && this->m_entries[*index].m_key.m_hi == key.m_hi) {
            return true;
        }
    }

    return false;
}

// VERIFY: d994337d manual
// OFFSET: 0x00435EA0
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Get(const K& key, V* value) {
    uint32_t index;

    if (!this->Find(key, &index)) {
        return false;
    }

    *value = this->m_entries[index].m_value;

    return true;
}

#endif
