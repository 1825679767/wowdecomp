#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOFileUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOMpqUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQEncrypt.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/NewZerofill.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SComp.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileSignatures.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
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

// STATIC: 0x00B38A6C
Blizzard::Util::Digest<16> System_SFile::s_emptyMd5;

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

// INLINED
void System_SFile::BlockEntryTable::UpdateLayout() {
    this->m_entryBits = this->m_offsetBits;
    this->m_offsetBit = 0;
    this->m_fileSizeBit = this->m_offsetBits;
    this->m_entryBits += this->m_fileSizeBits;
    this->m_compressedSizeBit = this->m_fileSizeBit + this->m_fileSizeBits;
    this->m_entryBits += this->m_compressedSizeBits;
    this->m_flagIndexBit = this->m_compressedSizeBit + this->m_compressedSizeBits;
    this->m_entryBits += this->m_flagIndexBits;
    this->m_md5Bit = this->m_flagIndexBit + this->m_flagIndexBits;
    this->m_entryBits += this->m_md5Bits;
}

// VERIFY: 519bc563 manual
// OFFSET: 0x0042C050
void System_SFile::BlockEntryTable::SetLayout(BLOCKENTRY& maxima) {
    uint32_t bits = 0;

    for (uint32_t value = static_cast<uint32_t>(maxima.m_offset >> 32); value; value >>= 1) {
        bits++;
    }

    if (bits) {
        bits += 32;
    } else {
        for (uint32_t value = static_cast<uint32_t>(maxima.m_offset); value; value >>= 1) {
            bits++;
        }
    }

    this->m_offsetBits = bits;
    this->UpdateLayout();

    bits = 0;

    for (uint32_t value = maxima.m_fileSize; value; value >>= 1) {
        bits++;
    }

    this->m_fileSizeBits = bits;
    this->UpdateLayout();

    bits = 0;

    for (uint32_t value = maxima.m_compressedSize; value; value >>= 1) {
        bits++;
    }

    this->m_compressedSizeBits = bits;
    this->UpdateLayout();

    uint32_t flagCount = static_cast<uint32_t>(this->m_flagTable.size());

    if (flagCount > 1) {
        bits = 0;

        for (uint32_t value = flagCount; value; value >>= 1) {
            bits++;
        }

        this->m_flagIndexBits = bits;
        this->UpdateLayout();
    }

    if (this->m_tableFlags & 0x200) {
        this->m_md5Bits = 1;
        this->UpdateLayout();
    }
}

// VERIFY: 2269e3f2 manual
// OFFSET: 0x00431570
bool System_SFile::BlockEntryTable::PackEntry(uint32_t index, const BLOCKENTRY* entry) {
    if (!this->m_entries) {
        this->m_entries = new BitPacked(this->m_entryBits * this->m_count, 0);
    }

    uint32_t base = index * this->m_entryBits;

    if (!this->m_entries->WriteBits(base + this->m_offsetBit, this->m_offsetBits, reinterpret_cast<const uint8_t*>(&entry->m_offset), 8)
        || !this->m_entries->WriteBits(base + this->m_fileSizeBit, this->m_fileSizeBits, reinterpret_cast<const uint8_t*>(&entry->m_fileSize), 4)
        || !this->m_entries->WriteBits(base + this->m_compressedSizeBit, this->m_compressedSizeBits, reinterpret_cast<const uint8_t*>(&entry->m_compressedSize), 4)) {
        return false;
    }

    if (this->m_flagTable.size() <= 1) {
        return true;
    }

    uint32_t flagIndex = 0;

    if (this->m_flagTable.size()) {
        while (flagIndex < this->m_flagTable.size() && this->m_flagTable[flagIndex] != entry->m_flags) {
            flagIndex++;
        }
    }

    if (flagIndex == this->m_flagTable.size()) {
        return false;
    }

    if (!this->m_entries->WriteBits(base + this->m_flagIndexBit, this->m_flagIndexBits, reinterpret_cast<const uint8_t*>(&flagIndex), 4)) {
        return false;
    }

    if (this->m_tableFlags & 0x200) {
        uint8_t md5Check = entry->m_md5Check != 0;
        return this->m_entries->WriteBits(base + this->m_md5Bit, this->m_md5Bits, &md5Check, 1);
    }

    return true;
}

// VERIFY: fc0aa738 manual
// OFFSET: 0x00437510
void System_SFile::ARCHIVEREC::PackBlockTable() {
    uint32_t index = 0;

    for (std::vector<BLOCKENTRY>::iterator it = this->m_blockTable.begin(); it != this->m_blockTable.end(); ++it) {
        this->m_blockEntryTable->PackEntry(index++, &*it);
    }
}

// VERIFY: 774ccb9f manual
// OFFSET: 0x00446FA0
System_SFile::BlockEntryTable::BlockEntryTable(uint32_t count, uint32_t flags) {
    this->m_entryBits = 0;
    this->m_offsetBit = 0;
    this->m_fileSizeBit = 0;
    this->m_compressedSizeBit = 0;
    this->m_flagIndexBit = 0;
    this->m_md5Bit = 0;
    this->m_offsetBits = 0;
    this->m_fileSizeBits = 0;
    this->m_compressedSizeBits = 0;
    this->m_flagIndexBits = 0;
    this->m_md5Bits = 0;
    this->m_hashEntryBits = 0;
    this->m_hashExtraBits = 0;
    this->m_hashBits = 0;
    this->m_hashes = nullptr;
    this->m_entries = nullptr;
    this->m_count = count;
    this->m_tableFlags = flags;

    if (flags & 0x80) {
        this->m_crcs.resize(this->m_count, 0);
    }

    if (this->m_tableFlags & 0x40) {
        this->m_md5s.resize(this->m_count);
    }

    if (this->m_tableFlags & 0x100) {
        this->m_fileTimes.resize(this->m_count, 0);
    }
}

// VERIFY: 087d3832 manual
// OFFSET: 0x004470B0
void System_SFile::BlockEntryTable::AddFlags(uint32_t flags) {
    for (std::vector<uint32_t>::iterator it = this->m_flagTable.begin(); it != this->m_flagTable.end(); ++it) {
        if (*it == flags) {
            return;
        }
    }

    this->m_flagTable.push_back(flags);
}

// VERIFY: e18a2c3d manual
// OFFSET: 0x00447170
System_SFile::BLOCKENTRY* System_SFile::BlockEntryTable::GetMaxima(BLOCKENTRY* maxima, const std::vector<BLOCKENTRY>& entries) {
    maxima->m_offset = 0;
    maxima->m_compressedSize = 0;
    maxima->m_fileSize = 0;

    for (std::vector<BLOCKENTRY>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        if (it->m_offset > maxima->m_offset) {
            maxima->m_offset = it->m_offset;
        }

        if (it->m_compressedSize > maxima->m_compressedSize) {
            maxima->m_compressedSize = it->m_compressedSize;
        }

        if (it->m_fileSize > maxima->m_fileSize) {
            maxima->m_fileSize = it->m_fileSize;
        }

        this->AddFlags(it->m_flags);
    }

    return maxima;
}

// VERIFY: e9ba7a76 manual
// OFFSET: 0x004472A0
void System_SFile::ARCHIVEREC::BuildBlockTable() {
    this->m_blockEntryTable = new BlockEntryTable(static_cast<uint32_t>(this->m_blockTable.size()), this->m_flags);

    BLOCKENTRY maxima;
    this->m_blockEntryTable->GetMaxima(&maxima, this->m_blockTable);
    this->m_blockEntryTable->SetLayout(maxima);
    this->PackBlockTable();

    std::vector<BLOCKENTRY>().swap(this->m_blockTable);
}

// VERIFY: adaffe17 manual
// OFFSET: 0x004473A0
bool System_SFile::InitEmptyTables(ARCHIVEREC* archive, uint32_t hashCount) {
    std::vector<BLOCKENTRY>().swap(archive->m_blockTable);
    delete archive->m_blockEntryTable;
    delete archive->m_hashEntryTable;
    archive->m_header.m_blockTableSize = 0;

    HASHENTRY empty;
    memset(&empty, 0xFF, sizeof(empty));

    std::vector<HASHENTRY>().swap(archive->m_hashTable);
    archive->m_hashTable.insert(archive->m_hashTable.end(), hashCount, empty);
    archive->m_header.m_hashTableSize = hashCount;
    archive->m_blockKeys.clear();
    archive->m_blockKeysBuilt = true;
    archive->m_tablesInitialized = true;

    return true;
}

// INLINED
System_SFile::ArchivePtr::ArchivePtr(ARCHIVEREC* archive) {
    this->m_archive = archive;
    this->m_locked = false;
}

// VERIFY: a3a3d82c manual
// OFFSET: 0x00457E20
System_SFile::ArchivePtr::~ArchivePtr() {
    if (this->m_locked && this->m_archive) {
        Blizzard::Lock::MutexLeave(this->m_archive->m_mutex.m_mutex);
    }

    this->m_locked = false;

    if (this->m_archive) {
        this->m_archive->RemoveInternalArchiveRef();
    }
}

// VERIFY: 7b172bc2 manual
// OFFSET: 0x0045B830
void System_SFile::ARCHIVEREC::ConvertToNewHashTableFormat() {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (!blocks) {
        return;
    }

    uint32_t count = blocks->m_count;
    blocks->InitializeHashCheckTable(64);
    this->m_hashEntryTable = new HashEntryTable(count, 64);

    if (this->FillNewHashTable(64)) {
        std::vector<HASHENTRY>().swap(this->m_hashTable);
    } else {
        delete this->m_hashEntryTable;
        this->m_hashEntryTable = nullptr;
    }
}

// VERIFY: 21a7ff6e manual
// OFFSET: 0x0045B8B0
void System_SFile::ARCHIVEREC::InitializeTables() {
    if (this->m_tablesInitialized) {
        return;
    }

    ArchivePtr archive(GetArchivePtr(this, true));

    if (this->m_hashTable.size() || this->m_blockEntryTable) {
        return;
    }

    HASHENTRY zero = {};
    this->m_hashTable.resize(this->m_header.m_hashTableSize, zero);

    uint32_t hashSize;
    uint32_t hashDiskSize;
    int64_t hashPos;
    bool success;

    if (!this->m_header.m_hashTableSize) {
        goto hashError;
    }

    hashPos = (static_cast<int64_t>(this->m_header.m_hashTablePosHi) << 32) | this->m_header.m_hashTablePos;
    hashDiskSize = this->m_header.m_blockTablePos - this->m_header.m_hashTablePos;
    hashSize = this->m_header.m_hashTableSize * 16;

    if (hashDiskSize != hashSize) {
        uint8_t* buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(hashDiskSize));
        success = this->m_file->Read(buffer, this->m_archiveOffset + hashPos, hashDiskSize, nullptr);

        if (success) {
            System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(buffer), hashDiskSize, 0xC3AF3770);
            hashSize = this->m_header.m_hashTableSize * 16;
            success = System_SComp::Decompress(&this->m_hashTable[0], &hashSize, buffer, hashDiskSize);

            if (success) {
                archive.m_archive->m_compressedTables = true;
            }
        }

        Blizzard::Memory::Free(buffer);
    } else {
        success = this->m_file->Read(&this->m_hashTable[0], this->m_archiveOffset + hashPos, hashSize, nullptr);

        if (success) {
            System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(&this->m_hashTable[0]), this->m_header.m_hashTableSize * 16, 0xC3AF3770);
        }
    }

    if (!success) {
        goto hashError;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(this->m_header.m_hashTableSize); i++) {
        this->m_hashTable[i].unk_000B = 0;
    }

    this->m_blockTable.resize(this->m_header.m_blockTableSize);

    if (this->m_header.m_blockTableSize) {
        int64_t blockPos = (static_cast<int64_t>(this->m_header.m_blockTablePosHi) << 32) | this->m_header.m_blockTablePos;
        uint32_t blockDiskSize;

        if (this->m_header.m_hiBlockTablePos) {
            blockDiskSize = static_cast<uint32_t>(this->m_header.m_hiBlockTablePos) - this->m_header.m_blockTablePos;
        } else {
            blockDiskSize = static_cast<uint32_t>(this->m_archiveEnd) - this->m_header.m_blockTablePos - static_cast<uint32_t>(this->m_archiveOffset);
        }

        uint32_t blockSize = this->m_header.m_blockTableSize * 16;

        if (blockDiskSize != blockSize) {
            uint8_t* buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(blockDiskSize));
            success = this->m_file->Read(buffer, this->m_archiveOffset + blockPos, blockDiskSize, nullptr);

            if (success) {
                System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(buffer), blockDiskSize, 0xEC83B3A3);
                blockSize = this->m_header.m_blockTableSize * 16;
                success = System_SComp::Decompress(&this->m_blockTable[0], &blockSize, buffer, blockDiskSize);

                if (success) {
                    archive.m_archive->m_compressedTables = true;
                }
            }

            Blizzard::Memory::Free(buffer);
        } else {
            success = this->m_file->Read(&this->m_blockTable[0], this->m_archiveOffset + blockPos, blockSize, nullptr);

            if (success) {
                System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(&this->m_blockTable[0]), this->m_header.m_blockTableSize * 16, 0xEC83B3A3);
            }
        }

        if (!success) {
            Blizzard::String::QuickFormat<1024> message("Unable to read archive block table: \"%s\"\n", this->m_name);
            fputs(message.m_buffer, stderr);
            InitEmptyTables(this, 1);
            return;
        }

        const uint32_t* raw = reinterpret_cast<const uint32_t*>(&this->m_blockTable[0]) + (this->m_header.m_blockTableSize * 16 - 16) / 4;
        BLOCKENTRY* first = &this->m_blockTable[0];
        int64_t archiveOffset = this->m_archiveOffset;

        for (BLOCKENTRY* entry = &this->m_blockTable[this->m_header.m_blockTableSize - 1]; entry >= first; entry--, raw -= 4) {
            uint32_t offset = raw[0];
            uint32_t compressedSize = raw[1];
            uint32_t fileSize = raw[2];
            uint32_t flags = raw[3];
            entry->m_compressedSize = compressedSize;
            entry->m_offset = offset + archiveOffset;
            entry->m_fileSize = fileSize;
            entry->m_flags = flags;
            entry->m_crc = 0;
            entry->m_fileTime = 0;
            entry->m_md5 = s_emptyMd5;
            entry->m_md5Check = false;
        }
    }

    if (this->m_header.m_hiBlockTablePos) {
        HeapArray<uint16_t> hiBlockTable(this->m_header.m_blockTableSize);

        if (!this->m_file->Read(hiBlockTable.m_data, this->m_archiveOffset + this->m_header.m_hiBlockTablePos, this->m_header.m_blockTableSize * 2, nullptr)) {
            Blizzard::String::QuickFormat<1024> message("Unable to read extended block table: \"%s\"\n", this->m_name);
            Blizzard::Debug::StderrOutput(message.m_buffer);
            InitEmptyTables(this, 1);
            return;
        }

        for (uint32_t i = 0; i < this->m_header.m_blockTableSize; i++) {
            this->m_blockTable[i].m_offset |= static_cast<int64_t>(hiBlockTable.m_data[i]) << 32;
        }
    }

    if (!(this->m_flags & 0x10) && (this->m_flags & 0x400)) {
        this->BuildBlockTable();
        this->ConvertToNewHashTableFormat();
    }

    if (!Blizzard::Streaming::s_streamingReady || !Blizzard::Streaming::s_isStreamingTrial) {
        if (!this->ReadAdditionalAttributes()) {
            Blizzard::String::QuickFormat<1024> message("Unable to read extra attributes: \"%s\"\n", this->m_name);
            Blizzard::Debug::StderrOutput(message.m_buffer);
        }
    }

    this->m_blockKeysBuilt = false;
    this->m_tablesInitialized = true;
    return;

hashError:
    {
        Blizzard::String::QuickFormat<1024> message("Unable to read archive hash table: \"%s\"\n", this->m_name);
        fputs(message.m_buffer, stderr);
    }

    InitEmptyTables(this, 1);
}

// VERIFY: 30fc5178 manual
// OFFSET: 0x0042C3F0
System_SFile::BLOCKENTRY System_SFile::ARCHIVEREC::GetBlockEntry(uint32_t index) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (!blocks) {
        return this->m_blockTable[index];
    }

    BLOCKENTRY entry;
    blocks->UnpackEntry(index, &entry);

    if (this->m_flags & 0x80) {
        entry.m_crc = this->m_blockEntryTable->m_crcs[index];
    }

    if (this->m_flags & 0x40) {
        entry.m_md5 = this->m_blockEntryTable->m_md5s[index];
    }

    if (this->m_flags & 0x100) {
        entry.m_fileTime = this->m_blockEntryTable->m_fileTimes[index];
    }

    return entry;
}

// VERIFY: 913cd4bd manual
// OFFSET: 0x0042C7E0
uint32_t System_SFile::SearchHashTable(ARCHIVEREC* archive, const char* name, bool matchNeutral, bool* deleted) {
    archive->InitializeTables();

    uint16_t locale = archive->m_locale;
    uint8_t platform = archive->m_platform;
    std::vector<HASHENTRY>& table = archive->m_hashTable;
    uint32_t mask = static_cast<uint32_t>(table.size()) - 1;
    uint32_t index = System_MPQEncrypt::Hash(name, 0) & mask;
    uint32_t found = -1;
    HASHENTRY* entry = &table[index];

    if (entry->m_blockIndex == -1) {
        return -1;
    }

    uint32_t hashA = System_MPQEncrypt::Hash(name, 1);
    uint32_t hashB = System_MPQEncrypt::Hash(name, 2);
    uint32_t start = index;

    while (true) {
        if (entry->m_hashA == hashA && entry->m_hashB == hashB && entry->m_blockIndex != -2) {
            if (entry->m_locale == locale && entry->m_platform == platform) {
                found = index;
                break;
            }

            if (matchNeutral && (!entry->m_locale || entry->m_locale == locale) && (!entry->m_platform || entry->m_platform == platform)) {
                found = index;
            }
        }

        index = (index + 1) & mask;

        if (index == start) {
            break;
        }

        entry = &table[index];

        if (entry->m_blockIndex == -1) {
            break;
        }
    }

    if (found != -1 && deleted) {
        *deleted = (archive->GetBlockEntry(table[found].m_blockIndex).m_flags & 0x2000000) != 0;
    }

    return found;
}

// VERIFY: a62931d4 manual
// OFFSET: 0x00454120
bool System_SFile::CheckFileExistsOnDisk(const char* name, uint32_t flags, char* result) {
    char buffer[260];

    if (!(flags & 3)) {
        return false;
    }

    if (!result) {
        result = buffer;
    }

    if (flags & 2) {
        ConvertRelativePathName(Blizzard::String::FindFilename(name), result);

        if (Blizzard::File::Exists(result) == 1) {
            return true;
        }
    }

    if (!(flags & 1)) {
        return false;
    }

    if (Blizzard::File::GetRootChars(name)) {
        Blizzard::String::Copy(result, name, 260);
    } else {
        ConvertRelativePathName(name, result);
    }

    return Blizzard::File::Exists(result) == 1;
}

// VERIFY: 138e567c manual
// OFFSET: 0x0045BED0
void System_SFile::ARCHIVEREC::BuildBlockKeys() {
    if (!this->m_tablesInitialized) {
        this->InitializeTables();
    }

    if (this->m_blockKeysBuilt) {
        return;
    }

    ArchivePtr archive(GetArchivePtr(this, true));

    if (this->m_blockKeys.empty()) {
        for (uint32_t i = 0; i < this->m_header.m_blockTableSize; i++) {
            this->m_blockKeys.insert(std::make_pair(this->m_blockTable[i].m_md5, i));
        }

        this->m_blockKeysBuilt = true;
    }
}

// VERIFY: 520c5718 manual
// OFFSET: 0x0045C880
uint32_t System_SFile::FindBlockEntry(ARCHIVEREC* archive, const Blizzard::Util::Digest<16>& md5, bool* deleted) {
    archive->BuildBlockKeys();

    std::map<Blizzard::Util::Digest<16>, uint32_t>::iterator it = archive->m_blockKeys.find(md5);

    if (it == archive->m_blockKeys.end()) {
        return -1;
    }

    *deleted = (archive->GetBlockEntry(it->second).m_flags & 0x2000000) != 0;

    return it->second;
}

// VERIFY: 962c3879 manual
// OFFSET: 0x0045F7D0
int32_t FindFileInOneArchive(System_SFile::ARCHIVEREC* archive, const char* name, const uint64_t& hash, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::FileLookup* lookup) {
    if (!(flags & 0x10) && (archive->m_externalRefs.m_value <= 0 || archive->unk_0200 > 0)) {
        return 0;
    }

    bool deleted = false;

    if (md5 != System_SFile::s_emptyMd5) {
        lookup->m_blockIndex = System_SFile::FindBlockEntry(archive, md5, &deleted);
    }

    if (name && lookup->m_blockIndex == -1 && md5 == System_SFile::s_emptyMd5) {
        char path[260];
        uint32_t prefix;
        bool prefixed = false;

        if (archive->m_directPath.empty()) {
            prefix = 0;
        } else {
            prefix = Blizzard::String::Copy(path, archive->m_directPath.c_str(), 260);
            Blizzard::String::Copy(&path[prefix], name, 260 - prefix);
            name = path;
            prefixed = true;
        }

        if (!archive->m_file) {
            if (!lookup->m_checkedDisk || prefix) {
                if (System_SFile::CheckFileExistsOnDisk(name, 1, lookup->m_path)) {
                    lookup->m_archive = archive;
                    return 2;
                }

                if (!prefix) {
                    lookup->m_checkedDisk = true;
                }
            }

            return 0;
        }

        if (archive->m_parent) {
            archive = archive->m_parent;
        }

        if (prefix && (flags & 1)) {
            if (System_SFile::CheckFileExistsOnDisk(name, 1, lookup->m_path)) {
                return 3;
            }
        }

        archive->InitializeTables();

        if (archive->m_hashTable.empty()) {
            uint64_t key = prefixed ? System_SFile::HashEntryTable::CalculateHash(name) : hash;
            lookup->m_blockIndex = archive->m_hashEntryTable->GetEntry(key, archive->m_blockEntryTable);

            if (lookup->m_blockIndex == -1) {
                return 0;
            }

            deleted = (archive->GetBlockEntry(lookup->m_blockIndex).m_flags & 0x2000000) != 0;
        } else {
            lookup->m_hashIndex = System_SFile::SearchHashTable(archive, name, true, &deleted);

            if (lookup->m_hashIndex != -1) {
                lookup->m_blockIndex = archive->m_hashTable[lookup->m_hashIndex].m_blockIndex;
            }
        }
    }

    if (deleted) {
        return 4;
    }

    if (lookup->m_blockIndex == -1) {
        return 0;
    }

    lookup->m_archive = archive;

    return 1;
}

// VERIFY: b9bc5aa5 manual
// OFFSET: 0x0045FA10
int32_t GetFileBlockEntry(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut, System_SFile::ARCHIVEREC** recordOut, uint32_t* blockIndex, uint32_t* hashIndex, char* path) {
    if (archiveOut) {
        *archiveOut = nullptr;
    }

    if (recordOut) {
        *recordOut = nullptr;
    }

    if (blockIndex) {
        *blockIndex = 0;
    }

    if (hashIndex) {
        *hashIndex = -1;
    }

    System_SFile::ArchivePtr archive(System_SFile::GetArchivePtr(handle, true));

    if (handle && !archive.m_archive) {
        return 0;
    }

    System_SFile::FileLookup lookup;
    lookup.m_hashIndex = -1;
    lookup.m_blockIndex = -1;
    int32_t result = 0;
    lookup.m_checkedDisk = false;
    lookup.m_archive = nullptr;
    lookup.m_path = path;

    if (flags & 3) {
        lookup.m_checkedDisk = true;

        if (System_SFile::CheckFileExistsOnDisk(name, flags, path)) {
            result = 3;
        }
    }

    if (!(flags & 8)) {
        if (!result) {
            Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
            uint64_t hash = System_SFile::HashEntryTable::CalculateHash(name);

            if (archive.m_archive) {
                result = FindFileInOneArchive(archive.m_archive, name, hash, md5, flags, &lookup);
            } else {
                System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();

                for (System_MPQGlobals::ArchiveList::iterator it = list->begin(); it != list->end(); ++it) {
                    result = FindFileInOneArchive(*it, name, hash, md5, flags, &lookup);

                    if (result) {
                        handle = *it;
                        break;
                    }
                }
            }
        }

        if (result == 1 || result == 2) {
            if (recordOut) {
                lookup.m_archive->m_internalRefs.Increment();
                *recordOut = lookup.m_archive;
            }

            if (archiveOut) {
                *archiveOut = handle;
            }

            if (result == 1) {
                if (hashIndex) {
                    *hashIndex = lookup.m_hashIndex;
                }

                if (blockIndex) {
                    *blockIndex = lookup.m_blockIndex;
                }
            }
        }
    }

    return result;
}

// VERIFY: adbb7274 manual
// OFFSET: 0x00460EA0
int32_t Blizzard::Mopaq::SFileFileExists(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    int32_t result = GetFileBlockEntry(handle, name, md5, flags, archiveOut, nullptr, nullptr, nullptr, nullptr);

    if (result == 4) {
        return 0;
    }

    return result;
}

// VERIFY: c8a17ac8 auto
// OFFSET: 0x00460F00
bool Blizzard::Mopaq::SFileGetFileDetails(System_SFile::ARCHIVEREC* handle, const char* name, System_SFile::ARCHIVEREC** archiveOut, int64_t* offset, uint32_t* size) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    System_SFile::ARCHIVEREC* record = nullptr;
    uint32_t blockIndex;

    if (GetFileBlockEntry(handle, name, md5, 0, nullptr, &record, &blockIndex, nullptr, nullptr) != 1 || !archiveOut) {
        return false;
    }

    System_SFile::BLOCKENTRY entry = record->GetBlockEntry(blockIndex);

    if (offset) {
        *offset = entry.m_offset;
    }

    if (size) {
        *size = entry.m_compressedSize;
    }

    *archiveOut = record;

    return true;
}

// VERIFY: eccb0185 auto
// OFFSET: 0x00461040
bool Blizzard::Mopaq::SFileGetIsLocalAmount(const char* name, int64_t& local, int64_t& total) {
    System_SFile::ARCHIVEREC* archive = nullptr;
    int64_t offset;
    uint32_t size;

    bool result = Blizzard::Mopaq::SFileGetFileDetails(nullptr, name, &archive, &offset, &size);

    if (result) {
        const char* path = archive->m_name;

        if (Blizzard::Streaming::IsStreamedFile(path)) {
            local += Blizzard::Streaming::GetIsLocalAmount(path, offset, size, name);
        } else {
            local += size;
        }

        total += size;

        if (archive) {
            archive->RemoveInternalArchiveRef();
        }
    }

    return result;
}

// VERIFY: 2e816731 manual
// OFFSET: 0x0042C680
void System_SFile::ARCHIVEREC::SetMD5Check(uint32_t index, bool enabled) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if ((this->m_flags & 0x200) && (blocks->m_tableFlags & 0x200)) {
            uint8_t bit = enabled;
            blocks->m_entries->WriteBits(blocks->m_md5Bit + index * blocks->m_entryBits, blocks->m_md5Bits, &bit, 1);
        }
    } else {
        this->m_blockTable[index].m_md5Check = enabled;
    }
}

// VERIFY: 42f50f1a manual
// OFFSET: 0x00431A20
void System_SFile::ARCHIVEREC::SetBlockEntry(uint32_t index, const BLOCKENTRY& entry) {
    this->m_blockTable[index] = entry;
}

// VERIFY: fb240c69 manual
// OFFSET: 0x0042C520
void System_SFile::ARCHIVEREC::SetBlockFileTime(uint32_t index, uint64_t fileTime) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x100) {
            blocks->m_fileTimes[index] = fileTime;
        }
    } else {
        this->m_blockTable[index].m_fileTime = fileTime;
    }
}

// VERIFY: ec838e62 manual
// OFFSET: 0x0042C5C0
void System_SFile::ARCHIVEREC::SetBlockMd5(uint32_t index, const Blizzard::Util::Digest<16>& md5) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x40) {
            blocks->m_md5s[index] = md5;
        }
    } else {
        this->m_blockTable[index].m_md5 = md5;
    }
}

// VERIFY: 4e54de6f manual
// OFFSET: 0x0042C720
void System_SFile::ARCHIVEREC::SetBlockCrc(uint32_t index, uint32_t crc) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x80) {
            blocks->m_crcs[index] = crc;
        }
    } else {
        this->m_blockTable[index].m_crc = crc;
    }
}

// VERIFY: 6163de47 manual
// OFFSET: 0x00431AC0
bool System_SFile::ARCHIVEREC::IncreaseHashBits(int32_t* hashBits) {
    if (*hashBits == 64) {
        return false;
    }

    HashEntryTable* table = this->m_hashEntryTable;

    if (table->m_indices) {
        delete table->m_indices;
        table->m_indices = nullptr;
    }

    memset(table->m_tags, 0, table->m_slotCount);
    this->m_hashEntryTable->SetHashBits(++*hashBits);
    this->m_blockEntryTable->InitializeHashCheckTable(*hashBits);

    return true;
}

// VERIFY: 82bbd725 manual
// OFFSET: 0x00431B50
bool System_SFile::ARCHIVEREC::ReadAdditionalAttributes() {
    Blizzard::Util::Digest<16> md5 = s_emptyMd5;
    IOMpqUnit* file;

    if (!Blizzard::Mopaq::SFileOpenFileEx(this, "(attributes)", md5, 0x10, &file)) {
        return false;
    }

    uint32_t count = this->m_header.m_blockTableSize;
    uint32_t size = Blizzard::Mopaq::SFileGetFileSize(file, nullptr);

    if (size < 8 || size > (count << 9) + 8) {
        file->Release();
        return false;
    }

    HeapArray<uint8_t> data(size);

    if (!file->Read(data.m_data, 0, size, nullptr)) {
        file->Release();
        return false;
    }

    file->Release();

    const uint32_t* header = reinterpret_cast<const uint32_t*>(data.m_data);
    uint32_t flags = header[1];
    uint32_t remaining = size - 8;
    const uint8_t* p = data.m_data + 8;
    uint32_t needed = 0;
    uint32_t unknown = flags;

    if (flags & 1) {
        needed = 4 * count;
        unknown &= ~1u;
    }

    if (flags & 2) {
        needed += 8 * count;
        unknown &= ~2u;
    }

    if (flags & 4) {
        needed += 16 * count;
        unknown &= ~4u;
    }

    if (remaining < needed || (!unknown && remaining != needed)) {
        return false;
    }

    if (flags & 1) {
        for (uint32_t i = 0; i < count; i++, p += 4) {
            this->SetBlockCrc(i, *reinterpret_cast<const uint32_t*>(p));
        }
    }

    if (flags & 2) {
        for (uint32_t i = 0; i < count; i++, p += 8) {
            this->SetBlockFileTime(i, *reinterpret_cast<const uint64_t*>(p));
        }
    }

    if (flags & 4) {
        for (uint32_t i = 0; i < count; i++, p += 16) {
            this->SetBlockMd5(i, *reinterpret_cast<const Blizzard::Util::Digest<16>*>(p));
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        this->SetMD5Check(i, false);
    }

    return true;
}

// VERIFY: a43d10d2 manual
// OFFSET: 0x0045C5C0
bool Blizzard::Mopaq::SFileOpenArchiveView(System_SFile::ARCHIVEREC* handle, const char* name, int32_t priority, System_SFile::ARCHIVEREC** result) {
    *result = nullptr;

    System_SFile::ArchivePtr archive(System_SFile::GetArchivePtr(handle, true));

    if (!archive.m_archive) {
        return false;
    }

    char path[260];

    if (!archive.m_archive->m_file) {
        Blizzard::String::JoinPath(path, sizeof(path), archive.m_archive->m_directPath.c_str(), name);
        return SFileOpenArchive(path, priority, archive.m_archive->m_flags, result);
    }

    if (archive.m_archive->m_parent) {
        Blizzard::String::JoinPath(path, sizeof(path), archive.m_archive->m_directPath.c_str(), name);
        return SFileOpenArchiveView(archive.m_archive->m_parent, path, priority, result);
    }

    System_SFile::ARCHIVEREC* view = NewZerofill<System_SFile::ARCHIVEREC>();
    view->m_priority = priority;
    view->m_id = System_SFile::s_nextArchiveId.Increment() - 1;
    view->m_flags = archive.m_archive->m_flags;
    view->m_externalRefs.Increment();
    view->m_internalRefs.Increment();

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        archive.m_archive->m_internalRefs.Increment();
        System_MPQGlobals::GetArchiveList()->insert(view);
    }

    Blizzard::String::Copy(view->m_name, archive.m_archive->m_name, sizeof(view->m_name));
    archive.m_archive->m_file->m_refCount.Increment();
    view->m_file = archive.m_archive->m_file;
    view->m_parent = handle;
    view->unk_0200 = 0;

    char directory[260];
    Blizzard::String::MakeBackslashPath(name, directory, sizeof(directory));

    if (directory[0]) {
        Blizzard::String::ForceTrailingSeparator(directory, sizeof(directory), '\\');
    }

    view->m_directPath.assign(directory, strlen(directory));
    *result = view;

    return true;
}

// VERIFY: 3b0579b7 manual
// OFFSET: 0x0045C920
void System_SFile::ClearAllFileFlags() {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
    System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();

    for (System_MPQGlobals::ArchiveList::iterator it = list->begin(); it != list->end(); ++it) {
        ARCHIVEREC* archive = *it;

        if (archive->m_parent) {
            continue;
        }

        Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);
        archive->InitializeTables();

        for (uint32_t i = 0; i < archive->m_header.m_blockTableSize; i++) {
            archive->SetMD5Check(i, false);
        }

        Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);
    }
}

// VERIFY: 7a6180d9 auto
// OFFSET: 0x004541B0
void Blizzard::Mopaq::SetMd5Mode(uint32_t mode) {
    System_MPQGlobals::GetMopaqGlobals()->m_md5Mode = mode;
}

// VERIFY: d84644e2 auto
// OFFSET: 0x004541D0
void Blizzard::Mopaq::SetMd5Callback(bool (*callback)(void* context, const uint8_t* md5)) {
    System_MPQGlobals::GetMopaqGlobals()->m_md5Callback = callback;
}

// VERIFY: 7b6e108c manual
// OFFSET: 0x00461180
bool Blizzard::Mopaq::SetFileSignature(const char* name, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Util::Digest<16> empty = System_SFile::s_emptyMd5;
    System_SFile::ARCHIVEREC* record = nullptr;
    uint32_t blockIndex;

    if (GetFileBlockEntry(nullptr, name, empty, 0, nullptr, &record, &blockIndex, nullptr, nullptr) != 1) {
        if (record) {
            record->RemoveInternalArchiveRef();
        }

        return false;
    }

    System_SFile::ArchivePtr archive(System_SFile::GetArchivePtr(record, true));

    if (archive.m_archive) {
        Blizzard::Lock::MutexEnter(archive.m_archive->m_mutex.m_mutex);
    }

    archive.m_locked = true;

    bool result = System_SFile::s_signatures.SetSignature(name, md5);

    if (result) {
        archive.m_archive->SetMD5Check(blockIndex, true);
    }

    if (record) {
        record->RemoveInternalArchiveRef();
    }

    return result;
}
