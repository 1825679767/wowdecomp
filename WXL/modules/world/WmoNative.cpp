// Native WMO byte/provider module; native object lifecycle and renderer remain owner.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "WorldCompat.hpp"
#include "WXL/WmoMaterials.hpp"
#include <cmath>
#include <algorithm>
#include "NativeFeatures.hpp"
#include "NativeStorage.hpp"
#include "FdidCompat.hpp"
#include "WorldBytes.hpp"
#include "platform/waystone/WmoAssetFamily.hpp"
#include "platform/waystone/WmoChunkIndex.hpp"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "Engine/Source/Storm/H/storm.h"
#include "common/Log.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <cstdio>

namespace wxl::native::world {
namespace {
namespace w=wxl::waystone::wmo;
using namespace bytes;
struct Family {Blob auxiliary;std::vector<Blob> groups;std::vector<std::shared_ptr<const wmo_materials::GroupSource>> render;size_t bytes=0;};
struct Binding {CMapObj* root;unsigned index;const void* file;};
std::map<CMapObjGroup*,Binding> bindings;
std::mutex mutex;
std::map<CMapObj*,std::shared_ptr<const Family>> families;
size_t residentBytes=0;
bool DonorAux(uint32_t tag){for(const char* name:{"MGI2","MDDI","MNLD","MFED","MAVG","MAVD","MDDL","MPVD","MOSI","MOUV","MOGX","MOBS","MOPB","MNLR","MDAL","MFVR","MPVR","MAVR","MPY2","MOQG"})if(tag==Tag(name))return true;return false;}
bool Group(const Blob& raw,uint32_t materials,const uint8_t* groupNames,uint32_t groupNameBytes,uint32_t lights,uint32_t doodads,uint32_t portalRefs,uint32_t fogs,Blob& output,wmo_materials::GroupSource& render,bool layeredReady) {
 w::GroupIndex index;if(!w::IndexGroup(raw,index) || !w::ValidateGroupGeometry(raw,index,materials))return false;
 // Additional index/strip ownership remains unsupported; layered UV/weights
 // are retained in the immutable renderer side stream below. Refuse instead
 // of feeding the classic positional walker incompatible optional chunks.
 if(index.indices32.present || index.uvSets>4 || index.colorSets>2 || index.vertices.size/12>65535)return false;
 // Match the donor's tag-driven reader: preserve its known ignored auxiliary
 // chunks, but never give the classic positional walker modern LOD/strip flags.
 std::vector<Part> nested;const size_t end=size_t(index.header.offset)+Read(raw.data()+index.header.offset-4);
 if(!Walk(raw,index.header.offset+68,end,nested))return false;uint32_t auxiliary=0;
 for(const auto& p:nested)if(DonorAux(p.tag)||p.tag==Tag("MORI")||p.tag==Tag("MORB")){if(p.tag!=Tag("MORB"))++auxiliary;Append(render.auxiliary,p.tag,raw.data()+p.at,p.size);}
 if(auxiliary!=index.unknownChunks)return false;
 constexpr uint32_t optional=0x200u|0x800u|1u|4u|0x1000u|0x2000000u|0x1000000u;
 uint32_t flags=index.flags&~(optional|0x400u|0x20000u);
 if(index.lights.present)flags|=0x200;if(index.doodads.present)flags|=0x800;
 if(index.bspNodes.present||index.bspFaces.present){if(!index.bspNodes.present||!index.bspFaces.present)return false;flags|=1;}
 if(index.colors[0].present)flags|=4;if(index.liquid.present)flags|=0x1000;if(index.uv[1].present)flags|=0x2000000;if(index.colors[1].present)flags|=0x1000000;
 const auto* header=raw.data()+index.header.offset;const auto name=Read(header);
 auto word=[&](unsigned at){uint16_t value;std::memcpy(&value,header+at,2);return uint32_t(value);};
 const auto batchCount=uint32_t(index.batches.size/24),transparent=word(0x28),interior=word(0x2a),exterior=word(0x2c);
 // Native Create can consume these counters before the adapter gets control
 // back. Geometry validation alone does not validate header-driven subranges.
 if(transparent>batchCount||interior>batchCount||exterior>batchCount||transparent+interior+exterior>batchCount)return false;
 const auto portalStart=word(0x24),portalCount=word(0x26);
 if(portalCount&&(portalStart>portalRefs||portalCount>portalRefs-portalStart))return false;
 for(unsigned i=0;i<4;++i)if(header[0x30+i]&&header[0x30+i]>=fogs)return false;
 render.vertices=index.vertices.size/12;
 bool needsLayers=false;for(size_t at=0;at<index.batches.size;at+=24){const auto* b=raw.data()+index.batches.offset+at;uint16_t material=b[23];if(b[22]&2)std::memcpy(&material,b+10,2);if(material>=render.materials->records.size())return false;needsLayers|=render.materials->records[material].layered;}
 if(needsLayers){if(!layeredReady||index.uvSets!=4||!index.layerWeights.present||index.layerWeights.size!=render.vertices*4)return false;
  render.stream.resize(size_t(render.vertices)*36);for(uint32_t v=0;v<render.vertices;++v){for(unsigned s=0;s<4;++s){if(index.uv[s].size!=render.vertices*8)return false;const auto* uv=raw.data()+index.uv[s].offset+v*8;float pair[2];std::memcpy(pair,uv,8);if(!std::isfinite(pair[0])||!std::isfinite(pair[1]))return false;std::memcpy(render.stream.data()+v*36+s*8,uv,8);}std::memcpy(render.stream.data()+v*36+32,raw.data()+index.layerWeights.offset+v*4,4);}
 }
 if(name>=groupNameBytes || !std::memchr(groupNames+name,0,groupNameBytes-name))return false;
 auto span=[&](const w::Range& r)->Blob{return r.present?Blob(raw.begin()+r.offset,raw.begin()+r.offset+r.size):Blob{};};
 for(auto item:{std::pair{index.lights,lights},std::pair{index.doodads,doodads}})
  for(size_t at=0;at<item.first.size;at+=2){uint16_t id;std::memcpy(&id,raw.data()+item.first.offset+at,2);if(id>=item.second)return false;}
 if(index.liquid.present){const auto* p=raw.data()+index.liquid.offset;if(index.liquid.size<30)return false;
  const auto vx=Read(p),vy=Read(p+4),tx=Read(p+8),ty=Read(p+12);
  uint16_t material;std::memcpy(&material,p+28,2);
  if(material>=materials || vx>256||vy>256||tx>255||ty>255||vx!=tx+1||vy!=ty+1 || size_t(vx)*vy*8+size_t(tx)*ty+30!=index.liquid.size)return false;
 }
 Blob body(header,header+68);Write(body.data()+8,flags);
 // Native stores the material index in the final byte. Normalize the donor's
 // already validated extended index marker before native group publication.
 auto batches=span(index.batches);for(size_t at=0;at<batches.size();at+=24){if(batches[at+22]&2){uint16_t id;std::memcpy(&id,batches.data()+at+10,2);batches[at+23]=uint8_t(id);batches[at+22]&=~2u;}}
 auto emit=[&](const char* tag,const w::Range& range){const auto bytes=span(range);Append(body,tag,bytes);};
 emit("MOPY",index.triangles);emit("MOVI",index.indices);emit("MOVT",index.vertices);emit("MONR",index.normals);emit("MOTV",index.uv[0]);Append(body,"MOBA",batches);
 if(flags&0x200)emit("MOLR",index.lights);if(flags&0x800)emit("MODR",index.doodads);
 if(flags&1){emit("MOBN",index.bspNodes);emit("MOBR",index.bspFaces);}
 if(flags&4)emit("MOCV",index.colors[0]);if(flags&0x1000)emit("MLIQ",index.liquid);
 if(flags&0x2000000)emit("MOTV",index.uv[1]);if(flags&0x1000000)emit("MOCV",index.colors[1]);
 Blob result,version(4);Write(version.data(),17);Append(result,"MVER",version);Append(result,"MOGP",body);output.swap(result);return true;
}
}
bool PrepareWmo(CMapObj& object) noexcept {
 if(!WmoEnabled())return true;
 try{
  if(!object.m_pWmoData || object.m_wmoFileSize<=0 || object.m_wmoFileSize>64*1024*1024)return false;
  const auto* data=static_cast<const uint8_t*>(object.m_pWmoData);w::RootIndex source;
  if(!w::IndexRoot({data,size_t(object.m_wmoFileSize)},source))return false;if(!source.modern)return true;
  Blob nativeRoot,rootAuxiliary,rootSource(data,data+object.m_wmoFileSize);std::vector<Part> rootChunks;
  if(!Walk(rootSource,0,rootSource.size(),rootChunks))return false;
  for(const auto& part:rootChunks){if(DonorAux(part.tag))Append(rootAuxiliary,part.tag,rootSource.data()+part.at,part.size);else Append(nativeRoot,part.tag,rootSource.data()+part.at,part.size);}
  w::AssetFamily prepared;std::string reason;
  if(!w::PrepareAssetFamily(object.m_wmoName,nativeRoot,&ResolveTexture,&ResolveModel,prepared,reason,wmo_materials::LayeredReady())){
   WLOG_WARN("native-wmo: refused %s: %s",object.m_wmoName,reason.c_str());return false;
  }
  w::RootIndex root;if(!w::IndexRoot(prepared.root.bytes,root))return false;
  const auto groups=Read(prepared.root.bytes.data()+root.slots[0].offset+4);
  if(groups>512 || prepared.groups.size()!=groups)return false;
  const auto portalRefs=root.slots[8].size/8,portals=root.slots[7].size/20,portalVertices=root.slots[6].size/12,fogs=root.slots[15].size/48;
  // Native interior lookup reads its authored entry0 even if no group selects
  // another entry. Refuse a missing native table; do not synthesize fog data.
  if(groups&&!fogs)return false;
  auto word=[](const uint8_t* at){uint16_t value;std::memcpy(&value,at,2);return uint32_t(value);};
  for(uint32_t i=0;i<portals;++i){const auto* row=prepared.root.bytes.data()+root.slots[7].offset+20*i;
   const auto first=word(row),count=word(row+2);if(first>portalVertices||count>portalVertices-first)return false;
  }
  for(uint32_t i=0;i<portalRefs;++i){const auto* row=prepared.root.bytes.data()+root.slots[8].offset+8*i;
   const auto portal=word(row),group=word(row+2);if(portal>=portals||group>=groups)return false;
   if(word(prepared.root.bytes.data()+root.slots[7].offset+20*portal+2)<3)return false;
  }
  auto materials=std::make_shared<const w::MaterialPlan>(std::move(prepared.root.materials));
  auto family=std::make_shared<Family>();family->auxiliary=std::move(rootAuxiliary);family->groups.reserve(groups);size_t total=prepared.root.bytes.size()+family->auxiliary.size()+materials->records.size()*sizeof(w::MaterialRecord);for(const auto& material:materials->records)for(const auto& name:material.paths)total+=name.size()+1;if(total>128u*1024u*1024u)return false;
  std::string stem=object.m_wmoName;if(stem.size()<4)return false;stem.resize(stem.size()-4);
  for(unsigned i=0;i<groups;++i){char suffix[32];std::snprintf(suffix,sizeof(suffix),"_%03u.wmo",i);
   // This key is the exact verified native ReadGroup filename contract. Only
   // its explicit GFID/named provider value is opened; no storage alias exists.
   auto path=prepared.groups.find(stem+suffix);if(path==prepared.groups.end())return false;
   auto render=std::make_shared<wmo_materials::GroupSource>();render->materials=materials;
   Blob raw,group;if(!ReadAsset(path->second.c_str(),raw) || !Group(raw,root.slots[2].size/64,prepared.root.bytes.data()+root.slots[3].offset,root.slots[3].size,root.slots[11].size/48,root.slots[14].size/40,portalRefs,fogs,group,*render,wmo_materials::LayeredReady()))return false;
   total+=group.size()+render->stream.size()+render->auxiliary.size();family->render.push_back(std::move(render));if(total>128u*1024u*1024u)return false;family->groups.push_back(std::move(group));
  }
  family->bytes=total;
  void* memory=SMemAlloc(prepared.root.bytes.size(),__FILE__,__LINE__,0);if(!memory)return false;
  std::memcpy(memory,prepared.root.bytes.data(),prepared.root.bytes.size());
  try{std::lock_guard lock(mutex);const auto old=families.find(&object);const auto oldBytes=old==families.end()?0:old->second->bytes;
   if((old==families.end()&&families.size()>=128)||residentBytes-oldBytes+total>256u*1024u*1024u){SMemFree(memory,__FILE__,__LINE__,0);return false;}
   families[&object]=std::move(family);residentBytes=residentBytes-oldBytes+total;}
  catch(...){SMemFree(memory,__FILE__,__LINE__,0);return false;}
  SMemFree(object.m_pWmoData,__FILE__,__LINE__,0);object.m_pWmoData=memory;object.m_wmoFileSize=uint32_t(prepared.root.bytes.size());
  WLOG_INFO("native-wmo: prepared %s groups=%u",object.m_wmoName,groups);return true;
 }catch(...){return false;}
}
GroupLoad LoadPreparedWmoGroup(CMapObj& root,unsigned index,CMapObjGroup& group) noexcept {
 try{
  std::shared_ptr<const Family> family;{std::lock_guard lock(mutex);auto it=families.find(&root);if(it==families.end())return GroupLoad::Classic;family=it->second;}
  if(index>=family->groups.size())return GroupLoad::Rejected;const auto& bytes=family->groups[index];
  void* memory=SMemAlloc(bytes.size(),__FILE__,__LINE__,0);if(!memory)return GroupLoad::Rejected;
  std::memcpy(memory,bytes.data(),bytes.size());group.m_filePtr=memory;group.m_fileSize=uint32_t(bytes.size());group.m_parent=&root;group.Create();if(group.m_vertexListCount!=family->render[index]->vertices)return GroupLoad::Rejected;
  if(root.m_header&&(root.m_header->m_flags&8)&&group.m_colorVertexList&&group.m_colorVertexListSize>0){
   uint32_t begin=0;const auto count=uint32_t(group.m_colorVertexListSize);if(group.m_transparencyBatchesCount>0&&group.m_transparencyBatchesCount<=group.m_batchListCount)begin=(std::min)(uint32_t(group.m_batchList[group.m_transparencyBatchesCount-1].m_vertexEnd)+1,count);
   const uint8_t alpha=(group.m_flags&8)?255:0;for(uint32_t v=begin;v<count;++v)group.m_colorVertexList[v].a=alpha;
  }{std::lock_guard lock(mutex);if(families.find(&root)==families.end()||families.at(&root)!=family)return GroupLoad::Rejected;bindings[&group]={&root,index,memory};}return GroupLoad::Loaded;
 }catch(...){SErrDisplayAppFatal("Unable to load prepared native WMO group");return GroupLoad::Rejected;}
}
bool IsPreparedWmo(const CMapObj* object) noexcept {try{std::lock_guard lock(mutex);return object&&families.find(const_cast<CMapObj*>(object))!=families.end();}catch(...){return false;}}
bool ValidWorldRay(const float* start,const float* end) noexcept {if(!WmoEnabled())return true;for(auto* p:{start,end}){if(!p)return false;for(unsigned i=0;i<3;++i){uint32_t bits;std::memcpy(&bits,p+i,4);if((bits&0x7f800000u)==0x7f800000u)return false;}}return true;}
void ReleaseWmo(CMapObj& object) noexcept {try{std::lock_guard lock(mutex);auto it=families.find(&object);if(it!=families.end()){residentBytes-=it->second->bytes;families.erase(it);for(auto b=bindings.begin();b!=bindings.end();)if(b->second.root==&object)b=bindings.erase(b);else ++b;}}catch(...){}}
}

namespace wxl::native::wmo_materials {
std::shared_ptr<const GroupSource> FindSource(CMapObj& root,CMapObjGroup& group) noexcept {try{std::lock_guard lock(world::mutex);auto f=world::families.find(&root);auto b=world::bindings.find(&group);if(f==world::families.end()||b==world::bindings.end()||b->second.root!=&root||b->second.file!=group.m_filePtr||group.m_parent!=&root||b->second.index>=f->second->render.size())return {};auto source=f->second->render[b->second.index];if(group.m_vertexListCount!=source->vertices)return {};return source;}catch(...){return {};}}
}
