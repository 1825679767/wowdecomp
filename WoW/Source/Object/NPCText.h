#ifndef WOW_SOURCE_OBJECT_NPCTEXT_H
#define WOW_SOURCE_OBJECT_NPCTEXT_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class NPCText {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ uint8_t unk_0000[320];

    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
