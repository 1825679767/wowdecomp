// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/WmoAssetFamily.hpp"
#include "platform/waystone/WmoChunkIndex.hpp"
#include <cstring>
#include <cstdio>
namespace wxl::waystone::wmo {
namespace {
uint32_t Get(const uint8_t* p){uint32_t n;std::memcpy(&n,p,4);return n;}
void Put(uint8_t* p,uint32_t n){std::memcpy(p,&n,4);}
bool Path(const char* p,const char* suffix){if(!p || !*p || std::strlen(p)>900 || *p=='/' || *p==92 || std::strstr(p,"..") || std::strchr(p,':'))return false;for(auto q=p;*q;++q)if(static_cast<unsigned char>(*q)<32)return false;size_t n=std::strlen(p),end=std::strlen(suffix);return n>=end && !_stricmp(p+n-end,suffix);}
void Chunk(std::vector<uint8_t>& b,const char* tag,std::span<const uint8_t> data){size_t at=b.size();b.resize(at+8);for(unsigned i=0;i<4;++i)b[at+i]=tag[3-i];Put(b.data()+at+4,uint32_t(data.size()));b.insert(b.end(),data.begin(),data.end());}
}
bool PrepareAssetFamily(const char* path,std::span<const uint8_t> source,TextureResolver texture,TextureResolver model,AssetFamily& output,std::string& reason,bool layeredRenderer){
 reason.clear();auto fail=[&](const char* why){reason=why;return false;};RootIndex index;
 if(!Path(path,".wmo") || !IndexRoot(source,index) || !index.modern)return fail("modern root required");
 const uint32_t providers=unsigned(index.groupFileIds.present)+unsigned(index.doodadFileIds.present);
 if(index.unknownChunks!=providers)return fail("unsupported root metadata");
 std::array<std::vector<uint8_t>,std::size(RootSlots)> slots;
 for(size_t i=0;i<slots.size();++i){auto range=index.slots[i];if(range.present)slots[i].assign(source.begin()+range.offset,source.begin()+range.offset+range.size);}
 // This path handles modern FDID materials only; legacy MOTX offsets must not be reinterpreted as FDIDs.
 if(!slots[1].empty())return fail("mixed named/FDID texture root unsupported");
 AssetFamily result;const auto groups=Get(slots[0].data()+4);
 if(index.groupFileIds.present){
  if(!model || index.groupFileIds.size/4!=groups || groups>4096)return fail("group FDID/LOD count mismatch");
  std::string stem=path;stem.resize(stem.size()-4);
  for(uint32_t i=0;i<groups;++i){const auto id=Get(source.data()+index.groupFileIds.offset+4*i);const char* target=id?model(id):nullptr;
   if(!Path(target,".wmo"))return fail("unresolved group FDID");char suffix[32]{};std::snprintf(suffix,sizeof(suffix),"_%03u.wmo",i);result.groups.emplace(stem+suffix,target);
  }
 }
 if(!index.groupFileIds.present && groups){
  if(groups>4096)return fail("group count exceeds ownership budget");
  std::string stem=path;stem.resize(stem.size()-4);
  for(uint32_t i=0;i<groups;++i){char suffix[32]{};std::snprintf(suffix,sizeof(suffix),"_%03u.wmo",i);const auto name=stem+suffix;result.groups.emplace(name,name);}
 }
 if(index.doodadFileIds.present){
  if(!model)return fail("doodad resolver unavailable");const auto count=index.doodadFileIds.size/4;
  std::vector<uint8_t> names(1,0);std::map<uint32_t,uint32_t> offsets;
  for(size_t at=0;at<slots[14].size();at+=40){const auto packed=Get(slots[14].data()+at),which=packed&0xffffff;
   if(which>=count)return fail("doodad FDID index out of range");const auto id=Get(source.data()+index.doodadFileIds.offset+4*which);const char* name=id?model(id):nullptr;
   if(!Path(name,".m2"))return fail("unresolved doodad FDID");auto found=offsets.find(id);uint32_t offset;
   if(found==offsets.end()){offset=uint32_t(names.size());if(offset>0xffffff || std::strlen(name)>0xffffff-offset)return fail("doodad name blob overflow");names.insert(names.end(),name,name+std::strlen(name)+1);offsets.emplace(id,offset);}else offset=found->second;
   Put(slots[14].data()+at,(packed&0xff000000)|offset);
  }
  slots[13]=std::move(names);Put(slots[0].data()+16,uint32_t(offsets.size()));
 }
 std::vector<uint8_t> shaped;uint32_t version=17;Chunk(shaped,"MVER",{reinterpret_cast<const uint8_t*>(&version),4});
 // Keep MOTX absent so the conservative material planner recognizes modern FDIDs.
 for(size_t i=0;i<slots.size();++i)if(i!=1)Chunk(shaped,RootSlots[i].tag,slots[i]);
 if(!PrepareNamedRoot(shaped,texture,true,result.root,reason,layeredRenderer))return false;
 output=std::move(result);return true;
}
}
