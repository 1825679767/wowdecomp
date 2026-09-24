#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOFileUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/NewZerofill.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00AB7B8C
Blizzard::Lock::Atomic System_SFile::s_nextArchiveId = { static_cast<int32_t>(0x80000000) };

// STATIC: 0x00B32508
uint32_t System_SFile::s_lastError;

// VERIFY: daebe118 auto
// OFFSET: 0x00426A30
uint32_t Blizzard::Mopaq::SFileGetLastError() {
    return System_SFile::s_lastError;
}

// VERIFY: 39e1ff1c manual
// OFFSET: 0x00426AC0
void System_SFile::BitPacked::ReadBits(uint32_t start, uint32_t bits, uint8_t* out, uint32_t outSize) const {
    uint32_t byte = start >> 3;
    uint8_t shift = start & 7;
    uint32_t next = byte + 1;
    uint32_t full = 0;

    if (bits >> 3) {
        full = bits >> 3;

        for (uint32_t i = 0; i < full; i++) {
            if (shift) {
                out[i] = (this->m_data[byte] >> shift) | (this->m_data[next] << (8 - shift));
            } else {
                out[i] = this->m_data[byte];
            }

            byte++;
            next++;
        }
    }

    uint8_t rest = bits & 7;

    if (rest) {
        uint8_t value = this->m_data[byte] >> shift;
        out[full] = value;

        if (8 - shift < rest) {
            out[full] = value | (this->m_data[next] << (8 - shift));
        }

        out[full] &= (1 << rest) - 1;
    }
}

// VERIFY: 4826c2a7 manual
// OFFSET: 0x00426BA0
bool System_SFile::BitPacked::WriteBits(uint32_t start, uint32_t bits, const uint8_t* in, uint32_t inSize) {
    if (start + bits > this->m_bitCount) {
        return false;
    }

    uint8_t shift = start & 7;
    uint32_t byte = start >> 3;
    uint32_t next = byte + 1;
    uint32_t full = 0;

    if (bits >> 3) {
        full = bits >> 3;

        for (uint32_t i = 0; i < full; i++) {
            if (shift) {
                uint8_t mask = (1 << shift) - 1;
                this->m_data[byte] &= mask;
                this->m_data[byte] |= in[i] << shift;
                this->m_data[next] &= ~mask;
                this->m_data[next] |= in[i] >> (8 - shift);
            } else {
                this->m_data[byte] = in[i];
            }

            byte++;
            next++;
        }
    }

    uint32_t rest = bits & 7;

    if (rest) {
        uint8_t value = in[full];
        uint32_t end = shift + rest;

        if (end > 8) {
            this->m_data[byte] &= (1 << shift) - 1;
            this->m_data[byte] |= value << shift;
            this->m_data[next] &= ~((1 << (end - 8)) - 1);
            this->m_data[next] |= value >> (8 - shift);
            return true;
        }

        this->m_data[byte] &= ((1 << shift) - 1) | ~((1 << end) - 1);
        this->m_data[byte] |= value << shift;
    }

    return true;
}

// VERIFY: 89f1ed17 manual
// OFFSET: 0x00426D30
void System_SFile::HashEntryTable::SetHashBits(int32_t bits) {
    this->m_hashBits = bits;
    this->m_hashMask = (bits == 64 ? 0 : static_cast<uint64_t>(1) << bits) - 1;
    this->m_hashTopBit = static_cast<uint64_t>(1) << (bits - 1);
}

// VERIFY: 6022a108 manual
// OFFSET: 0x0042BF40
bool System_SFile::BlockEntryTable::UnpackEntry(uint32_t index, BLOCKENTRY* entry) {
    if (!this->m_entries) {
        return false;
    }

    memset(entry, 0, sizeof(BLOCKENTRY));

    uint32_t base = index * this->m_entryBits;
    this->m_entries->ReadBits(base + this->m_offsetBit, this->m_offsetBits, reinterpret_cast<uint8_t*>(&entry->m_offset), 8);
    this->m_entries->ReadBits(base + this->m_fileSizeBit, this->m_fileSizeBits, reinterpret_cast<uint8_t*>(&entry->m_fileSize), 4);
    this->m_entries->ReadBits(base + this->m_compressedSizeBit, this->m_compressedSizeBits, reinterpret_cast<uint8_t*>(&entry->m_compressedSize), 4);

    size_t flagCount = this->m_flagTable.size();

    if (!flagCount) {
        return false;
    }

    if (flagCount == 1) {
        entry->m_flags = this->m_flagTable[0];
    } else {
        this->m_entries->ReadBits(base + this->m_flagIndexBit, this->m_flagIndexBits, reinterpret_cast<uint8_t*>(&entry->m_flags), 4);

        if (entry->m_flags >= this->m_flagTable.size()) {
            return false;
        }

        entry->m_flags = this->m_flagTable[entry->m_flags];
    }

    if (this->m_tableFlags & 0x200) {
        uint8_t md5Check = 0;
        this->m_entries->ReadBits(base + this->m_md5Bit, this->m_md5Bits, &md5Check, 1);
        entry->m_md5Check = md5Check != 0;
    }

    return true;
}

// VERIFY: 5d56af1c manual
// OFFSET: 0x0042C250
uint64_t System_SFile::HashEntryTable::CalculateHash(const char* name) {
    uint8_t buffer[260];
    uint32_t length = 0;

    if (name) {
        uint8_t* dst = buffer;

        do {
            char c = *name++;

            if (!c) {
                break;
            }

            if (static_cast<uint8_t>(c - 'A') > 25) {
                if (c == '/') {
                    c = '\\';
                }
            } else {
                c += 32;
            }

            *dst++ = c;
        } while (dst < buffer + 259);

        *dst = 0;
        length = static_cast<uint32_t>(dst - buffer);
    }

    uint32_t high = 1;
    uint32_t low = 2;
    Blizzard::Util::hashlittle2(buffer, length, &low, &high);

    return low + (static_cast<uint64_t>(high) << 32);
}

// VERIFY: 76834dff manual
// OFFSET: 0x0042C2F0
uint32_t System_SFile::HashEntryTable::GetEntry(const uint64_t& hash, BlockEntryTable* blocks) {
    uint64_t key = (hash & this->m_hashMask) | this->m_hashTopBit;
    uint32_t start = static_cast<uint32_t>(key % this->m_slotCount);
    uint8_t tag = static_cast<uint8_t>(key >> (this->m_hashBits - 8));
    uint64_t check = key & (this->m_hashMask >> 8);
    uint32_t slot = start;

    do {
        uint8_t current = this->m_tags[slot];

        if (!current) {
            break;
        }

        if (current == tag) {
            uint32_t index = 0;
            this->m_indices->ReadBits(slot * this->m_entryBits, this->m_indexBits, reinterpret_cast<uint8_t*>(&index), 4);

            uint64_t stored = 0;
            blocks->m_hashes->ReadBits(index * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<uint8_t*>(&stored), 8);

            if (stored == check) {
                return index;
            }
        }

        slot = (slot + 1) % this->m_slotCount;
    } while (slot != start);

    return -1;
}

// VERIFY: dab71bcf manual
// OFFSET: 0x00431520
System_SFile::BitPacked::BitPacked(uint32_t bitCount, uint8_t fill) {
    this->m_data = static_cast<uint8_t*>(Blizzard::Memory::Allocate((bitCount + 7) / 8));
    memset(this->m_data, fill, (bitCount + 7) / 8);
    this->m_bitCount = bitCount;
}

// INLINED
System_SFile::BitPacked::~BitPacked() {
    Blizzard::Memory::Free(this->m_data);
}

// VERIFY: cacac497 manual
// OFFSET: 0x00431700
void System_SFile::BlockEntryTable::InitializeHashCheckTable(int32_t bits) {
    int32_t hashBits = bits - 8;

    if (hashBits == this->m_hashBits) {
        return;
    }

    if (hashBits > 0) {
        this->m_hashBits = hashBits;
        this->m_hashExtraBits = 0;
        this->m_hashEntryBits = hashBits;
    }

    delete this->m_hashes;
    this->m_hashes = new BitPacked(this->m_hashEntryBits * this->m_count, 0);
}

// VERIFY: db7caa48 manual
// OFFSET: 0x00431780
System_SFile::HashEntryTable::HashEntryTable(uint32_t count, int32_t hashBits) {
    this->unk_0004 = 0;
    this->m_indexBits = 0;
    this->unk_0000 = 0;
    this->m_tags = nullptr;
    this->m_indices = nullptr;
    this->m_slotCount = 100 * count / 75;

    uint32_t bits = 0;

    for (uint32_t i = count; i; i >>= 1) {
        bits++;
    }

    this->m_indexBits = bits;
    this->unk_0000 = this->m_indexBits;
    this->m_entryBits = this->unk_0000;
    this->unk_0004 = 0;
    this->m_tags = static_cast<uint8_t*>(SMemAlloc(this->m_slotCount, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    this->SetHashBits(hashBits);
}

// VERIFY: f058c50f manual
// OFFSET: 0x00431800
System_SFile::HashEntryTable::~HashEntryTable() {
    if (this->m_indices) {
        delete this->m_indices;
        this->m_indices = nullptr;
    }

    memset(this->m_tags, 0, this->m_slotCount);

    if (this->m_tags) {
        Blizzard::Memory::Free(this->m_tags);
        this->m_tags = nullptr;
    }
}

// VERIFY: 56bf72d7 manual
// OFFSET: 0x00431860
bool System_SFile::HashEntryTable::AddEntry(const char* name, uint32_t blockIndex, BlockEntryTable* blocks) {
    if (!this->m_indices) {
        this->m_indices = new BitPacked(this->m_slotCount * this->m_entryBits, 0xFF);
    }

    uint64_t key = (CalculateHash(name) & this->m_hashMask) | this->m_hashTopBit;
    uint32_t slot = static_cast<uint32_t>(key % this->m_slotCount);
    uint32_t bit = slot * this->m_entryBits;
    uint32_t start = slot;
    uint8_t tag = static_cast<uint8_t>(key >> (this->m_hashBits - 8));
    uint8_t* tags = this->m_tags;
    uint64_t check = key & (this->m_hashMask >> 8);

    if (tags[slot]) {
        for (;;) {
            uint32_t index = 0;
            this->m_indices->ReadBits(bit, this->m_indexBits, reinterpret_cast<uint8_t*>(&index), 4);

            uint64_t stored = 0;
            blocks->m_hashes->ReadBits(index * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<uint8_t*>(&stored), 8);

            if (tags[slot] == tag && stored == check) {
                return false;
            }

            slot = (slot + 1) % this->m_slotCount;

            if (slot == start) {
                return false;
            }

            bit = slot * this->m_entryBits;

            if (!tags[slot]) {
                break;
            }
        }
    }

    if (!this->m_indices->WriteBits(bit, this->m_indexBits, reinterpret_cast<const uint8_t*>(&blockIndex), 4)) {
        return false;
    }

    this->m_tags[slot] = tag;

    return blocks->m_hashes->WriteBits(blockIndex * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<const uint8_t*>(&check), 8);
}

// VERIFY: 9f92b902 manual
// OFFSET: 0x0043D620
System_SFile::BlockEntryTable::~BlockEntryTable() {
    delete this->m_entries;
    delete this->m_hashes;
}

// VERIFY: 8c7d6610 manual
// OFFSET: 0x00437590
System_SFile::ArchiveBuffers::~ArchiveBuffers() {
    memset(this->unk_0000.m_data, 0, this->unk_0000.m_count);
    memset(this->unk_000C.m_data, 0, this->unk_000C.m_count);
    memset(this->unk_0114.m_data, 0, this->unk_0114.m_count);
}

// VERIFY: e3eae1bf manual
// OFFSET: 0x00449AF0
System_SFile::ARCHIVEREC::~ARCHIVEREC() {
    delete this->m_blockEntryTable;
    delete this->m_hashEntryTable;

    if (this->m_file) {
        this->m_file->Release();
        this->m_file = nullptr;
    }
}

// VERIFY: c4093b37 manual
// OFFSET: 0x0044B1C0
System_SFile::ARCHIVEREC::ARCHIVEREC() {
    this->m_blockEntryTable = nullptr;
    this->m_hashEntryTable = nullptr;
}

// VERIFY: ce2acc62 manual
// OFFSET: 0x00453CB0
bool Blizzard::Mopaq::SFileEnableDirectAccess(uint32_t enable) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
    System_MPQGlobals::GetMopaqGlobals()->m_directAccess = enable;
    return true;
}

// VERIFY: edda13c8 manual
// OFFSET: 0x00453D10
bool System_SFile::ARCHIVEREC::RemoveInternalArchiveRef() {
    bool result = true;

    if (this->m_internalRefs.Decrement()) {
        return true;
    }

    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);

    if (this->m_internalRefs.m_value) {
        Blizzard::String::QuickFormat<1024> message("Potential race condition in client code for archive \"%s\"!\n", this->m_name);
        fputs(message.m_buffer, stderr);
        return true;
    }

    this->m_internalRefs.m_value = 1;

    if (this->m_closeCallback && !this->m_closeCallback(this)) {
        result = false;
    }

    if (this->m_buffers) {
        result = false;
        delete this->m_buffers;
    }

    System_MPQGlobals::GetArchiveList()->erase(this);
    this->~ARCHIVEREC();
    Blizzard::Memory::Free(this);

    return result;
}

// VERIFY: 0460da73 manual
// OFFSET: 0x00453E30
bool System_SFile::OpenDirectPathAsArchive(const char* path, int32_t priority, uint32_t flags, ARCHIVEREC** result) {
    ARCHIVEREC* archive = NewZerofill<ARCHIVEREC>();
    archive->m_priority = priority;
    archive->m_id = s_nextArchiveId.Increment() - 1;
    archive->m_flags = flags;
    archive->m_externalRefs.Increment();
    archive->m_internalRefs.Increment();

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        System_MPQGlobals::GetArchiveList()->insert(archive);
    }

    archive->m_name[0] = '\0';
    archive->m_file = nullptr;
    archive->m_parent = nullptr;
    archive->unk_0200 = 0;

    char directory[260];
    Blizzard::String::MakeBackslashPath(path, directory, sizeof(directory));

    if (directory[0]) {
        Blizzard::String::ForceTrailingSeparator(directory, sizeof(directory), '\\');
    }

    archive->m_directPath = directory;
    *result = archive;

    return true;
}

// VERIFY: 571a18d1 manual
// OFFSET: 0x00454070
void System_SFile::ConvertRelativePathName(const char* path, char* result) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);

    char joined[260];
    Blizzard::String::JoinPath(joined, sizeof(joined), System_MPQGlobals::GetMopaqGlobals()->m_basePath, path);
    Blizzard::File::MakeAbsolutePath(joined, result, 260, false);
}

// VERIFY: c3c35a26 manual
// OFFSET: 0x00458730
bool Blizzard::Mopaq::SFileSetBasePath(const char* path) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
    System_MPQGlobals::MopaqGlobals* globals = System_MPQGlobals::GetMopaqGlobals();

    if (!*path) {
        if (!Blizzard::Process::GetProcessDirectory(globals->m_basePath, sizeof(globals->m_basePath))) {
            globals->m_basePath[0] = '\0';
        }

        return true;
    }

    if (strlen(path) + 1 >= sizeof(globals->m_basePath)) {
        System_SFile::s_lastError = 161;
        Blizzard::String::QuickFormat<1024> message("SFileSetBasePath - SFILE_ERROR_BAD_PATHNAME");
        Blizzard::File::AddToLastErrorStack(161, message.m_buffer, 4);
        return false;
    }

    Blizzard::String::Copy(globals->m_basePath, path, sizeof(globals->m_basePath));
    Blizzard::String::ForceTrailingSeparator(globals->m_basePath, sizeof(globals->m_basePath), Blizzard::String::FirstPathSeparator(globals->m_basePath));

    return true;
}

// VERIFY: 17798d55 manual
// OFFSET: 0x00458880
System_SFile::ARCHIVEREC* System_SFile::GetArchivePtr(ARCHIVEREC* handle, bool addRef) {
    ARCHIVEREC* archive = nullptr;

    if (!handle) {
        return nullptr;
    }

    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
    System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();
    System_MPQGlobals::ArchiveList::iterator it = list->find(handle);

    if (it == list->end()) {
        s_lastError = 6;
        Blizzard::String::QuickFormat<1024> message("GetArchivePtr - SFILE_ERROR_INVALID_HANDLE");
        Blizzard::File::AddToLastErrorStack(6, message.m_buffer, 4);
    } else {
        archive = *it;

        if (addRef) {
            archive->m_internalRefs.Increment();
        }
    }

    return archive;
}

// VERIFY: 11a84348 manual
// OFFSET: 0x00458980
bool Blizzard::Mopaq::SFileCloseArchive(System_SFile::ARCHIVEREC* handle) {
    bool result = true;
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, false);

    if (!archive) {
        return false;
    }

    if (archive->m_parent) {
        System_SFile::ARCHIVEREC* parent = System_SFile::GetArchivePtr(archive->m_parent, false);
        result = parent ? parent->RemoveInternalArchiveRef() : false;
    }

    archive->m_externalRefs.Decrement();

    return archive->RemoveInternalArchiveRef() && result;
}

// VERIFY: 21794a23 manual
// OFFSET: 0x0045BFE0
bool System_SFile::s_OpenArchive(ARCHIVEREC* archive, uint32_t flags, ARCHIVEREC** result) {
    int64_t bufferPos = 0;
    uint32_t bufferSize = 0;
    int64_t candidate = 0;
    bool haveCandidate = false;
    bool retried = false;
    int64_t fileSize;
    uint8_t buffer[4096];
    const uint32_t* header;
    const char* error;

    if (!archive->m_file->GetFileSize(&fileSize)) {
        return false;
    }

    for (;;) {
        for (;;) {
            while (archive->m_archiveOffset >= bufferPos + bufferSize) {
                uint32_t read = 0;
                bufferPos = archive->m_archiveOffset;

                if (!archive->m_file->Read(buffer, archive->m_archiveOffset, sizeof(buffer), &read)) {
                    System_File::FileError& fileError = System_File::s_error;

                    if (fileError.m_errorStack && fileError.m_errorStack->FindCategory(21)) {
                        error = "OpenArchive - HTTP error - %s";
                        goto fail;
                    }
                }

                if (read >= 0x2C && archive->m_archiveOffset + 44 < fileSize) {
                    bufferSize = read;
                    break;
                }

                if (!(flags & 0x20) || !haveCandidate) {
                    error = "OpenArchive - SFILE_ERROR_NOT_ARCHIVE - %s";
                    goto fail;
                }

                archive->m_archiveOffset = candidate;
                bufferPos = 0;
                bufferSize = 0;
                retried = true;
            }

            header = reinterpret_cast<const uint32_t*>(buffer + static_cast<uint32_t>(archive->m_archiveOffset - bufferPos));

            if (header[0] == 0x1A51504D) {
                break;
            }

            if (header[0] != 0x1B51504D) {
                goto next;
            }

            archive->m_userDataOffset = archive->m_archiveOffset + 12;
            archive->m_userDataSize = header[1];
            archive->m_archiveOffset += header[2];
        }

        {
            memcpy(&archive->m_header, header, sizeof(MPQHEADER));

            bool valid;

            if (archive->m_header.m_formatVersion == 0) {
                valid = archive->m_header.m_headerSize == 32;
                archive->m_header.m_hiBlockTablePos = 0;
                archive->m_header.m_hashTablePosHi = 0;
                archive->m_header.m_blockTablePosHi = 0;
            } else if (archive->m_header.m_formatVersion == 1) {
                valid = archive->m_header.m_headerSize == 44;
            } else {
                valid = archive->m_header.m_headerSize >= 0x2C;
            }

            int64_t blockEnd = ((static_cast<int64_t>(archive->m_header.m_blockTablePosHi) << 32) | archive->m_header.m_blockTablePos) + 16 * archive->m_header.m_blockTableSize;
            int64_t hiBlockEnd = archive->m_header.m_hiBlockTablePos + 2 * archive->m_header.m_blockTableSize;
            int64_t hashEnd = ((static_cast<int64_t>(archive->m_header.m_hashTablePosHi) << 32) | archive->m_header.m_hashTablePos) + 16 * archive->m_header.m_hashTableSize;
            const int64_t* end = hashEnd >= blockEnd ? &hashEnd : &blockEnd;

            if (hiBlockEnd >= *end) {
                end = &hiBlockEnd;
            }

            if (*end >= fileSize) {
                end = &fileSize;
            }

            archive->m_archiveEnd = *end + archive->m_archiveOffset;

            if (archive->m_header.m_magic == 0x1A51504D && valid
                && ((static_cast<int64_t>(archive->m_header.m_hashTablePosHi) << 32) | archive->m_header.m_hashTablePos) < archive->m_archiveEnd
                && ((static_cast<int64_t>(archive->m_header.m_blockTablePosHi) << 32) | archive->m_header.m_blockTablePos) < archive->m_archiveEnd) {
                haveCandidate = true;
                candidate = archive->m_archiveOffset;

                if (retried || !(flags & 0x20)) {
                    break;
                }
            }
        }

    next:
        if (flags & 8) {
            error = "OpenArchive - SFILE_ERROR_NOT_ARCHIVE - %s";
            goto fail;
        }

        archive->m_archiveOffset += 512;
    }

    archive->m_sectorSize = 512 << archive->m_header.m_sectorSizeShift;
    archive->unk_01E0 = 1;

    {
        uint16_t version = 2;
        archive->unk_01E4.push_back(version);
    }

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        System_MPQGlobals::GetArchiveList()->insert(archive);
    }

    if (flags & 0x800) {
        archive->InitializeTables();
    }

    *result = archive;
    return true;

fail:
    s_lastError = 108;

    {
        Blizzard::String::QuickFormat<1024> message(error, archive->m_name);
        Blizzard::File::AddToLastErrorStack(108, message.m_buffer, 4);
    }

    archive->m_externalRefs.Decrement();
    archive->RemoveInternalArchiveRef();

    return false;
}

// VERIFY: 726dd6d5 manual
// OFFSET: 0x0045C3D0
bool Blizzard::Mopaq::SFileOpenArchive(IOUnit* file, const char* name, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result) {
    file->m_refCount.Increment();

    System_SFile::ARCHIVEREC* archive = NewZerofill<System_SFile::ARCHIVEREC>();
    archive->m_priority = priority;
    archive->m_id = System_SFile::s_nextArchiveId.Increment() - 1;
    archive->m_flags = flags;
    Blizzard::String::Copy(archive->m_name, name, sizeof(archive->m_name));
    archive->m_file = file;
    archive->m_externalRefs.Increment();
    archive->m_internalRefs.Increment();
    archive->m_archiveOffset = 0;
    archive->m_archiveEnd = 0;
    archive->unk_0200 = 0;

    return System_SFile::s_OpenArchive(archive, flags, result);
}

// VERIFY: 0ed52187 manual
// OFFSET: 0x0045C480
bool Blizzard::Mopaq::SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result) {
    const char* name = path;
    char converted[260];

    *result = nullptr;

    int32_t exists = Blizzard::File::Exists(path);

    if (!exists && !Blizzard::File::GetRootChars(path)) {
        System_SFile::ConvertRelativePathName(path, converted);
        name = converted;
        exists = Blizzard::File::Exists(converted);
    }

    if (exists == 1) {
        int32_t mode = ((flags & 0x10) | 0x8028) >> 3;
        IOUnit* file;

        if (mode & 0x40) {
            file = System_Mopaq::OpenDiskFile(nullptr, name, mode);
        } else {
            file = IOFileUnit::Create(nullptr, name, mode);
        }

        if (file) {
            bool opened = SFileOpenArchive(file, name, priority, flags, result);
            file->Release();
            return opened;
        }

        System_File::FileError& fileError = System_File::s_error;

        if (fileError.m_errorStack->FindCategoryCode(1, 4)) {
            System_SFile::s_lastError = 5;
            return false;
        }
    } else if (exists == 2) {
        return System_SFile::OpenDirectPathAsArchive(name, priority, flags, result);
    }

    System_SFile::s_lastError = 2;
    Blizzard::String::QuickFormat<1024> message("SFileOpenArchive - SFILE_ERROR_FILE_NOT_FOUND - %s", name);
    Blizzard::File::AddToLastErrorStack(2, message.m_buffer, 4);

    return false;
}
