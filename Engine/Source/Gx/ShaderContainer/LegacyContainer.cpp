#include "LegacyContainer.h"
#include <new>
namespace gx::shader {
namespace {
uint32_t Read(const uint8_t* p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
bool Fail(std::string& e,const char* message){e=message;return false;}
}
bool LegacyContainer::Open(const uint8_t* data,size_t size,std::string& error){
    m_permutations.clear();error.clear();
    if(!data||size<12||size>MaxFileBytes)return Fail(error,"legacy container size outside bounds");
    if(Read(data)!=0x47585348||Read(data+4)!=0x10003)return Fail(error,"requires native GXSH version 0x10003");
    const uint32_t count=Read(data+8);if(!count||count>65536||count>(size-12)/16)return Fail(error,"legacy permutation count outside bounds");
    try{
        std::vector<LegacyPermutation> candidate;candidate.reserve(count);size_t at=12;
        for(uint32_t i=0;i<count;++i){
            if(at>size||size-at<16)return Fail(error,"truncated legacy permutation header");
            const uint32_t codeSize=Read(data+at+12);
            if(codeSize>MaxCodeBytes||codeSize>size-at-16)return Fail(error,"legacy bytecode span outside bounds");
            const size_t padded=(size_t(codeSize)+3)&~size_t(3);
            if(padded>size-at-16)return Fail(error,"truncated legacy bytecode padding");
            LegacyPermutation row;row.flags=Read(data+at);row.unknown=Read(data+at+4);
            row.wordA=uint16_t(data[at+8])|uint16_t(data[at+9])<<8;row.wordB=uint16_t(data[at+10])|uint16_t(data[at+11])<<8;
            row.code.assign(data+at+16,data+at+16+codeSize);candidate.push_back(std::move(row));at+=16+padded;
        }
        if(at!=size)return Fail(error,"trailing legacy container data");
        m_permutations.swap(candidate);return true;
    }catch(const std::bad_alloc&){return Fail(error,"legacy container allocation failed");}
}
}
