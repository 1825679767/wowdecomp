#ifndef STORM_H_STPL_H
#define STORM_H_STPL_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Storm/H/storm.h"

template <class T>
class TSLink {
    public:
    /* 0000 */ TSLink<T>* m_prevLink;
    /* 0004 */ T* m_next;

    TSLink();
    ~TSLink();
    int32_t IsLinked();
    T* Next();
    TSLink<T>* NextLink(ptrdiff_t linkoffset);
    void Unlink();
};

template <class T>
class TSGetLink {
    public:
    static TSLink<T>* Link(T* nodeptr, ptrdiff_t linkoffset);
};

template <class T>
class TSGetExplicitLink {
    public:
    static TSLink<T>* Link(T* nodeptr, ptrdiff_t linkoffset);
};

template <class T, class TGetLink = TSGetLink<T> >
class TSList {
    public:
    /* 0000 */ ptrdiff_t m_linkoffset;
    /* 0004 */ TSLink<T> m_terminator;

    TSList();
    TSList(const TSList<T, TGetLink>& source);
    ~TSList();
    void ChangeLinkOffset(ptrdiff_t linkoffset);
    void Clear();
    T* DeleteNode(T* ptr);
    T* Head();
    void InitializeTerminator();
    int32_t IsLinked(T* ptr);
    TSLink<T>* Link(T* ptr);
    void LinkNode(T* ptr, uint32_t linktype, T* existingptr);
    void LinkToHead(T* ptr);
    void LinkToTail(T* ptr);
    T* NewNode(uint32_t location, size_t extrabytes, uint32_t flags);
    T* Next(T* ptr);
    void SetLinkOffset(ptrdiff_t linkoffset);
    void UnlinkAll();
    void UnlinkNode(T* ptr);
};

template <class T, int offset = static_cast<int>(0xDDDDDDDD)>
class TSExplicitList : public TSList<T, TSGetExplicitLink<T> > {
    public:
    TSExplicitList();
};

template <class T>
class TSLinkedNode {
    public:
    /* 0000 */ TSLink<T> m_link;
};

template <class T>
class TSBaseArray {
    public:
    /* 0000 */ uint32_t m_alloc;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ T* m_data;

    T& operator[](uint32_t index);
    uint32_t Count();
    const char* MemFileName();
    int32_t MemLineNo();
    T* Ptr();
    T* Top();
};

template <class T>
class TSFixedArray : public TSBaseArray<T> {
    public:
    TSFixedArray();
    ~TSFixedArray();
    void Clear();
    void ReallocAndClearData(uint32_t count);
    void ReallocData(uint32_t count);
    void Set(uint32_t count, const T* data);
    void SetCount(uint32_t count);
};

template <class T>
class TSGrowableArray : public TSFixedArray<T> {
    public:
    /* 000C */ uint32_t m_chunk;

    TSGrowableArray();
    uint32_t Add(uint32_t count, const T* data);
    uint32_t CalcChunkSize(uint32_t count);
    void GrowToFit(uint32_t index, int32_t zero);
    T* New();
    T* New(const T& source);
    void SetCount(uint32_t count);
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
    /* 0004 */ TSExplicitList<T> m_fulllist;
    /* 0010 */ uint32_t m_fullnessIndicator;
    /* 0014 */ TSGrowableArray<TSExplicitList<T> > m_slotlistarray;
    /* 0024 */ uint32_t m_slotmask;

    virtual void InternalDelete(T* ptr);
    virtual T* InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags);
    virtual ~TSHashTable();

    TSHashTable();
    void Clear();
    void Destroy();
    void GrowListArray(uint32_t newarraysize);
    void Initialize();
    void Insert(T* ptr, uint32_t hashval, const TKey& key);
    void Insert(T* ptr, uint32_t hashval, const char* str);
    void InternalClear(int32_t warn);
    void InternalLinkNode(T* ptr, uint32_t hashval);
    T* InternalNewNode(uint32_t hashval, size_t extrabytes, uint32_t flags);
    int32_t MonitorFullness(uint32_t slot);
    T* New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags);
    T* New(const char* str, size_t extrabytes, uint32_t flags);
    T* Ptr(uint32_t hashval, const TKey& key);
    T* Ptr(const char* str);
    void Unlink(T* ptr);
};

struct HASHKEY_CONSTSTR {
    /* 0000 */ const char* m_str;

    bool operator==(const char* str);
};

struct HASHKEY_NONE {
    /* 0000 */ int32_t m_unused;

    bool operator==(const HASHKEY_NONE& key);
};

struct HASHKEY_PTR {
    /* 0000 */ void* m_key;
};

struct HASHKEY_STR {
    /* 0000 */ char* m_str;

    HASHKEY_STR();
    ~HASHKEY_STR();
    HASHKEY_STR& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_STRI {
    /* 0000 */ char* m_str;

    bool operator==(const char* str);
};

struct HASHKEY_UTF8I {
    /* 0000 */ char* m_str;

    bool operator==(const char* str);
};

template <class T>
TSLink<T>::TSLink() {
    this->m_prevLink = nullptr;
    this->m_next = nullptr;
}

template <class T>
TSLink<T>::~TSLink() {
    this->Unlink();
}

template <class T>
int32_t TSLink<T>::IsLinked() {
    return this->m_next != nullptr;
}

template <class T>
T* TSLink<T>::Next() {
    return (reinterpret_cast<uintptr_t>(this->m_next) & 1) ? nullptr : this->m_next;
}

template <class T>
TSLink<T>* TSLink<T>::NextLink(ptrdiff_t linkoffset) {
    uintptr_t next = reinterpret_cast<uintptr_t>(this->m_next);

    if ((next & 1) || !next) {
        return reinterpret_cast<TSLink<T>*>(next & ~static_cast<uintptr_t>(1));
    }

    if (linkoffset < 0) {
        linkoffset = reinterpret_cast<uint8_t*>(this) - reinterpret_cast<uint8_t*>(this->m_prevLink->m_next);
    }

    return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uint8_t*>(this->m_next) + linkoffset);
}

// OFFSET: 0x0079F820
template <class T>
void TSLink<T>::Unlink() {
    if (this->m_prevLink) {
        this->NextLink(-1)->m_prevLink = this->m_prevLink;
        this->m_prevLink->m_next = this->m_next;
        this->m_prevLink = nullptr;
        this->m_next = nullptr;
    }
}

template <class T>
TSLink<T>* TSGetLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return &static_cast<TSLinkedNode<T>*>(nodeptr)->m_link;
}

template <class T>
TSLink<T>* TSGetExplicitLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uint8_t*>(nodeptr) + linkoffset);
}

template <class T, class TGetLink>
TSList<T, TGetLink>::TSList() {
    this->SetLinkOffset(0);
}

template <class T, class TGetLink>
TSList<T, TGetLink>::TSList(const TSList<T, TGetLink>& source) {
    this->SetLinkOffset(source.m_linkoffset);
}

template <class T, class TGetLink>
TSList<T, TGetLink>::~TSList() {
    this->UnlinkAll();
}

template <class T, class TGetLink>
void TSList<T, TGetLink>::ChangeLinkOffset(ptrdiff_t linkoffset) {
    if (linkoffset != this->m_linkoffset) {
        this->UnlinkAll();
        this->SetLinkOffset(linkoffset);
    }
}

// OFFSET: 0x00485970
template <class T, class TGetLink>
void TSList<T, TGetLink>::Clear() {
    T* node;

    while ((node = this->Head()) != nullptr) {
        this->DeleteNode(node);
    }
}

// OFFSET: 0x004652A0
template <class T, class TGetLink>
T* TSList<T, TGetLink>::DeleteNode(T* ptr) {
    T* next = this->Next(ptr);

    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), -2, 0);

    return next;
}

template <class T, class TGetLink>
T* TSList<T, TGetLink>::Head() {
    return this->m_terminator.Next();
}

template <class T, class TGetLink>
void TSList<T, TGetLink>::InitializeTerminator() {
    this->m_terminator.m_prevLink = &this->m_terminator;
    this->m_terminator.m_next = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(&this->m_terminator) | 1);
}

// OFFSET: 0x004D4820
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsLinked(T* ptr) {
    return this->Link(ptr)->IsLinked();
}

template <class T, class TGetLink>
TSLink<T>* TSList<T, TGetLink>::Link(T* ptr) {
    return ptr ? TGetLink::Link(ptr, this->m_linkoffset) : &this->m_terminator;
}

// OFFSET: 0x006714A0
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkNode(T* ptr, uint32_t linktype, T* existingptr) {
    TSLink<T>* link = this->Link(ptr);
    link->Unlink();

    TSLink<T>* existinglink = this->Link(existingptr);

    if (linktype == 1) {
        link->m_prevLink = existinglink;
        link->m_next = existinglink->m_next;
        existinglink->NextLink(this->m_linkoffset)->m_prevLink = link;
        existinglink->m_next = ptr;
    } else {
        link->m_prevLink = existinglink->m_prevLink;
        link->m_next = existinglink->m_prevLink->m_next;
        existinglink->m_prevLink->m_next = ptr;
        existinglink->m_prevLink = link;
    }
}

// OFFSET: 0x007B5020
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToHead(T* ptr) {
    this->LinkNode(ptr, 1, nullptr);
}

// OFFSET: 0x006DED60
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToTail(T* ptr) {
    this->LinkNode(ptr, 2, nullptr);
}

// OFFSET: 0x004F3420
template <class T, class TGetLink>
T* TSList<T, TGetLink>::NewNode(uint32_t location, size_t extrabytes, uint32_t flags) {
    T* node = new (SMemAlloc(sizeof(T) + extrabytes, typeid(T).raw_name(), -2, flags | 0x8)) T;

    if (location) {
        this->LinkNode(node, location, nullptr);
    }

    return node;
}

template <class T, class TGetLink>
T* TSList<T, TGetLink>::Next(T* ptr) {
    return this->Link(ptr)->Next();
}

template <class T, class TGetLink>
void TSList<T, TGetLink>::SetLinkOffset(ptrdiff_t linkoffset) {
    this->m_linkoffset = linkoffset;
    this->InitializeTerminator();
}

// OFFSET: 0x007CECD0
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkAll() {
    T* node;

    while ((node = this->Head()) != nullptr) {
        this->UnlinkNode(node);
    }
}

// OFFSET: 0x004BFEE0
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkNode(T* ptr) {
    this->Link(ptr)->Unlink();
}

template <class T, int offset>
TSExplicitList<T, offset>::TSExplicitList() {
    this->SetLinkOffset(offset);
}

template <class T>
T& TSBaseArray<T>::operator[](uint32_t index) {
    return this->m_data[index];
}

template <class T>
uint32_t TSBaseArray<T>::Count() {
    return this->m_count;
}

template <class T>
const char* TSBaseArray<T>::MemFileName() {
    return typeid(T).raw_name();
}

template <class T>
int32_t TSBaseArray<T>::MemLineNo() {
    return -2;
}

template <class T>
T* TSBaseArray<T>::Ptr() {
    return this->m_data;
}

// OFFSET: 0x0077AC60
template <class T>
T* TSBaseArray<T>::Top() {
    return this->m_count ? &this->m_data[this->m_count - 1] : nullptr;
}

template <class T>
TSFixedArray<T>::TSFixedArray() {
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = nullptr;
}

// OFFSET: 0x0047C630
template <class T>
TSFixedArray<T>::~TSFixedArray() {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (this->m_data) {
        SMemFree(this->m_data, this->MemFileName(), this->MemLineNo(), 0);
    }
}

// OFFSET: 0x00409770
template <class T>
void TSFixedArray<T>::Clear() {
    this->~TSFixedArray<T>();
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = nullptr;
}

template <class T>
void TSFixedArray<T>::ReallocAndClearData(uint32_t count) {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (count != this->m_count) {
        this->m_alloc = count;

        if (this->m_data || count) {
            this->m_data = static_cast<T*>(SMemReAlloc(this->m_data, sizeof(T) * count, this->MemFileName(), this->MemLineNo(), 0));
        }
    }
}

// OFFSET: 0x00496680
template <class T>
void TSFixedArray<T>::ReallocData(uint32_t count) {
    T* olddata = this->m_data;

    for (uint32_t i = count; i < this->m_count; i++) {
        olddata[i].~T();
    }

    this->m_alloc = count;
    this->m_data = static_cast<T*>(SMemReAlloc(olddata, sizeof(T) * count, this->MemFileName(), this->MemLineNo(), 0x10));

    if (!this->m_data) {
        this->m_data = static_cast<T*>(SMemAlloc(sizeof(T) * count, this->MemFileName(), this->MemLineNo(), 0));

        if (olddata) {
            uint32_t copycount = count < this->m_count ? count : this->m_count;

            for (uint32_t i = 0; i < copycount; i++) {
                new (&this->m_data[i]) T(olddata[i]);
                olddata[i].~T();
            }

            SMemFree(olddata, this->MemFileName(), this->MemLineNo(), 0);
        }
    }
}

// OFFSET: 0x0077AC80
template <class T>
void TSFixedArray<T>::Set(uint32_t count, const T* data) {
    this->ReallocAndClearData(count);

    for (uint32_t i = 0; i < count; i++) {
        new (&this->m_data[i]) T(data[i]);
    }

    this->m_count = count;
}

// OFFSET: 0x004670D0
template <class T>
void TSFixedArray<T>::SetCount(uint32_t count) {
    if (count != this->m_count) {
        if (count) {
            this->ReallocData(count);

            for (uint32_t i = this->m_count; i < count; i++) {
                new (&this->m_data[i]) T;
            }

            this->m_count = count;
        } else {
            this->Clear();
        }
    }
}

template <class T>
TSGrowableArray<T>::TSGrowableArray() {
    this->m_chunk = 0;
}

// OFFSET: 0x006BADD0
template <class T>
uint32_t TSGrowableArray<T>::Add(uint32_t count, const T* data) {
    uint32_t newcount = this->m_count + count;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count];
    T* end = element + count;

    for (; element != end; element++) {
        new (element) T(*data++);
    }

    this->m_count += count;

    return this->m_count - count;
}

// OFFSET: 0x005D0040
template <class T>
uint32_t TSGrowableArray<T>::CalcChunkSize(uint32_t count) {
    uint32_t maxchunk = 256 / sizeof(T);

    if (count >= maxchunk) {
        this->m_chunk = maxchunk;
        return maxchunk;
    }

    uint32_t result = count;

    for (uint32_t bits = count & (count - 1); bits; bits &= bits - 1) {
        result = bits;
    }

    return result ? result : 1;
}

// OFFSET: 0x00481080
template <class T>
void TSGrowableArray<T>::GrowToFit(uint32_t index, int32_t zero) {
    if (index >= this->m_count) {
        uint32_t newcount = index + 1;

        if (newcount > this->m_alloc) {
            uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);
            uint32_t alloc = newcount;

            if (newcount % chunk) {
                alloc = newcount + chunk - newcount % chunk;
            }

            this->ReallocData(alloc);
        }

        if (zero) {
            memset(&this->m_data[this->m_count], 0, sizeof(T) * (index - this->m_count + 1));
        }

        this->m_count = newcount;
    }
}

// OFFSET: 0x00405970
template <class T>
T* TSGrowableArray<T>::New() {
    uint32_t newcount = this->m_count + 1;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count++];

    return new (element) T;
}

// OFFSET: 0x00480FD0
template <class T>
T* TSGrowableArray<T>::New(const T& source) {
    uint32_t newcount = this->m_count + 1;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count++];

    return new (element) T(source);
}

// OFFSET: 0x004969C0
template <class T>
void TSGrowableArray<T>::SetCount(uint32_t count) {
    if (count > this->m_count) {
        if (count > this->m_alloc) {
            uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(count);
            uint32_t alloc = count;

            if (count % chunk) {
                alloc = count + chunk - count % chunk;
            }

            this->ReallocData(alloc);
        }

        for (uint32_t i = this->m_count; i < count; i++) {
            new (&this->m_data[i]) T;
        }
    } else if (count < this->m_count) {
        for (uint32_t i = count; i < this->m_count; i++) {
            this->m_data[i].~T();
        }
    }

    this->m_count = count;
}

// OFFSET: 0x0047CA30
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalDelete(T* ptr) {
    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), -2, 0);
}

// OFFSET: 0x0047C9D0
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    T* node = new (SMemAlloc(sizeof(T) + extrabytes, typeid(T).raw_name(), -2, flags | 0x8)) T;
    listptr->LinkToHead(node);

    return node;
}

// OFFSET: 0x0047C870
template <class T, class TKey>
TSHashTable<T, TKey>::~TSHashTable() {
    this->InternalClear(1);
}

// OFFSET: 0x0047C750
template <class T, class TKey>
TSHashTable<T, TKey>::TSHashTable() {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.ChangeLinkOffset(offsetof(T, m_linktofull));
    this->m_slotmask = 0xFFFFFFFF;
}

template <class T, class TKey>
void TSHashTable<T, TKey>::Clear() {
    this->InternalClear(0);
}

// OFFSET: 0x0047C910
template <class T, class TKey>
void TSHashTable<T, TKey>::Destroy() {
    this->InternalClear(1);
    this->m_slotmask = 0xFFFFFFFF;
    this->m_slotlistarray.Clear();
}

// OFFSET: 0x00492C70
template <class T, class TKey>
void TSHashTable<T, TKey>::GrowListArray(uint32_t newarraysize) {
    uint32_t oldarraysize = this->m_slotmask + 1;

    TSExplicitList<T> templist;
    templist.ChangeLinkOffset(offsetof(T, m_linktoslot));

    for (uint32_t i = 0; i < oldarraysize; i++) {
        T* node;

        while ((node = this->m_slotlistarray[i].Head()) != nullptr) {
            templist.LinkToTail(node);
        }
    }

    this->m_slotlistarray.SetCount(newarraysize);

    for (uint32_t i = 0; i < newarraysize; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }

    this->m_slotmask = newarraysize - 1;

    T* node;

    while ((node = templist.Head()) != nullptr) {
        this->m_slotlistarray[node->m_hashval & this->m_slotmask].LinkToTail(node);
    }
}

// OFFSET: 0x00492C10
template <class T, class TKey>
void TSHashTable<T, TKey>::Initialize() {
    this->m_slotmask = 3;
    this->m_slotlistarray.SetCount(4);

    for (uint32_t i = 0; i <= this->m_slotmask; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }
}

// OFFSET: 0x004B93B0
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const TKey& key) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = key;
}

// OFFSET: 0x006C8A20
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const char* str) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = str;
}

// OFFSET: 0x00591990
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalClear(int32_t warn) {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.UnlinkAll();

    if (warn) {
        for (uint32_t i = 0; i < this->m_slotlistarray.Count(); i++) {
            this->m_slotlistarray[i].UnlinkAll();
        }
    } else {
        for (uint32_t i = 0; i < this->m_slotlistarray.Count(); i++) {
            T* node;

            while ((node = this->m_slotlistarray[i].Head()) != nullptr) {
                this->InternalDelete(node);
            }
        }
    }
}

// OFFSET: 0x004F38D0
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalLinkNode(T* ptr, uint32_t hashval) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        this->Initialize();
    }

    uint32_t slot = hashval & this->m_slotmask;

    if (this->MonitorFullness(slot)) {
        slot = hashval & this->m_slotmask;
    }

    this->m_slotlistarray[slot].LinkToTail(ptr);
    this->m_fulllist.LinkToTail(ptr);
    ptr->m_hashval = hashval;
}

// OFFSET: 0x00492F10
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNewNode(uint32_t hashval, size_t extrabytes, uint32_t flags) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        this->Initialize();
    }

    uint32_t slot = hashval & this->m_slotmask;

    if (this->MonitorFullness(slot)) {
        slot = hashval & this->m_slotmask;
    }

    T* node = this->InternalNew(&this->m_slotlistarray[slot], extrabytes, flags);
    this->m_fulllist.LinkToTail(node);

    return node;
}

// OFFSET: 0x00492E70
template <class T, class TKey>
int32_t TSHashTable<T, TKey>::MonitorFullness(uint32_t slot) {
    if (this->m_slotmask >= 0x1FFF) {
        return 0;
    }

    this->m_fullnessIndicator = this->m_fullnessIndicator > 3 ? this->m_fullnessIndicator - 3 : 0;

    for (T* node = this->m_slotlistarray[slot].Head(); node; node = this->m_slotlistarray[slot].Next(node)) {
        if (++this->m_fullnessIndicator > 13) {
            this->m_fullnessIndicator = 0;
            this->GrowListArray(2 * this->m_slotmask + 2);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00509720
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = str;

    return node;
}

// OFFSET: 0x005097A0
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(const char* str, size_t extrabytes, uint32_t flags) {
    return this->New(SStrHashHT(str), str, extrabytes, flags);
}

// OFFSET: 0x007BDDF0
template <class T, class TKey>
T* TSHashTable<T, TKey>::Ptr(uint32_t hashval, const TKey& key) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        return nullptr;
    }

    TSExplicitList<T>& list = this->m_slotlistarray[hashval & this->m_slotmask];

    for (T* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_hashval == hashval && node->m_key == key) {
            return node;
        }
    }

    return nullptr;
}

// OFFSET: 0x004BE0E0
template <class T, class TKey>
T* TSHashTable<T, TKey>::Ptr(const char* str) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        return nullptr;
    }

    uint32_t hashval = SStrHashHT(str);
    TSExplicitList<T>& list = this->m_slotlistarray[hashval & this->m_slotmask];

    for (T* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_hashval == hashval && node->m_key == str) {
            return node;
        }
    }

    return nullptr;
}

// OFFSET: 0x00565030
template <class T, class TKey>
void TSHashTable<T, TKey>::Unlink(T* ptr) {
    if (ptr->m_linktoslot.IsLinked()) {
        ptr->m_linktoslot.Unlink();
        ptr->m_linktofull.Unlink();
    }
}

inline bool HASHKEY_CONSTSTR::operator==(const char* str) {
    return this->m_str == str || SStrCmp(this->m_str, str, 0x7FFFFFFF) == 0;
}

inline bool HASHKEY_NONE::operator==(const HASHKEY_NONE& key) {
    return true;
}

inline HASHKEY_STR::HASHKEY_STR() {
    this->m_str = nullptr;
}

inline HASHKEY_STR::~HASHKEY_STR() {
    if (this->m_str) {
        SMemFree(this->m_str, __FILE__, __LINE__, 0);
    }
}

inline HASHKEY_STR& HASHKEY_STR::operator=(const char* str) {
    if (this->m_str != str) {
        if (this->m_str) {
            SMemFree(this->m_str, __FILE__, __LINE__, 0);
        }

        this->m_str = SStrDupA(str, __FILE__, __LINE__);
    }

    return *this;
}

inline bool HASHKEY_STR::operator==(const char* str) {
    return SStrCmp(this->m_str, str, 0x7FFFFFFF) == 0;
}

inline bool HASHKEY_STRI::operator==(const char* str) {
    return SStrCmpI(this->m_str, str, 0x7FFFFFFF) == 0;
}

inline bool HASHKEY_UTF8I::operator==(const char* str) {
    return SStrCmpUTF8I(this->m_str, str, 0x7FFFFFFF) == 0;
}

#endif
