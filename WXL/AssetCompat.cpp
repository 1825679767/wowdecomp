// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "FdidCompat.hpp"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include <algorithm>
#include <cmath>
#include "platform/waystone/M2Bytes.hpp"
#include "engine/assets/shared/textures/blp/BlpTranscode.hpp"
#include "common/Log.hpp"
#include <cstring>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <span>
namespace wxl::native {
namespace {
constexpr uint32_t Limit=64u*1024*1024;
template<class T>T Read(const uint8_t* p,size_t at){T v;std::memcpy(&v,p+at,sizeof(v));return v;}
std::string Key(const char* path){std::string key=path?path:"";for(auto& c:key){if(c=='/')c='\\';if(c>='A' && c<='Z')c=char(c-'A'+'a');}return key;}
struct Skin {std::vector<uint8_t> original,converted;};
}
struct ModelAssets {std::map<std::string,Skin> skins;uint32_t profile{};};
namespace {
thread_local ModelAssets* loading{};
bool ReadCompanion(const char* path,std::vector<uint8_t>& output){
 if(!ReadAsset(path,output))return false;
 const auto key=Key(path);if(loading && key.ends_with(".skin"))loading->skins[key].original=output;
 return true;
}
// EXP2's supported subset is a constant RGB/alpha multiplier and zero cutoff.
// Bake it into private native key arrays before relocation. Native simulation,
// lifetime interpolation, batching and render ownership remain unchanged.
bool ApplyParticleExtensions(std::span<const uint8_t> raw,std::vector<uint8_t>& native,std::string& reason){
 auto fail=[&](const char* text){reason=text;return false;};
 if(!ParticlesEnabled())return true;
 if(raw.size()<8 || native.size()<sizeof(M2Data))return fail("particle extension header bounds");
 const auto bodySize=Read<uint32_t>(raw.data(),4);
 if(bodySize>raw.size()-8 || bodySize<304)return fail("particle extension body bounds");
 constexpr auto particleField=offsetof(M2Data,m_particles);
 const auto count=Read<uint32_t>(raw.data()+8,particleField);
 if(!count)return true;
 if(count>65535 || count!=Read<uint32_t>(native.data(),particleField))return fail("particle extension emitter identity");
 const auto materials=Read<uint32_t>(raw.data()+8,offsetof(M2Data,m_materials));
 std::span<const uint8_t> txac,exp2;bool seenTxac=false,seenExp2=false;
 for(size_t at=0;at<raw.size();){
  if(raw.size()-at<8)return fail("particle extension chunk header bounds");
  const auto length=Read<uint32_t>(raw.data(),at+4);
  if(length>raw.size()-at-8)return fail("particle extension chunk bounds");
  const auto payload=raw.subspan(at+8,length);
  if(!std::memcmp(raw.data()+at,"TXAC",4)){
   if(seenTxac)return fail("duplicate TXAC");seenTxac=true;txac=payload;
  }else if(!std::memcmp(raw.data()+at,"EXP2",4)){
   if(seenExp2)return fail("duplicate EXP2");seenExp2=true;exp2=payload;
  }else if(!std::memcmp(raw.data()+at,"EXPT",4)){
   return fail("EXPT particle semantics are not supported");
  }
  at+=8+length;
 }
 if(!seenExp2)return true; // Existing single-texture compatibility path is unchanged.
 if(seenTxac){
  if((uint64_t(materials)+count)*2!=txac.size())return fail("TXAC row count");
  for(uint32_t i=0;i<count;++i){const auto at=2*(uint64_t(materials)+i);
   if(txac[at] || txac[at+1])return fail("EXP2 requires neutral particle TXAC");
  }
 }
 auto fits=[](size_t limit,uint32_t at,uint32_t n,size_t stride){return at<=limit && n<=(limit-at)/stride;};
 if(exp2.size()<8)return fail("EXP2 header bounds");
 const auto records=Read<uint32_t>(exp2.data(),0),offset=Read<uint32_t>(exp2.data(),4);
 if(records!=count || offset<8 || !fits(exp2.size(),offset,count,28))return fail("EXP2 record bounds/count");
 const auto particlesAt=Read<uint32_t>(native.data(),particleField+4);
 if(!fits(native.size(),particlesAt,count,sizeof(M2Particle)))return fail("native particle bounds");
 for(uint32_t i=0;i<count;++i){
  const auto rec=offset+i*28;
  const float extra=Read<float>(exp2.data(),rec),rgb=Read<float>(exp2.data(),rec+4),alpha=Read<float>(exp2.data(),rec+8);
  if(extra!=0 || !std::isfinite(rgb) || !std::isfinite(alpha) || rgb<0 || rgb>16 || alpha<0 || alpha>16)return fail("EXP2 scalar subset");
  const auto times=Read<uint32_t>(exp2.data(),rec+12),timesAt=Read<uint32_t>(exp2.data(),rec+16);
  const auto keys=Read<uint32_t>(exp2.data(),rec+20),keysAt=Read<uint32_t>(exp2.data(),rec+24);
  if(times!=keys || keys>4096 || !fits(exp2.size(),timesAt,times,2) || !fits(exp2.size(),keysAt,keys,2) || (keys && (!timesAt || !keysAt)))return fail("EXP2 cutoff bounds");
  uint16_t previous=0;
  for(uint32_t k=0;k<keys;++k){const auto t=Read<uint16_t>(exp2.data(),timesAt+k*2);
   if(Read<uint16_t>(exp2.data(),keysAt+k*2) || t>32767 || (k && t<previous))return fail("EXP2 nonzero/invalid cutoff");previous=t;
  }
  // Every emitter owns its appended copies even if exporters alias key arrays.
  const auto particle=particlesAt+i*sizeof(M2Particle);
  for(unsigned component=0;component<2;++component){
   const float multiplier=component?alpha:rgb;if(multiplier==1)continue;
   const auto track=particle+(component?offsetof(M2Particle,m_alphaTrack):offsetof(M2Particle,m_colorTrack));
   const auto values=track+(component?offsetof(M2PartTrack<fixed16>,m_values):offsetof(M2PartTrack<C3Vector>,m_values));
   const auto n=Read<uint32_t>(native.data(),values),source=Read<uint32_t>(native.data(),values+4);
   const size_t stride=component?sizeof(fixed16):sizeof(C3Vector);
   if(n>65535 || !fits(native.size(),source,n,stride))return fail("native particle key bounds");
   if(!n)continue;
   const auto aligned=(native.size()+3)&~size_t(3);
   const auto bytes=size_t(n)*stride;
   if(aligned>Limit || bytes>Limit-aligned)return fail("particle key byte budget");
   std::vector<uint8_t> copy(native.begin()+source,native.begin()+source+bytes);
   if(component){
    for(uint32_t k=0;k<n;++k){const auto original=Read<int16_t>(copy.data(),k*2);const float value=original*multiplier;
     if(original<0 || value>32767)return fail("EXP2 alpha exceeds native range");
     const int16_t result=static_cast<int16_t>(std::lround(value));std::memcpy(copy.data()+k*2,&result,2);
    }
   }else{
    for(size_t k=0;k<bytes;k+=4){const auto original=Read<float>(copy.data(),k);const float value=original*multiplier;
     if(!std::isfinite(original) || original<0 || value>255)return fail("EXP2 RGB exceeds native range");
     std::memcpy(copy.data()+k,&value,4);
    }
   }
   native.resize(aligned,0);native.insert(native.end(),copy.begin(),copy.end());
   const auto target=static_cast<uint32_t>(aligned);std::memcpy(native.data()+values+4,&target,4);
  }
 }
 return true;
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
bool ReadAsset(const char* path,std::vector<uint8_t>& output){
 if(!path || !*path)return false;void* data=nullptr;uint32_t size=0;
 if(!SFile::LoadFile(path,&data,&size,0,nullptr) || !data)return false;
 bool ok=false;try{if(size && size<=Limit){std::vector<uint8_t> next(static_cast<const uint8_t*>(data),static_cast<const uint8_t*>(data)+size);output.swap(next);ok=true;}}catch(...){}
 SFile::Unload(data);return ok;
}
bool ConvertBlp(const void* raw,uint32_t size,std::vector<uint8_t>& output){
 if(!BlpEnabled() || !raw || size>Limit)return false;
 try{return wxl::modern::assets::textures::blp::TranscodeBlp({static_cast<const uint8_t*>(raw),size},output);}catch(...){return false;}
}
bool PrepareModel(CM2Shared& shared){
 if(!shared.m_data || shared.m_fileSize<8)return false;
 const auto* raw=reinterpret_cast<const uint8_t*>(shared.m_data);
 if(std::memcmp(raw,"MD21",4))return true;
 if(!AssetsEnabled())return true; // Native unsupported-format handling remains authoritative.
 try{auto assets=std::make_unique<ModelAssets>();std::vector<uint8_t> converted;wxl::waystone::m2bytes::Stats stats;
  struct Scope{ModelAssets* prior{loading};explicit Scope(ModelAssets* value){loading=value;}~Scope(){loading=prior;}} scope(assets.get());
  uint32_t paletteLimit=75;
  // Same typed capability calculation as native CM2Shared::Initialize. A D3D9
  // backend may expose fewer constants than the former15595/DX11 host.
  if(shared.m_cache && (shared.m_cache->m_flags&8)){
   if(!g_theGxDevicePtr)return false;
   const auto constants=g_theGxDevicePtr->Caps().m_shaderConstants[0];
   if(constants<=32)return false;
   paletteLimit=(std::min)(75u,(constants-(constants>0xc0?64u:32u))/3u);
   if(paletteLimit<3)return false;
  }
  if(!wxl::waystone::m2bytes::Convert(shared.m_fileName,{raw,shared.m_fileSize},&ResolveTexture,&ReadCompanion,converted,stats,ParticlesEnabled(),true,paletteLimit)){
   WLOG_WARN("native-modern-model: refused path=%s reason=%s",shared.m_fileName,stats.reason.c_str());return false;
  }
  std::string particleReason;
  if(!ApplyParticleExtensions({raw,shared.m_fileSize},converted,particleReason)){
   WLOG_WARN("native-modern-particles: refused path=%s reason=%s",shared.m_fileName,particleReason.c_str());return false;
  }
  PreserveMaterials({raw,shared.m_fileSize},converted);
  for(auto& [name,skin]:assets->skins)if(!wxl::waystone::m2bytes::ConvertSkin(skin.original,skin.converted,true))return false;
  auto* replacement=static_cast<M2Data*>(SIMD::AllocA16(converted.size(),__FILE__,__LINE__));if(!replacement)return false;
  std::memcpy(replacement,converted.data(),converted.size());SIMD::FreeA16(shared.m_data);
  shared.m_data=replacement;shared.m_fileSize=uint32_t(converted.size());shared.m_wxlAssets=assets.release();
  WLOG_INFO("native-modern-model: converted path=%s bones=%u skins=%u particles=%u omitted=%u",shared.m_fileName,shared.m_data->m_bones.m_count,stats.skins,stats.particles,stats.omittedEmitters);return true;
 }catch(...){return false;}
}
void SelectSkin(CM2Shared& shared,uint32_t profile){if(shared.m_wxlAssets)shared.m_wxlAssets->profile=profile;}
bool PrepareSkin(CM2Shared& shared,uint32_t& size){
 if(!shared.m_wxlAssets)return true;
 try{char suffix[32]{};std::snprintf(suffix,sizeof(suffix),"%02u.skin",shared.m_wxlAssets->profile);
  auto stem=Key(shared.m_fileName);const auto dot=stem.find_last_of('.');if(dot==std::string::npos)return false;stem.resize(dot);stem+=suffix;
  const auto it=shared.m_wxlAssets->skins.find(stem);if(it==shared.m_wxlAssets->skins.end())return false;const auto& skin=it->second;
  if(!shared.m_skinData || size!=skin.original.size() || std::memcmp(shared.m_skinData,skin.original.data(),size) || skin.converted.empty())return false;
  auto* replacement=static_cast<M2SkinProfile*>(SIMD::AllocA16(skin.converted.size(),__FILE__,__LINE__));if(!replacement)return false;
  std::memcpy(replacement,skin.converted.data(),skin.converted.size());SIMD::FreeA16(shared.m_skinData);shared.m_skinData=replacement;size=uint32_t(skin.converted.size());return true;
 }catch(...){return false;}
}
void ReleaseModel(CM2Shared& shared) noexcept {delete shared.m_wxlAssets;shared.m_wxlAssets=nullptr;}
}
