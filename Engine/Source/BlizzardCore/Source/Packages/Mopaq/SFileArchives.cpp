#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Storm/H/storm.h"

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

// VERIFY: 030c5f2c manual
// OFFSET: 0x00431800
void System_SFile::HashEntryTable::Clear() {
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
