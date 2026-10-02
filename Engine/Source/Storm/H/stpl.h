#ifndef ENGINE_SOURCE_STORM_H_STPL_H
#define ENGINE_SOURCE_STORM_H_STPL_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"

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
    T* Prev();
    T* RawNext();
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

    static int32_t IsEnd(T* ptr);

    TSList();
    TSList(const TSList<T, TGetLink>& source);
    ~TSList();
    void ChangeLinkOffset(ptrdiff_t linkoffset);
    void Clear();
    void Combine(TSList<T, TGetLink>* list, uint32_t linktype, T* existingptr);
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
    T* Prev(T* ptr);
    T* RawNext(T* ptr);
    void SetLinkOffset(ptrdiff_t linkoffset);
    T* Tail();
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

    T* Next();
    T* RawNext();
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
    TSFixedArray(const TSFixedArray<T>& source);
    ~TSFixedArray();
    TSFixedArray<T>& operator=(const TSFixedArray<T>& source);
    void Clear();
    void ReallocAndClearData(uint32_t count);
    void ReallocData(uint32_t count);
    void Set(uint32_t count, const T* data);
    void SetCount(uint32_t count);
};

template <class T>
class TSBitField {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T m_bits;

    void Load(const void* data, uint32_t bytes, int32_t bits);
};

template <class T>
class TSStackArray {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;

    TSStackArray(T* data, uint32_t count, int32_t construct);
    void SetCount(uint32_t count);
};

template <class T>
class TSGrowableArray : public TSFixedArray<T> {
    public:
    /* 000C */ uint32_t m_chunk;

    TSGrowableArray();
    uint32_t Add(uint32_t count, const T* data);
    uint32_t Add(uint32_t count, uint32_t incr, const T* data);
    uint32_t CalcChunkSize(uint32_t count);
    void GrowToFit(uint32_t index, int32_t zero);
    T* New();
    T* New(const T& source);
    void Reserve(uint32_t count, int32_t round);
    void SetCount(uint32_t count);
};

template <class T, uint32_t N>
class TSCGrowableArray {
    public:
    /* 0000 */ union {
        T m_inline[N];
        TSGrowableArray<T> m_dynamic;
    };
    uint32_t m_count;

    TSCGrowableArray();
    ~TSCGrowableArray();
    T& operator[](uint32_t index);
    void BecomeDynamic();
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

    ~TSHashObject();
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
    virtual void Destroy();

    TSHashTable();
    void Clear();
    void Delete(T* ptr);
    T* DeleteNode(T* ptr);
    void GrowListArray(uint32_t newarraysize);
    void Initialize();
    void Insert(T* ptr, uint32_t hashval, const TKey& key);
    void Insert(T* ptr, uint32_t hashval, const char* str);
    void Insert(T* ptr, const char* str);
    void InternalClear(int32_t warn);
    void InternalLinkNode(T* ptr, uint32_t hashval);
    T* InternalNewNode(uint32_t hashval, size_t extrabytes, uint32_t flags);
    int32_t MonitorFullness(uint32_t slot);
    T* New(uint32_t hashval, const TKey& key, size_t extrabytes, uint32_t flags);
    T* New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags);
    T* New(const char* str, size_t extrabytes, uint32_t flags);
    T* Next(T* ptr);
    T* Ptr(uint32_t hashval, const TKey& key);
    T* Ptr(const char* str);
    void Unlink(T* ptr);
};

struct HASHKEY_CONSTSTR {
    /* 0000 */ const char* m_str;

    bool operator==(const char* str);
};

struct HASHKEY_CONSTSTRI : public HASHKEY_CONSTSTR {
    HASHKEY_CONSTSTRI& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_DWORD {
    /* 0000 */ uint32_t m_key;

    bool operator==(const HASHKEY_DWORD& key);
};

struct HASHKEY_NONE {
    bool operator==(const HASHKEY_NONE& key);
};

struct HASHKEY_PTR {
    /* 0000 */ void* m_key;

    bool operator==(const HASHKEY_PTR& key);
};

struct HASHKEY_STR {
    /* 0000 */ char* m_str;

    HASHKEY_STR();
    ~HASHKEY_STR();
    HASHKEY_STR& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_STRI : public HASHKEY_STR {
    HASHKEY_STRI& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_UTF8I {
    /* 0000 */ char* m_str;

    HASHKEY_UTF8I& operator=(const char* str);
    bool operator==(const char* str);
};

template <class T, class TKey>
class TSHashTableReuse : public TSHashTable<T, TKey> {
    public:
    /* 0028 */ TSExplicitList<T> m_reuseList;

    virtual void InternalDelete(T* ptr);
    virtual T* InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags);
    virtual ~TSHashTableReuse();
    virtual void Destroy();

    TSHashTableReuse();
    void Destructor();
};

template <class T, class THandle>
class TSExportTableSimpleReuse : public TSHashTableReuse<T, HASHKEY_NONE> {
    public:
    /* 0034 */ HASHKEY_NONE m_key;
    /* 0038 */ uint32_t m_sequence;
    /* 003C */ int32_t m_wrapped;

    TSExportTableSimpleReuse();
    void Delete(T* ptr);
    THandle GenerateUniqueHandle();
    T* New(THandle* handle);
    T* Ptr(THandle handle);
};

template <class T, class THandle, class TLockedHandle, class TSync>
class TSExportTableSyncReuse : public TSExportTableSimpleReuse<T, THandle> {
    public:
    /* 0040 */ TSync m_sync;

    ~TSExportTableSyncReuse();
    void Delete(THandle handle);
    void DeleteUnlock(T* ptr, TLockedHandle lockedhandle);
    int32_t IsForWriting(TLockedHandle lockedhandle);
    T* Lock(THandle handle, TLockedHandle* lockedhandle, int32_t forwriting);
    T* NewLock(THandle* handle, TLockedHandle* lockedhandle);
    void SyncEnterLock(TLockedHandle* lockedhandle, int32_t forwriting);
    void SyncLeaveLock(TLockedHandle lockedhandle);
    void Unlock(TLockedHandle lockedhandle);
};

// INLINED
template <class T>
TSLink<T>::TSLink() {
    this->m_prevLink = nullptr;
    this->m_next = nullptr;
}

// INLINED
template <class T>
TSLink<T>::~TSLink() {
    this->Unlink();
}

// INLINED
template <class T>
int32_t TSLink<T>::IsLinked() {
    return this->m_next != nullptr;
}

// INLINED
template <class T>
T* TSLink<T>::Next() {
    return (reinterpret_cast<uintptr_t>(this->m_next) & 1) ? nullptr : this->m_next;
}

// VERIFY: 0f453bf0 manual
// OFFSET: 0x0047DB10
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

// INLINED
template <class T>
T* TSLink<T>::Prev() {
    return this->m_prevLink->m_prevLink->Next();
}

// INLINED
template <class T>
T* TSLink<T>::RawNext() {
    return this->m_next;
}

// VERIFY: ecb66e10 auto
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

// INLINED
template <class T>
TSLink<T>* TSGetLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return &static_cast<TSLinkedNode<T>*>(nodeptr)->m_link;
}

// INLINED
template <class T>
TSLink<T>* TSGetExplicitLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uint8_t*>(nodeptr) + linkoffset);
}

// VERIFY: 718ec6f6 auto
// OFFSET: 0x00407900
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsEnd(T* ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) & 1) || !ptr;
}

// INLINED
template <class T, class TGetLink>
TSList<T, TGetLink>::TSList() {
    this->SetLinkOffset(0);
}

// INLINED
template <class T, class TGetLink>
TSList<T, TGetLink>::TSList(const TSList<T, TGetLink>& source) {
    this->SetLinkOffset(source.m_linkoffset);
}

// INLINED
template <class T, class TGetLink>
TSList<T, TGetLink>::~TSList() {
    this->UnlinkAll();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::ChangeLinkOffset(ptrdiff_t linkoffset) {
    if (linkoffset != this->m_linkoffset) {
        this->UnlinkAll();
        this->SetLinkOffset(linkoffset);
    }
}

// VERIFY: ab66a452 manual
// OFFSET: [0x0047C470, 0x0047E7C0, 0x0047E830, 0x0047E8A0, 0x00485970, 0x00488940, 0x00489A60, 0x00498AE0, 0x00498B80, 0x0049A610, 0x004B73E0, 0x004B7460, 0x004BA230, 0x004BA2A0, 0x004BCEF0, 0x004C9650, 0x004CA310, 0x004CDA60, 0x004DA510, 0x004F1650, 0x004F92A0, 0x00502270, 0x00503060, 0x00527B00, 0x00528690, 0x0052F090, 0x0053EEB0, 0x0055FD20, 0x0056C9C0, 0x005AE0C0, 0x005B2170, 0x005B2260, 0x005B4CF0, 0x005BE660, 0x005BE6D0, 0x005BE740, 0x005F3DB0, 0x00606480, 0x00615FF0, 0x00617210, 0x00633470, 0x00670070, 0x00687FD0, 0x006BC100, 0x006BDDC0, 0x006BDE40, 0x006BF210, 0x006E4B80, 0x006EBF90, 0x006F2B70, 0x006F2BE0, 0x006FAF40, 0x0074F550, 0x0074F640, 0x00764420, 0x00771C20, 0x00772EF0, 0x00788B20, 0x007FBC20, 0x00804D90, 0x00804E00, 0x00807650, 0x0081A530, 0x0083D2D0, 0x008489F0, 0x00848A60, 0x0086E850, 0x008C8550, 0x008C8640, 0x0095B8E0]
template <class T, class TGetLink>
void TSList<T, TGetLink>::Clear() {
    T* node;

    while ((node = this->Head()) != nullptr) {
        this->DeleteNode(node);
    }
}

// VERIFY: 2b2e6d24 manual
// OFFSET: [0x004652A0, 0x0047E600, 0x00480720, 0x004807A0, 0x00485570, 0x00489AD0, 0x00490D00, 0x0049A680, 0x004D4FB0, 0x005022E0, 0x005030F0, 0x00527B70, 0x0053EF20, 0x0057AE00, 0x005AE130, 0x005B21E0, 0x005B22D0, 0x005B4C70, 0x005B4D60, 0x005BE7B0, 0x005EDA10, 0x005EDA90, 0x006064F0, 0x006330F0, 0x00633130, 0x0066FF60, 0x006BDE00, 0x006F20A0, 0x0074F5C0, 0x0077E830, 0x00788B90, 0x007C3330, 0x007CB600, 0x007D5CD0, 0x007FC070, 0x00804E70, 0x0081A5A0, 0x0082DB30, 0x00831BB0, 0x008C85C0, 0x008C86B0, 0x008CED10, 0x0095AA30, 0x0096CE80]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::DeleteNode(T* ptr) {
    T* next = this->Next(ptr);

    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);

    return next;
}

// VERIFY: 1639e241 manual
// OFFSET: 0x006D7BA0
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Head() {
    return this->m_terminator.Next();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::InitializeTerminator() {
    this->m_terminator.m_prevLink = &this->m_terminator;
    this->m_terminator.m_next = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(&this->m_terminator) | 1);
}

// VERIFY: e5b0ed2f auto
// OFFSET: 0x004D4820
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsLinked(T* ptr) {
    return this->Link(ptr)->IsLinked();
}

// INLINED
template <class T, class TGetLink>
TSLink<T>* TSList<T, TGetLink>::Link(T* ptr) {
    return ptr ? TGetLink::Link(ptr, this->m_linkoffset) : &this->m_terminator;
}

// VERIFY: 2f0fe850 manual
// OFFSET: [0x004F71A0, 0x006714A0]
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

// VERIFY: c759121a manual
// OFFSET: [0x0055E610, 0x007B5020]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToHead(T* ptr) {
    this->LinkNode(ptr, 1, nullptr);
}

// VERIFY: e89481d9 auto
// OFFSET: [0x0056CA00, 0x006DED60, 0x0086E200, 0x008795F0]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToTail(T* ptr) {
    this->LinkNode(ptr, 2, nullptr);
}

// VERIFY: 0fb37856 manual
// OFFSET: [0x00496C00, 0x004B6E40, 0x004BFFC0, 0x004F3420, 0x004F89E0, 0x0055A1D0, 0x00560A00, 0x005BDF00, 0x006329D0, 0x0066CCB0, 0x006705D0, 0x00671030, 0x006883B0, 0x006BBDB0, 0x00776BC0, 0x007FBC60]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::NewNode(uint32_t location, size_t extrabytes, uint32_t flags) {
    T* node = new (SMemAlloc(sizeof(T) + extrabytes, typeid(T).raw_name(), SLOG_OBJECT, flags | SMEM_FLAG_ZEROMEMORY)) T;

    if (location) {
        this->LinkNode(node, location, nullptr);
    }

    return node;
}

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Next(T* ptr) {
    return this->Link(ptr)->Next();
}

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Prev(T* ptr) {
    return this->Link(ptr)->Prev();
}

// VERIFY: 230ebf8d auto
// OFFSET: [0x004B6670, 0x004D4770]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::RawNext(T* ptr) {
    return this->Link(ptr)->RawNext();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::SetLinkOffset(ptrdiff_t linkoffset) {
    this->m_linkoffset = linkoffset;
    this->InitializeTerminator();
}

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Tail() {
    return this->m_terminator.Prev();
}

// VERIFY: cdd999f0 manual
// OFFSET: [0x0047D170, 0x005BD800, 0x007CECD0, 0x00879980]
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkAll() {
    T* node;

    while ((node = this->Head()) != nullptr) {
        this->UnlinkNode(node);
    }
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::Combine(TSList<T, TGetLink>* list, uint32_t linktype, T* existingptr) {
    if (!list) {
        DebugValidate("list");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list == this) {
        DebugValidate("list != this");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list->m_linkoffset != this->m_linkoffset) {
        DebugValidate("list->m_linkoffset == m_linkoffset");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list->m_terminator.m_prevLink == &list->m_terminator) {
        return;
    }

    TSLink<T>* link = existingptr ? this->Link(existingptr) : &this->m_terminator;

    if (linktype == 1) {
        link = link->NextLink(this->m_linkoffset);
    }

    TSLink<T>* prev = link->m_prevLink;
    T* next = prev->m_next;
    prev->m_next = list->m_terminator.m_next;
    link->m_prevLink = list->m_terminator.m_prevLink;
    list->m_terminator.NextLink(this->m_linkoffset)->m_prevLink = prev;
    list->m_terminator.m_prevLink->m_next = next;
    list->m_terminator.m_prevLink = &list->m_terminator;
    list->m_terminator.m_next = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(&list->m_terminator) | 1);
}

// VERIFY: 58dea224 auto
// OFFSET: [0x004BFEE0, 0x005B1EF0, 0x00723300]
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkNode(T* ptr) {
    this->Link(ptr)->Unlink();
}

// INLINED
template <class T, int offset>
TSExplicitList<T, offset>::TSExplicitList() {
    this->SetLinkOffset(offset);
}

// INLINED
template <class T>
T* TSLinkedNode<T>::Next() {
    return this->m_link.Next();
}

// INLINED
template <class T>
T* TSLinkedNode<T>::RawNext() {
    return this->m_link.RawNext();
}

// VERIFY: d9cd8150 manual
// OFFSET: 0x0075B560
template <class T>
void TSSwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// INLINED
template <class T>
T& TSBaseArray<T>::operator[](uint32_t index) {
    return this->m_data[index];
}

// INLINED
template <class T>
uint32_t TSBaseArray<T>::Count() {
    return this->m_count;
}

// INLINED
template <class T>
const char* TSBaseArray<T>::MemFileName() {
    return typeid(T).raw_name();
}

// INLINED
template <class T>
int32_t TSBaseArray<T>::MemLineNo() {
    return -2;
}

// INLINED
template <class T>
T* TSBaseArray<T>::Ptr() {
    return this->m_data;
}

// VERIFY: af9b80d8 auto
// OFFSET: 0x0077AC60
template <class T>
T* TSBaseArray<T>::Top() {
    return this->m_count ? &this->m_data[this->m_count - 1] : nullptr;
}

// INLINED
template <class T>
TSFixedArray<T>::TSFixedArray() {
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = nullptr;
}

// INLINED
template <class T>
TSFixedArray<T>::TSFixedArray(const TSFixedArray<T>& source) {
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = nullptr;
    this->Set(source.m_count, source.m_data);
}

// VERIFY: 0f3f0aeb manual
// OFFSET: [0x0047C630, 0x0047C6C0, 0x00490BE0, 0x004967E0, 0x004B74F0, 0x004BAFD0, 0x004BC2F0, 0x004BC380, 0x004BE2D0, 0x004C0260, 0x004C9520, 0x004CA720, 0x004CC6F0, 0x004CC780, 0x004CDDD0, 0x004CF070, 0x004D0A70, 0x004D3130, 0x004D33C0, 0x004D4E10, 0x004F31F0, 0x00502360, 0x005032A0, 0x00503330, 0x005033C0, 0x00503450, 0x005053E0, 0x00527BF0, 0x00527C80, 0x00527E70, 0x0052F100, 0x0053F160, 0x00548B70, 0x00558C50, 0x00558CE0, 0x0055F560, 0x0055F5F0, 0x0055F680, 0x0055F710, 0x0055F7A0, 0x00566700, 0x00575F60, 0x005920F0, 0x005B4DE0, 0x005B4E70, 0x005BE830, 0x005BE8C0, 0x005C7290, 0x005C7320, 0x005C91F0, 0x005D24E0, 0x005E6880, 0x005F03B0, 0x005F3E20, 0x005F5AD0, 0x005F5B60, 0x005F5BF0, 0x005FCB80, 0x00617AA0, 0x00617B30, 0x00617BC0, 0x0066E830, 0x0066E8C0, 0x0066E950, 0x0066E9E0, 0x0066EA70, 0x0066EB00, 0x0066EB90, 0x0066EC20, 0x0066ECB0, 0x0066ED40, 0x0066EDD0, 0x0066EE60, 0x0066EEF0, 0x0066EF80, 0x0066F010, 0x0066F0A0, 0x0066F130, 0x0066F1C0, 0x0066F250, 0x0066F2E0, 0x0066F370, 0x0066F400, 0x0066F490, 0x0066F520, 0x0066F5B0, 0x0066F640, 0x0066F6D0, 0x0066F760, 0x0066F7F0, 0x0066F880, 0x006881A0, 0x006BBE30, 0x006BEE90, 0x006C2650, 0x006C2AC0, 0x006C2B50, 0x006C8380, 0x006E4EB0, 0x006E4F40, 0x006F2C50, 0x006F2CE0, 0x006F2D70, 0x006F2E00, 0x006F2E90, 0x006F2F20, 0x006F2FB0, 0x00709AA0, 0x00709B30, 0x0072FFC0, 0x007675F0, 0x007685D0, 0x00776AD0, 0x007B04D0, 0x007B5DF0, 0x007C0B40, 0x007C0BD0, 0x007DD360, 0x007DD3F0, 0x007E01C0, 0x007F2A20, 0x007F4ED0, 0x00804EF0, 0x00804F80, 0x008109F0, 0x00810A80, 0x00810B10, 0x00812BC0, 0x00812C50, 0x00814910, 0x0081A620, 0x00848800, 0x0084B570, 0x00876550, 0x00877760, 0x0087B0F0, 0x00965260, 0x009681C0, 0x009694A0, 0x009694F0, 0x0097BB80]
template <class T>
TSFixedArray<T>::~TSFixedArray() {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (this->m_data) {
        SMemFree(this->m_data, this->MemFileName(), this->MemLineNo(), 0);
    }
}

// VERIFY: 88921419 auto
// OFFSET: [0x00409770, 0x004D9FB0, 0x005918E0, 0x00591960, 0x00879950, 0x008C6E30]
template <class T>
void TSFixedArray<T>::Clear() {
    this->~TSFixedArray<T>();
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = nullptr;
}

// VERIFY: de17895f manual
// OFFSET: 0x004D2E90
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

// VERIFY: e2efd710 manual
// OFFSET: [0x00403760, 0x004037F0, 0x00408340, 0x004083F0, 0x00408490, 0x00408D80, 0x004644C0, 0x004657D0, 0x00466910, 0x0047CCC0, 0x0047D9F0, 0x0047DA80, 0x00480EE0, 0x004827C0, 0x00490A80, 0x00493A40, 0x00493AD0, 0x00496680, 0x00497970, 0x00497A00, 0x004B7580, 0x004B9990, 0x004B9A20, 0x004BB060, 0x004BBDA0, 0x004BBF00, 0x004BC060, 0x004BE170, 0x004BEBB0, 0x004C00B0, 0x004C46C0, 0x004C4750, 0x004CA380, 0x004CB930, 0x004CB9C0, 0x004CC360, 0x004CC4C0, 0x004CDB20, 0x004CEA00, 0x004CEAA0, 0x004CEF00, 0x004D0910, 0x004D2D90, 0x004D34A0, 0x004D4FF0, 0x004D85A0, 0x004DE520, 0x004E0EC0, 0x004E25C0, 0x004E6660, 0x004F2FB0, 0x004FCFC0, 0x004FD050, 0x004FD150, 0x004FFCE0, 0x005014A0, 0x005023E0, 0x00502540, 0x005026A0, 0x00502800, 0x005034E0, 0x00514100, 0x005141A0, 0x0051A2E0, 0x00525720, 0x00525880, 0x00527D10, 0x0052EF00, 0x0053B790, 0x0053B820, 0x0053EFA0, 0x005436A0, 0x00548640, 0x005489C0, 0x0054EC50, 0x0054ECF0, 0x00553220, 0x005532C0, 0x00553350, 0x005533E0, 0x00558710, 0x00558870, 0x0055EAD0, 0x0055EC30, 0x0055ED90, 0x0055EEF0, 0x0055F050, 0x00564CE0, 0x00566790, 0x005685D0, 0x0056BAD0, 0x0056BD40, 0x0056D780, 0x0056D810, 0x00575DC0, 0x00577410, 0x0057C120, 0x0057C1B0, 0x0057E360, 0x0057E400, 0x0058E030, 0x0058F8F0, 0x005907A0, 0x00590870, 0x00590920, 0x005909C0, 0x00590A60, 0x00591F70, 0x00592180, 0x00592290, 0x00593FC0, 0x00594050, 0x00599030, 0x0059B150, 0x0059B1F0, 0x005A2AF0, 0x005A46F0, 0x005B49B0, 0x005B4B10, 0x005B8C80, 0x005BDF60, 0x005BE0C0, 0x005C58D0, 0x005C5960, 0x005C6C60, 0x005C6DC0, 0x005C9080, 0x005CA3D0, 0x005CD240, 0x005CD2D0, 0x005CEB50, 0x005CFFB0, 0x005D0080, 0x005D0110, 0x005D2360, 0x005D3260, 0x005DA3F0, 0x005DA480, 0x005DA510, 0x005E6E40, 0x005E7940, 0x005ED620, 0x005EEB80, 0x005F0240, 0x005F0440, 0x005F1420, 0x005F39B0, 0x005F4E30, 0x005F5020, 0x005F5180, 0x005F52E0, 0x005FC9C0, 0x005FEF70, 0x0060A1C0, 0x0060A260, 0x006152E0, 0x006169A0, 0x00616C20, 0x00617280, 0x006173E0, 0x00617540, 0x0061BD20, 0x0061BDB0, 0x00634790, 0x00634820, 0x00669EB0, 0x0066CD10, 0x0066CE70, 0x0066CFD0, 0x0066D130, 0x0066D290, 0x0066D3F0, 0x0066D550, 0x0066D6B0, 0x0066D810, 0x0066D970, 0x0066DAD0, 0x0066DC30, 0x0066DD90, 0x0066DEF0, 0x0066E050, 0x00681800, 0x006822B0, 0x00683760, 0x00683810, 0x006838B0, 0x00683960, 0x00688040, 0x00689F90, 0x006A54A0, 0x006A9440, 0x006B1EF0, 0x006BA590, 0x006BBBE0, 0x006C26E0, 0x006C2840, 0x006C4210, 0x006C4E20, 0x006C5EE0, 0x006C5F80, 0x006C6020, 0x006C8220, 0x006C9DF0, 0x006CF4A0, 0x006CF540, 0x006CF5E0, 0x006E4BF0, 0x006E4D50, 0x006F2120, 0x006F2280, 0x006F23E0, 0x006F2540, 0x006F26A0, 0x006F2800, 0x006F2960, 0x006FECD0, 0x00709580, 0x007096E0, 0x00716260, 0x00716300, 0x007163B0, 0x0072FE60, 0x00746400, 0x00747C10, 0x00747CA0, 0x007583D0, 0x00767490, 0x00768460, 0x00769D80, 0x0076D060, 0x007760A0, 0x00776950, 0x00786B90, 0x00790250, 0x007902E0, 0x00790370, 0x007917D0, 0x00791870, 0x0079B370, 0x0079BA30, 0x0079D510, 0x0079F650, 0x007A01C0, 0x007B0370, 0x007B0D60, 0x007B0DF0, 0x007B0E80, 0x007B4F10, 0x007BA120, 0x007BDFE0, 0x007BE140, 0x007C30B0, 0x007CC020, 0x007CCBE0, 0x007CCC80, 0x007CDEC0, 0x007DBB10, 0x007DBBC0, 0x007DD0A0, 0x007DD200, 0x007E0050, 0x007E6600, 0x007E7FF0, 0x007EBEA0, 0x007ECFF0, 0x007F1F80, 0x007F3BF0, 0x007F3D70, 0x007F4CF0, 0x007F78F0, 0x007F79C0, 0x007F8E30, 0x007F9F80, 0x007FA020, 0x007FA0B0, 0x00804830, 0x00804990, 0x00810370, 0x00810500, 0x00810660, 0x008107C0, 0x008128D0, 0x00812A30, 0x00814800, 0x008196A0, 0x0081A2F0, 0x0081CDC0, 0x00824660, 0x00824700, 0x00848890, 0x0084B140, 0x0084B2A0, 0x008555E0, 0x0086D0E0, 0x0086E8E0, 0x008704B0, 0x00876370, 0x00878E90, 0x00878F30, 0x0087AEB0, 0x00887490, 0x008A17D0, 0x008A1860, 0x008A18F0, 0x00959260, 0x0095E2C0, 0x009633C0, 0x00967E50, 0x00967EF0, 0x009690F0, 0x00969540, 0x00978EC0, 0x0097BBE0, 0x0097F650, 0x00980EE0, 0x009A80F0, 0x009A8D50, 0x009A8DF0]
template <class T>
void TSFixedArray<T>::ReallocData(uint32_t count) {
    T* olddata = this->m_data;

    for (uint32_t i = count; i < this->m_count; i++) {
        olddata[i].~T();
    }

    this->m_alloc = count;
    this->m_data = static_cast<T*>(SMemReAlloc(olddata, sizeof(T) * count, this->MemFileName(), this->MemLineNo(), SMEM_FLAG_NOREALLOC));

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

// VERIFY: 3666e861 manual
// OFFSET: [0x00409470, 0x004C4B10, 0x004D3180, 0x0056B7C0, 0x0058EB60, 0x0058EBD0, 0x00591250, 0x005912C0, 0x005C5F00, 0x005E0800, 0x005EF6A0, 0x006EAAB0, 0x006FF5C0, 0x006FF630, 0x00700C80, 0x0077AC80, 0x007F8240, 0x0097FE80]
template <class T>
void TSFixedArray<T>::Set(uint32_t count, const T* data) {
    this->ReallocAndClearData(count);

    for (uint32_t i = 0; i < count; i++) {
        new (&this->m_data[i]) T(data[i]);
    }

    this->m_count = count;
}

// VERIFY: 30433c72 manual
// OFFSET: [0x00464B30, 0x00465AC0, 0x004670D0, 0x004957B0, 0x004D0B00, 0x004F5840, 0x00547E20, 0x00592F00, 0x00592FC0, 0x00599820, 0x005C9690, 0x006171B0, 0x0061F9C0, 0x006C0270, 0x006CA0B0, 0x0079E080, 0x007F1220, 0x007F1EA0, 0x007F1F20, 0x00966B10, 0x009AAF10]
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

// INLINED
template <class T>
TSFixedArray<T>& TSFixedArray<T>::operator=(const TSFixedArray<T>& source) {
    if (this != &source) {
        this->Set(source.m_count, source.m_data);
    }

    return *this;
}

// INLINED
template <class T>
TSGrowableArray<T>::TSGrowableArray() {
    this->m_chunk = 0;
}

// VERIFY: d0bba67f manual
// OFFSET: [0x004BBE20, 0x0057FCB0, 0x0057FD50, 0x0057FEA0, 0x0061F7F0, 0x006BADD0, 0x006C6550, 0x006C6760, 0x0071C780, 0x0071C830, 0x00723410, 0x00758630, 0x00791EB0, 0x00791F80, 0x007920A0, 0x007940F0, 0x007A3DB0, 0x007A8E90, 0x007CD140, 0x007CD3F0]
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

// VERIFY: e11e147e manual
// OFFSET: [0x006C6430, 0x006C64B0, 0x006C69A0]
template <class T>
uint32_t TSGrowableArray<T>::Add(uint32_t count, uint32_t incr, const T* data) {
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
        new (element) T(*data);
        data = reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(data) + incr);
    }

    this->m_count += count;

    return this->m_count - count;
}

// VERIFY: 52330a88 manual
// OFFSET: [0x004BBD60, 0x00548600, 0x0057C0E0, 0x00590830, 0x00590AE0, 0x005D0040, 0x006C41D0, 0x00790400, 0x007AE270, 0x007F7980]
template <class T>
uint32_t TSGrowableArray<T>::CalcChunkSize(uint32_t count) {
    uint32_t maxchunk = 256 / sizeof(T) > 8 ? 256 / sizeof(T) : 8;

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

// VERIFY: c55e1baf auto
// OFFSET: [0x00481080, 0x008A1B30, 0x008A1BB0]
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

// VERIFY: 910046a1 auto
// OFFSET: [0x00405970, 0x00409410, 0x0047DE50, 0x00498BC0, 0x004C69F0, 0x004CEBE0, 0x004D31E0, 0x004D35C0, 0x004E3760, 0x00500F60, 0x005221D0, 0x00565420, 0x00579AD0, 0x0057F670, 0x0058FEF0, 0x005916F0, 0x00591890, 0x005C6480, 0x00615660, 0x006850E0, 0x00685130, 0x006851D0, 0x0068A030, 0x0068A090, 0x006B2A40, 0x006DEC70, 0x007761E0, 0x007CD1D0, 0x007E6E50, 0x007F0CC0, 0x007F4450, 0x00821670, 0x0082DB70, 0x0086D180, 0x00870550, 0x00879900, 0x008BFE30, 0x008C6DE0, 0x008C6E60, 0x00968330, 0x00980FB0]
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

// VERIFY: 864fc686 auto
// OFFSET: [0x004097A0, 0x00480FD0, 0x00498B20, 0x004B9C00, 0x00503130, 0x0054EFE0, 0x005D1490, 0x005F2980, 0x00614390, 0x00616D10, 0x00749730, 0x0076A1C0, 0x007CEC70, 0x007EA1C0, 0x007FB270, 0x008A1C30, 0x009A9C40]
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

// INLINED
template <class T>
void TSGrowableArray<T>::Reserve(uint32_t count, int32_t round) {
    uint32_t newcount = this->m_count + count;

    if (newcount <= this->m_alloc) {
        return;
    }

    if (round) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }
    }

    this->ReallocData(newcount);
}

// VERIFY: aa735f18 auto
// OFFSET: [0x00409280, 0x0040AFE0, 0x00481030, 0x004845F0, 0x004920B0, 0x004949C0, 0x004969C0, 0x004B8610, 0x004BB540, 0x004BC4A0, 0x004BC5B0, 0x004BE360, 0x004C05B0, 0x004C4B80, 0x004C4C80, 0x004CABE0, 0x004CC810, 0x004CC920, 0x004CDE60, 0x004CEC60, 0x004CF320, 0x004D6640, 0x004D99F0, 0x004E1BD0, 0x004F3280, 0x00500EC0, 0x00500F10, 0x005031B0, 0x005054F0, 0x00505600, 0x00505710, 0x00505820, 0x00506210, 0x00522220, 0x00528700, 0x00528810, 0x00529BC0, 0x0052F1F0, 0x0053F440, 0x005462C0, 0x00548C00, 0x00556D70, 0x00556DC0, 0x005595E0, 0x005596F0, 0x0055FDB0, 0x0055FEC0, 0x0055FFD0, 0x005600E0, 0x005601F0, 0x00565490, 0x00567830, 0x0056BC00, 0x0056BE70, 0x005714D0, 0x00575FF0, 0x0057F5D0, 0x0057F6E0, 0x0057FDD0, 0x0057FF20, 0x0058EC40, 0x00591760, 0x00591910, 0x005923A0, 0x0059DF60, 0x005B5310, 0x005B5420, 0x005BF870, 0x005BF980, 0x005C8310, 0x005C8420, 0x005CC510, 0x005CF530, 0x005D2570, 0x005D5A60, 0x005DCA90, 0x005E72B0, 0x005EFFD0, 0x005F0570, 0x005F0680, 0x005F3ED0, 0x005F5C80, 0x005F5D90, 0x005F5EA0, 0x005FCC60, 0x00603930, 0x006156C0, 0x00616CA0, 0x00616D70, 0x00617CE0, 0x00617DF0, 0x00617F00, 0x006700E0, 0x00671CB0, 0x00671DC0, 0x00671ED0, 0x00671FE0, 0x006720F0, 0x00672200, 0x00672310, 0x00672420, 0x00672530, 0x00672640, 0x00672750, 0x00672860, 0x00672970, 0x00672A80, 0x00672B90, 0x00682790, 0x00682A40, 0x00685180, 0x00685E40, 0x00688450, 0x0068A0F0, 0x006BC250, 0x006C2E60, 0x006C2F70, 0x006C49B0, 0x006C59D0, 0x006C65E0, 0x006C6800, 0x006C6A20, 0x006C8410, 0x006DECC0, 0x006E6920, 0x006E6A30, 0x006F31A0, 0x006F32B0, 0x006F33C0, 0x006F34D0, 0x006F35E0, 0x006F36F0, 0x006F3800, 0x00700CF0, 0x00709BC0, 0x00709CD0, 0x00723170, 0x007233C0, 0x00734800, 0x00767AB0, 0x00768B00, 0x0076D260, 0x00776CF0, 0x00788930, 0x00791F30, 0x00792010, 0x00792130, 0x00793FD0, 0x00794190, 0x0079F980, 0x007A02F0, 0x007B06F0, 0x007B25B0, 0x007B2710, 0x007BA250, 0x007BE2A0, 0x007BE3B0, 0x007CD250, 0x007CD470, 0x007DD480, 0x007DD590, 0x007E02E0, 0x007E6EF0, 0x007F2AB0, 0x007F4A70, 0x007F4F60, 0x007FB220, 0x008076C0, 0x008077D0, 0x00810BA0, 0x00810CB0, 0x00810DC0, 0x00812D40, 0x00812E50, 0x008149E0, 0x0081AB60, 0x0082DAC0, 0x0082DBC0, 0x0084B600, 0x0084B710, 0x0086EED0, 0x008765E0, 0x008798B0, 0x0087BB70, 0x00968390, 0x00968400, 0x00969800, 0x00969B80, 0x0097D600, 0x0097DF90, 0x00980810]
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

// VERIFY: 8457e24c auto
// OFFSET: [0x0047C560, 0x005C6800]
template <class T, class TKey>
TSHashObject<T, TKey>::~TSHashObject() {
}

// VERIFY: fcbebc9d auto
// OFFSET: [0x0047C7B0, 0x0047CA30, 0x00492080, 0x00496980, 0x004B83F0, 0x004BB2D0, 0x004BC7D0, 0x004BD1B0, 0x004BE4D0, 0x004C0510, 0x004C9820, 0x004CADB0, 0x004CCB80, 0x004CCD70, 0x004CE310, 0x004CF500, 0x004D5DD0, 0x004F33F0, 0x00506060, 0x005063F0, 0x00506590, 0x00506790, 0x00529680, 0x00529720, 0x005299B0, 0x0052F3D0, 0x0053F740, 0x00549010, 0x0055A190, 0x0055A3D0, 0x00560650, 0x00560800, 0x00560C10, 0x00560E70, 0x00561DA0, 0x00567670, 0x00576300, 0x00592DD0, 0x005B64C0, 0x005B6660, 0x005C15F0, 0x005C1850, 0x005C8590, 0x005C8790, 0x005D2780, 0x005F0820, 0x005F4040, 0x005F6050, 0x005F6210, 0x005F7240, 0x005FCDD0, 0x00618400, 0x00618630, 0x00618850, 0x00670390, 0x00670540, 0x00670730, 0x006708E0, 0x00670A90, 0x00670C40, 0x00670DF0, 0x00670FA0, 0x006711A0, 0x00671350, 0x006715C0, 0x00671770, 0x00671920, 0x00671AD0, 0x00671C80, 0x00678F70, 0x00678FA0, 0x00678FD0, 0x00679000, 0x00679030, 0x00679060, 0x00679090, 0x006790C0, 0x006790F0, 0x00679120, 0x00679150, 0x00679180, 0x006791B0, 0x006791E0, 0x00679210, 0x00688380, 0x006BC1A0, 0x006BEF80, 0x006C2C80, 0x006C30E0, 0x006C8580, 0x006E6590, 0x006E6690, 0x006F3BE0, 0x006F3D70, 0x006F4090, 0x006F4280, 0x006F5970, 0x006F5EC0, 0x006F5F70, 0x00709EC0, 0x0070A120, 0x00734760, 0x00767A00, 0x00768C70, 0x00776CC0, 0x007B0660, 0x007B7110, 0x007C0CE0, 0x007C0DD0, 0x007DD810, 0x007DDA00, 0x007E04B0, 0x007F2C20, 0x007F53D0, 0x0080A400, 0x0080A5F0, 0x00811080, 0x00811270, 0x00811400, 0x00813230, 0x008134B0, 0x0081AFA0, 0x0084A180, 0x0084B8E0, 0x00876750, 0x00877850, 0x0087C010]
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalDelete(T* ptr) {
    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);
}

// VERIFY: 2a1eed0a auto
// OFFSET: [0x0047C800, 0x0047C9D0, 0x00492020, 0x00496EB0, 0x004B92B0, 0x004BB300, 0x004BC760, 0x004BD1F0, 0x004BE550, 0x004C0540, 0x004C96C0, 0x004CAD50, 0x004CCB20, 0x004CCD10, 0x004CE340, 0x004CF530, 0x004D65E0, 0x004F34B0, 0x00506090, 0x00506380, 0x005065D0, 0x005067D0, 0x00529610, 0x005296C0, 0x005299F0, 0x0052F360, 0x0053F6D0, 0x00548FB0, 0x0055A250, 0x0055A400, 0x00560690, 0x00560840, 0x00560A90, 0x00560C40, 0x00560E10, 0x005676A0, 0x005762A0, 0x00592D70, 0x005B6450, 0x005B65F0, 0x005C1620, 0x005C17F0, 0x005C85C0, 0x005C87C0, 0x005D2710, 0x005F07B0, 0x005F4070, 0x005F6090, 0x005F61B0, 0x005F7270, 0x005FCE10, 0x00618380, 0x006185B0, 0x006187E0, 0x00670330, 0x006704E0, 0x006706D0, 0x00670880, 0x00670A30, 0x00670B20, 0x00670BE0, 0x00670CD0, 0x00670D90, 0x00670E80, 0x00670F40, 0x006710C0, 0x00671140, 0x00671230, 0x006712F0, 0x00671560, 0x00671650, 0x00671710, 0x00671800, 0x006718C0, 0x006719B0, 0x00671A70, 0x00671B60, 0x00671C20, 0x00688430, 0x006BC1D0, 0x006BEFB0, 0x006C2CB0, 0x006C3110, 0x006C85B0, 0x006E6520, 0x006E6630, 0x006F3B70, 0x006F3D10, 0x006F4020, 0x006F42B0, 0x006F59A0, 0x006F5F00, 0x006F5FA0, 0x00709F00, 0x0070A0C0, 0x00734700, 0x00767A50, 0x00768CA0, 0x00776C40, 0x007B0800, 0x007B7180, 0x007C0D10, 0x007C0E00, 0x007DD7B0, 0x007DD9A0, 0x007E0450, 0x007F2C50, 0x007F5370, 0x0080A3A0, 0x0080A590, 0x00811020, 0x00811210, 0x00811430, 0x00813280, 0x00813450, 0x0081AF40, 0x0084A100, 0x0084B880, 0x00876780, 0x008778B0, 0x0087C040]
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    return listptr->NewNode(1, extrabytes, flags);
}

// VERIFY: 4a402d4d manual
// OFFSET: [0x0047C870, 0x0047CA60, 0x004921C0, 0x00496AD0, 0x004B84E0, 0x004BB410, 0x004BB510, 0x004BC6C0, 0x004BC930, 0x004BCA30, 0x004BCA60, 0x004BE5C0, 0x004BE6C0, 0x004C0890, 0x004C0990, 0x004C95B0, 0x004C97F0, 0x004CAE00, 0x004CAF00, 0x004CCBB0, 0x004CCDA0, 0x004CCEA0, 0x004CCED0, 0x004CE3C0, 0x004CE4C0, 0x004CF590, 0x004CF690, 0x004D6040, 0x004F34D0, 0x00506110, 0x00506430, 0x00506630, 0x00506830, 0x00506930, 0x00506960, 0x00506990, 0x005069C0, 0x00529750, 0x00529850, 0x00529A60, 0x0052F400, 0x0053F770, 0x00549040, 0x0055A270, 0x0055A480, 0x0055A580, 0x0055A5B0, 0x00560700, 0x005608A0, 0x00560AB0, 0x00560CB0, 0x00560EA0, 0x00567700, 0x00576330, 0x00592E00, 0x005B64F0, 0x005B6690, 0x005B6790, 0x005B67C0, 0x005C1690, 0x005C1880, 0x005C8630, 0x005C8820, 0x005D27C0, 0x005F0850, 0x005F40E0, 0x005F5FB0, 0x005F6240, 0x005F6400, 0x005FCE70, 0x00618450, 0x00618680, 0x006188A0, 0x00672CA0, 0x00672E50, 0x00672F50, 0x00673100, 0x00673200, 0x006733B0, 0x006734B0, 0x00673660, 0x00673760, 0x00673910, 0x00673A10, 0x00673BD0, 0x00673CD0, 0x00673E70, 0x00673F70, 0x00674110, 0x00674210, 0x006743C0, 0x006744C0, 0x00674670, 0x00674770, 0x00674910, 0x00674A10, 0x00674BB0, 0x00674CB0, 0x00674E50, 0x00674F50, 0x006750F0, 0x006751F0, 0x006753B0, 0x00688560, 0x006BC390, 0x006BF030, 0x006C2D30, 0x006C3180, 0x006C8620, 0x006E66C0, 0x006E67C0, 0x006F3AD0, 0x006F3C70, 0x006F3EC0, 0x006F40C0, 0x006F4310, 0x006F44D0, 0x006F4690, 0x00709F60, 0x0070A150, 0x00734910, 0x00767BC0, 0x00768D00, 0x00776E00, 0x007B0860, 0x007B71F0, 0x007C0E60, 0x007C0F60, 0x007DD840, 0x007DDA30, 0x007E04E0, 0x007F2CC0, 0x007F5400, 0x0080A430, 0x0080A620, 0x008110B0, 0x008112A0, 0x008114A0, 0x008132F0, 0x008134E0, 0x0081AFD0, 0x0084A1B0, 0x0084B910, 0x008767E0, 0x00877910, 0x0087C0A0]
template <class T, class TKey>
TSHashTable<T, TKey>::~TSHashTable() {
    this->InternalClear(1);
}

// VERIFY: 8328884c auto
// OFFSET: [0x0047C750, 0x0047C970, 0x00491FC0, 0x00496920, 0x004B8390, 0x004BB270, 0x004BC870, 0x004BC8D0, 0x004BE470, 0x004C04B0, 0x004CACF0, 0x004CCAC0, 0x004CCCB0, 0x004CE2B0, 0x004CF430, 0x004D5D70, 0x004F3390, 0x00506000, 0x00506320, 0x00506530, 0x00506730, 0x005295B0, 0x00529950, 0x0052F300, 0x0053F670, 0x00548F50, 0x0055A130, 0x0055A370, 0x005605F0, 0x005609A0, 0x00560BB0, 0x00560DB0, 0x00567610, 0x00576240, 0x00592D10, 0x005B63F0, 0x005C1790, 0x005C8530, 0x005C8730, 0x005D26B0, 0x005F0750, 0x005F3FE0, 0x005F6150, 0x005F63A0, 0x005FCD70, 0x00618320, 0x00618550, 0x00618780, 0x00670210, 0x006702D0, 0x006703C0, 0x00670480, 0x00670570, 0x00670670, 0x00670760, 0x00670820, 0x00670910, 0x006709D0, 0x00670AC0, 0x00670B80, 0x00670C70, 0x00670D30, 0x00670E20, 0x00670EE0, 0x00670FD0, 0x006710E0, 0x006711D0, 0x00671290, 0x00671380, 0x00671440, 0x006715F0, 0x006716B0, 0x006717A0, 0x00671860, 0x00671950, 0x00671A10, 0x00671B00, 0x00671BC0, 0x006BC140, 0x006BEF20, 0x006C2BE0, 0x006C3080, 0x006C8520, 0x006E64C0, 0x006E65D0, 0x006F3E00, 0x006F3E60, 0x006F3FC0, 0x006F41C0, 0x006F4410, 0x006F45D0, 0x006F4630, 0x00709E60, 0x0070A060, 0x007346A0, 0x007679A0, 0x00768C10, 0x0076D370, 0x00776B60, 0x007B0600, 0x007B70B0, 0x007C0C80, 0x007C0D70, 0x007DD750, 0x007DD940, 0x007E03F0, 0x007F2BC0, 0x007F5310, 0x0080A340, 0x0080A530, 0x00810FC0, 0x008111B0, 0x008113A0, 0x008131D0, 0x008133F0, 0x0081AE80, 0x0084A0A0, 0x0084B820, 0x008766F0, 0x008777F0, 0x0087BFB0]
template <class T, class TKey>
TSHashTable<T, TKey>::TSHashTable() {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.ChangeLinkOffset(offsetof(T, m_linktofull));
    this->m_slotmask = 0xFFFFFFFF;
}

// INLINED
template <class T, class TKey>
void TSHashTable<T, TKey>::Clear() {
    this->InternalClear(0);
}

// VERIFY: 05dc3778 auto
// OFFSET: [0x0047C910, 0x0047CB00, 0x00492260, 0x00496B70, 0x004B8580, 0x004BB4B0, 0x004BC810, 0x004BC9D0, 0x004BE660, 0x004C0930, 0x004C9790, 0x004CAEA0, 0x004CCC50, 0x004CCE40, 0x004CE460, 0x004CF630, 0x004D60E0, 0x004F3570, 0x005061B0, 0x005064D0, 0x005066D0, 0x005068D0, 0x005297F0, 0x005298F0, 0x00529B00, 0x0052F4A0, 0x0053F810, 0x005490E0, 0x0055A310, 0x0055A520, 0x005607A0, 0x00560940, 0x00560B50, 0x00560D50, 0x00560F40, 0x005677A0, 0x005763D0, 0x00592EA0, 0x005B6590, 0x005B6730, 0x005C1730, 0x005C1920, 0x005C86D0, 0x005C88C0, 0x005D2860, 0x005F08F0, 0x005F4180, 0x005F60F0, 0x005F62E0, 0x005F64A0, 0x005FCF10, 0x006184F0, 0x00618720, 0x00618940, 0x00672EF0, 0x006731A0, 0x00673450, 0x00673700, 0x006739B0, 0x00673AB0, 0x00673C70, 0x00673D70, 0x00673F10, 0x00674010, 0x006741B0, 0x006742B0, 0x00674460, 0x00674560, 0x00674710, 0x006749B0, 0x00674AB0, 0x00674C50, 0x00674D50, 0x00674EF0, 0x00674FF0, 0x00675190, 0x00675290, 0x00675450, 0x00688600, 0x006BC430, 0x006BF0D0, 0x006C2DD0, 0x006C3220, 0x006C86C0, 0x006E6760, 0x006E6860, 0x006F3C10, 0x006F3DA0, 0x006F3F60, 0x006F4160, 0x006F43B0, 0x006F4570, 0x006F4730, 0x0070A000, 0x0070A1F0, 0x007349B0, 0x00767C60, 0x00768DA0, 0x00776EA0, 0x007B0900, 0x007B7290, 0x007C0F00, 0x007C1000, 0x007DD8E0, 0x007DDAD0, 0x007E0580, 0x007F2D60, 0x007F54A0, 0x0080A4D0, 0x0080A6C0, 0x00811150, 0x00811340, 0x00811540, 0x00813390, 0x00813580, 0x0081B070, 0x0084A250, 0x0084B9B0, 0x00876880, 0x008779B0, 0x0087C140]
template <class T, class TKey>
void TSHashTable<T, TKey>::Destroy() {
    this->InternalClear(1);
    this->m_slotmask = 0xFFFFFFFF;
    this->m_slotlistarray.Clear();
}

// INLINED
template <class T, class TKey>
void TSHashTable<T, TKey>::Delete(T* ptr) {
    this->Unlink(ptr);
    this->InternalDelete(ptr);
}

// VERIFY: 5884344f manual
// OFFSET: 0x007671D0
template <class T, class TKey>
T* TSHashTable<T, TKey>::DeleteNode(T* ptr) {
    T* next = this->m_fulllist.Next(ptr);
    this->Unlink(ptr);
    this->InternalDelete(ptr);

    return next;
}

// VERIFY: fecb3f7f manual
// OFFSET: 0x00767000
template <class T, class TKey>
T* TSHashTable<T, TKey>::Next(T* ptr) {
    return this->m_fulllist.Next(ptr);
}

// VERIFY: 35086749 auto
// OFFSET: [0x00492C70, 0x00496CE0, 0x004B9000, 0x004BB800, 0x004BCB50, 0x004BCD20, 0x004BE750, 0x004C0A20, 0x004CAF90, 0x004CCFC0, 0x004CD190, 0x004CE550, 0x004CF720, 0x004D6800, 0x004F3660, 0x00506AB0, 0x00506D40, 0x00506F10, 0x005070E0, 0x00529D90, 0x00529F60, 0x0052A240, 0x0052F590, 0x0053F900, 0x005491D0, 0x0055A6A0, 0x0055A870, 0x00561270, 0x00561440, 0x00561610, 0x005617E0, 0x005619B0, 0x005679A0, 0x005764C0, 0x00593110, 0x005B68B0, 0x005B6A80, 0x005C1AA0, 0x005C1C70, 0x005C8A40, 0x005C8C10, 0x005D2950, 0x005F09E0, 0x005F4270, 0x005F66B0, 0x005F6880, 0x005F6A50, 0x005FD000, 0x00618AF0, 0x00618CC0, 0x00618EF0, 0x00675AB0, 0x006773F0, 0x006775C0, 0x00677790, 0x00677960, 0x00677B30, 0x00677D00, 0x00677ED0, 0x006780A0, 0x00678270, 0x00678440, 0x00678610, 0x006787E0, 0x006789B0, 0x00678B80, 0x00678D50, 0x006894C0, 0x006BC520, 0x006C3370, 0x006C3540, 0x006C5B40, 0x006C87B0, 0x006E74A0, 0x006E7670, 0x006F4B80, 0x006F4D50, 0x006F4F20, 0x006F50F0, 0x006F52C0, 0x006F5490, 0x006F5660, 0x0070A310, 0x0070A540, 0x007378D0, 0x00767D50, 0x00768E90, 0x0076D430, 0x00777120, 0x007B09F0, 0x007BE580, 0x007BE760, 0x007DDC50, 0x007DDE20, 0x007E0670, 0x007F2E50, 0x007F5590, 0x0080A840, 0x0080AA10, 0x00811720, 0x008118F0, 0x00811AC0, 0x00813700, 0x008138D0, 0x0081B160, 0x0084BB00, 0x0084BCD0, 0x0086F280, 0x00876970, 0x0087C230]
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

// VERIFY: f91710d3 manual
// OFFSET: [0x00492C10, 0x00496C80, 0x004B8FA0, 0x004BB7A0, 0x004BCA90, 0x004BCAF0, 0x004BE6F0, 0x004C09C0, 0x004CAF30, 0x004CCF00, 0x004CCF60, 0x004CE4F0, 0x004CF6C0, 0x004D67A0, 0x004F3600, 0x005069F0, 0x00506A50, 0x00506C80, 0x00506CE0, 0x00529CD0, 0x00529D30, 0x0052A1E0, 0x0052F530, 0x0053F8A0, 0x00549170, 0x0055A5E0, 0x0055A640, 0x00561090, 0x005610F0, 0x00561150, 0x005611B0, 0x00561210, 0x00567940, 0x00576460, 0x005930B0, 0x005B67F0, 0x005B6850, 0x005C19E0, 0x005C1A40, 0x005C8980, 0x005C89E0, 0x005D28F0, 0x005F0980, 0x005F4210, 0x005F6590, 0x005F65F0, 0x005F6650, 0x005FCFA0, 0x00618A30, 0x00618A90, 0x00618E90, 0x00675A50, 0x00676E50, 0x00676EB0, 0x00676F10, 0x00676F70, 0x00676FD0, 0x00677030, 0x00677090, 0x006770F0, 0x00677150, 0x006771B0, 0x00677210, 0x00677270, 0x006772D0, 0x00677330, 0x00677390, 0x00689460, 0x006BC4C0, 0x006C32B0, 0x006C3310, 0x006C5AE0, 0x006C8750, 0x006E73E0, 0x006E7440, 0x006F48E0, 0x006F4940, 0x006F49A0, 0x006F4A00, 0x006F4A60, 0x006F4AC0, 0x006F4B20, 0x0070A2B0, 0x0070A4E0, 0x00737870, 0x00767CF0, 0x00768E30, 0x0076D3D0, 0x007770C0, 0x007B0990, 0x007BE4C0, 0x007BE520, 0x007DDB90, 0x007DDBF0, 0x007E0610, 0x007F2DF0, 0x007F5530, 0x0080A780, 0x0080A7E0, 0x00811600, 0x00811660, 0x008116C0, 0x00813640, 0x008136A0, 0x0081B100, 0x0084BA40, 0x0084BAA0, 0x0086F220, 0x00876910, 0x0087C1D0]
template <class T, class TKey>
void TSHashTable<T, TKey>::Initialize() {
    this->m_slotmask = 3;
    this->m_slotlistarray.SetCount(4);

    for (uint32_t i = 0; i <= this->m_slotmask; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }
}

// VERIFY: cb2fc38e manual
// OFFSET: [0x004B93B0, 0x004D6A70, 0x004F38D0, 0x0073F200, 0x007B0C60, 0x007BEA70, 0x007BEAE0]
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const TKey& key) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = key;
}

// VERIFY: 97fea5a3 manual
// OFFSET: 0x006C8A20
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const char* str) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = str;
}

// VERIFY: 831e7198 auto
// OFFSET: 0x006C8AB0
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, const char* str) {
    this->Insert(ptr, SStrHashHT(str), str);
}

// VERIFY: 48e089d6 manual
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

// INLINED
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

// VERIFY: 532abd43 auto
// OFFSET: [0x00492F10, 0x004BD260, 0x004BD2C0, 0x004CB520, 0x005082C0, 0x0052A5F0, 0x005626A0, 0x00562700, 0x00562760, 0x005627C0, 0x005B6D90, 0x005C9020, 0x005C9190, 0x005F73B0, 0x005FD370, 0x006192A0, 0x00679D30, 0x0067AF50, 0x0067AFB0, 0x0067B010, 0x0067B070, 0x0067B0D0, 0x0067B130, 0x0067B190, 0x0067B1F0, 0x0067B250, 0x0067B2B0, 0x0067B310, 0x0067B370, 0x0067B3D0, 0x0067B430, 0x0067B490, 0x00689760, 0x006C3C50, 0x006C5DB0, 0x006E7A40, 0x006E7AA0, 0x006F5D20, 0x006F6080, 0x0076D7B0, 0x0086F4F0]
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

// VERIFY: 96f0e432 manual
// OFFSET: [0x00492E70, 0x00496ED0, 0x004B9310, 0x004BB9D0, 0x004BD010, 0x004BD0B0, 0x004BE920, 0x004C0DC0, 0x004CB310, 0x004CD360, 0x004CD400, 0x004CE720, 0x004CF8F0, 0x004D69D0, 0x004F3830, 0x00507E10, 0x00507EB0, 0x00507F50, 0x00507FF0, 0x0052A410, 0x0052A4B0, 0x0052A550, 0x0052F910, 0x005405B0, 0x005493A0, 0x0055AC40, 0x0055ACE0, 0x00561DD0, 0x00561E70, 0x00561F10, 0x00561FB0, 0x00562050, 0x00567B70, 0x00576690, 0x00593830, 0x005B6C50, 0x005B6CF0, 0x005C1EF0, 0x005C1F90, 0x005C8E50, 0x005C8EF0, 0x005D2B20, 0x005F0D50, 0x005F4440, 0x005F7060, 0x005F7100, 0x005F71A0, 0x005FD220, 0x006190C0, 0x00619160, 0x00619200, 0x00679240, 0x006793D0, 0x00679470, 0x00679510, 0x006795B0, 0x00679650, 0x006796F0, 0x00679790, 0x00679830, 0x006798D0, 0x00679970, 0x00679A10, 0x00679AB0, 0x00679B50, 0x00679BF0, 0x00679C90, 0x006896C0, 0x006BC8C0, 0x006C3820, 0x006C38C0, 0x006C5D10, 0x006C8980, 0x006E7900, 0x006E79A0, 0x006F5A00, 0x006F5AA0, 0x006F5B40, 0x006F5BE0, 0x006F5C80, 0x006F5D80, 0x006F5E20, 0x0070A710, 0x0070A7B0, 0x0073A7F0, 0x00767F20, 0x00769060, 0x0076D6A0, 0x00777380, 0x007B0BC0, 0x007BE930, 0x007BE9D0, 0x007DF2D0, 0x007DF370, 0x007E0840, 0x007F3020, 0x007F5B00, 0x0080C650, 0x0080C6F0, 0x00811D40, 0x00811DE0, 0x00811E80, 0x00813AA0, 0x00813B40, 0x0081B550, 0x0084BEA0, 0x0084BF40, 0x0086F450, 0x00876B40, 0x0087C670]
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

// VERIFY: f5512cc0 auto
// OFFSET: [0x0067B660]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const TKey& key, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = key;

    return node;
}

// VERIFY: 5082a074 auto
// OFFSET: [0x00509720, 0x00562870, 0x005628D0]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = str;

    return node;
}

// VERIFY: 69b20f8a auto
// OFFSET: [0x005097A0, 0x00562960]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(const char* str, size_t extrabytes, uint32_t flags) {
    return this->New(SStrHashHT(str), str, extrabytes, flags);
}

// VERIFY: cb777ac9 manual
// OFFSET: [0x004B6D90, 0x004BB1E0, 0x004D4BB0, 0x00505480, 0x00679350, 0x006BBD40, 0x006C2A50, 0x006F6020, 0x007B0690, 0x007BDD80, 0x007BDDF0, 0x00810970]
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

// VERIFY: fca99b11 manual
// OFFSET: [0x004BC410, 0x004BE0E0, 0x0053F3B0, 0x0055F4D0, 0x00617C50, 0x00687870, 0x006C8100, 0x007E0250]
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

// VERIFY: b253b260 manual
// OFFSET: [0x004D4790, 0x00565030, 0x00685230, 0x0079E6A0, 0x007BFE80]
template <class T, class TKey>
void TSHashTable<T, TKey>::Unlink(T* ptr) {
    if (ptr->m_linktoslot.IsLinked()) {
        ptr->m_linktoslot.Unlink();
        ptr->m_linktofull.Unlink();
    }
}

// VERIFY: b87209d2 manual
// OFFSET: 0x00776F30
template <class T, class TKey>
TSHashTableReuse<T, TKey>::TSHashTableReuse() {
    this->m_reuseList.ChangeLinkOffset(offsetof(T, m_linktoslot));
}

// VERIFY: dcbf18cd manual
// OFFSET: [0x004B8F10, 0x004B8F70, 0x004BB710, 0x004BB770, 0x004C0D30, 0x004C0D90, 0x006BC830, 0x007772F0]
template <class T, class TKey>
TSHashTableReuse<T, TKey>::~TSHashTableReuse() {
    this->Destructor();
}

// VERIFY: 1442481d auto
// OFFSET: [0x004B8EF0, 0x004BB650, 0x004C0BF0, 0x006BBB40, 0x007768E0]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destroy() {
    this->Clear();
    this->Destructor();
}

// VERIFY: 6604cd54 auto
// OFFSET: [0x004B6690, 0x004BAEF0, 0x004C0040, 0x006BB880, 0x00776870]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destructor() {
    T* node;

    while ((node = this->m_reuseList.Head()) != nullptr) {
        this->m_reuseList.UnlinkNode(node);
        SMemFree(node, typeid(T).raw_name(), SLOG_OBJECT, 0);
    }
}

// VERIFY: a0b4896c manual
// OFFSET: [0x004B8E70, 0x004C0C10, 0x006BC6F0, 0x00777000]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::InternalDelete(T* ptr) {
    this->m_fulllist.UnlinkNode(ptr);
    ptr->~T();
    new (&ptr->m_linktoslot) TSLink<T>;
    this->m_reuseList.LinkToHead(ptr);
}

// VERIFY: 187b5e87 manual
// OFFSET: [0x004B9240, 0x004BB670, 0x004C0C90, 0x006BC770, 0x00776F90]
template <class T, class TKey>
T* TSHashTableReuse<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    T* node = this->m_reuseList.Head();

    if (!node) {
        return listptr->NewNode(1, 0, 0);
    }

    this->m_reuseList.UnlinkNode(node);
    new (node) T;
    listptr->LinkToHead(node);

    return node;
}

// INLINED
template <class T, class THandle>
TSExportTableSimpleReuse<T, THandle>::TSExportTableSimpleReuse() {
    this->m_sequence = static_cast<uint32_t>(~reinterpret_cast<uintptr_t>(this) & 0xFFFFFFF);
    this->m_wrapped = 0;
}

// INLINED
template <class T, class THandle>
void TSExportTableSimpleReuse<T, THandle>::Delete(T* ptr) {
    TSHashTable<T, HASHKEY_NONE>::Delete(ptr);
}

// VERIFY: 50a85424 manual
// OFFSET: 0x00777080
template <class T, class THandle>
THandle TSExportTableSimpleReuse<T, THandle>::GenerateUniqueHandle() {
    do {
        while (++this->m_sequence == 0) {
            this->m_wrapped = 1;
        }
    } while (this->m_wrapped && this->Ptr(reinterpret_cast<THandle>(static_cast<uintptr_t>(this->m_sequence))));

    return reinterpret_cast<THandle>(static_cast<uintptr_t>(this->m_sequence));
}

// INLINED
template <class T, class THandle>
T* TSExportTableSimpleReuse<T, THandle>::New(THandle* handle) {
    THandle uniquehandle = this->GenerateUniqueHandle();
    *handle = uniquehandle;

    return TSHashTable<T, HASHKEY_NONE>::New(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(uniquehandle)), this->m_key, 0, 0);
}

// INLINED
template <class T, class THandle>
T* TSExportTableSimpleReuse<T, THandle>::Ptr(THandle handle) {
    return TSHashTable<T, HASHKEY_NONE>::Ptr(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handle)), this->m_key);
}

// VERIFY: 252836f7 auto
// OFFSET: 0x00777900
template <class T, class THandle, class TLockedHandle, class TSync>
TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::~TSExportTableSyncReuse() {
}

// VERIFY: 21249c03 manual
// OFFSET: 0x007778A0
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Delete(THandle handle) {
    TLockedHandle lockedhandle;
    T* ptr = this->Lock(handle, &lockedhandle, 1);
    this->DeleteUnlock(ptr, lockedhandle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::DeleteUnlock(T* ptr, TLockedHandle lockedhandle) {
    TSExportTableSimpleReuse<T, THandle>::Delete(ptr);
    this->Unlock(lockedhandle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
int32_t TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::IsForWriting(TLockedHandle lockedhandle) {
    return lockedhandle == reinterpret_cast<TLockedHandle>(1);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
T* TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Lock(THandle handle, TLockedHandle* lockedhandle, int32_t forwriting) {
    this->SyncEnterLock(lockedhandle, forwriting);
    T* ptr = this->Ptr(handle);

    if (!ptr) {
        this->SyncLeaveLock(*lockedhandle);
        *lockedhandle = nullptr;
    }

    return ptr;
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
T* TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::NewLock(THandle* handle, TLockedHandle* lockedhandle) {
    this->SyncEnterLock(lockedhandle, 1);
    return this->New(handle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::SyncEnterLock(TLockedHandle* lockedhandle, int32_t forwriting) {
    this->m_sync.Enter(forwriting);
    *lockedhandle = reinterpret_cast<TLockedHandle>(static_cast<intptr_t>(2 * (forwriting != 0) - 1));
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::SyncLeaveLock(TLockedHandle lockedhandle) {
    if (lockedhandle) {
        this->m_sync.Leave(this->IsForWriting(lockedhandle));
    }
}

// VERIFY: d83857d4 manual
// OFFSET: 0x00776240
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Unlock(TLockedHandle lockedhandle) {
    this->SyncLeaveLock(lockedhandle);
}

// INLINED
inline bool HASHKEY_CONSTSTR::operator==(const char* str) {
    return this->m_str == str || SStrCmp(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_CONSTSTRI& HASHKEY_CONSTSTRI::operator=(const char* str) {
    this->m_str = str;
    return *this;
}

// INLINED
inline bool HASHKEY_CONSTSTRI::operator==(const char* str) {
    return this->m_str == str || SStrCmpI(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline bool HASHKEY_NONE::operator==(const HASHKEY_NONE& key) {
    return true;
}

// INLINED
inline bool HASHKEY_DWORD::operator==(const HASHKEY_DWORD& key) {
    return this->m_key == key.m_key;
}

// INLINED
inline bool HASHKEY_PTR::operator==(const HASHKEY_PTR& key) {
    return this->m_key == key.m_key;
}

// INLINED
inline HASHKEY_STR::HASHKEY_STR() {
    this->m_str = nullptr;
}

// INLINED
inline HASHKEY_STR::~HASHKEY_STR() {
    if (this->m_str) {
        SMemFree(this->m_str, __FILE__, __LINE__, 0);
    }
}

// INLINED
inline HASHKEY_STR& HASHKEY_STR::operator=(const char* str) {
    if (this->m_str != str) {
        if (this->m_str) {
            SMemFree(this->m_str, __FILE__, __LINE__, 0);
        }

        this->m_str = SStrDupA(str, __FILE__, __LINE__);
    }

    return *this;
}

// INLINED
inline bool HASHKEY_STR::operator==(const char* str) {
    return SStrCmp(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_STRI& HASHKEY_STRI::operator=(const char* str) {
    HASHKEY_STR::operator=(str);
    return *this;
}

// INLINED
inline bool HASHKEY_STRI::operator==(const char* str) {
    return SStrCmpI(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_UTF8I& HASHKEY_UTF8I::operator=(const char* str) {
    if (this->m_str != str) {
        if (this->m_str) {
            SMemFree(this->m_str, __FILE__, __LINE__, 0);
        }

        this->m_str = SStrDupA(str, __FILE__, __LINE__);
    }

    return *this;
}

// INLINED
inline bool HASHKEY_UTF8I::operator==(const char* str) {
    return SStrCmpUTF8I(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
template <class T>
void TSBitField<T>::Load(const void* data, uint32_t bytes, int32_t bits) {
    uint32_t count = bits == -1 ? bytes * 8 : bits;
    this->m_count = count;
    this->m_bits.SetCount(count ? ((count - 1) >> 5) + 1 : 0);
    memcpy(this->m_bits.m_data, data, bytes);
}

// VERIFY: d4042b48 manual
// OFFSET: [0x00408270, 0x00801020]
template <class T>
TSStackArray<T>::TSStackArray(T* data, uint32_t count, int32_t construct) {
    this->m_data = data;

    if (construct) {
        this->m_count = count;

        for (uint32_t i = 0; i < count; i++) {
            new (&this->m_data[i]) T();
        }
    } else {
        this->m_count = 0;
    }
}

// INLINED
template <class T, uint32_t N>
TSCGrowableArray<T, N>::TSCGrowableArray() {
    this->m_count = 0;
}

// INLINED
template <class T, uint32_t N>
TSCGrowableArray<T, N>::~TSCGrowableArray() {
    if (this->m_count == 0xFFFFFFFF) {
        this->m_dynamic.~TSGrowableArray<T>();
    } else {
        this->m_count = 0;
    }
}

// VERIFY: 47bab0b9 auto
// OFFSET: [0x007917B0]
template <class T, uint32_t N>
T& TSCGrowableArray<T, N>::operator[](uint32_t index) {
    if (this->m_count == 0xFFFFFFFF) {
        return this->m_dynamic.m_data[index];
    }

    return this->m_inline[index];
}

// VERIFY: ab2e9eb9 manual
// OFFSET: [0x00728B10, 0x00728B90, 0x00728C10, 0x007B3B70]
template <class T, uint32_t N>
void TSCGrowableArray<T, N>::BecomeDynamic() {
    const uint32_t overlap = (sizeof(TSGrowableArray<T>) + sizeof(T) - 1) / sizeof(T);
    uint32_t count = this->m_count;

    if (count > overlap) {
        count = overlap;
    }

    T saved[overlap];

    if (count) {
        memcpy(saved, this->m_inline, sizeof(T) * count);
    }

    new (&this->m_dynamic) TSGrowableArray<T>();
    this->m_dynamic.Add(count, saved);
    this->m_dynamic.Add(this->m_count - count, &this->m_inline[count]);
    this->m_count = 0xFFFFFFFF;
}

// INLINED
template <class T, uint32_t N>
void TSCGrowableArray<T, N>::SetCount(uint32_t count) {
    if (this->m_count == 0xFFFFFFFF) {
        this->m_dynamic.SetCount(count);
    } else if (count <= N) {
        this->m_count = count;
    } else {
        this->BecomeDynamic();
        this->m_dynamic.SetCount(count);
    }
}

// VERIFY: dbe4dc73 manual
// OFFSET: [0x00801120]
template <class T>
void TSStackArray<T>::SetCount(uint32_t count) {
    if (count == this->m_count) {
        return;
    }

    for (uint32_t i = this->m_count; i < count; i++) {
        new (&this->m_data[i]) T();
    }

    this->m_count = count;
}

#endif
