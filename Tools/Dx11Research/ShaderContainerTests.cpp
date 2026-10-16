#include "TestFixtures.h"
#include <iostream>
#include <random>

static uint32_t checks = 0;
static void Check(bool result, const char* label) {
    ++checks; if (!result) throw std::runtime_error(label);
}
int main() try {
    using namespace fixtures;
    gx::shader::Container c; gx::shader::Permutation p; std::string error;
    Bytes code(70000); std::mt19937 rng(0x115533);
    for (auto& v : code) v = uint8_t(rng());
    Bytes small{'D','X','B','C',1,2,3,4};
    const auto first = Record(small, 64), large = Record(code, 4096);
    for (bool raw : {false, true}) {
        auto b = Container({first, large, first}, raw);
        Check(c.Open(b.data(), b.size(), error), "open multi-block fixture");
        Check(c.Count() == 3 && c.DecodedBytes() == first.size() * 2 + large.size(), "counts");
        std::fill(b.begin(), b.end(), 0); // Open must own its bytes.
        Check(c.Extract(1, p, error) && p.code == code && p.constantBytes == 4096, "unaligned cross-block permutation");
        Check(c.Extract(2, p, error) && p.code == small && p.constantBytes == 64, "final partial block");
        Check(gx::shader::HasDxbcSignature(p), "signature classification");
        Check(!c.Extract(3, p, error) && p.code.empty(), "bad index clears output");
    }
    const auto valid = Container({first});
    auto rejectOpen = [&](Bytes b, const char* label) {
        Check(c.Open(valid.data(), valid.size(), error), "reload before rejection");
        Check(!c.Open(b.data(), b.size(), error) && !error.empty() && c.Count() == 0, label);
    };
    Check(!c.Open(nullptr, 100, error), "null input");
    for (size_t n = 0; n < 28; ++n) rejectOpen(Bytes(valid.begin(), valid.begin() + n), "short header");
    for (const auto& mutation : std::vector<std::pair<size_t,uint32_t>>{
        {0,0x48535847},{4,0x10003},{8,0xffffffff},{8,27},{12,0},{12,65537},
        {16,28},{20,0},{20,16385},{24,0},{24,uint32_t(valid.size())},
        {28,1},{32,0xffffffff},{36,1},{40,0xffffffff}}) {
        auto b=valid; Put(b,mutation.first,mutation.second); rejectOpen(b,"invalid table/count/span");
    }
    auto truncated=valid; truncated.pop_back(); rejectOpen(truncated,"truncated payload");
    auto unaligned=valid; unaligned.insert(unaligned.begin(),0);
    Check(c.Open(unaligned.data()+1,unaligned.size()-1,error) && c.Extract(0,p,error) && p.code==small,"unaligned input pointer");
    auto badCode=first; Put(badCode,28,0xffffffff);
    auto b=Container({badCode});
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error) && p.code.empty(),"oversized bytecode rejected");
    badCode=first; Put(badCode,28,0); b=Container({badCode});
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error),"zero bytecode rejected");
    b=Container({{},first});
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error) && c.Extract(1,p,error),"empty permutation stays explicit");
    b=valid; b.back()^=0x40;
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error),"corrupt zlib stream");
    b=valid; b.push_back(0); Put(b,40,Get(b,40)+1);
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error),"trailing compressed bytes");
    b=Container({Record(Bytes(16384-32,17))}); Put(b,32,16383);
    Check(c.Open(b.data(),b.size(),error) && !c.Extract(0,p,error),"decoded length mismatch");
    for (uint32_t i=0;i<1024;++i) {
        b=valid; b[rng()%b.size()]^=uint8_t(1u<<(rng()%8));
        if (c.Open(b.data(),b.size(),error)) {
            if (c.Extract(0,p,error)) Check(!p.code.empty() && p.code.size()<=gx::shader::Container::MaxPermutationBytes,"mutation invariant");
            else Check(p.code.empty() && !error.empty(),"failed mutation clears output");
        } else Check(c.Count()==0 && !error.empty(),"failed mutation clears container");
    }
    std::cout << checks << " checks passed; compressed/raw/cross-block and malformed input covered\n";
    return 0;
} catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return 1; }
