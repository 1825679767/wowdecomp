#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H

#include <cstddef>
#include <cstdint>
#include <vector>

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
    bool AddEntry(const char* name, uint32_t blockIndex, BlockEntryTable* blocks);
    void Clear();
    uint32_t GetEntry(const uint64_t& hash, BlockEntryTable* blocks);
    void SetHashBits(int32_t bits);
};

class ARCHIVEREC {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint8_t unk_003C[8];
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint8_t unk_0048[4];
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[36];
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint8_t unk_0078[12];
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ uint8_t unk_0088[284];
    /* 01A4 */ uint32_t unk_01A4;
    /* 01A8 */ uint32_t unk_01A8;
    /* 01AC */ uint8_t unk_01AC[4];
    /* 01B0 */ uint32_t unk_01B0;
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ uint8_t unk_01B8[8];
    /* 01C0 */ uint32_t unk_01C0;
    /* 01C4 */ uint32_t unk_01C4;
};

}

#endif
