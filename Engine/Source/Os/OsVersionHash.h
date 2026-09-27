#ifndef ENGINE_SOURCE_OS_OSVERSIONHASH_H
#define ENGINE_SOURCE_OS_OSVERSIONHASH_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class OsFileVersionInfo {
    public:
    /* 0000 */ TSFixedArray<uint8_t> m_data;
    /* 000C */ uint16_t m_fileVersion[4];
    /* 0014 */ uint16_t m_productVersion[4];

    OsFileVersionInfo(const char* filename);
};

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
