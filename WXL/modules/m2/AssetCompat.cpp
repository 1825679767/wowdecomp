// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "FdidCompat.hpp"
#include "ParticleCompat.hpp"
#include "MaterialCompat.hpp"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include <algorithm>
#include "platform/waystone/M2Bytes.hpp"
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
std::string Key(const char* path){std::string key=path?path:"";for(auto& c:key){if(c=='/')c='\\';if(c>='A' && c<='Z')c=char(c-'A'+'a');}return key;}
struct Skin {std::vector<uint8_t> original,converted;};
}
struct ModelAssets {std::map<std::string,Skin> skins;uint32_t profile{};ParticleCutoffSet cutoffs;};
namespace {
thread_local ModelAssets* loading{};
bool ReadCompanion(const char* path,std::vector<uint8_t>& output){
 if(!ReadAsset(path,output))return false;
 const auto key=Key(path);if(loading && key.ends_with(".skin"))loading->skins[key].original=output;
 return true;
}

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
  if(!ApplyParticleExtensions({raw,shared.m_fileSize},converted,particleReason,&assets->cutoffs)){
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
std::shared_ptr<const ParticleCutoffTrack> ParticleCutoffFor(const CM2Shared& shared,uint32_t index) noexcept {
 if(!shared.m_wxlAssets || index>=shared.m_wxlAssets->cutoffs.size())return {};
 return shared.m_wxlAssets->cutoffs[index];
}
void ReleaseModel(CM2Shared& shared) noexcept {delete shared.m_wxlAssets;shared.m_wxlAssets=nullptr;}
}
