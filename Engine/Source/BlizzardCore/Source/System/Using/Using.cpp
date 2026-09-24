#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

#define USING_MAX 32
#define DEPENDS_MAX 128
#define STRINGPOOL_SIZE 1024
#define HASH_SIZE 64

namespace System_Using {

class UsingHashTable {
    public:
    struct Entry {
        /* 0000 */ uint32_t m_hash;
        /* 0004 */ uint16_t m_value;
        /* 0006 */ uint8_t unk_0006[2];
    };

    static uint32_t Hash(const char* name);

    /* 0000 */ Entry m_entries[HASH_SIZE];
    /* 0200 */ uint32_t m_used;

    bool Find(const char* name, uint32_t& slot);
    void Insert(const char* name, uint16_t value);
};

// STATIC: 0x00B325E0
char s_UsingNames[STRINGPOOL_SIZE];

// STATIC: 0x00B329E0
Depends s_Depends[DEPENDS_MAX];

// STATIC: 0x00B32DE0
UsingRec use[USING_MAX];

// STATIC: 0x00B33060
UsingHashTable s_Using;

// STATIC: 0x00B382B8
char* s_UsingNamesTail;

// STATIC: 0x00B382BC
int16_t s_NextUsingID;

// STATIC: 0x00B382C0
int32_t s_NextDependsID;

}

// VERIFY: 146a9c90 manual
// OFFSET: 0x00429220
uint32_t System_Using::UsingHashTable::Hash(const char* name) {
    uint32_t hash = 0x1000193;

    for (; *name; name++) {
        hash = (hash ^ static_cast<uint8_t>(*name)) * 0x1000193;
    }

    return hash ? hash : 1;
}

// VERIFY: 24d15bd3 manual
// OFFSET: 0x00429260
bool System_Using::UsingHashTable::Find(const char* name, uint32_t& slot) {
    uint32_t hash = Hash(name);
    slot = hash & (HASH_SIZE - 1);

    while (this->m_entries[slot].m_hash) {
        if (this->m_entries[slot].m_hash == hash) {
            return true;
        }

        slot = (slot + 1) & (HASH_SIZE - 1);
    }

    return false;
}

// INLINED
void System_Using::UsingHashTable::Insert(const char* name, uint16_t value) {
    uint32_t slot;

    if (!this->Find(name, slot)) {
        this->m_used++;
        this->m_entries[slot].m_hash = Hash(name);
        this->m_entries[slot].m_value = value;
    }
}

// VERIFY: 1417d808 manual
// OFFSET: 0x004292D0
void System_Using::ShutdownUnused() {
    bool done;

    do {
        done = true;

        for (uint32_t i = 0; i < HASH_SIZE; i++) {
            if (!s_Using.m_entries[i].m_hash) {
                continue;
            }

            UsingRec* rec = &use[s_Using.m_entries[i].m_value];

            if (rec->shutdownproc && rec->refCount <= 0) {
                rec->shutdownproc();
                rec->shutdownproc = nullptr;

                for (Depends* depends = rec->depends; depends; depends = depends->next) {
                    use[depends->id].refCount--;
                }

                rec->dependCount = 0;
                rec->depends = nullptr;
                done = false;
            }
        }
    } while (!done);
}

void System_Using::RegisterNull() {
}

// VERIFY: acf05c84 manual
// OFFSET: 0x0042E910
System_Using::UsingRec* System_Using::Intern(const char* name) {
    uint32_t slot;

    if (s_Using.Find(name, slot)) {
        return &use[s_Using.m_entries[slot].m_value];
    }

    int16_t id = s_NextUsingID++;

    if (!s_UsingNamesTail) {
        s_UsingNamesTail = s_UsingNames;
    }

    UsingRec* rec = &use[id];
    rec->refCount = 0;
    rec->dependCount = 0;
    rec->depends = nullptr;
    rec->name = s_UsingNamesTail;
    rec->id = id;
    rec->shutdownproc = RegisterNull;
    s_UsingNamesTail += Blizzard::String::Copy(s_UsingNamesTail, name, static_cast<uint32_t>(s_UsingNames + STRINGPOOL_SIZE - s_UsingNamesTail)) + 1;
    s_Using.Insert(name, id);

    return rec;
}

// VERIFY: caec2b8b auto
// OFFSET: 0x00433830
void Blizzard::Using::StartUsing(const char* name) {
    System_Using::Intern(name)->refCount++;
}

// VERIFY: 8fd6bc7b manual
// OFFSET: 0x00433850
void Blizzard::Using::StopUsing(const char* name) {
    System_Using::Intern(name)->refCount--;
    System_Using::ShutdownUnused();
}

// VERIFY: 7007a482 auto
// OFFSET: 0x00433870
void Blizzard::Using::DependsOn(const char* me, const char* dependsOn) {
    System_Using::UsingRec* rec = System_Using::Intern(me);
    System_Using::UsingRec* dep = System_Using::Intern(dependsOn);
    System_Using::Depends** link = &rec->depends;

    for (System_Using::Depends* depends = rec->depends; depends; depends = depends->next) {
        if (depends->id == dep->id) {
            return;
        }

        link = &depends->next;
    }

    rec->dependCount++;
    dep->refCount++;
    *link = &System_Using::s_Depends[System_Using::s_NextDependsID++];
    (*link)->id = dep->id;
    (*link)->next = nullptr;
}
