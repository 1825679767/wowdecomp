#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"

// STATIC: 0x00B33E58
Blizzard::Lock::ConstructOnce<System_MPQGlobals::Globals> System_MPQGlobals::s_globals;

// VERIFY: be9441c5 manual
// INLINED
bool System_MPQGlobals::ArchiveLess::operator()(const System_SFile::ARCHIVEREC* a, const System_SFile::ARCHIVEREC* b) const {
    if (a->m_priority == b->m_priority) {
        return a->m_id > b->m_id;
    }

    return a->m_priority > b->m_priority;
}

// VERIFY: df235665 manual
// OFFSET: 0x0044FFF0
System_MPQGlobals::Globals::Globals() {
    this->m_mopaq.m_directAccess = 0;

    if (!Blizzard::Process::GetProcessDirectory(this->m_mopaq.m_basePath, sizeof(this->m_mopaq.m_basePath))) {
        this->m_mopaq.m_basePath[0] = '\0';
    }

    this->m_mopaq.unk_0108 = 0;
    this->m_mopaq.unk_010C = 0;
    this->m_mopaq.unk_0110 = 0;
}

// VERIFY: e5b01062 manual
// OFFSET: 0x00451D00
System_MPQGlobals::ArchiveList* System_MPQGlobals::GetArchiveList() {
    return &s_globals.Get()->m_archives;
}

// VERIFY: 3d7e5cb3 manual
// OFFSET: 0x00451D60
Blizzard::Lock::RMutex* System_MPQGlobals::GetArchiveLock() {
    return &s_globals.Get()->m_archiveLock;
}

// VERIFY: 82f39fdc manual
// OFFSET: 0x00451DC0
System_MPQGlobals::MopaqGlobals* System_MPQGlobals::GetMopaqGlobals() {
    return &s_globals.Get()->m_mopaq;
}

// VERIFY: b9034813 manual
// OFFSET: 0x00451E20
Blizzard::Lock::RMutex* System_MPQGlobals::GetMopaqGlobalsLock() {
    return &s_globals.Get()->m_mopaqLock;
}

// VERIFY: 2488e5d1 manual
// OFFSET: 0x00451E80
System_MPQGlobals::WorkerPool* System_MPQGlobals::GetWorkerPool() {
    return &s_globals.Get()->m_workerPool;
}
