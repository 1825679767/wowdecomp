#ifndef WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_H
#define WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class NameCache {
    public:
    static const uint32_t s_cacheVersion = 4;

    struct DeclinedNames {
        /* 0000 */ char m_names[5][64];
    };

    /* 0000 */ char m_name[48];
    /* 0030 */ NameCache::DeclinedNames* m_declinedNames;
    /* 0034 */ char m_realmName[256];
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ uint64_t m_guid;
    /* 0140 */ uint32_t m_race;
    /* 0144 */ uint32_t m_sex;
    /* 0148 */ uint32_t m_class;

    NameCache();
    ~NameCache();
    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
