// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AssetCompat.hpp"
#include "DonorMaterialFallback.hpp"
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
struct Skin {std::vector<uint8_t> original,prepared,converted;std::shared_ptr<const mesh_materials::Source> mesh;std::shared_ptr<const DonorMaterialRecipe> donor;};
}
struct ModelAssets {bool donorFallback=false;std::vector<uint8_t> donorSourceModel;std::string meshPath;const uint8_t* meshInput=nullptr;size_t meshInputSize=0;std::shared_ptr<const mesh_materials::Source> selectedMesh;std::map<std::string,Skin> skins;uint32_t profile{};ParticleCutoffSet cutoffs;ParticleLayerSet layers;bool riftSource=false;std::shared_ptr<const ParticleRefractionRecipe> refraction;};
namespace {
thread_local ModelAssets* loading{};
bool ReadCompanion(const char* path,std::vector<uint8_t>& output){
 if(!ReadAsset(path,output))return false;
 const auto key=Key(path);
 if(loading && key.ends_with(".skin")){
  auto& skin=loading->skins[key];skin.original=output;
  skin.mesh=mesh_materials::Prepare(loading->meshPath.c_str(),loading->meshInput,loading->meshInputSize,skin.original,skin.prepared);
  if(!skin.mesh)mesh_materials::PrepareNativeCompatibilitySkin(loading->meshPath.c_str(),loading->meshInput,loading->meshInputSize,skin.original,skin.prepared);
  // Exact renderer-owned prepared skins have priority. Sample the optional
  // approximation policy once per model, never per batch or asynchronous read.
  if(skin.prepared.empty() && loading->donorFallback){
   std::string reason;
   if(!PrepareDonorMaterialSkin(loading->meshInput,loading->meshInputSize,skin.original,skin.prepared,skin.donor,reason)){
    WLOG_WARN("native-donor-material: refused model=%s skin=%s reason=%s",loading->meshPath.c_str(),path,reason.c_str());return false;
   }
  }
  if(!skin.prepared.empty())output=skin.prepared;
 }
 return true;
}

}
bool PrepareModel(CM2Shared& shared){
 if(!shared.m_data || shared.m_fileSize<8)return false;
 const auto* raw=reinterpret_cast<const uint8_t*>(shared.m_data);
 if(std::memcmp(raw,"MD21",4))return true;
 if(!AssetsEnabled())return true; // Native unsupported-format handling remains authoritative.
 try{auto assets=std::make_unique<ModelAssets>();assets->donorFallback=DonorMaterialFallbackEnabled();assets->meshPath=shared.m_fileName;assets->meshInput=raw;assets->meshInputSize=shared.m_fileSize;std::vector<uint8_t> converted;wxl::waystone::m2bytes::Stats stats;
  assets->refraction=CaptureParticleRefraction(shared.m_fileName,raw,shared.m_fileSize);
  assets->riftSource=MatchExactRiftSource(shared.m_fileName,raw,shared.m_fileSize);
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
  std::vector<std::vector<uint8_t>> rebuiltSkins;
  if(!wxl::waystone::m2bytes::ConvertForNativeRuntime(shared.m_fileName,{raw,shared.m_fileSize},&ResolveTexture,&ReadCompanion,converted,stats,ParticlesEnabled(),true,paletteLimit,&rebuiltSkins)){
   WLOG_WARN("native-modern-model: refused path=%s reason=%s",shared.m_fileName,stats.reason.c_str());return false;
  }
  std::string particleReason;
  if(!assets->layers.empty()){
   uint32_t count=0;if(converted.size()<sizeof(M2Data))return false;
   std::memcpy(&count,converted.data()+offsetof(M2Data,m_particles),sizeof(count));
   if(count!=assets->layers.size())return false;
  }
  if(!assets->riftSource && !ApplyParticleExtensions({raw,shared.m_fileSize},converted,particleReason,&assets->cutoffs,&assets->layers)){
   WLOG_WARN("native-modern-particles: refused path=%s reason=%s",shared.m_fileName,particleReason.c_str());return false;
  }
  PreserveMaterials({raw,shared.m_fileSize},converted);
  if(!rebuiltSkins.empty()){
   // Publish only model-owned output paired to the exact named source. The
   // normal native SKIN file read is still compared against original bytes.
   auto stem=Key(shared.m_fileName);const auto dot=stem.find_last_of('.');if(dot==std::string::npos)return false;stem.resize(dot);
   for(uint32_t profile=0;profile<rebuiltSkins.size();++profile){char suffix[16]{};std::snprintf(suffix,sizeof(suffix),"%02u.skin",profile);
    const auto it=assets->skins.find(stem+suffix);if(it==assets->skins.end()||rebuiltSkins[profile].empty())return false;
    // Renderer recipes contain original per-batch/section identities. A
    // topology rewrite must not silently reuse those identities after a split.
    if(it->second.mesh||it->second.donor)return false;
    it->second.converted=std::move(rebuiltSkins[profile]);
   }
   WLOG_INFO("native-modern-model: full topology repartitioned model=%s profiles=%zu device-palette=%u",shared.m_fileName,rebuiltSkins.size(),paletteLimit);
  }
  for(const auto& [name,skin]:assets->skins)if(skin.mesh&&!mesh_materials::ApplyPreparedMaterials(*skin.mesh,converted))return false;
  for(auto& [name,skin]:assets->skins){
   const auto& prepared=!skin.prepared.empty()?skin.prepared:skin.original;
   if(skin.donor && wxl::waystone::m2bytes::Fingerprint(prepared)!=skin.donor->preparedSkinFingerprint)return false;
   if(skin.converted.empty()&&!wxl::waystone::m2bytes::ConvertSkin(prepared,skin.converted,true))return false;
   if(skin.mesh&&!mesh_materials::ConvertedMatches(*skin.mesh,skin.converted))return false;
   if(skin.donor){
    std::string reason;
    if(!FinishDonorMaterialSkin(*skin.donor,converted,skin.converted,reason)){
     WLOG_WARN("native-donor-material: converted contract refused model=%s skin=%s reason=%s",shared.m_fileName,name.c_str(),reason.c_str());return false;
    }
    if(assets->donorSourceModel.empty())assets->donorSourceModel.assign(raw,raw+shared.m_fileSize);
    uint32_t rules[5]{};for(const auto& batch:skin.donor->batches)++rules[uint32_t(batch.rule)];
    WLOG_WARN("native-donor-material: donor approximation model=%s skin=%s source-hash=%llx skin-hash=%llx rebase=%u collapse=%u default-diffuse=%u env-split=%u uv1-split=%u; source bytes retained; retail shader parity not provided",shared.m_fileName,name.c_str(),static_cast<unsigned long long>(skin.donor->modelFingerprint),static_cast<unsigned long long>(skin.donor->sourceSkinFingerprint),rules[0],rules[1],rules[2],rules[3],rules[4]);
    uint32_t logged=0;for(const auto& batch:skin.donor->batches){if(logged++==16)break;WLOG_WARN("native-donor-material: source-batch=%u shader=0x%04x textures=%u rule=%u native-batch=%u",batch.sourceBatch,unsigned(batch.sourceShader),unsigned(batch.sourceTextureCount),unsigned(batch.rule),batch.firstNativeBatch);}
   }
  }
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
bool IsExactRiftSource(const CM2Shared& shared) noexcept {return shared.m_wxlAssets&&shared.m_wxlAssets->riftSource;}
std::shared_ptr<const ParticleLayerRecipe> ParticleLayersFor(const CM2Shared& shared,uint32_t index) noexcept {
 if(!shared.m_wxlAssets || index>=shared.m_wxlAssets->layers.size())return {};
 return shared.m_wxlAssets->layers[index];
}
const mesh_materials::Source* MeshSourceFor(const CM2Shared& shared) noexcept {return shared.m_wxlAssets?shared.m_wxlAssets->selectedMesh.get():nullptr;}
void ReleaseModel(CM2Shared& shared) noexcept {delete shared.m_wxlAssets;shared.m_wxlAssets=nullptr;}
}
