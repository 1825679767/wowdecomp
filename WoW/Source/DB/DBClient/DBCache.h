#ifndef WOW_SOURCE_DB_DBCLIENT_DBCACHE_H
#define WOW_SOURCE_DB_DBCLIENT_DBCACHE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

class CHashKeyGUID;
class CWardenKey;

struct HASHKEY_INT {
    HASHKEY_INT();
    HASHKEY_INT(int32_t key);
    bool operator==(const HASHKEY_INT& key);
};

struct DBCACHECALLBACK : public TSLinkedNode<DBCACHECALLBACK> {
    /* 0008 */ void (*m_callback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ void* m_param;
};

template <class T, class TKey, class THashKey>
class DBCache : public Blizzard::Using::GlobalUse {
    public:
    struct DBCACHEHASH : public TSHashObject<DBCACHEHASH, THashKey> {
        T m_record;
        TKey m_id;
        bool m_loaded;
        TSList<DBCACHECALLBACK> m_callbacks;
        bool m_noSave;
        bool m_dispatching;
        bool m_invalidatePending;
        TSLink<DBCACHEHASH> m_pendingLink;
    };

    struct REVERSEENTRY : public TSHashObject<REVERSEENTRY, HASHKEY_CONSTSTR> {
        T* m_record;
    };

    /* 0008 */ TSHashTable<DBCACHEHASH, THashKey> m_entries;
    /* 0030 */ uint32_t m_signature;
    /* 0034 */ const char* m_fileName;
    /* 0038 */ uint32_t m_version;
    /* 003C */ uint32_t m_queryOpcode;
    /* 0040 */ uint32_t m_multipleQueryOpcode;
    /* 0044 */ bool m_sendRequester;
    /* 0045 */ bool m_enabled;
    /* 0046 */ bool m_loaded;
    /* 0048 */ uint32_t m_maxQueriesPerPump;
    /* 004C */ uint32_t m_queriesSent;
    /* 0050 */ uint32_t m_nextQueryTime;
    /* 0054 */ TSExplicitList<DBCACHEHASH, offsetof(DBCACHEHASH, m_pendingLink)> m_pending;
    /* 0060 */ TSHashTable<REVERSEENTRY, HASHKEY_CONSTSTR> m_reverse;

    virtual void InvalidateIntKey(int32_t key);

    DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t multipleQueryOpcode, bool sendRequester, bool enabled, uint32_t queriesPerMinute);
    ~DBCache();
    void AddItem(T* item, TKey key);
    void AddItems(CDataStore* msg, bool single);
    void CancelCallback(TKey key, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);
    void Clear();
    void ClearPending();
    void DenyItem(TKey key);
    T* GetRecord(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param, bool unique);
    void Destroy();
    void Invalidate(TKey key);
    REVERSEENTRY* FindReverseEntry(const char* name);
    T* GetRecordByName(const char* name);
    void RemoveReverseEntry(const char* name);
    void ItemAdded(T* item);
    void Load();
    void RetryItem(TKey key);
    void Save();
    void SendSingleQuery(DBCACHEHASH* hash);
    void SetTemporary(TKey key);
    void VerifyAndUpdateVersion(uint32_t version);
};

void DBCacheReadKey(CDataStore& msg, int32_t& key);
void DBCacheReadKey(CDataStore& msg, uint64_t& key);
void DBCacheReadKey(CDataStore& msg, CWardenKey& key);
void DBCacheWriteKey(CDataStore& msg, int32_t key);
void DBCacheWriteKey(CDataStore& msg, uint64_t key);
void DBCacheWriteKey(CDataStore& msg, const CWardenKey& key);
bool DBCacheIsNullKey(int32_t key);
bool DBCacheIsNullKey(uint64_t key);
bool DBCacheIsNullKey(const CWardenKey& key);
uint32_t DBCacheHashKey(const CWardenKey& key);
void GetCacheDirectory(char* path);

// INLINED
inline uint32_t DBCacheHashKey(int32_t key) {
    return key;
}

// INLINED
inline uint32_t DBCacheHashKey(uint64_t key) {
    return static_cast<uint32_t>(key);
}

// VERIFY: 8997838f auto
// OFFSET: [0x00675C80, 0x00675DB0, 0x00675EE0, 0x00676010, 0x00676140, 0x00676270, 0x006763A0, 0x006764D0, 0x00676600, 0x00676730, 0x00676860, 0x00676990, 0x00676AC0, 0x00676BF0, 0x00676D20]
template <class T, class TKey, class THashKey>
DBCache<T, TKey, THashKey>::DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t multipleQueryOpcode, bool sendRequester, bool enabled, uint32_t queriesPerMinute) : m_signature(signature), m_fileName(fileName), m_version(0), m_queryOpcode(queryOpcode), m_multipleQueryOpcode(multipleQueryOpcode), m_sendRequester(sendRequester), m_enabled(enabled), m_loaded(false), m_maxQueriesPerPump(30000 * queriesPerMinute / 60000), m_queriesSent(0), m_nextQueryTime(0) {
}

// VERIFY: 52330a1f auto
// OFFSET: [0x00675D20, 0x00675E50, 0x00675F80, 0x006760B0, 0x006761E0, 0x00676310, 0x00676440, 0x00676570, 0x006766A0, 0x006767D0, 0x00676900, 0x00676A30, 0x00676B60, 0x00676C90, 0x00676DC0]
template <class T, class TKey, class THashKey>
DBCache<T, TKey, THashKey>::~DBCache() {
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
}

// INLINED
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::ItemAdded(T* item) {
}

// VERIFY: c6498a89 manual
// OFFSET: [0x0067BA40, 0x0067C0E0, 0x0067C740, 0x0067CDD0, 0x0067D470, 0x0067DB90, 0x0067E0E0, 0x0067E740, 0x0067EC80, 0x0067F1C0, 0x0067F760, 0x0067FE00, 0x006803D0, 0x00680920, 0x00680D70]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Load() {
    this->m_loaded = true;

    if (!this->m_enabled) {
        return;
    }

    char cacheDirectory[260];
    GetCacheDirectory(cacheDirectory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", cacheDirectory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_READ, 1, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return;
    }

    uint8_t localBuffer[0x4000];
    uint8_t* buffer = localBuffer;
    uint32_t bufferSize = sizeof(localBuffer);
    uint32_t bytesRead;

    OsReadFile(file, buffer, 6 * sizeof(uint32_t), &bytesRead);

    if (bytesRead != 6 * sizeof(uint32_t)) {
        OsCloseFile(file);
        return;
    }

    CDataStore header(buffer, 6 * sizeof(uint32_t));

    uint32_t signature;
    header.Get(signature);

    if (signature != this->m_signature) {
        OsCloseFile(file);
        return;
    }

    uint32_t build;
    header.Get(build);

    if (build != 12340) {
        OsCloseFile(file);
        return;
    }

    uint32_t locale;
    header.Get(locale);

    if (locale != SFile::s_locale) {
        OsCloseFile(file);
        return;
    }

    uint32_t recordSize;
    header.Get(recordSize);

    if (recordSize != sizeof(T)) {
        OsCloseFile(file);
        return;
    }

    uint32_t cacheVersion;
    header.Get(cacheVersion);

    if (cacheVersion != T::s_cacheVersion) {
        OsCloseFile(file);
        return;
    }

    uint32_t version;
    header.Get(version);
    this->m_version = version;

    bool corrupt = false;
    OsReadFile(file, buffer, sizeof(TKey) + sizeof(uint32_t), &bytesRead);

    if (bytesRead == sizeof(TKey) + sizeof(uint32_t)) {
        while (true) {
            CDataStore entryHeader(buffer, sizeof(TKey) + sizeof(uint32_t));

            TKey key;
            DBCacheReadKey(entryHeader, key);

            uint32_t size;
            entryHeader.Get(size);

            if (DBCacheIsNullKey(key)) {
                break;
            }

            if (size > bufferSize) {
                if (bufferSize > sizeof(localBuffer)) {
                    SMemFree(buffer, __FILE__, __LINE__, 0);
                }

                bufferSize = size;
                buffer = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
            }

            OsReadFile(file, buffer, size, &bytesRead);

            if (bytesRead != size) {
                corrupt = true;
                break;
            }

            CDataStore data(buffer, size);

            DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

            if (!hash) {
                hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            }

            hash->m_record.Unpack(&data);
            hash->m_loaded = true;
            hash->m_id = key;
            this->ItemAdded(&hash->m_record);

            OsReadFile(file, buffer, sizeof(TKey) + sizeof(uint32_t), &bytesRead);

            if (bytesRead != sizeof(TKey) + sizeof(uint32_t)) {
                corrupt = true;
                break;
            }
        }
    } else {
        corrupt = true;
    }

    if (bufferSize > sizeof(localBuffer)) {
        SMemFree(buffer, __FILE__, __LINE__, 0);
    }

    OsCloseFile(file);

    if (corrupt) {
        this->Clear();
    }
}

// VERIFY: a9ebf5d0 manual
// OFFSET: [0x0067B6A0, 0x0067BD40, 0x0067CA30, 0x0067D930]
template <class T, class TKey, class THashKey>
T* DBCache<T, TKey, THashKey>::GetRecord(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param, bool unique) {
    if (DBCacheIsNullKey(key)) {
        return nullptr;
    }

    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        if (callback) {
            hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            hash->m_id = key;

            DBCACHECALLBACK* node = hash->m_callbacks.NewNode(2, 0, 0);
            node->m_callback = callback;
            node->m_guid = guid;
            node->m_param = param;

            if (this->m_maxQueriesPerPump && this->m_queriesSent >= this->m_maxQueriesPerPump) {
                this->m_pending.LinkToTail(hash);
            } else {
                this->SendSingleQuery(hash);
            }
        }

        return nullptr;
    }

    if (hash->m_loaded) {
        return &hash->m_record;
    }

    if (callback) {
        if (unique) {
            for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node; node = hash->m_callbacks.Next(node)) {
                if (node->m_callback == callback && node->m_param == param) {
                    return nullptr;
                }
            }
        }

        DBCACHECALLBACK* node = hash->m_callbacks.NewNode(2, 0, 0);
        node->m_callback = callback;
        node->m_guid = guid;
        node->m_param = param;
    }

    return nullptr;
}

// VERIFY: def725c6 manual
// OFFSET: [0x0067A0F0, 0x0067A3A0, 0x0067A8C0]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::CancelCallback(TKey key, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (hash->m_dispatching) {
        ConsolePrintf("DBCache::CancelCallback ignored for id %016I64X.", static_cast<int64_t>(key));
        return;
    }

    DBCACHECALLBACK* node = hash->m_callbacks.Head();

    while (node) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);

        if (node->m_callback == callback && node->m_param == param) {
            hash->m_callbacks.DeleteNode(node);
        }

        node = next;
    }
}

// VERIFY: 4266cb5c manual
// OFFSET: 0x00668DA0
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::SendSingleQuery(DBCACHEHASH* hash) {
    CDataStore msg;
    msg.Put(this->m_queryOpcode);
    DBCacheWriteKey(msg, hash->m_id);

    if (this->m_sendRequester) {
        DBCACHECALLBACK* node = hash->m_callbacks.Head();
        msg.Put(node ? node->m_guid.m_guid : 0);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_queriesSent++;
}

// VERIFY: e975696b auto
// OFFSET: [0x0066CC90]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Clear() {
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
}

// VERIFY: 0b524eff manual
// OFFSET: [0x0066A010, 0x0066A310, 0x0066A610, 0x0066A900, 0x0066AC00, 0x0066AF00, 0x0066B210, 0x0066B510, 0x0066B810, 0x0066BB00, 0x0066BDF0, 0x0066C0F0, 0x0066C400, 0x0066C710]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::ClearPending() {
    DBCACHEHASH* hash = this->m_entries.m_fulllist.Head();

    while (hash) {
        if (hash->m_loaded) {
            hash = this->m_entries.Next(hash);
        } else {
            DBCACHEHASH* next = this->m_entries.Next(hash);
            this->m_entries.Delete(hash);
            hash = next;
        }
    }
}

// VERIFY: b97fec3b manual
// OFFSET: [0x0066A090, 0x0066A390, 0x0066A690, 0x0066A980, 0x0066AC80, 0x0066AF80, 0x0066B290, 0x0066B590, 0x0066B890, 0x0066BB80, 0x0066BE70, 0x0066C170, 0x0066C480, 0x0066C790, 0x0066CA10]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Save() {
    if (!this->m_enabled || !this->m_loaded) {
        return;
    }

    char cacheDirectory[260];
    GetCacheDirectory(cacheDirectory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", cacheDirectory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 1, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        DebugLog("OsCreateFile failed for cache file: %s\n", path);
        return;
    }

    void* data;
    uint32_t written;

    CDataStore header;
    header.Put(this->m_signature);
    header.Put(static_cast<uint32_t>(12340));
    header.Put(SFile::s_locale);
    header.Put(static_cast<uint32_t>(sizeof(T)));
    header.Put(T::s_cacheVersion);
    header.Put(this->m_version);
    header.Finalize();
    header.GetDataInSitu(data, header.m_size);
    OsWriteFile(file, data, header.m_size, &written);

    CDataStore entry;

    for (DBCACHEHASH* hash = this->m_entries.m_fulllist.Head(); hash; hash = this->m_entries.Next(hash)) {
        if (!hash->m_loaded || hash->m_noSave) {
            continue;
        }

        entry.Reset();
        DBCacheWriteKey(entry, hash->m_id);
        uint32_t sizePos = entry.m_size;
        entry.Put(static_cast<uint32_t>(0));
        hash->m_record.Pack(&entry);
        entry.Set(sizePos, static_cast<uint32_t>(entry.m_size - sizeof(TKey) - sizeof(uint32_t)));
        entry.Finalize();
        entry.GetDataInSitu(data, entry.m_size);
        OsWriteFile(file, data, entry.m_size, &written);
    }

    TKey nullKey = TKey();
    uint32_t nullSize = 0;
    OsWriteFile(file, &nullKey, sizeof(nullKey), &written);
    OsWriteFile(file, &nullSize, sizeof(nullSize), &written);
    OsCloseFile(file);
}

// VERIFY: 0a542c5a auto
// OFFSET: [0x0066F910, 0x0066F980, 0x0066F9F0, 0x0066FA60, 0x0066FAD0, 0x0066FB40, 0x0066FBB0, 0x0066FC20, 0x0066FC90, 0x0066FD00, 0x0066FD70, 0x0066FDE0, 0x0066FE50, 0x0066FEC0]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Destroy() {
    this->Save();
    this->ClearPending();
    this->m_entries.Clear();
    this->m_reverse.Clear();
    this->m_loaded = false;
}

// VERIFY: dc40f23c auto
// OFFSET: [0x0066F940, 0x0066F9B0, 0x0066FA20, 0x0066FA90, 0x0066FB00, 0x0066FB70, 0x0066FBE0, 0x0066FC50, 0x0066FCC0, 0x0066FD30, 0x0066FDA0, 0x0066FE10, 0x0066FE80, 0x0066FEF0]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::VerifyAndUpdateVersion(uint32_t version) {
    if (!this->m_enabled || this->m_version == version) {
        return;
    }

    this->ClearPending();
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
    this->m_version = version;
}

// VERIFY: 27b57ae6 manual
// OFFSET: [0x0067CBD0]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::AddItems(CDataStore* msg, bool single) {
    uint8_t count;

    if (single) {
        count = 1;
    } else {
        msg->Get(count);

        if (!count) {
            return;
        }
    }

    do {
        bool denied = false;
        uint32_t id;
        msg->Get(id);

        if (static_cast<int32_t>(id) < 0) {
            id &= 0x7FFFFFFF;
            denied = true;
        }

        TKey key = id;

        DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

        if (denied) {
            if (hash) {
                hash->m_dispatching = true;

                for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
                    DBCACHECALLBACK* next = hash->m_callbacks.Next(node);
                    node->m_callback(key, node->m_guid, node->m_param, false);
                    node = next;
                }

                hash->m_dispatching = false;
                this->m_entries.Delete(hash);
            }
        } else {
            if (!hash) {
                hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            }

            hash->m_dispatching = true;
            hash->m_record.Unpack(msg);
            hash->m_loaded = true;
            hash->m_id = key;

            for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
                DBCACHECALLBACK* next = hash->m_callbacks.Next(node);
                node->m_callback(key, node->m_guid, node->m_param, true);
                node = next;
            }

            hash->m_callbacks.Clear();
            bool invalidate = hash->m_invalidatePending;
            hash->m_dispatching = false;

            if (invalidate) {
                this->Invalidate(key);
            }
        }
    } while (--count);
}

// VERIFY: 0168a135 manual
// OFFSET: [0x0067E030]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::AddItem(T* item, TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
    }

    hash->m_dispatching = true;
    hash->m_record = *item;
    hash->m_loaded = true;
    hash->m_id = key;

    for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);
        node->m_callback(static_cast<int32_t>(key), node->m_guid, node->m_param, true);
        node = next;
    }

    hash->m_callbacks.Clear();
    bool invalidate = hash->m_invalidatePending;
    hash->m_dispatching = false;

    if (invalidate) {
        this->Invalidate(key);
    }
}

// VERIFY: e5e95050 manual
// OFFSET: [0x0067A070]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::DenyItem(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    hash->m_dispatching = true;

    for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);
        node->m_callback(static_cast<int32_t>(key), node->m_guid, node->m_param, false);
        node = next;
    }

    hash->m_dispatching = false;
    this->m_entries.Delete(hash);
}

// VERIFY: 80ad7a9d auto
// OFFSET: [0x0067A170]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Invalidate(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (!hash->m_loaded) {
        this->DenyItem(key);
    } else if (hash->m_dispatching) {
        hash->m_invalidatePending = true;
    } else {
        this->m_entries.Delete(hash);
    }
}

#endif
