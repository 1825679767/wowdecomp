#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace gx::shader {

// Container decoding only. This does not establish WotLK permutation, register,
// input-layout or rendering compatibility and never creates an API shader.
struct Permutation {
    uint32_t flags = 0;
    uint32_t unknown = 0;
    uint16_t constantBytes = 0;
    uint32_t metadata[4]{};
    std::vector<uint8_t> code;
};

class Container {
public:
    static constexpr size_t MaxFileBytes = 64 * 1024 * 1024;
    static constexpr uint32_t MaxDecodedBytes = 256 * 1024 * 1024;
    static constexpr uint32_t MaxPermutationBytes = 16 * 1024 * 1024;
    static constexpr uint32_t BlockBytes = 0x4000;

    // Owns a copy; failed Open clears previous state. Input may be unaligned.
    bool Open(const uint8_t* data, size_t size, std::string& error);
    bool Extract(uint32_t index, Permutation& output, std::string& error) const;
    uint32_t Count() const { return m_offsets.empty() ? 0 : uint32_t(m_offsets.size() - 1); }
    uint32_t DecodedBytes() const { return m_offsets.empty() ? 0 : m_offsets.back(); }

private:
    std::vector<uint8_t> m_data;
    std::vector<uint32_t> m_offsets;
    std::vector<uint32_t> m_blocks;
    uint32_t m_payload = 0;
};

bool HasDxbcSignature(const Permutation& permutation);

} // namespace gx::shader
