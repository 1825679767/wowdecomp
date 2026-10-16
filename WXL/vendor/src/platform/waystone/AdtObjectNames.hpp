// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtWmoPlacementNames.hpp"
namespace wxl::waystone::adtcompat {
struct ObjectNameStats {DoodadStats doodads;WmoPlacementStats wmos;};
namespace objectdetail {
struct Part {uint32_t tag,n;size_t at;};
inline bool Parts(const std::vector<uint8_t>& bytes,std::vector<Part>& parts){
 size_t pos=0;while(pos<bytes.size()){if(parts.size()>=65536 || bytes.size()-pos<8)return false;auto tag=Word(bytes.data()+pos),n=Word(bytes.data()+pos+4);if(n>bytes.size()-pos-8)return false;parts.push_back({tag,n,pos});pos+=8+n;}return true;
}
inline bool Append(std::vector<uint8_t>& out,uint32_t tag,const uint8_t* bytes,size_t n){
 constexpr size_t limit=16u*1024u*1024u;if(out.size()>limit-8 || n>limit-out.size()-8)return false;auto pos=out.size();out.resize(pos+8+n);Put(out.data()+pos,tag);Put(out.data()+pos+4,uint32_t(n));if(n)std::memcpy(out.data()+pos+8,bytes,n);return true;
}
}
// Compose the existing strict adapters. The standalone doodad API still refuses WMO input.
// Isolate only doodad chunks for its validation, then merge back into validated WMO bytes.
inline bool PrepareObjectNames(const std::vector<uint8_t>& raw,ResolveModel resolve,std::vector<uint8_t>& output,ObjectNameStats& stats){
 if(&raw==&output)return false;std::vector<uint8_t> wmo;ObjectNameStats next;
 if(!PrepareWmoPlacementNames(raw,resolve,wmo,next.wmos))return false;
 std::vector<objectdetail::Part> parts;if(!objectdetail::Parts(wmo,parts))return false;
 std::vector<uint8_t> isolated;
 for(const auto& part:parts){if(part.tag==0x4d4f4446)continue;
  if(part.tag!=0x4d434e4b){if(!objectdetail::Append(isolated,part.tag,wmo.data()+part.at+8,part.n))return false;continue;}
  std::vector<uint8_t> chunk(wmo.begin()+part.at+8,wmo.begin()+part.at+8+part.n),kept;std::vector<objectdetail::Part> nested;if(!objectdetail::Parts(chunk,nested))return false;
  for(const auto& p:nested)if(p.tag!=0x4d435257 && !objectdetail::Append(kept,p.tag,chunk.data()+p.at+8,p.n))return false;
  if(!objectdetail::Append(isolated,part.tag,kept.data(),kept.size()))return false;
 }
 std::vector<uint8_t> doodad;if(!PrepareDoodadNames(isolated,resolve,doodad,next.doodads))return false;
 std::vector<objectdetail::Part> converted;if(!objectdetail::Parts(doodad,converted))return false;
 const objectdetail::Part *entries=nullptr,*names=nullptr,*ids=nullptr;
 for(const auto& part:converted){if(part.tag==0x4d444446)entries=&part;else if(part.tag==0x4d4d4458)names=&part;else if(part.tag==0x4d4d4944)ids=&part;}
 if(!entries || !names || !ids)return false;std::vector<uint8_t> prepared;
 for(const auto& part:parts){if(part.tag==0x4d4d4458 || part.tag==0x4d4d4944)continue;
  if(part.tag==0x4d444446){if(!objectdetail::Append(prepared,part.tag,doodad.data()+entries->at+8,entries->n))return false;}
  else if(!objectdetail::Append(prepared,part.tag,wmo.data()+part.at+8,part.n))return false;
 }
 if(!objectdetail::Append(prepared,names->tag,doodad.data()+names->at+8,names->n) || !objectdetail::Append(prepared,ids->tag,doodad.data()+ids->at+8,ids->n))return false;
 output.swap(prepared);stats=next;return true;
}
}
