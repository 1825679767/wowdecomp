#ifndef WOW_SOURCE_DB_WOWCLIENTDB_H
#define WOW_SOURCE_DB_WOWCLIENTDB_H

#include <cstddef>
#include <cstdint>

class WowClientDB_Base {
    public:
    /* 0004 */ int32_t m_loaded;
    /* 0008 */ int32_t m_numRecords;
    /* 000C */ int32_t m_maxID;
    /* 0010 */ int32_t m_minID;
    /* 0014 */ const char* m_strings;

    virtual ~WowClientDB_Base();
};

template <class T>
class IDatabase {
    public:
    /* 0004 */ T* m_records;
    /* 0008 */ T** m_recordsById;

    virtual ~IDatabase();
};

template <class T>
class WowClientDB_Common : public WowClientDB_Base {
};

template <class T>
class WowClientDB : public WowClientDB_Common<T>, public IDatabase<T> {
};


#endif
