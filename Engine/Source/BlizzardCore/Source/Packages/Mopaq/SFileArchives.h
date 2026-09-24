#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/iounit/IOUnit.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"

namespace System_SFile {

#pragma pack(push, 1)
struct BLOCKENTRY {
    /* 0000 */ uint64_t m_offset;
    /* 0008 */ uint32_t m_compressedSize;
    /* 000C */ uint32_t m_fileSize;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ uint8_t unk_0014[28];
    /* 0030 */ bool m_md5Check;
};
#pragma pack(pop)

class BitPacked {
    public:
    /* 0000 */ uint8_t* m_data;
    /* 0004 */ uint32_t m_bitCount;

    BitPacked(uint32_t bitCount, uint8_t fill);
    ~BitPacked();
    void ReadBits(uint32_t start, uint32_t bits, uint8_t* out, uint32_t outSize) const;
    bool WriteBits(uint32_t start, uint32_t bits, const uint8_t* in, uint32_t inSize);
};

class BlockEntryTable {
    public:
    /* 0000 */ std::vector<uint32_t> m_flagTable;
    /* 0010 */ uint32_t m_entryBits;
    /* 0014 */ uint32_t m_offsetBit;
    /* 0018 */ uint32_t m_fileSizeBit;
    /* 001C */ uint32_t m_compressedSizeBit;
    /* 0020 */ uint32_t m_flagIndexBit;
    /* 0024 */ uint32_t m_md5Bit;
    /* 0028 */ uint32_t m_offsetBits;
    /* 002C */ uint32_t m_fileSizeBits;
    /* 0030 */ uint32_t m_compressedSizeBits;
    /* 0034 */ uint32_t m_flagIndexBits;
    /* 0038 */ uint32_t m_md5Bits;
    /* 003C */ uint32_t m_hashEntryBits;
    /* 0040 */ uint32_t m_hashExtraBits;
    /* 0044 */ int32_t m_hashBits;
    /* 0048 */ BitPacked* m_hashes;
    /* 004C */ BitPacked* m_entries;
    /* 0050 */ uint32_t m_count;
    /* 0054 */ std::vector<uint8_t> unk_0054;
    /* 0064 */ std::vector<uint8_t> unk_0064;
    /* 0074 */ std::vector<uint8_t> unk_0074;
    /* 0084 */ uint32_t m_tableFlags;

    ~BlockEntryTable();
    void InitializeHashCheckTable(int32_t bits);
    bool UnpackEntry(uint32_t index, BLOCKENTRY* entry);
};

class HashEntryTable {
    public:
    static uint64_t CalculateHash(const char* name);

    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_indexBits;
    /* 000C */ uint8_t* m_tags;
    /* 0010 */ BitPacked* m_indices;
    /* 0014 */ uint32_t m_slotCount;
    /* 0018 */ int32_t m_hashBits;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ uint64_t m_hashMask;
    /* 0028 */ uint64_t m_hashTopBit;
    /* 0030 */ uint32_t m_entryBits;

    HashEntryTable(uint32_t count, int32_t hashBits);
    ~HashEntryTable();
    bool AddEntry(const char* name, uint32_t blockIndex, BlockEntryTable* blocks);
    uint32_t GetEntry(const uint64_t& hash, BlockEntryTable* blocks);
    void SetHashBits(int32_t bits);
};

struct HASHENTRY {
    /* 0000 */ uint32_t m_hashA;
    /* 0004 */ uint32_t m_hashB;
    /* 0008 */ uint16_t m_locale;
    /* 000A */ uint16_t m_platform;
    /* 000C */ uint32_t m_blockIndex;
};

struct MPQHEADER {
    /* 0000 */ uint32_t m_magic;
    /* 0004 */ uint32_t m_headerSize;
    /* 0008 */ uint32_t m_archiveSize;
    /* 000C */ uint16_t m_formatVersion;
    /* 000E */ uint16_t m_sectorSizeShift;
    /* 0010 */ uint32_t m_hashTablePos;
    /* 0014 */ uint32_t m_blockTablePos;
    /* 0018 */ uint32_t m_hashTableSize;
    /* 001C */ uint32_t m_blockTableSize;
    /* 0020 */ uint64_t m_hiBlockTablePos;
    /* 0028 */ uint16_t m_hashTablePosHi;
    /* 002A */ uint16_t m_blockTablePosHi;
};

struct BlockKey {
    /* 0000 */ uint32_t m_data[4];

    bool operator<(const BlockKey& other) const;
};

struct ArchiveBuffers {
    /* 0000 */ Blizzard::Memory::TempBuffer<uint8_t, 4> unk_0000;
    /* 000C */ Blizzard::Memory::TempBuffer<uint8_t, 256> unk_000C;
    /* 0114 */ Blizzard::Memory::TempBuffer<uint8_t, 256> unk_0114;
    /* 021C */ std::string unk_021C;

    ~ArchiveBuffers();
};

class ARCHIVEREC {
    public:
    /* 0000 */ Blizzard::Lock::RMutex m_mutex;
    /* 0018 */ IOUnit* m_file;
    /* 001C */ int32_t m_priority;
    /* 0020 */ int32_t m_id;
    /* 0024 */ ARCHIVEREC* m_parent;
    /* 0028 */ MPQHEADER m_header;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ int64_t m_archiveOffset;
    /* 0060 */ int64_t m_archiveEnd;
    /* 0068 */ int64_t m_userDataOffset;
    /* 0070 */ uint32_t m_userDataSize;
    /* 0074 */ uint32_t m_flags;
    /* 0078 */ uint32_t m_sectorSize;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ char m_name[260];
    /* 0188 */ std::string m_directPath;
    /* 01A4 */ BlockEntryTable* m_blockEntryTable;
    /* 01A8 */ HashEntryTable* m_hashEntryTable;
    /* 01AC */ std::vector<BLOCKENTRY> m_blockTable;
    /* 01BC */ std::vector<HASHENTRY> m_hashTable;
    /* 01CC */ std::map<BlockKey, uint32_t> m_blockKeys;
    /* 01D8 */ bool m_blockKeysBuilt;
    /* 01D9 */ bool m_tablesInitialized;
    /* 01DA */ uint8_t unk_01DA[6];
    /* 01E0 */ uint32_t unk_01E0;
    /* 01E4 */ std::vector<uint16_t> unk_01E4;
    /* 01F4 */ uint32_t unk_01F4;
    /* 01F8 */ ArchiveBuffers* m_buffers;
    /* 01FC */ bool (*m_closeCallback)(ARCHIVEREC* archive);
    /* 0200 */ uint32_t unk_0200;
    /* 0204 */ std::map<std::string, uint32_t> unk_0204;
    /* 0210 */ bool unk_0210;
    /* 0211 */ uint8_t unk_0211[3];
    /* 0214 */ uint32_t unk_0214;
    /* 0218 */ uint32_t unk_0218;
    /* 021C */ uint32_t unk_021C;
    /* 0220 */ Blizzard::Lock::Atomic m_internalRefs;
    /* 0224 */ Blizzard::Lock::Atomic m_externalRefs;

    ARCHIVEREC();
    ~ARCHIVEREC();
    void InitializeTables();
    bool RemoveInternalArchiveRef();
};

extern Blizzard::Lock::Atomic s_nextArchiveId;
extern uint32_t s_lastError;

void ConvertRelativePathName(const char* path, char* result);
ARCHIVEREC* GetArchivePtr(ARCHIVEREC* handle, bool addRef);
bool OpenDirectPathAsArchive(const char* path, int32_t priority, uint32_t flags, ARCHIVEREC** result);
bool s_OpenArchive(ARCHIVEREC* archive, uint32_t flags, ARCHIVEREC** result);

}

namespace Blizzard {
namespace Mopaq {

bool SFileCloseArchive(System_SFile::ARCHIVEREC* handle);
bool SFileEnableDirectAccess(uint32_t enable);
uint32_t SFileGetLastError();
bool SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result);
bool SFileOpenArchive(IOUnit* file, const char* name, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result);
bool SFileSetBasePath(const char* path);

}
}

#endif
