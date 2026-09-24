#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOAlignUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOChecksumUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOFileUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOUnitContainer.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOMpqUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQEncrypt.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B38344
Blizzard::Thread::LocalObject<System_SFile::RawReadInfo> System_SFile::s_rawReadInfo;

// STATIC: 0x00B3858C
uint32_t System_SFile::s_threadingMode;

// INLINED
System_SFile::RawReadInfo::RawReadInfo() {
    this->m_retryNative = false;
}

void STORMAPI SFile::CreateOverlapped(SOVERLAPPED* overlapped) {
}

void STORMAPI SFile::DestroyOverlapped(SOVERLAPPED* overlapped) {
}

// VERIFY: 65f10e31 auto
// OFFSET: 0x00426E20
bool System_SFile::CheckCrc(void* context, uint32_t crc) {
    return crc == static_cast<IOMpqArchiveUnit*>(context)->m_blockEntry.m_crc;
}

// VERIFY: dc1be6b1 auto
// OFFSET: 0x00426FD0
void Blizzard::Mopaq::SFileSetThreadingMode(uint32_t mode) {
    System_SFile::s_threadingMode = mode;
}

// VERIFY: 94d8b66b manual
// OFFSET: 0x0042ED90
IOUnit* System_SFile::GetLockedUnit(IOUnit* file) {
    if (!file || file->GetType() != 'mpq ') {
        return nullptr;
    }

    IOUnit* lock = file->m_next;

    if (!lock || lock->GetType() != 'lock') {
        return nullptr;
    }

    return lock->m_next;
}

// VERIFY: df584044 auto
// OFFSET: 0x0042EDD0
bool System_SFile::InsertLockedUnit(IOUnit* file, IOUnit* unit) {
    if (!file || !unit || file->GetType() != 'mpq ') {
        return false;
    }

    return IOLockUnit::ReplaceNext(file->m_next, unit);
}

// VERIFY: 07c1b49c manual
// OFFSET: 0x00437310
IOUnit* System_SFile::OpenFileUnit(IOUnitContainer* container, const char* path, int32_t mode) {
    if (mode & 0x40) {
        return System_Mopaq::OpenDiskFile(container, path, mode);
    }

    return IOFileUnit::Create(container, path, mode);
}

// VERIFY: 66befaed auto
// OFFSET: 0x00437340
IOUnit* System_SFile::CreateRangeUnit(IOUnitContainer* container, int64_t start, int64_t end, IOUnit* next) {
    return IORangeUnit::Create(container, start, end, next);
}

// VERIFY: ed823991 manual
// OFFSET: 0x004609B0
int32_t Blizzard::Mopaq::SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, IOMpqUnit** result) {
    IOMpqUnit* file = nullptr;
    *result = nullptr;
    System_SFile::ARCHIVEREC* archive = nullptr;
    uint32_t error = 104;
    uint32_t blockIndex;
    char path[260];
    int32_t found = GetFileBlockEntry(handle, name, md5, flags, nullptr, &archive, &blockIndex, nullptr, path);

    Blizzard::Log::Printf(3, "Logs/Profiling_File_SFileOpenFileEx.log", "%s\n", name);

    switch (found) {
        case 0:
        case 4: {
            error = 2;
            goto fail;
        }

        case 1: {
            System_SFile::BLOCKENTRY entry = archive->GetBlockEntry(blockIndex);

            if (!(entry.m_flags & 0x80000000)) {
                error = 1006;
                goto fail;
            }

            IOChecksumUnit* checksum = nullptr;

            if (entry.m_flags & 0xFFF00) {
                uint32_t fileSize = entry.m_fileSize;
                uint32_t key = 0;

                if (entry.m_flags & 0xF0000) {
                    key = System_MPQEncrypt::Hash(Blizzard::String::FindFilename(name), 3);

                    if (entry.m_flags & 0x20000) {
                        key = fileSize ^ (key + static_cast<uint32_t>(entry.m_offset) - static_cast<uint32_t>(archive->m_archiveOffset));
                    }
                }

                archive->m_file->m_refCount.Increment();

                if (entry.m_flags & 0x1000000) {
                    IOUnitContainer container(sizeof(IOMpqArchiveReadUnit), sizeof(IOCrcUnit), sizeof(IOAlignUnit), sizeof(IOLockUnit), sizeof(IOMpqReadUnit), sizeof(IORangeUnit), 0);
                    IOUnit* range = System_SFile::CreateRangeUnit(&container, entry.m_offset, entry.m_offset + entry.m_compressedSize, archive->m_file);
                    IOUnit* read = IOMpqReadUnit::CreateSingle(&container, fileSize, entry.m_compressedSize, entry.m_flags, key, range);
                    IOUnit* crc = IOCrcUnit::Create(&container, fileSize, entry.m_crc ? System_SFile::CheckCrc : nullptr, false, &checksum, read);
                    IOUnit* align = IOAlignUnit::Create(&container, 0, fileSize, 1, false, false, 1, crc);
                    IOUnit* lock = IOLockUnit::Create(&container, align);
                    file = IOMpqArchiveReadUnit::Create(&container, archive, name, blockIndex, lock);
                } else {
                    bool checkCrc = !(entry.m_flags & 0x4000000) && entry.m_crc;
                    IOUnitContainer container(sizeof(IOMpqArchiveReadUnit), sizeof(IOCrcUnit), sizeof(IOAlignUnit), sizeof(IOLockUnit), sizeof(IOMpqReadUnit), sizeof(IORangeUnit), 0);
                    IOUnit* range = System_SFile::CreateRangeUnit(&container, entry.m_offset, entry.m_offset + entry.m_compressedSize, archive->m_file);
                    IOUnit* read = IOMpqReadUnit::Create(&container, fileSize, entry.m_compressedSize, archive->m_sectorSize, entry.m_flags, key, range);
                    IOUnit* crc = IOCrcUnit::Create(&container, fileSize, checkCrc ? System_SFile::CheckCrc : nullptr, false, &checksum, read);
                    IOUnit* align = IOAlignUnit::Create(&container, 0, archive->m_sectorSize, 1, true, true, 1, crc);
                    IOUnit* lock = IOLockUnit::Create(&container, align);
                    file = IOMpqArchiveReadUnit::Create(&container, archive, name, blockIndex, lock);
                }
            } else {
                if (entry.m_compressedSize != entry.m_fileSize) {
                    error = 1006;
                    goto fail;
                }

                archive->m_file->m_refCount.Increment();

                IOUnitContainer container(sizeof(IOMpqArchiveReadUnit), sizeof(IOCrcUnit), sizeof(IOLockUnit), sizeof(IORangeUnit), 0);
                IOUnit* range = System_SFile::CreateRangeUnit(&container, entry.m_offset, entry.m_offset + entry.m_fileSize, archive->m_file);
                IOUnit* crc = IOCrcUnit::Create(&container, entry.m_fileSize, entry.m_crc ? System_SFile::CheckCrc : nullptr, false, &checksum, range);
                IOUnit* lock = IOLockUnit::Create(&container, crc);
                file = IOMpqArchiveReadUnit::Create(&container, archive, name, blockIndex, lock);
            }

            if (checksum) {
                checksum->SetContext(file);
            }

            found = 1;
            break;
        }

        case 2:
        case 3: {
            IOUnitContainer container(sizeof(IOMpqDiskUnit), sizeof(IOUnbufferedFileUnit), 0);
            IOUnit* disk = System_SFile::OpenFileUnit(&container, path, (((flags & 4) | 2) >> 1) | 0x1004);
            file = IOMpqDiskUnit::Create(&container, archive, name, path, disk);
            found = 2;
            break;
        }

        default: {
            goto fail;
        }
    }

    if (!file || (archive && !archive->RemoveInternalArchiveRef())) {
        goto fail;
    }

    if (found) {
        void (*callback)(const char* name, void* param);
        void* param;

        {
            Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
            System_MPQGlobals::MopaqGlobals* globals = System_MPQGlobals::GetMopaqGlobals();
            callback = globals->m_openCallback;
            param = globals->m_openCallbackParam;
        }

        if (callback) {
            callback(name, param);
        }

        if (System_MPQGlobals::GetMopaqGlobals()->m_md5Mode == 1 && file->HasMd5Check()) {
            int64_t size;
            file->GetFileSize(&size);

            IOUnit* inner = System_SFile::GetLockedUnit(file);
            IOUnit* md5Unit = IOMd5Unit::Create(nullptr, size, System_MPQGlobals::GetMopaqGlobals()->m_md5Callback, true, nullptr, inner);
            static_cast<IOChecksumUnit*>(md5Unit)->SetContext(file);

            bool inserted = System_SFile::InsertLockedUnit(file, md5Unit);

            if (!inner || !inserted) {
                inner->m_refCount.Increment();
                md5Unit->Release();
                found = 0;
            }
        }
    }

    *result = file;
    return found;

fail:
    System_SFile::s_lastError = error;
    found = 0;

    if (file) {
        file->Release();
        file = nullptr;
    }

    *result = file;
    return found;
}
