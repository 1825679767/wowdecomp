// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "FdidCompat.hpp"
#include "ParticleCompat.hpp"
#include "ParticleRefraction.hpp"
#include "MaterialCompat.hpp"
#include "MeshMaterials.hpp"
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
struct Skin {std::vector<uint8_t> original,prepared,converted;std::shared_ptr<const mesh_materials::Source> mesh;};
}
struct ModelAssets {std::string meshPath;const uint8_t* meshInput=nullptr;size_t meshInputSize=0;std::shared_ptr<const mesh_materials::Source> selectedMesh;std::map<std::string,Skin> skins;uint32_t profile{};ParticleCutoffSet cutoffs;ParticleLayerSet layers;std::shared_ptr<const ParticleRefractionRecipe> refraction;};
namespace {
thread_local ModelAssets* loading{};
bool ReadCompanion(const char* path,std::vector<uint8_t>& output){
 if(!ReadAsset(path,output))return false;
 const auto key=Key(path);if(loading && key.ends_with(".skin")){auto& skin=loading->skins[key];skin.original=output;skin.mesh=mesh_materials::Prepare(loading->meshPath.c_str(),loading->meshInput,loading->meshInputSize,skin.original,skin.prepared);if(!skin.mesh)mesh_materials::PrepareNativeCompatibilitySkin(loading->meshPath.c_str(),loading->meshInput,loading->meshInputSize,skin.original,skin.prepared);if(!skin.prepared.empty())output=skin.prepared;}
 return true;
}

}
bool PrepareModel(CM2Shared& shared){
 if(!shared.m_data || shared.m_fileSize<8)return false;
 const auto* raw=reinterpret_cast<const uint8_t*>(shared.m_data);
 if(std::memcmp(raw,"MD21",4))return true;
 if(!AssetsEnabled())return true; // Native unsupported-format handling remains authoritative.
 try{auto assets=std::make_unique<ModelAssets>();assets->meshPath=shared.m_fileName;assets->meshInput=raw;assets->meshInputSize=shared.m_fileSize;std::vector<uint8_t> converted;wxl::waystone::m2bytes::Stats stats;
  assets->refraction=CaptureParticleRefraction(shared.m_fileName,raw,shared.m_fileSize);
  std::string layerReason;
  if(!CaptureParticleLayers(shared.m_fileName,raw,shared.m_fileSize,assets->layers,layerReason)){
   WLOG_WARN("native-modern-layers: refused path=%s reason=%s",shared.m_fileName,layerReason.c_str());return false;
  }
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
  if(!wxl::waystone::m2bytes::ConvertForNativeRuntime(shared.m_fileName,{raw,shared.m_fileSize},&ResolveTexture,&ReadCompanion,converted,stats,ParticlesEnabled(),true,paletteLimit)){
   WLOG_WARN("native-modern-model: refused path=%s reason=%s",shared.m_fileName,stats.reason.c_str());return false;
  }
  std::string particleReason;
  if(!assets->layers.empty()){
   uint32_t count=0;if(converted.size()<sizeof(M2Data))return false;
   std::memcpy(&count,converted.data()+offsetof(M2Data,m_particles),sizeof(count));
   if(count!=assets->layers.size())return false;
  }
  if(!ApplyParticleExtensions({raw,shared.m_fileSize},converted,particleReason,&assets->cutoffs,&assets->layers)){
   WLOG_WARN("native-modern-particles: refused path=%s reason=%s",shared.m_fileName,particleReason.c_str());return false;
  }
  PreserveMaterials({raw,shared.m_fileSize},converted);
  for(const auto& [name,skin]:assets->skins)if(skin.mesh&&!mesh_materials::ApplyPreparedMaterials(*skin.mesh,converted))return false;
  for(auto& [name,skin]:assets->skins){if(!wxl::waystone::m2bytes::ConvertSkin(!skin.prepared.empty()?skin.prepared:skin.original,skin.converted,true))return false;if(skin.mesh&&!mesh_materials::ConvertedMatches(*skin.mesh,skin.converted))return false;}
  assets->meshInput=nullptr;assets->meshInputSize=0;
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
  shared.m_wxlAssets->selectedMesh=skin.mesh;std::memcpy(replacement,skin.converted.data(),skin.converted.size());SIMD::FreeA16(shared.m_skinData);shared.m_skinData=replacement;size=uint32_t(skin.converted.size());return true;
 }catch(...){return false;}
}
std::shared_ptr<const ParticleCutoffTrack> ParticleCutoffFor(const CM2Shared& shared,uint32_t index) noexcept {
 if(!shared.m_wxlAssets || index>=shared.m_wxlAssets->cutoffs.size())return {};
 return shared.m_wxlAssets->cutoffs[index];
}
std::shared_ptr<const ParticleRefractionRecipe> ParticleRefractionFor(const CM2Shared& shared,uint32_t index) noexcept {
 return shared.m_wxlAssets && index==0 ? shared.m_wxlAssets->refraction : nullptr;
}
std::shared_ptr<const ParticleLayerRecipe> ParticleLayersFor(const CM2Shared& shared,uint32_t index) noexcept {
 if(!shared.m_wxlAssets || index>=shared.m_wxlAssets->layers.size())return {};
 return shared.m_wxlAssets->layers[index];
}
const mesh_materials::Source* MeshSourceFor(const CM2Shared& shared) noexcept {return shared.m_wxlAssets?shared.m_wxlAssets->selectedMesh.get():nullptr;}
void ReleaseModel(CM2Shared& shared) noexcept {delete shared.m_wxlAssets;shared.m_wxlAssets=nullptr;}
}
