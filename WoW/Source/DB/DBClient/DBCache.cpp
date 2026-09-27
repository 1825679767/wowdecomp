#include "DBCache.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/GameClient/WardenClient.h"

// INLINED
HASHKEY_INT::HASHKEY_INT() {
}

// INLINED
HASHKEY_INT::HASHKEY_INT(int32_t key) {
}

// INLINED
bool HASHKEY_INT::operator==(const HASHKEY_INT& key) {
    return true;
}

// INLINED
void DBCacheReadKey(CDataStore& msg, int32_t& key) {
    msg.Get(reinterpret_cast<uint32_t&>(key));
}

// INLINED
void DBCacheReadKey(CDataStore& msg, uint64_t& key) {
    msg.Get(key);
}

// INLINED
void DBCacheReadKey(CDataStore& msg, CWardenKey& key) {
    msg >> key;
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, int32_t key) {
    msg.Put(static_cast<uint32_t>(key));
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, uint64_t key) {
    msg.Put(key);
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, const CWardenKey& key) {
    msg << key;
}

// INLINED
bool DBCacheIsNullKey(int32_t key) {
    return !key;
}

// INLINED
bool DBCacheIsNullKey(uint64_t key) {
    return !key;
}

// INLINED
bool DBCacheIsNullKey(const CWardenKey& key) {
    for (uint32_t i = 0; i < sizeof(key.m_key); i++) {
        if (key.m_key[i]) {
            return false;
        }
    }

    return true;
}

// INLINED
uint32_t DBCacheHashKey(int32_t key) {
    return key;
}

// INLINED
uint32_t DBCacheHashKey(uint64_t key) {
    return static_cast<uint32_t>(key);
}

// INLINED
uint32_t DBCacheHashKey(const CWardenKey& key) {
    return key.Hash();
}

// VERIFY: 8655bf5d auto
// OFFSET: 0x00667EF0
void GetCacheDirectory(char* path) {
    if (CheckDataFilesExist()) {
        SStrPrintf(path, 260, "Cache/%s/%s", "WDB", s_localeCode);
        OsCreateDirectory(path, 1);
    } else {
        SStrCopy(path, "WDB", 260);
        OsCreateDirectory(path, 1);
    }
}
