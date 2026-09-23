#ifndef STORM_H_STPL_H
#define STORM_H_STPL_H

#include <cstddef>
#include <cstdint>

template <class T>
class TSLink {
    public:
    /* 0000 */ TSLink<T>* m_prevLink;
    /* 0004 */ T* m_next;
};

template <class T>
class TSGetLink {
};

template <class T>
class TSGetExplicitLink {
};

template <class T, class TGetLink = TSGetLink<T> >
class TSList {
    public:
    /* 0000 */ ptrdiff_t m_linkoffset;
    /* 0004 */ TSLink<T> m_terminator;
};

template <class T>
class TSExplicitList : public TSList<T, TSGetExplicitLink<T> > {
};

template <class T>
class TSLinkedNode {
    public:
    /* 0000 */ TSLink<T> m_terminator;
};

template <class T>
class TSBaseArray {
    public:
    /* 0000 */ uint32_t m_alloc;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ T* m_data;
};

template <class T>
class TSFixedArray : public TSBaseArray<T> {
};

template <class T>
class TSGrowableArray : public TSFixedArray<T> {
    public:
    /* 000C */ uint32_t m_chunk;
};

template <class T, class TKey>
class TSHashObject {
    public:
    /* 0000 */ uint32_t m_hashval;
    /* 0004 */ TSLink<T> m_linktoslot;
    /* 000C */ TSLink<T> m_linktofull;
    /* 0014 */ TKey m_key;
};

template <class T, class TKey>
class TSHashTable {
    public:
    /* 0004 */ TSList<T> m_fulllist;
    /* 0010 */ uint32_t m_fullnessIndicator;
    /* 0014 */ TSGrowableArray<TSList<T> > m_slotlistarray;
    /* 0024 */ uint32_t m_slotmask;

    virtual ~TSHashTable();
};


struct HASHKEY_STR {
    /* 0000 */ char* m_str;
};

struct HASHKEY_STRI {
    /* 0000 */ char* m_str;
};

#endif
