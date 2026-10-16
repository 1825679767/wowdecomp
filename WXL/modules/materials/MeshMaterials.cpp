// Source21/33/35 WXL donor compatibility, source-indexed rigid/GPU/CPU weighted two-UV batches.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "MeshMaterials.hpp"
#include "DndMaterials.hpp"
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "MaterialBackend.hpp"
#include "platform/waystone/M2Bytes.hpp"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/CM2SceneRender.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include <windows.h>
#include <d3d9.h>
#include <d3dcompiler.h>
#include <algorithm>
#include <cwchar>
#include "common/Log.hpp"
#include <cstring>
#include <span>
#include <string_view>
#include "shaders/CombinersModAddAlphaVs.h"
#include "shaders/CombinersModAddAlphaPs.h"
#include "shaders/CombinersModAddAlphaSkinnedSource.hpp"
namespace wxl::native::mesh_materials {
namespace {
template<class T>T Read(const uint8_t* p,size_t at){T value{};std::memcpy(&value,p+at,sizeof(T));return value;}
bool Range(size_t size,uint32_t offset,uint32_t count,uint32_t stride){return offset<=size&&uint64_t(count)*stride<=size-offset;}
bool D3D9(){
 if(!LegacyMaterialBackend(g_theGxDevicePtr))return false;
 // Source preparation can execute on a loader worker. Native cached caps are
 // immutable device metadata; do not invoke a COM device method from here.
 const auto& caps=g_theGxDevicePtr->Caps();
 return caps.m_shaderTargets[GxSh_Vertex]>=3&&caps.m_shaderTargets[GxSh_Pixel]>=4;
}
uint64_t Fingerprint(std::span<const uint8_t> bytes){uint64_t hash=14695981039346656037ull;for(auto b:bytes){hash^=b;hash*=1099511628211ull;}return hash;}
}
bool AddAlphaFamily(uint16_t shader) noexcept {return shader==0x8015||shader==0x8021||shader==0x8023;}
bool RiftPath(const char* name) noexcept {
 if(!name)return false;const std::string_view path(name),stem="spells\\cfx_priest_entropicrift_areatrigger";
 if(path.size()<stem.size())return false;
 auto fold=[](char c){return c=='/'?'\\':c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
 for(size_t i=0;i<stem.size();++i)if(fold(path[i])!=stem[i])return false;
 const auto extension=path.substr(stem.size());
 return extension.empty()||(extension.size()==3&&extension[0]=='.'&&fold(extension[1])=='m'&&extension[2]=='2')||(extension.size()==4&&extension[0]=='.'&&fold(extension[1])=='m'&&fold(extension[2])=='d'&&fold(extension[3])=='x');
}
bool PrepareNativeCompatibilitySkin(const char* modelPath,const uint8_t* raw,size_t size,const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared){
 prepared.clear();

 // Port the exact existing donor RiftMeshPolicy signature, not a global33 map.
 if(!MaterialsEnabled()||!RiftPath(modelPath)||!raw||size<312||size>64u*1024*1024||std::memcmp(raw,"MD21",4)||original.size()<48||original.size()>64u*1024*1024||std::memcmp(original.data(),"SKIN",4))return false;
 const uint32_t bodySize=Read<uint32_t>(raw,4);if(bodySize<304||bodySize>size-8)return false;
 const auto* body=raw+8;if(std::memcmp(body,"MD20",4)||Read<uint32_t>(body,4)!=274)return false;
 const uint32_t count=Read<uint32_t>(original.data(),36),at=Read<uint32_t>(original.data(),40);
 const uint32_t materials=Read<uint32_t>(body,0x70),materialAt=Read<uint32_t>(body,0x74),textures=Read<uint32_t>(body,0x50),textureAt=Read<uint32_t>(body,0x54),combos=Read<uint32_t>(body,0x80),comboAt=Read<uint32_t>(body,0x84);
 if(!count||count>4096||at<48||!Range(original.size(),at,count,24)||!Range(bodySize,materialAt,materials,4)||!Range(bodySize,textureAt,textures,16)||!Range(bodySize,comboAt,combos,2))return false;
 std::vector<uint8_t> candidate;size_t corrected=0;
 for(uint32_t i=0;i<count;++i){
  const auto* batch=original.data()+at+24*i;
  if(Read<uint16_t>(batch,2)!=0x8021)continue;
  const uint32_t material=Read<uint16_t>(batch,10),combo=Read<uint16_t>(batch,16);
  if(Read<uint16_t>(batch,14)!=2||material>=materials||combo>combos||combos-combo<2)return false;
  const auto flags=Read<uint16_t>(body,materialAt+4*material),blend=Read<uint16_t>(body,materialAt+4*material+2);
  if(!(flags&1)||(blend!=2&&blend!=7))return false;
  for(uint32_t stage=0;stage<2;++stage){const auto index=Read<uint16_t>(body,comboAt+2*(combo+stage));if(index>=textures||Read<uint32_t>(body,textureAt+16*index)!=0)return false;}
  if(candidate.empty())candidate=original;
  const uint16_t packed=0x4014;std::memcpy(candidate.data()+at+24*i+2,&packed,2);++corrected;
 }
 if(!corrected)return false;
 prepared.swap(candidate);return true;
}
std::shared_ptr<const Source> Prepare(const char* modelPath,const uint8_t* raw,size_t size,const std::vector<uint8_t>& skin,std::vector<uint8_t>& prepared,bool nativeGpuSkinning){
 prepared.clear();
 if(auto dnd=PrepareDnd(modelPath,raw,size,skin,prepared))return dnd;
 if(!MaterialsEnabled()||!D3D9()||!modelPath||!raw||size<312||size>64u*1024*1024||skin.size()<48||skin.size()>64u*1024*1024||std::memcmp(raw,"MD21",4)||std::memcmp(skin.data(),"SKIN",4))return {};
 const uint32_t bodySize=Read<uint32_t>(raw,4);if(bodySize<304||bodySize>size-8)return {};
 const auto* body=raw+8;
  const uint32_t bones=Read<uint32_t>(body,0x2c),profiles=Read<uint32_t>(body,0x44);
 const bool weighted=bones!=1,cpuSkinning=weighted&&!nativeGpuSkinning;
 if(std::memcmp(body,"MD20",4)||Read<uint32_t>(body,4)!=274||!bones||bones>4096||!profiles||profiles>4)return {};
 if(weighted && !cpuSkinning && g_theGxDevicePtr->Caps().m_shaderConstants[GxSh_Vertex]<256)return {};
 const uint32_t batches=Read<uint32_t>(skin.data(),36),at=Read<uint32_t>(skin.data(),40),sections=Read<uint32_t>(skin.data(),28),sectionAt=Read<uint32_t>(skin.data(),32);
 if(!batches||batches>32||!sections||sections>32||at<48||sectionAt<48||!Range(skin.size(),at,batches,24)||!Range(skin.size(),sectionAt,sections,48))return {};
 const uint32_t boneCombos=Read<uint32_t>(body,0x78),boneAt=Read<uint32_t>(body,0x7c);
 const uint32_t vertexCount=Read<uint32_t>(body,0x3c),vertexAt=Read<uint32_t>(body,0x40),lookupCount=Read<uint32_t>(skin.data(),4),lookupAt=Read<uint32_t>(skin.data(),8),propsAt=Read<uint32_t>(skin.data(),24);
 if(!Range(bodySize,boneAt,boneCombos,2)||!Range(bodySize,vertexAt,vertexCount,48)||!Range(skin.size(),lookupAt,lookupCount,2)||Read<uint32_t>(skin.data(),20)!=lookupCount||!Range(skin.size(),propsAt,lookupCount,4))return {};
 std::vector<SectionSource> sourceSections;sourceSections.reserve(sections);uint32_t cpuVertexBudget=0;
 for(uint32_t i=0;i<sections;++i){
  const auto* section=skin.data()+sectionAt+48*i;SectionSource captured;
  const uint32_t count=Read<uint16_t>(section,12),palette=Read<uint16_t>(section,14),influences=Read<uint16_t>(section,16),first=Read<uint16_t>(section,4),vertices=Read<uint16_t>(section,6);
  // This is a serialized source recipe, not GPU publication. Oversized byte
  // palettes are partitioned by the native converter before BindTopology.
  if(!count||count>(weighted?256u:1u)||!influences||influences>4||!Read<uint16_t>(section,10)||palette>boneCombos||count>boneCombos-palette||first>lookupCount||vertices>lookupCount-first)return {};
  captured.firstVertex=uint16_t(first);captured.vertexCount=uint16_t(vertices);captured.paletteOffset=uint16_t(palette);captured.influences=uint16_t(influences);
  for(uint32_t bone=0;bone<count;++bone){const auto global=Read<uint16_t>(body,boneAt+2*(palette+bone));if(global>=bones||(cpuSkinning&&global>255))return {};captured.palette.push_back(global);}
  if(cpuSkinning){if(vertices>65535-cpuVertexBudget)return {};cpuVertexBudget+=vertices;captured.cpuVertices.reserve(vertices);}
  for(uint32_t vertex=first;vertex<first+vertices;++vertex){
   const auto index=Read<uint16_t>(skin.data(),lookupAt+2*vertex);if(index>=vertexCount)return {};
   const auto* source=body+vertexAt+48*index;const auto* local=skin.data()+propsAt+4*vertex;
   uint32_t sum=0;bool ended=false;
   for(uint32_t lane=0;lane<4;++lane){const auto weight=source[12+lane];if(!weight){ended=true;continue;}if(ended||lane>=influences||local[lane]>=count)return {};sum+=weight;}
   if(sum!=255)return {};
   if(cpuSkinning){std::array<uint8_t,48> value;std::memcpy(value.data(),source,48);for(uint32_t lane=0;lane<4;++lane)value[16+lane]=source[12+lane]?uint8_t(captured.palette[local[lane]]):0;captured.cpuVertices.push_back(value);}
  }
  if(cpuSkinning){
   const uint32_t indexCount=Read<uint32_t>(skin.data(),12),indexAt=Read<uint32_t>(skin.data(),16),start=uint32_t(Read<uint16_t>(section,8))|(uint32_t(Read<uint16_t>(section,2))<<16),n=Read<uint16_t>(section,10);
   if(!Range(skin.size(),indexAt,indexCount,2)||start>indexCount||n>indexCount-start)return {};
   captured.cpuIndices.reserve(n);for(uint32_t index=0;index<n;++index){const auto value=Read<uint16_t>(skin.data(),indexAt+2*(start+index));if(value<first||uint32_t(value)-first>=vertices)return {};captured.cpuIndices.push_back(uint16_t(value-first));}
  }
  sourceSections.push_back(std::move(captured));
 } const uint32_t materials=Read<uint32_t>(body,0x70),materialAt=Read<uint32_t>(body,0x74);
 const uint32_t textures=Read<uint32_t>(body,0x50),textureAt=Read<uint32_t>(body,0x54),combos=Read<uint32_t>(body,0x80),comboAt=Read<uint32_t>(body,0x84);
 if(!textures||textures>64||!Range(bodySize,materialAt,materials,4)||!Range(bodySize,textureAt,textures,16)||!Range(bodySize,comboAt,combos,2))return {};
 std::span<const uint8_t> txid;
 for(size_t off=0;off<size;){
  if(size-off<8)return {};const auto n=Read<uint32_t>(raw,off+4);if(n>size-off-8)return {};
  if(!std::memcmp(raw+off,"TXID",4)){if(!txid.empty()||n!=textures*4)return {};txid={raw+off+8,n};}off+=8+n;
 }
 if(txid.size()!=textures*4)return {};
 auto result=std::make_shared<Source>();result->weighted=weighted;result->cpuSkinning=cpuSkinning;result->boneCount=bones;result->sections=std::move(sourceSections);result->modelPath=modelPath;result->sectionCount=sections;result->textureCount=textures;result->batches.reserve(batches);
 for(uint32_t b=0;b<batches;++b){
  const auto* batch=skin.data()+at+24*b;const uint16_t shader=Read<uint16_t>(batch,2);
  // The donor's exact named Rift33 correction has a different pixel equation.
  if(!AddAlphaFamily(shader)||(shader==0x8021&&RiftPath(modelPath))||Read<uint16_t>(batch,14)!=2||Read<uint16_t>(batch,4)>=sections||Read<uint16_t>(batch,12))return {};
  const uint32_t material=Read<uint16_t>(batch,10),combo=Read<uint16_t>(batch,16);
  if(material>=materials||combo>combos||combos-combo<2)return {};
  BatchSource record;record.flags=Read<uint16_t>(body,materialAt+material*4);record.blend=Read<uint16_t>(body,materialAt+material*4+2);
  if(record.flags>31||record.blend!=7)return {};
  for(uint32_t i=0;i<2;++i){
   const auto index=Read<uint16_t>(body,comboAt+2*(combo+i));if(index>=textures||Read<uint32_t>(body,textureAt+index*16)!=0||Read<uint32_t>(body,textureAt+index*16+8)!=0)return {};
   record.textureIndices[i]=index;record.textureIds[i]=Read<uint32_t>(txid.data(),index*4);if(!record.textureIds[i])return {};
   for(uint32_t head:{0x90u,0x98u}){
    const uint32_t count=Read<uint32_t>(body,head),lookup=Read<uint32_t>(body,head+4),first=Read<uint16_t>(batch,head==0x90?20:22);
    if(first>count||count-first<2||!Range(bodySize,lookup,count,2))return {};
    (head==0x90?record.weightIndices:record.transformIndices)[i]=Read<uint16_t>(body,lookup+2*(first+i));
   }
  }
  std::memcpy(record.batch.data(),batch,24);result->batches.push_back(record);
 }
 result->modelFingerprint=Fingerprint({raw,size});result->skinFingerprint=Fingerprint(skin);
 prepared=skin;
 // Preserve every source-indexed two-stage record. Native conversion neither
 // expands nor reorders this private clone; the exact originals stay immutable.
 for(uint32_t i=0;i<batches;++i){const uint16_t packed=0x4011;std::memcpy(prepared.data()+at+24*i+2,&packed,2);}
 return result;
}
std::shared_ptr<const Source> BindTopology(std::shared_ptr<const Source> source,const std::vector<uint8_t>& prepared,
 const std::vector<uint8_t>& model,const std::vector<uint8_t>& rebuilt,const std::vector<uint32_t>& origins,
 uint32_t paletteLimit,bool nativeGpuSkinning){
 if(!source||!source->weighted||source->dnd||source->topologyParent||(!nativeGpuSkinning&&!source->cpuSkinning)||!paletteLimit||paletteLimit>75||prepared.size()<48||model.size()<304||rebuilt.size()<48)return {};
 if(std::memcmp(model.data(),"MD20",4)||Read<uint32_t>(model.data(),4)!=264||std::memcmp(rebuilt.data(),"SKIN",4))return {};
 const auto sourceN=Read<uint32_t>(prepared.data(),36),sourceAt=Read<uint32_t>(prepared.data(),40);
 if(sourceN!=source->batches.size()||!sourceN||sourceAt<48||!Range(prepared.size(),sourceAt,sourceN,24))return {};
 // The unchanged material serializer receives no geometry, never a native
 // publication. Actual rebuilt geometry was validated against the device.
 std::vector<uint8_t> carrier(48,0);std::memcpy(carrier.data(),"SKIN",4);
 carrier.insert(carrier.end(),prepared.begin()+sourceAt,prepared.begin()+sourceAt+size_t(sourceN)*24);
 std::memcpy(carrier.data()+36,&sourceN,4);const uint32_t header=48;std::memcpy(carrier.data()+40,&header,4);
 std::vector<uint8_t> canonical;if(!wxl::waystone::m2bytes::ConvertSkin(carrier,canonical,true))return {};
 const auto canonicalN=Read<uint32_t>(canonical.data(),36),canonicalAt=Read<uint32_t>(canonical.data(),40);
 const auto batchN=Read<uint32_t>(rebuilt.data(),36),batchAt=Read<uint32_t>(rebuilt.data(),40),sectionN=Read<uint32_t>(rebuilt.data(),28),sectionAt=Read<uint32_t>(rebuilt.data(),32);
 const auto bones=Read<uint32_t>(model.data(),0x2c),comboN=Read<uint32_t>(model.data(),0x78),comboAt=Read<uint32_t>(model.data(),0x7c),vertexN=Read<uint32_t>(model.data(),0x3c),vertexAt=Read<uint32_t>(model.data(),0x40);
 const auto lookupN=Read<uint32_t>(rebuilt.data(),4),lookupAt=Read<uint32_t>(rebuilt.data(),8),propsN=Read<uint32_t>(rebuilt.data(),20),propsAt=Read<uint32_t>(rebuilt.data(),24);
 const auto indexN=Read<uint32_t>(rebuilt.data(),12),indexAt=Read<uint32_t>(rebuilt.data(),16);
 if(canonicalN!=sourceN||origins.size()!=batchN||!batchN||batchN>16384||!sectionN||sectionN>16384||bones!=source->boneCount||lookupN!=propsN||
    !Range(canonical.size(),canonicalAt,canonicalN,24)||!Range(rebuilt.size(),batchAt,batchN,24)||!Range(rebuilt.size(),sectionAt,sectionN,48)||
    !Range(model.size(),comboAt,comboN,2)||!Range(model.size(),vertexAt,vertexN,48)||!Range(rebuilt.size(),lookupAt,lookupN,2)||!Range(rebuilt.size(),propsAt,propsN,4)||!Range(rebuilt.size(),indexAt,indexN,2))return {};
 auto next=std::make_shared<Source>(*source);next->topologyParent=source;next->nativeBatchOrigins=origins;next->sectionCount=sectionN;next->sections.clear();next->batches.clear();
 uint32_t cpuVertexBudget=0;
 for(uint32_t index=0;index<sectionN;++index){const auto* section=rebuilt.data()+sectionAt+index*48;SectionSource captured;
  captured.firstVertex=Read<uint16_t>(section,4);captured.vertexCount=Read<uint16_t>(section,6);captured.paletteOffset=Read<uint16_t>(section,14);captured.influences=Read<uint16_t>(section,16);const auto count=Read<uint16_t>(section,12);
  if(!count||count>paletteLimit||!captured.influences||captured.influences>4||captured.paletteOffset>comboN||count>comboN-captured.paletteOffset||uint32_t(captured.firstVertex)+captured.vertexCount>lookupN)return {};
  for(uint32_t bone=0;bone<count;++bone){const auto global=Read<uint16_t>(model.data(),comboAt+2*(captured.paletteOffset+bone));if(global>=bones||(source->cpuSkinning&&global>255))return {};captured.palette.push_back(global);}
  if(source->cpuSkinning){if(captured.vertexCount>65535-cpuVertexBudget)return {};cpuVertexBudget+=captured.vertexCount;captured.cpuVertices.reserve(captured.vertexCount);}
  for(uint32_t vertex=captured.firstVertex;vertex<uint32_t(captured.firstVertex)+captured.vertexCount;++vertex){const auto origin=Read<uint16_t>(rebuilt.data(),lookupAt+vertex*2);if(origin>=vertexN)return {};const auto* raw=model.data()+vertexAt+size_t(origin)*48;const auto* props=rebuilt.data()+propsAt+size_t(vertex)*4;uint32_t sum=0;bool ended=false;
   for(uint32_t lane=0;lane<4;++lane){if(!raw[12+lane]){ended=true;continue;}if(ended||lane>=captured.influences||props[lane]>=count)return {};sum+=raw[12+lane];}if(sum!=255)return {};
   if(source->cpuSkinning){std::array<uint8_t,48> value;std::memcpy(value.data(),raw,48);for(uint32_t lane=0;lane<4;++lane)value[16+lane]=raw[12+lane]?uint8_t(captured.palette[props[lane]]):0;captured.cpuVertices.push_back(value);}
  }
  if(source->cpuSkinning){const uint32_t first=uint32_t(Read<uint16_t>(section,8))|(uint32_t(Read<uint16_t>(section,2))<<16),number=Read<uint16_t>(section,10);
   if(first>indexN||number>indexN-first||number%3)return {};captured.cpuIndices.reserve(number);
   for(uint32_t triangle=0;triangle<number;++triangle){const auto vertex=Read<uint16_t>(rebuilt.data(),indexAt+2*(first+triangle));if(vertex<captured.firstVertex||uint32_t(vertex)>=uint32_t(captured.firstVertex)+captured.vertexCount)return {};captured.cpuIndices.push_back(uint16_t(vertex-captured.firstVertex));}
  }
  next->sections.push_back(std::move(captured));
 }
 for(uint32_t batch=0;batch<batchN;++batch){const auto origin=origins[batch];if(origin>=canonicalN)return {};const auto* actual=rebuilt.data()+batchAt+24*batch;const auto* expected=canonical.data()+canonicalAt+24*origin;
  for(uint32_t byte=0;byte<24;++byte)if(byte!=4&&byte!=5&&actual[byte]!=expected[byte])return {};
  const auto section=Read<uint16_t>(actual,4);if(section>=sectionN)return {};auto record=source->batches[origin];std::memcpy(record.batch.data()+4,&section,2);next->batches.push_back(record);
 }
 return ConvertedMatches(*next,rebuilt)?next:nullptr;
}
bool ConvertedMatches(const Source& source,const std::vector<uint8_t>& skin) noexcept {
 if(source.dnd)return DndConvertedMatches(source,skin);
 if(skin.size()<48||Read<uint32_t>(skin.data(),36)!=source.batches.size()||Read<uint32_t>(skin.data(),28)!=source.sectionCount)return false;
 const auto at=Read<uint32_t>(skin.data(),40);if(!Range(skin.size(),at,uint32_t(source.batches.size()),24))return false;
 for(size_t i=0;i<source.batches.size();++i){
  const auto* b=skin.data()+at+24*i;const auto* raw=source.batches[i].batch.data();
  if(b[0]!=(raw[0]&0x10)||b[1]!=raw[1])return false;
  if(Read<uint16_t>(b,2)!=0||Read<uint16_t>(b,14)!=2||Read<uint16_t>(b,18)!=4)return false;
  for(size_t offset:{size_t(4),size_t(6),size_t(8),size_t(10),size_t(12),size_t(16),size_t(20),size_t(22)})if(Read<uint16_t>(b,offset)!=Read<uint16_t>(raw,offset))return false;
 }
 return true;
}

namespace {
constexpr const char* VertexName="WXL_Mesh_AddAlpha21_VS_20261016";
constexpr const char* WeightedVertexName="WXL_Mesh_AddAlpha21_Weighted_VS_20261016";
constexpr const char* PixelName="WXL_Mesh_AddAlpha21_PS_20261016";
CGxShader* Program(CGxDevice& device,EGxShTarget target,const char* name,const uint8_t* code,uint32_t size){
 auto* shader=device.m_shaderList[target].Ptr(name);
 if(shader){if(shader->m_target!=target||shader->m_code.Count()!=size||std::memcmp(shader->m_code.m_data,code,size))return nullptr;}
 else{
  shader=device.m_shaderList[target].New(name,0,0);if(!shader)return nullptr;
  shader->m_refCount=1;shader->m_target=target;shader->m_code.SetCount(size);std::memcpy(shader->m_code.m_data,code,size);
 }
 // Native list/reset ownership preserves code and invalidates/recreates API handles.
 return shader->Valid()?shader:nullptr;
}
struct CompilerModule {
 HMODULE value=nullptr;
 ~CompilerModule(){if(value)FreeLibrary(value);}
 bool Open(){
  wchar_t path[MAX_PATH]{};const UINT length=GetSystemDirectoryW(path,MAX_PATH);
  constexpr wchar_t suffix[]=L"\\d3dcompiler_47.dll";
  if(!length||length>=MAX_PATH||length+sizeof(suffix)/sizeof(wchar_t)>MAX_PATH)return false;
  std::memcpy(path+length,suffix,sizeof(suffix));value=LoadLibraryW(path);return value!=nullptr;
 }
};
struct Blob {ID3DBlob* value=nullptr;~Blob(){if(value)value->Release();}};
CGxShader* WeightedProgram(CGxDevice& device){
 if(auto* existing=device.m_shaderList[GxSh_Vertex].Ptr(WeightedVertexName))
  return existing->m_target==GxSh_Vertex&&existing->m_code.Count()&&existing->Valid()?existing:nullptr;
 auto* shader=device.m_shaderList[GxSh_Vertex].New(WeightedVertexName,0,0);if(!shader)return nullptr;
 shader->m_refCount=1;shader->m_target=GxSh_Vertex;
 // This empty owner also caches a compiler refusal until device teardown. Never
 // recreate/load/compile a missing shader on every emitter draw.
 CompilerModule compiler;Blob code,errors;
 if(!compiler.Open()) {WLOG_WARN("native-mesh: weighted AddAlpha unavailable: system d3dcompiler_47.dll missing");return nullptr;}
 auto compile=reinterpret_cast<decltype(&D3DCompile)>(GetProcAddress(compiler.value,"D3DCompile"));
 if(!compile){WLOG_WARN("native-mesh: weighted AddAlpha unavailable: D3DCompile export missing");return nullptr;}
 const HRESULT result=compile(kCombinersModAddAlphaSkinnedSource,sizeof(kCombinersModAddAlphaSkinnedSource)-1,"WXL/CombinersModAddAlphaSkinned.vs.hlsl",nullptr,nullptr,"main","vs_3_0",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code.value,&errors.value);
 if(FAILED(result)||!code.value){
  WLOG_WARN("native-mesh: weighted AddAlpha compilation refused hr=0x%08x detail=%.*s",unsigned(result),errors.value?int((std::min)(size_t(2048),errors.value->GetBufferSize())):0,errors.value?static_cast<const char*>(errors.value->GetBufferPointer()):"");return nullptr;
 }
 const auto bytes=code.value->GetBufferSize();
 if(bytes<8||bytes>65536||bytes%4||Read<uint32_t>(static_cast<const uint8_t*>(code.value->GetBufferPointer()),0)!=0xfffe0300||Read<uint32_t>(static_cast<const uint8_t*>(code.value->GetBufferPointer()),bytes-4)!=0x0000ffff)return nullptr;
 shader->m_code.SetCount(uint32_t(bytes));std::memcpy(shader->m_code.m_data,code.value->GetBufferPointer(),bytes);
 return shader->Valid()?shader:nullptr;
}
bool WeightedDeclaration(const CGxDevice& device){
 if(device.m_primVertexFormat!=GxVBF_PBNT2||device.m_primVertexSize!=48)return false;
#ifdef WXL_ENABLE_DX11
 // Native DX11 maps the same PBNT2 descriptor to UINT byte indices and
 // normalized byte weights; it never reads a D3D9 declaration table.
 if(device.m_api==GxApi_D3d11)return true;
#endif
 const auto* types=device.m_api==GxApi_D3d9?CGxDeviceD3d::s_gxAttribToD3dAttribType:CGxDeviceD3d9Ex::s_gxAttribToD3dAttribType;
 return types[1]==D3DDECLTYPE_UBYTE4&&types[2]==D3DDECLTYPE_UBYTE4N;
}
void UploadPalette(CGxDevice& device,const CM2Model& model,const SectionSource& section){
 float* rows=device.ShaderConstantsLock(GxSh_Vertex)+31*4;
 for(const auto index:section.palette){const auto& bone=model.m_boneMatrices[index];
  const float native[]={bone.a0,bone.b0,bone.c0,bone.d0,bone.a1,bone.b1,bone.c1,bone.d1,bone.a2,bone.b2,bone.c2,bone.d2};std::memcpy(rows,native,sizeof(native));rows+=12;
 }
 device.ShaderConstantsUnlock(GxSh_Vertex,31,uint32_t(section.palette.size())*3);
}
void SyncCpuState(CGxDevice& device){SyncMaterialState(device);}
struct CpuState {
 CGxDevice& device;CGxVertexAttrib attributes[14];CGxBuf* buffers[14];uint32_t mask,size;EGxVertexBufferFormat format;CGxBuf* vertices;CGxBuf* indices;float rows[12];
 explicit CpuState(CGxDevice& value):device(value),mask(value.m_primVertexMask),size(value.m_primVertexSize),format(value.m_primVertexFormat),vertices(value.m_primVertexBuf),indices(value.m_primIndexBuf){
  std::memcpy(attributes,value.m_primVertexFormatAttrib,sizeof(attributes));std::memcpy(buffers,value.m_primVertexFormatBuf,sizeof(buffers));std::memcpy(rows,CGxDevice::s_shadowConstants[1].constants+31*4,sizeof(rows));device.RsPush();
 }
 ~CpuState(){
  device.ShaderConstantsSet(GxSh_Vertex,31,rows,3);
  for(uint32_t slot=0;slot<14;++slot)if((mask&(1u<<slot))&&buffers[slot])device.PrimVertexFormat(buffers[slot],attributes+slot,1);
  std::memcpy(device.m_primVertexFormatAttrib,attributes,sizeof(attributes));std::memcpy(device.m_primVertexFormatBuf,buffers,sizeof(buffers));device.PrimVertexMask(mask);
  device.m_primVertexFormat=format;device.m_primVertexBuf=vertices;device.m_primVertexSize=size;device.PrimIndexPtr(indices);
  device.RsPop();SyncCpuState(device);
 }
};
static_assert(sizeof(M2Vertex)==48&&sizeof(M2VertexPNT2)==40,"native captured CPU vertex ABI" );
bool DrawCpu(CM2SceneRender& context,const CGxBatch& draw,const SectionSource& section,CGxShader* vertex,CGxShader* pixel){
 auto& device=*g_theGxDevicePtr;
 if(section.cpuVertices.empty()||section.cpuVertices.size()>65535||section.cpuIndices.empty()||draw.m_count!=section.cpuIndices.size())return false;
 // Synchronize the current native binding before taking its typed restore image.
 SyncCpuState(device);
 if(device.m_primVertexFormat<GxVertexBufferFormats_Last&&!device.m_primVertexBuf)return false;
 for(uint32_t slot=0;slot<14;++slot)if((device.m_primVertexMask&(1u<<slot))&&!device.m_primVertexFormatBuf[slot])return false;
 auto* vb=device.BufStream(GxPoolTarget_Vertex,sizeof(M2VertexPNT2),uint32_t(section.cpuVertices.size()));if(!vb)return false;
 auto* out=reinterpret_cast<M2VertexPNT2*>(device.BufLock(vb));if(!out)return false;
 for(size_t index=0;index<section.cpuVertices.size();++index){
  M2Vertex source;std::memcpy(&source,section.cpuVertices[index].data(),sizeof(source));C44Matrix matrix;
  M2BlendBoneMatrices(context.m_curModel->m_boneMatrices,source.m_weights,source.m_indices,&matrix);
  auto& target=out[index];target.m_position=source.m_position*matrix;
  target.m_normal.x=source.m_normal.z*matrix.c0+source.m_normal.x*matrix.a0+matrix.b0*source.m_normal.y;
  target.m_normal.y=source.m_normal.z*matrix.c1+source.m_normal.x*matrix.a1+matrix.b1*source.m_normal.y;
  target.m_normal.z=source.m_normal.z*matrix.c2+source.m_normal.x*matrix.a2+matrix.b2*source.m_normal.y;
  target.m_texcoord[0]=source.m_texcoord[0];target.m_texcoord[1]=source.m_texcoord[1];
 }
 device.BufUnlock(vb,0);vb->m_hasData=1;
 auto* ib=device.BufStream(GxPoolTarget_Index,sizeof(uint16_t),uint32_t(section.cpuIndices.size()));if(!ib)return false;
 void* indices=device.BufLock(ib);if(!indices)return false;std::memcpy(indices,section.cpuIndices.data(),section.cpuIndices.size()*sizeof(uint16_t));device.BufUnlock(ib,0);ib->m_hasData=1;
 CpuState state(device);
 // Native CPU positions/normals are already in view space. Preserve donor
 // projection/UV/light/colour/grazing using identity for its single affine row.
 const float identity[]={1,0,0,0,0,1,0,0,0,0,1,0};device.ShaderConstantsSet(GxSh_Vertex,31,identity,3);
 device.PrimVertexPtr(vb,GxVBF_PNT2);device.PrimIndexPtr(ib);device.RsSet(GxRs_VertexShader,vertex);device.RsSet(GxRs_PixelShader,pixel);
 CGxBatch local=draw;local.m_start=0;local.m_minIndex=0;local.m_maxIndex=uint32_t(section.cpuVertices.size()-1);device.Draw(&local,1);return true;
}
struct StateScope {
 CGxDevice& device;explicit StateScope(CGxDevice& d):device(d){device.RsPush();}
 ~StateScope(){device.RsPop();SyncMaterialState(device);}
};
}
bool OwnsBatch(const CM2SceneRender& context) noexcept {
 if(!context.m_curShared)return false;const auto* source=MeshSourceFor(*context.m_curShared);if(!source)return false;
 if(!source->dnd)return true;
 if(!context.m_curShared->m_skinData)return true;const auto& skin=*context.m_curShared->m_skinData;
 if(skin.m_batches.m_count!=3)return true;
 // Only the donor's first two passes own a custom shader. The third stays native.
 return context.m_curBatch!=&skin.m_batches[2];
}
bool Draw(CM2SceneRender& context,CGxBatch& draw){
 if(!MaterialsEnabled()||!D3D9()||!CShaderEffect::s_enableShaders||context.m_curType!=0||!context.m_curShared||!context.m_curShared->m_data||!context.m_curShared->m_skinData||!context.m_curModel||!context.m_curBatch||!context.m_curSkinSection||!context.m_curMaterial||!context.m_curElement||context.m_pass!=M2PASS_0)return false;
 const auto* source=MeshSourceFor(*context.m_curShared);if(!source)return false;
 if(source->dnd){const auto& skin=*context.m_curShared->m_skinData;if(skin.m_batches.m_count!=3)return false;for(uint32_t i=0;i<2;++i)if(&skin.m_batches[i]==context.m_curBatch)return DrawDnd(context,draw,*source,i);return false;}
 const auto& data=*context.m_curShared->m_data;const auto& skin=*context.m_curShared->m_skinData;const auto& b=*context.m_curBatch;
 if(source->batches.empty()||skin.m_batches.m_count!=source->batches.size()||skin.m_skinSections.m_count!=source->sectionCount||data.m_textures.m_count!=source->textureCount)return false;
 size_t sourceIndex=source->batches.size();
 for(uint32_t i=0;i<skin.m_batches.m_count;++i)if(&skin.m_batches[i]==context.m_curBatch){sourceIndex=i;break;}
 if(sourceIndex>=source->batches.size())return false;
 const auto& record=source->batches[sourceIndex];const auto* raw=record.batch.data();
 if(!AddAlphaFamily(Read<uint16_t>(raw,2))||(Read<uint16_t>(raw,2)==0x8021&&RiftPath(source->modelPath.c_str()))||b.m_skinSectionIndex!=Read<uint16_t>(raw,4)||b.m_skinSectionIndex>=skin.m_skinSections.m_count||!context.m_curShared->m_skinSections||&context.m_curShared->m_skinSections[b.m_skinSectionIndex]!=context.m_curSkinSection)return false;
 if(!context.m_curModel->m_boneMatrices||data.m_bones.m_count!=source->boneCount||b.m_skinSectionIndex>=source->sections.size())return false;
 const auto& captured=source->sections[b.m_skinSectionIndex];const auto& section=*context.m_curSkinSection;
 if(source->cpuSkinning){
  if(!context.m_curShared->m_cache||(context.m_curShared->m_cache->m_flags&8)||(context.m_curElement->m_flags&4)||section.m_vertexStart!=captured.firstVertex||section.m_vertexCount!=captured.vertexCount||section.m_boneInfluences!=captured.influences||captured.cpuVertices.size()!=section.m_vertexCount||captured.cpuIndices.size()!=section.m_indexCount)return false;
 }else if(source->weighted){
  if(section.m_vertexStart!=captured.firstVertex||section.m_vertexCount!=captured.vertexCount||section.m_boneInfluences!=captured.influences||section.m_boneComboIndex!=captured.paletteOffset||section.m_boneCount!=captured.palette.size()||uint32_t(section.m_boneComboIndex)+section.m_boneCount>data.m_boneCombos.m_count)return false;
  for(uint32_t bone=0;bone<section.m_boneCount;++bone)if(data.m_boneCombos[section.m_boneComboIndex+bone]!=captured.palette[bone])return false;
  if(!context.m_curShared->m_cache||!(context.m_curShared->m_cache->m_flags&8)||g_theGxDevicePtr->Caps().m_shaderConstants[GxSh_Vertex]<256||!WeightedDeclaration(*g_theGxDevicePtr)||!g_theGxDevicePtr->m_primVertexBuf||!g_theGxDevicePtr->m_primVertexBuf->m_hasData||g_theGxDevicePtr->m_primVertexBuf!=context.m_curShared->m_vertexBuf)return false; }else if(section.m_boneCount!=1||section.m_boneComboIndex>=data.m_boneCombos.m_count||data.m_boneCombos[section.m_boneComboIndex]!=0)return false; if(b.m_shader!=0x11||b.m_textureCount!=2||b.m_textureCoordComboIndex!=4||b.m_materialLayer)return false;
 if(b.m_materialIndex!=Read<uint16_t>(raw,10)||b.m_textureComboIndex!=Read<uint16_t>(raw,16)||b.m_colorIndex!=Read<uint16_t>(raw,8)||b.m_textureWeightComboIndex!=Read<uint16_t>(raw,20)||b.m_textureTransformComboIndex!=Read<uint16_t>(raw,22)||context.m_curMaterial->m_blendMode!=record.blend||context.m_curMaterial->m_flags!=record.flags)return false;
 for(uint32_t i=0;i<2;++i){
  if(b.m_textureComboIndex+i>=data.m_textureCombos.m_count||data.m_textureCombos[b.m_textureComboIndex+i]!=record.textureIndices[i]||!context.m_curModel->m_textures[record.textureIndices[i]])return false;
  if(b.m_textureWeightComboIndex+i>=data.m_textureWeightCombos.m_count||data.m_textureWeightCombos[b.m_textureWeightComboIndex+i]!=record.weightIndices[i]||b.m_textureTransformComboIndex+i>=data.m_textureTransformCombos.m_count||data.m_textureTransformCombos[b.m_textureTransformComboIndex+i]!=record.transformIndices[i])return false;
 }
 try{auto* vertex=source->weighted&&!source->cpuSkinning?WeightedProgram(*g_theGxDevicePtr):Program(*g_theGxDevicePtr,GxSh_Vertex,VertexName,kCombinersModAddAlphaVs,sizeof(kCombinersModAddAlphaVs));
  auto* pixel=Program(*g_theGxDevicePtr,GxSh_Pixel,PixelName,kCombinersModAddAlphaPs,sizeof(kCombinersModAddAlphaPs));
  if(!vertex||!pixel)return false;
  if(source->cpuSkinning)return DrawCpu(context,draw,captured,vertex,pixel);
  StateScope state(*g_theGxDevicePtr);
  if(source->weighted)UploadPalette(*g_theGxDevicePtr,*context.m_curModel,captured);
  g_theGxDevicePtr->RsSet(GxRs_VertexShader,vertex);g_theGxDevicePtr->RsSet(GxRs_PixelShader,pixel);
  g_theGxDevicePtr->Draw(&draw,1);return true;
 }catch(...){return false;}
}
void DestroyPrograms(CGxDevice& device) noexcept {
 DestroyDndPrograms(device);
 try{auto* pixel=device.m_shaderList[GxSh_Pixel].Ptr(PixelName);if(pixel)device.ShaderDestroy(pixel);
  auto* vertex=device.m_shaderList[GxSh_Vertex].Ptr(VertexName);if(vertex)device.ShaderDestroy(vertex);
  auto* weighted=device.m_shaderList[GxSh_Vertex].Ptr(WeightedVertexName);if(weighted)device.ShaderDestroy(weighted);
 }catch(...){}
}
}
