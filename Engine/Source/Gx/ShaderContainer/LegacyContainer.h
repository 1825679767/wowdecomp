#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace gx::shader {
struct LegacyPermutation {
    uint32_t flags=0,unknown=0;
    uint16_t wordA=0,wordB=0;
    std::vector<uint8_t> code;
};
class LegacyContainer {
public:
    static constexpr size_t MaxFileBytes=32*1024*1024;
    static constexpr uint32_t MaxCodeBytes=1024*1024;
    bool Open(const uint8_t* data,size_t size,std::string& error);
    const std::vector<LegacyPermutation>& Permutations() const{return m_permutations;}
private:
    std::vector<LegacyPermutation> m_permutations;
};
}
