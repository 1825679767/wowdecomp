#include "MaterialCompat.hpp"
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeFeatures.hpp"
#include "Engine/Source/Model2/M2Data.h"
#include <cstring>
#include <cmath>
namespace wxl::native {
namespace {
constexpr uint32_t Limit=64u*1024*1024;
template<class T>T Read(const uint8_t* p,size_t at){T v;std::memcpy(&v,p+at,sizeof(v));return v;}
}
void PreserveMaterials(std::span<const uint8_t> raw,std::vector<uint8_t>& native){
 if(!MaterialsEnabled() || raw.size()<8 || native.size()<304)return;
 const uint32_t bodyBytes=Read<uint32_t>(raw.data(),4);if(bodyBytes<304 || bodyBytes>raw.size()-8)return;
 const auto* body=raw.data()+8;const auto count=Read<uint32_t>(body,0x70),at=Read<uint32_t>(body,0x74),target=Read<uint32_t>(native.data(),0x74);
 if(count!=Read<uint32_t>(native.data(),0x70) || at>bodyBytes || uint64_t(count)*4>bodyBytes-at || target>native.size() || uint64_t(count)*4>native.size()-target)return;
 for(uint32_t i=0;i<count;++i)if(Read<uint16_t>(body,at+i*4+2)==7){const uint16_t flags=Read<uint16_t>(body,at+i*4)&31,blend=7;
  std::memcpy(native.data()+target+i*4,&flags,2);std::memcpy(native.data()+target+i*4+2,&blend,2);
 }
}
}
