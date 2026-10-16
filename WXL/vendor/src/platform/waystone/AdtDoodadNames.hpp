// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtChunkCompatibility.hpp"
#include <string>
#include <unordered_map>
namespace wxl::waystone::adtcompat {
struct DoodadStats {uint32_t entries{},resolved{},unresolved{},referenceChunks{};};
using ResolveModel=const char*(*)(uint32_t);
inline bool PrepareDoodadNames(const std::vector<uint8_t>& raw,ResolveModel resolve,std::vector<uint8_t>& output,DoodadStats& stats){
 if(&raw==&output || !resolve || raw.size()<8 || raw.size()>16*1024*1024)return false;
 struct Part{uint32_t tag,n;size_t at;};std::vector<Part> parts;std::vector<uint8_t> names,ids,entries;bool haveEntries=false,haveNames=false,haveIds=false;size_t at=0;DoodadStats next;
 while(at<raw.size()){if(raw.size()-at<8)return false;const auto tag=Word(raw.data()+at),n=Word(raw.data()+at+4);if(n>raw.size()-at-8)return false;parts.push_back({tag,n,at});
  if(tag==0x4d444446){if(haveEntries || n%36 || n/36>65535)return false;haveEntries=true;entries.assign(raw.begin()+at+8,raw.begin()+at+8+n);next.entries=n/36;}
  if(tag==0x4d4d4458){if(haveNames)return false;haveNames=true;names.assign(raw.begin()+at+8,raw.begin()+at+8+n);}
  if(tag==0x4d4d4944){if(haveIds || n%4 || n/4>65535)return false;haveIds=true;ids.assign(raw.begin()+at+8,raw.begin()+at+8+n);}
  // Map-object FDID/reference ownership is a separate adapter. Refuse it here.
  if(tag==0x4d4f4446 && n)return false;at+=8+n;
 }
 if(!haveEntries)return false;if(!names.empty() && names.back()!=0)return false;
 for(size_t i=0;i<ids.size();i+=4){const auto offset=Word(ids.data()+i);if(offset>=names.size())return false;size_t end=offset;while(end<names.size() && names[end])++end;if(end==names.size() || end-offset>=1024)return false;}
 std::unordered_map<uint32_t,uint32_t> interned;uint32_t unresolved=UINT32_MAX;
 auto intern=[&](const std::string& path){if(ids.size()/4>=65535 || path.size()+1>16u*1024u*1024u-names.size())return UINT32_MAX;const auto id=uint32_t(ids.size()/4),offset=uint32_t(names.size());ids.resize(ids.size()+4);Put(ids.data()+ids.size()-4,offset);names.insert(names.end(),path.begin(),path.end());names.push_back(0);return id;};
 for(unsigned i=0;i<next.entries;++i){auto* row=entries.data()+i*36;uint16_t flags{};std::memcpy(&flags,row+34,2);const auto value=Word(row);
  if(!(flags&0x40)){if(value>=ids.size()/4)return false;continue;}
  auto found=interned.find(value);uint32_t index=0;
  if(found!=interned.end())index=found->second;else{const char* path=resolve(value);std::string owned;
   if(path && path[0]){size_t length=0;while(length<1024 && path[length])++length;if(length>=1024)return false;owned.assign(path,length);for(auto& c:owned)if(c=='/')c=char(92);if(owned.find("..")!=std::string::npos || owned.find(':')!=std::string::npos || owned.front()==char(92) || (!owned.ends_with(".m2") && !owned.ends_with(".mdx")))return false;index=intern(owned);++next.resolved;}
   else{if(unresolved==UINT32_MAX)unresolved=intern("");index=unresolved;++next.unresolved;}interned.emplace(value,index);
  }if(index==UINT32_MAX)return false;Put(row,index);flags=uint16_t(flags&~0x40u);std::memcpy(row+34,&flags,2);
 }
 // Per-chunk indices retain MDDF entry order. Never shift placement records.
 for(const auto& part:parts)if(part.tag==0x4d434e4b){size_t pos=part.at+8,end=pos+part.n;while(pos<end){if(end-pos<8)return false;const auto tag=Word(raw.data()+pos),n=Word(raw.data()+pos+4);if(n>end-pos-8)return false;if(tag==0x4d435244){if(n%4 || n/4>65535)return false;for(size_t i=0;i<n;i+=4)if(Word(raw.data()+pos+8+i)>=next.entries)return false;++next.referenceChunks;}if(tag==0x4d435257 && n)return false;pos+=8+n;}}
 std::vector<uint8_t> prepared;auto append=[&](uint32_t tag,const std::vector<uint8_t>& bytes){auto at=prepared.size();prepared.resize(at+8+bytes.size());Put(prepared.data()+at,tag);Put(prepared.data()+at+4,uint32_t(bytes.size()));if(!bytes.empty())std::memcpy(prepared.data()+at+8,bytes.data(),bytes.size());};
 for(const auto& part:parts){if(part.tag==0x4d4d4458 || part.tag==0x4d4d4944)continue;if(part.tag==0x4d444446)append(part.tag,entries);else prepared.insert(prepared.end(),raw.begin()+part.at,raw.begin()+part.at+8+part.n);}
 append(0x4d4d4458,names);append(0x4d4d4944,ids);if(prepared.size()>16u*1024u*1024u)return false;output.swap(prepared);stats=next;return true;
}
}
