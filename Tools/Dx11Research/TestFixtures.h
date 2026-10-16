#pragma once
#include "Engine/Source/Gx/ShaderContainer/ShaderContainer.h"
#include "ThirdParty/zlib/zlib.h"
#include <algorithm>
#include <stdexcept>

namespace fixtures {
using Bytes = std::vector<uint8_t>;
inline void Put(Bytes& b, size_t o, uint32_t v) {
    for (size_t i = 0; i < 4; ++i) b.at(o + i) = uint8_t(v >> (8 * i));
}
inline uint32_t Get(const Bytes& b, size_t o) {
    uint32_t v = 0; for (size_t i = 0; i < 4; ++i) v |= uint32_t(b.at(o + i)) << (8 * i); return v;
}
inline Bytes Record(const Bytes& code, uint16_t constants = 0) {
    Bytes r(32); Put(r, 8, constants); Put(r, 28, uint32_t(code.size()));
    r.insert(r.end(), code.begin(), code.end()); return r;
}
// Synthetic reference-format assets, never substituted for mounted game shaders.
inline Bytes Container(const std::vector<Bytes>& records, bool rawFullBlocks = false) {
    Bytes decoded;
    std::vector<uint32_t> offsets{0}, blocks{0};
    for (const auto& r : records) { decoded.insert(decoded.end(), r.begin(), r.end()); offsets.push_back(uint32_t(decoded.size())); }
    Bytes payload;
    for (size_t pos = 0; pos < decoded.size(); pos += gx::shader::Container::BlockBytes) {
        const size_t n = std::min(size_t(gx::shader::Container::BlockBytes), decoded.size() - pos);
        Bytes encoded;
        if (rawFullBlocks && n == gx::shader::Container::BlockBytes) encoded.assign(decoded.begin() + pos, decoded.begin() + pos + n);
        else {
            uLongf capacity = compressBound(uLong(n)); encoded.resize(capacity);
            if (compress2(encoded.data(), &capacity, decoded.data() + pos, uLong(n), 9) != Z_OK) throw std::runtime_error("fixture compression failed");
            encoded.resize(capacity);
            if (capacity == gx::shader::Container::BlockBytes) throw std::runtime_error("ambiguous compressed fixture");
        }
        payload.insert(payload.end(), encoded.begin(), encoded.end()); blocks.push_back(uint32_t(payload.size()));
    }
    const uint32_t po = 28, bo = po + uint32_t(offsets.size()) * 4, payloadOffset = bo + uint32_t(blocks.size()) * 4;
    Bytes result(payloadOffset);
    Put(result, 0, 0x47585348); Put(result, 4, 0x10004); Put(result, 8, po);
    Put(result, 12, uint32_t(records.size())); Put(result, 16, bo);
    Put(result, 20, uint32_t(blocks.size() - 1)); Put(result, 24, payloadOffset);
    for (size_t i = 0; i < offsets.size(); ++i) Put(result, po + i * 4, offsets[i]);
    for (size_t i = 0; i < blocks.size(); ++i) Put(result, bo + i * 4, blocks[i]);
    result.insert(result.end(), payload.begin(), payload.end()); return result;
}
}
