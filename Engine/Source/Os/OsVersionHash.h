#ifndef ENGINE_SOURCE_OS_OSVERSIONHASH_H
#define ENGINE_SOURCE_OS_OSVERSIONHASH_H

#include <cstdint>

class IOsVersionHashCallback {
    public:
    virtual void Process(const void* data, uint32_t size) = 0;
};

struct OsVersionHashEntry {
    /* 0000 */ uint32_t m_offset;
    /* 0004 */ uint32_t m_size;
    /* 0008 */ uint32_t m_type;
    /* 000C */ uint32_t m_param;
};

class OsVersionHash {
    public:
    /* 0000 */ OsVersionHashEntry* m_entries;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_capacity;

    OsVersionHash();
    ~OsVersionHash();
    bool AddEntry(const OsVersionHashEntry& entry);
};

int32_t IOsVersionChecksum(IOsVersionHashCallback& callback, const char* path, int32_t loaded);

#endif
