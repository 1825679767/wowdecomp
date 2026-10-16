// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtDoodadNames.hpp"
namespace wxl::waystone::adtcompat {
struct WmoPlacementStats {uint32_t entries{},resolved{},suppressed{},removedReferences{};};
// Build4 CreateMapObjDefs scans every MODF entry, independently of MCRW.
// Dead entries must carry native skip bit1 as well as losing chunk references.
inline bool PrepareWmoPlacementNames(const std::vector<uint8_t>& raw,ResolveModel resolve,std::vector<uint8_t>& output,WmoPlacementStats& stats){
 if(&raw==&output || !resolve || raw.size()<8 || raw.size()>16*1024*1024)return false;
 struct Part{uint32_t tag,n;size_t at;};std::vector<Part> parts;std::vector<uint8_t> names,ids,entries;bool haveEntries=false,haveNames=false,haveIds=false;size_t at=0;WmoPlacementStats next;
 while(at<raw.size()){if(raw.size()-at<8)return false;const auto tag=Word(raw.data()+at),n=Word(raw.data()+at+4);if(n>raw.size()-at-8)return false;parts.push_back({tag,n,at});
  if(tag==0x4d4f4446){if(haveEntries || n%64 || n/64>65535)return false;haveEntries=true;entries.assign(raw.begin()+at+8,raw.begin()+at+8+n);next.entries=n/64;}
  if(tag==0x4d574d4f){if(haveNames)return false;haveNames=true;names.assign(raw.begin()+at+8,raw.begin()+at+8+n);}
  if(tag==0x4d574944){if(haveIds || n%4 || n/4>65535)return false;haveIds=true;ids.assign(raw.begin()+at+8,raw.begin()+at+8+n);}at+=8+n;
 }
 if(!haveEntries || (!names.empty() && names.back()!=0))return false;
 for(size_t i=0;i<ids.size();i+=4){const auto offset=Word(ids.data()+i);if(offset>=names.size())return false;size_t end=offset;while(end<names.size() && names[end])++end;if(end==names.size() || end-offset>=1024)return false;}
 std::unordered_map<uint32_t,uint32_t> interned;std::vector<uint8_t> dead(next.entries,0);
 for(unsigned i=0;i<next.entries;++i){auto* row=entries.data()+i*64;uint16_t flags{};std::memcpy(&flags,row+56,2);const auto value=Word(row);if(flags&1){dead[i]=1;++next.suppressed;continue;}if(!(flags&8)){if(value>=ids.size()/4)return false;continue;}
  auto found=interned.find(value);uint32_t index=UINT32_MAX;if(found!=interned.end())index=found->second;else{const char* path=resolve(value);
   if(path && path[0]){size_t length=0;while(length<1024 && path[length])++length;if(length>=1024)return false;std::string owned(path,length);for(auto& c:owned)if(c=='/')c=char(92);if(owned.find("..")!=std::string::npos || owned.find(':')!=std::string::npos || !owned.starts_with(std::string("world")+char(92)+"wmo"+char(92)) || !owned.ends_with(".wmo") || ids.size()/4>=65535 || owned.size()+1>16u*1024u*1024u-names.size())return false;index=uint32_t(ids.size()/4);auto offset=uint32_t(names.size());ids.resize(ids.size()+4);Put(ids.data()+ids.size()-4,offset);names.insert(names.end(),owned.begin(),owned.end());names.push_back(0);++next.resolved;}interned.emplace(value,index);
  }
  if(index==UINT32_MAX){dead[i]=1;++next.suppressed;flags=uint16_t((flags&~8u)|1u);}else{Put(row,index);flags=uint16_t(flags&~8u);}std::memcpy(row+56,&flags,2);
 }
 std::vector<uint8_t> prepared;auto append=[&](uint32_t tag,const std::vector<uint8_t>& bytes){auto at=prepared.size();prepared.resize(at+8+bytes.size());Put(prepared.data()+at,tag);Put(prepared.data()+at+4,uint32_t(bytes.size()));if(!bytes.empty())std::memcpy(prepared.data()+at+8,bytes.data(),bytes.size());};
 for(const auto& part:parts){if(part.tag==0x4d574d4f || part.tag==0x4d574944)continue;if(part.tag==0x4d4f4446){append(part.tag,entries);continue;}
  if(part.tag==0x4d434e4b){std::vector<uint8_t> chunk;size_t pos=part.at+8,end=pos+part.n;bool refs=false;while(pos<end){if(end-pos<8)return false;const auto tag=Word(raw.data()+pos),n=Word(raw.data()+pos+4);if(n>end-pos-8)return false;
    if(tag==0x4d435257){if(refs || n%4 || n/4>65535)return false;refs=true;std::vector<uint8_t> kept;for(size_t i=0;i<n;i+=4){const auto index=Word(raw.data()+pos+8+i);if(index>=dead.size())return false;if(dead[index]){++next.removedReferences;continue;}const auto at=kept.size();kept.resize(at+4);Put(kept.data()+at,index);}auto at=chunk.size();chunk.resize(at+8+kept.size());Put(chunk.data()+at,tag);Put(chunk.data()+at+4,uint32_t(kept.size()));if(!kept.empty())std::memcpy(chunk.data()+at+8,kept.data(),kept.size());}
    else chunk.insert(chunk.end(),raw.begin()+pos,raw.begin()+pos+8+n);pos+=8+n;
   }append(part.tag,chunk);
  }else prepared.insert(prepared.end(),raw.begin()+part.at,raw.begin()+part.at+8+part.n);
 }
 append(0x4d574d4f,names);append(0x4d574944,ids);if(prepared.size()>16u*1024u*1024u)return false;output.swap(prepared);stats=next;return true;
}
}
