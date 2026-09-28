#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_GUILDSTATS_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_GUILDSTATS_C_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class GuildStats_C {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ int32_t m_id;
    /* 0004 */ uint8_t unk_0004[736];
    /* 02E4 */ int32_t m_emblemStyle;
    /* 02E8 */ int32_t m_emblemColor;
    /* 02EC */ int32_t m_borderStyle;
    /* 02F0 */ int32_t m_borderColor;
    /* 02F4 */ int32_t m_background;
    /* 02F8 */ uint32_t unk_02F8;

    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
