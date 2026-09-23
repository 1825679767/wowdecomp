#ifndef WOW_SOURCE_DB_DBCLIENT_DBCACHE_H
#define WOW_SOURCE_DB_DBCLIENT_DBCACHE_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"

struct HASHKEY_NONE;

class DBCache {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t m_entries[40];
    /* 0030 */ uint32_t m_signature;
    /* 0034 */ const char* m_fileName;
    /* 0038 */ uint32_t m_version;
    /* 003C */ uint32_t m_queryOpcode;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint8_t m_sendRequester;
    /* 0045 */ uint8_t m_enabled;
    /* 0046 */ uint8_t m_loaded;
    /* 0047 */ uint8_t unk_0047;
    /* 0048 */ uint32_t m_maxQueriesPerPump;
    /* 004C */ uint32_t m_queriesSent;
    /* 0050 */ uint32_t m_nextQueryTime;
    /* 0054 */ TSList<void> m_pending;
    /* 0060 */ uint8_t m_reverse[40];
};

#endif
