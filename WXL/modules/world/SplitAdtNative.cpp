// Native split terrain bridge. Reuses WXL portable source parsers; native rendering remains owner.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "WorldCompat.hpp"
#include "WoW/Source/WorldClient/MapChunk.h"
#include <cmath>
#include "NativeFeatures.hpp"
#include "NativeStorage.hpp"
#include "FdidCompat.hpp"
#include "WorldBytes.hpp"
#include "platform/waystone/AdtPreparedByteFamily.hpp"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "common/Log.hpp"
#include <array>
#include <set>
#include <string>

namespace wxl::native::world {
namespace {
namespace a = wxl::waystone::adtcompat;
using namespace bytes;
bool NativeLiquid(const Blob& b) {
 if(b.empty())return true;if(b.size()<256*12)return false;
 constexpr uint32_t stride[]{5,8,1,9};
 for(unsigned i=0;i<256;++i){const auto* h=b.data()+i*12;const auto at=Read(h),count=Read(h+4),attributes=Read(h+8);
  if(count>16 || (count && (at>b.size() || count*24>b.size()-at)) || (attributes && (attributes>b.size() || 16>b.size()-attributes)))return false;
  for(unsigned n=0;n<count;++n){const auto* p=b.data()+at+n*24;uint16_t type,format;std::memcpy(&type,p,2);std::memcpy(&format,p+2,2);
   if(format>3 || !g_liquidTypeDB.GetRecord(type) || p[12]>8 || p[13]>8 || p[14]>8-p[12] || p[15]>8-p[13])return false;
   const auto exists=Read(p+16),vertices=Read(p+20);const auto mask=(uint32_t(p[14])*p[15]+7)/8;
   const auto need=(uint32_t(p[14])+1)*(uint32_t(p[15])+1)*stride[format];
   if((exists && (exists>b.size() || mask>b.size()-exists)) || (vertices && (vertices>b.size() || need>b.size()-vertices)) || (!vertices && format!=2))return false;
  }
 }return true;
}
bool Assemble(const a::PreparedByteFamily& family,Blob& output) {
 std::array<std::vector<Part>,3> indexed;std::array<std::vector<Part>,3> chunks;
 std::map<uint32_t,std::pair<unsigned,Part>> top;
 for(unsigned s=0;s<3;++s){if(!Walk(family.bytes[s],0,family.bytes[s].size(),indexed[s]))return false;
  for(const auto& p:indexed[s]){if(p.tag==Tag("MCNK")){chunks[s].push_back(p);continue;}
   if(p.tag==Tag("MVER") || p.tag==Tag("MHDR") || p.tag==Tag("MCIN"))continue;
   if(!top.emplace(p.tag,std::pair{s,p}).second)return false;
  }if(chunks[s].size()!=256)return false;
 }
 auto get=[&](const char* tag)->Blob{auto it=top.find(Tag(tag));return it==top.end()?Blob{}:Copy(family.bytes[it->second.first],it->second.second);};
 const auto liquid=get("MH2O");if(!NativeLiquid(liquid))return false;
 Blob out,version(4),header(64),index(256*16);Write(version.data(),18);Append(out,"MVER",version);const auto headerChunk=Append(out,"MHDR",header);const size_t headerBase=headerChunk+8;
 const auto indexAt=Append(out,"MCIN",index);Write(out.data()+headerBase+4,uint32_t(indexAt-headerBase));
 struct HeaderField{const char* name;unsigned offset;};constexpr HeaderField fields[]={{"MTEX",8},{"MMDX",12},{"MMID",16},{"MWMO",20},{"MWID",24},{"MDDF",28},{"MODF",32},{"MFBO",36},{"MH2O",40},{"MTXF",44}};
 for(const auto& f:fields){const auto payload=get(f.name);if(f.offset<=32 && !top.contains(Tag(f.name)))return false;
  if(f.offset>32 && payload.empty())continue;
  if(f.offset==36 && payload.size()!=36)return false;
  const auto at=Append(out,f.name,payload);Write(out.data()+headerBase+f.offset,uint32_t(at-headerBase));if(f.offset==36)Write(out.data()+headerBase,Read(out.data()+headerBase)|1u);
 }
 // Retain non-native metadata as opaque chunks instead of changing active rendering.
 for(const auto& [tag,source]:top){bool native=false;for(const auto& f:fields)if(tag==Tag(f.name))native=true;if(!native)Append(out,tag,family.bytes[source.first].data()+source.second.at,source.second.size);}
 std::set<uint32_t> slots;
 for(unsigned ordinal=0;ordinal<256;++ordinal){const auto& root=chunks[0][ordinal];if(root.size<128)return false;
  Blob chunk(family.bytes[0].begin()+root.at,family.bytes[0].begin()+root.at+128);
  const auto x=Read(chunk.data()+4),y=Read(chunk.data()+8);if(x>=16 || y>=16 || !slots.insert(y*16+x).second)return false;
  std::map<uint32_t,Blob> parts;
  for(unsigned s=0;s<3;++s){const auto& p=chunks[s][ordinal];std::vector<Part> nested;const auto begin=p.at+(s?0:128);if(!Walk(family.bytes[s],begin,p.at+p.size,nested))return false;
   for(const auto& sub:nested)if(!parts.emplace(sub.tag,Copy(family.bytes[s],sub)).second)return false;
  }
  auto height=parts.find(Tag("MCVT")),normal=parts.find(Tag("MCNR"));if(height==parts.end() || height->second.size()!=145*4 || normal==parts.end() || (normal->second.size()!=435 && normal->second.size()!=448))return false;
  normal->second.resize(448,0); // Stock first-use walker advances by its native 448-byte normal span.
  Blob refs;for(const char* tag:{"MCRD","MCRW"}){auto it=parts.find(Tag(tag));if(it!=parts.end()){refs.insert(refs.end(),it->second.begin(),it->second.end());parts.erase(it);}}
  if(refs.size()!=size_t(Read(chunk.data()+0x10)+Read(chunk.data()+0x38))*4)return false;
  if(!parts.emplace(Tag("MCRF"),std::move(refs)).second)return false;
  static_assert(sizeof(CWSoundEmitter)==28);
  Write(chunk.data()+0x5c,0);
  if(auto sound=parts.find(Tag("MCSE"));sound!=parts.end()){
   if(sound->second.size()%sizeof(CWSoundEmitter))return false;
   for(size_t at=0;at<sound->second.size();at+=28)for(unsigned field=4;field<28;field+=4){float v;std::memcpy(&v,sound->second.data()+at+field,4);if(!std::isfinite(v))return false;}
   Write(chunk.data()+0x5c,uint32_t(sound->second.size()/28));
  }
  for(unsigned offset:{0x14u,0x18u,0x1cu,0x20u,0x24u,0x2cu,0x58u,0x60u,0x74u})Write(chunk.data()+offset,0);
  const char* ordered[]{"MCVT","MCNR","MCLY","MCRF","MCAL","MCSH","MCCV","MCSE"};
  for(const char* name:ordered){const auto tag=Tag(name);auto it=parts.find(tag);if(it==parts.end())continue;
   const auto at=Append(chunk,tag,it->second.data(),it->second.size());unsigned field=0;
   if(tag==Tag("MCVT"))field=0x14;else if(tag==Tag("MCNR"))field=0x18;else if(tag==Tag("MCLY"))field=0x1c;else if(tag==Tag("MCRF"))field=0x20;else if(tag==Tag("MCAL"))field=0x24;else if(tag==Tag("MCSH"))field=0x2c;else if(tag==Tag("MCCV"))field=0x74;else if(tag==Tag("MCSE"))field=0x58;
   Write(chunk.data()+field,uint32_t(at+8));parts.erase(it);
  }
  for(const auto& [tag,payload]:parts)Append(chunk,tag,payload.data(),payload.size());
  const auto chunkAt=Append(out,"MCNK",chunk);const auto entry=indexAt+8+(y*16+x)*16;Write(out.data()+entry,uint32_t(chunkAt));Write(out.data()+entry+4,uint32_t(chunk.size()+8));
 }
 output.swap(out);return true;
}
}
SplitLoad LoadSplitArea(CMapArea& area,const char* filename) noexcept {
 if(!SplitAdtEnabled() || !filename)return SplitLoad::Classic;
 try{
  std::string path=filename;for(auto& c:path){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}
  if(!path.starts_with("world\\maps\\") || !path.ends_with(".adt") || path.find("..")!=std::string::npos || path.find(':')!=std::string::npos)return SplitLoad::Classic;
  const auto stem=path.substr(0,path.size()-4);std::array<std::string,3> paths{path,stem+"_tex0.adt",stem+"_obj0.adt"};std::array<Blob,3> source;
  const bool tex=ReadAsset(paths[1].c_str(),source[1]),obj=ReadAsset(paths[2].c_str(),source[2]);if(!tex&&!obj)return SplitLoad::Classic;if(!tex||!obj)return SplitLoad::Rejected;
  Blob alternate;if(ReadAsset((stem+"_tex1.adt").c_str(),alternate))return SplitLoad::Rejected;
  if(!ReadAsset(path.c_str(),source[0]))return SplitLoad::Rejected;
  const a::SourceAlphaPolicy policy{CMap::header.m_flags,bool(CMap::header.m_flags&4u)};a::PreparedByteFamily family;
  if(!a::PrepareByteFamily(paths,source,policy,&ResolveTexture,&ResolveModel,family))return SplitLoad::Rejected;
  Blob monolithic;if(!Assemble(family,monolithic))return SplitLoad::Rejected;
  auto* memory=CMapArea::AllocAsyncLoadBuffer(int32_t(monolithic.size()));if(!memory)return SplitLoad::Rejected;
  std::memcpy(memory,monolithic.data(),monolithic.size());area.m_fileBuffer=memory;area.m_fileSize=int32_t(monolithic.size());area.m_file=nullptr;area.m_asyncObject=nullptr;
  area.ProcessIffChunks();WLOG_INFO("native-split-adt: loaded %s bytes=%zu",path.c_str(),monolithic.size());return SplitLoad::Loaded;
 }catch(...){return SplitLoad::Rejected;}
}
}
