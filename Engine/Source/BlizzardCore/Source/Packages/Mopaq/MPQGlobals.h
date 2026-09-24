#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQGLOBALS_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQGLOBALS_H

#include <cstdint>
#include <functional>
#include <set>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"

namespace System_MPQGlobals {

struct ArchiveLess {
    bool operator()(const System_SFile::ARCHIVEREC* a, const System_SFile::ARCHIVEREC* b) const;
};

typedef std::set<System_SFile::ARCHIVEREC*, ArchiveLess, Blizzard::Memory::AllocSTL_raw<System_SFile::ARCHIVEREC*>> ArchiveList;

struct MopaqGlobals {
    /* 0000 */ uint32_t m_directAccess;
    /* 0004 */ char m_basePath[260];
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint32_t unk_0114;
};

struct WorkerPool {
    /* 0000 */ Blizzard::Lock::RMutex m_mutex;
    /* 0018 */ std::vector<void*> m_workers;
};

class Globals {
    public:
    /* 0000 */ ArchiveList m_archives;
    /* 000C */ Blizzard::Lock::RMutex m_archiveLock;
    /* 0024 */ MopaqGlobals m_mopaq;
    /* 013C */ Blizzard::Lock::RMutex m_mopaqLock;
    /* 0154 */ WorkerPool m_workerPool;

    Globals();
};

extern Blizzard::Lock::ConstructOnce<Globals> s_globals;

ArchiveList* GetArchiveList();
Blizzard::Lock::RMutex* GetArchiveLock();
MopaqGlobals* GetMopaqGlobals();
Blizzard::Lock::RMutex* GetMopaqGlobalsLock();
WorkerPool* GetWorkerPool();

}

#endif
