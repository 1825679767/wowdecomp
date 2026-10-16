// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "FdidCompat.hpp"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Shared.h"
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
  if(!wxl::waystone::m2bytes::Convert(shared.m_fileName,{raw,shared.m_fileSize},&ResolveTexture,&ReadCompanion,converted,stats,ParticlesEnabled(),false,75)){
   WLOG_WARN("native-modern-model: refused path=%s reason=%s",shared.m_fileName,stats.reason.c_str());return false;
  }
  PreserveMaterials({raw,shared.m_fileSize},converted);
  for(auto& [name,skin]:assets->skins)if(!wxl::waystone::m2bytes::ConvertSkin(skin.original,skin.converted))return false;
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
