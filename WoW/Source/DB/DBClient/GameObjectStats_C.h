#ifndef WOW_SOURCE_DB_DBCLIENT_GAMEOBJECTSTATS_C_H
#define WOW_SOURCE_DB_DBCLIENT_GAMEOBJECTSTATS_C_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class GameObjectStats_C {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[140];

    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
