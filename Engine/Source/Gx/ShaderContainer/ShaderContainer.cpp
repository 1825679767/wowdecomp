#include "ShaderContainer.h"
#include "ThirdParty/zlib/zlib.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <new>

namespace gx::shader {
namespace {
uint32_t U32(const uint8_t* p) {
    return uint32_t(p[0]) | uint32_t(p[1]) << 8 | uint32_t(p[2]) << 16 | uint32_t(p[3]) << 24;
}
uint16_t U16(const uint8_t* p) { return uint16_t(p[0]) | uint16_t(p[1]) << 8; }
bool Fail(std::string& error, const char* message) { error = message; return false; }
bool Overlap(size_t a, size_t an, size_t b, size_t bn) { return a < b + bn && b < a + an; }
}

bool Container::Open(const uint8_t* data, size_t size, std::string& error) {
    m_data.clear(); m_offsets.clear(); m_blocks.clear(); m_payload = 0;
    error.clear();
    if (!data || size < 28 || size > MaxFileBytes)
        return Fail(error, "container size outside supported bounds");
    // MSVC's reference multi-character literal 'GXSH' is 0x47585348:
    // the actual little-endian bytes on disk are H S X G, not G X S H.
    if (U32(data) != 0x47585348 || U32(data + 4) != 0x10004)
        return Fail(error, "requires GXSH version 0x10004; legacy 0x10003 is a separate format");
    const uint32_t po = U32(data + 8), pc = U32(data + 12);
    const uint32_t bo = U32(data + 16), bc = U32(data + 20), payload = U32(data + 24);
    if (!pc || pc > 65536 || !bc || bc > MaxDecodedBytes / BlockBytes)
        return Fail(error, "permutation or block count outside supported bounds");
    const size_t pn = (size_t(pc) + 1) * 4, bn = (size_t(bc) + 1) * 4;
    if (po < 28 || bo < 28 || payload < 28 || po > size || pn > size - po ||
        bo > size || bn > size - bo || payload >= size)
        return Fail(error, "offset table or payload outside file");
    if (Overlap(po, pn, bo, bn) || Overlap(po, pn, payload, size - payload) ||
        Overlap(bo, bn, payload, size - payload))
        return Fail(error, "overlapping offset tables or payload");
    if (U32(data + po) || U32(data + bo))
        return Fail(error, "nonzero initial logical or compressed offset");
    const uint32_t total = U32(data + po + size_t(pc) * 4);
    if (!total || total > MaxDecodedBytes || (uint64_t(total) + BlockBytes - 1) / BlockBytes != bc)
        return Fail(error, "decoded length does not match block count");
    for (uint32_t i = 0; i < pc; ++i) {
        const uint32_t a = U32(data + po + size_t(i) * 4), b = U32(data + po + (size_t(i) + 1) * 4);
        if (a > b || b > total || b - a > MaxPermutationBytes || (a != b && b - a < 32))
            return Fail(error, "invalid permutation span");
    }
    for (uint32_t i = 0; i < bc; ++i) {
        const uint32_t a = U32(data + bo + size_t(i) * 4), b = U32(data + bo + (size_t(i) + 1) * 4);
        if (a >= b || b > size - payload)
            return Fail(error, "invalid compressed block span");
    }
    try {
        std::vector<uint8_t> owned(data, data + size);
        std::vector<uint32_t> offsets(pc + 1), blocks(bc + 1);
        for (uint32_t i = 0; i <= pc; ++i) offsets[i] = U32(data + po + size_t(i) * 4);
        for (uint32_t i = 0; i <= bc; ++i) blocks[i] = U32(data + bo + size_t(i) * 4);
        m_data.swap(owned); m_offsets.swap(offsets); m_blocks.swap(blocks); m_payload = payload;
    } catch (const std::bad_alloc&) {
        return Fail(error, "container allocation failed");
    }
    return true;
}

bool Container::Extract(uint32_t index, Permutation& output, std::string& error) const {
    output = {}; error.clear();
    if (index >= Count()) return Fail(error, "permutation index outside container");
    const uint32_t start = m_offsets[index], end = m_offsets[index + 1];
    if (end == start) return Fail(error, "empty permutation");
    try {
        std::vector<uint8_t> record(end - start);
        std::array<uint8_t, BlockBytes> block{};
        uint32_t cursor = start;
        while (cursor < end) {
            const uint32_t bi = cursor / BlockBytes, offset = cursor % BlockBytes;
            const uint32_t expected = std::min(BlockBytes, DecodedBytes() - bi * BlockBytes);
            const uint32_t encoded = m_blocks[bi + 1] - m_blocks[bi];
            const uint8_t* src = m_data.data() + m_payload + m_blocks[bi];
            if (encoded == BlockBytes) {
                // Reference v4 convention: only a full 16 KiB block is raw.
                if (expected != BlockBytes) return Fail(error, "raw partial block is unsupported");
                std::memcpy(block.data(), src, BlockBytes);
            } else {
                z_stream stream{};
                stream.next_in = const_cast<Bytef*>(src); stream.avail_in = encoded;
                stream.next_out = block.data(); stream.avail_out = BlockBytes;
                if (inflateInit(&stream) != Z_OK) return Fail(error, "zlib initialization failed");
                const int status = inflate(&stream, Z_FINISH);
                const bool valid = status == Z_STREAM_END && stream.total_out == expected && stream.total_in == encoded;
                inflateEnd(&stream);
                if (!valid) return Fail(error, "compressed block is corrupt, trailing, truncated or wrong length");
            }
            const uint32_t n = std::min(end - cursor, expected - offset);
            std::memcpy(record.data() + cursor - start, block.data() + offset, n);
            cursor += n;
        }
        const uint32_t codeBytes = U32(record.data() + 28);
        if (!codeBytes || codeBytes > record.size() - 32)
            return Fail(error, "bytecode length outside permutation");
        Permutation candidate;
        candidate.flags = U32(record.data()); candidate.unknown = U32(record.data() + 4);
        candidate.constantBytes = U16(record.data() + 8);
        for (uint32_t i = 0; i < 4; ++i) candidate.metadata[i] = U32(record.data() + 12 + i * 4);
        candidate.code.assign(record.begin() + 32, record.begin() + 32 + codeBytes);
        output = std::move(candidate);
    } catch (const std::bad_alloc&) {
        return Fail(error, "permutation allocation failed");
    }
    return true;
}

bool HasDxbcSignature(const Permutation& p) {
    return p.code.size() >= 4 && std::memcmp(p.code.data(), "DXBC", 4) == 0;
}
} // namespace gx::shader
