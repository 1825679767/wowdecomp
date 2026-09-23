#ifndef WOW_SOURCE_GAME_GAMECLIENT_PETNAMECACHE_H
#define WOW_SOURCE_GAME_GAMECLIENT_PETNAMECACHE_H

#include <cstddef>
#include <cstdint>

class PetNameCache {
    public:
    /* 0000 */ uint8_t unk_0000[80];
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
};

#endif
