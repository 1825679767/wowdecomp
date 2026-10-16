// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ParticleLayers.hpp"
#include "ParticleLayerMaterial.hpp"
#include "NativeFeatures.hpp"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Data.h"
#include "engine/assets/shared/models/m2/M2RecordWidths.hpp"
#include <cstring>
#include <string_view>
namespace wxl::native {
namespace {
template<class T>T Read(const uint8_t* p,size_t at){T value;std::memcpy(&value,p+at,sizeof(value));return value;}
bool Fits(size_t limit,uint32_t at,uint32_t n,size_t stride){return at<=limit && n<=(limit-at)/stride;}
float Scroll(uint16_t raw){return float(raw&0x7fff)*(raw&0x8000?-1.0f:1.0f)/512.0f;}
}
bool CaptureParticleLayers(const char* path,const uint8_t* raw,size_t size,ParticleLayerSet& output,std::string& reason){
 output.clear();auto fail=[&](const char* text){reason=text;return false;};
 if(!ParticlesEnabled())return true;
 if(!raw || size<8 || std::memcmp(raw,"MD21",4))return fail("layer source requires MD21");
 const auto bodySize=Read<uint32_t>(raw,4);const auto* body=raw+8;
 if(bodySize>size-8 || bodySize<304)return fail("layer source body bounds");
 const auto count=Read<uint32_t>(body,offsetof(M2Data,m_particles));
 const auto particlesAt=Read<uint32_t>(body,offsetof(M2Data,m_particles)+4);
 constexpr auto stride=wxl::structure::m2records::kParticleStrideModern;
 static_assert(stride>=0x1ec,"source layer scale/scroll payload width");
 if(count>65535 || !Fits(bodySize,particlesAt,count,stride))return fail("layer source emitter bounds");
 bool any=false;for(uint32_t i=0;i<count;++i)any|=(Read<uint32_t>(body,particlesAt+size_t(i)*stride+offsetof(M2Particle,m_flags))&wxl::structure::m2records::kParticleFlagMultiTex)!=0;
 if(!any)return true;
 auto* device=g_theGxDevicePtr;
 if(!device || (device->DeviceApi()!=GxApi_D3d9 && device->DeviceApi()!=GxApi_D3d9Ex) ||
    device->Caps().m_shaderTargets[GxSh_Vertex]<3 || device->Caps().m_shaderTargets[GxSh_Pixel]<4 ||
    device->Caps().m_shaderConstants[GxSh_Vertex]<35 || device->Caps().m_numTmus<3)return fail("layer source requires native D3D9 SM3 backend; OpenGL/DX11 deferred");
 if(Read<uint32_t>(body,4)!=274)return fail("layer source version is not 274");
 const auto global=Read<uint32_t>(body,offsetof(M2Data,m_flags));
 if(global!=0x3090u && global!=0x2090u)return fail("layer source global flags are outside the donor native-sprite family");
 const auto textures=Read<uint32_t>(body,offsetof(M2Data,m_textures));
 const auto texturesAt=Read<uint32_t>(body,offsetof(M2Data,m_textures)+4);
 const auto materials=Read<uint32_t>(body,offsetof(M2Data,m_materials));
 if(!Fits(bodySize,texturesAt,textures,sizeof(M2Texture)))return fail("layer source texture bounds");
 const uint8_t* txac=nullptr,*txid=nullptr;size_t txacBytes=0,txidBytes=0;uint32_t seen=0;
 for(size_t at=0;at<size;){
  if(size-at<8)return fail("layer source chunk header bounds");
  const auto n=Read<uint32_t>(raw,at+4);if(n>size-at-8)return fail("layer source chunk bounds");
  auto is=[&](const char* tag){return !std::memcmp(raw+at,tag,4);};uint32_t bit=0;
  if(is("MD21"))bit=1;else if(is("TXID")){bit=2;txid=raw+at+8;txidBytes=n;}else if(is("SFID")){bit=4;if(n%4)return fail("layer source SFID bounds");}
  else if(is("AFID"))bit=8;else if(is("SKID"))bit=16;else if(is("LDV1"))bit=32;
  else if(is("EXPT"))bit=64;else if(is("EXP2"))bit=128;
  else if(is("TXAC")){bit=256;txac=raw+at+8;txacBytes=n;}
  else return fail("layer source has unimplemented auxiliary chunk");
  if(seen&bit)return fail("layer source duplicate chunk");seen|=bit;at+=8+n;
 }
 if(txac && (uint64_t(materials)+count)*2!=txacBytes)return fail("layer source TXAC row bounds");
 // Whole-model source contract is established before any exceptional emitter
 // is admitted. Every multitexture record in these named families must match.
 auto samePath=[&](std::string_view target){if(!path)return false;const std::string_view name(path);if(name.size()!=target.size())return false;for(size_t i=0;i<name.size();++i){char c=name[i];if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c+'a'-'A');if(c!=target[i])return false;}return true;};
 uint32_t txac11=0;bool rowsKnown=true;
 if(txac)for(uint32_t i=0;i<count;++i){const size_t row=2*(uint64_t(materials)+i);if(txac[row]==1&&txac[row+1]==1){if(i<32)txac11|=1u<<i;else rowsKnown=false;}else if(txac[row]||txac[row+1])rowsKnown=false;}
 const bool simpleChunks=(seen&~(1u|2u|4u|256u))==0;
 const auto id=[&](uint32_t i){return txid && i<textures && txidBytes==uint64_t(textures)*4?Read<uint32_t>(txid,4*i):0u;};
 const bool boil=simpleChunks&&rowsKnown&&samePath("spells\\cfx_deathknight_bloodboil_castworld.m2")&&global==0x3090&&count==7&&textures==6&&txac&&txac11==0x42&&id(0)==1601218&&id(1)==1601219&&id(2)==1602396&&id(3)==1601199&&id(4)==1601200;
 const bool plague=simpleChunks&&rowsKnown&&samePath("spells\\cfx_deathknight_bloodplague_statechest.m2")&&global==0x2090&&count==2&&textures==3&&txac&&txac11==2&&id(0)==667363&&id(1)==1601219&&id(2)==1601220;
 if(global!=0x3090&&!plague)return fail("layer source 2090 requires exact Blood Plague family");
 auto bloodKind=[&](uint32_t i,const uint8_t* p){
  const auto flags=Read<uint32_t>(p,offsetof(M2Particle,m_flags));const auto blend=Read<uint8_t>(p,offsetof(M2Particle,m_blendMode));const auto packed=Read<uint16_t>(p,offsetof(M2Particle,m_textureIndex));
  if(boil&&(i==1||i==2||i==5||i==6)){
   const bool screen=i==2||i==5;const uint32_t expected=(i==1||i==2)?0x70831011u:0x70830011u;
   if(flags==expected&&blend==(screen?7:2)&&packed==(screen?4227:2113)&&p[0x2c]==(screen?6:32)&&p[0x2d]==(screen?3:16))return screen?BloodSpriteFamily::BoilScreen:BloodSpriteFamily::BoilSmoke;
  }
  if(plague&&i==1&&flags==0x70830011u&&blend==2&&packed==2113&&p[0x2c]==32&&p[0x2d]==16){
   const uint16_t scroll[]={0,32896,0,32896,0,0,0,0};for(unsigned j=0;j<8;++j)if(Read<uint16_t>(p,0x1dc+2*j)!=scroll[j])return BloodSpriteFamily::None;
   return BloodSpriteFamily::PlagueSmoke;
  }
  return BloodSpriteFamily::None;
 };
 if(boil||plague)for(uint32_t i=0;i<count;++i){const auto* p=body+particlesAt+size_t(i)*stride;const bool layered=(Read<uint32_t>(p,offsetof(M2Particle,m_flags))&wxl::structure::m2records::kParticleFlagMultiTex)!=0;
  const bool expected=boil?(i==1||i==2||i==5||i==6):(i==1);if(layered!=expected || (expected&&bloodKind(i,p)==BloodSpriteFamily::None))return fail("blood sprite whole-model source contract");
 }
 ParticleLayerSet captured(count);size_t bytes=0;
 for(uint32_t i=0;i<count;++i){
  const auto* p=body+particlesAt+size_t(i)*stride;const auto flags=Read<uint32_t>(p,offsetof(M2Particle,m_flags));
  if(!(flags&wxl::structure::m2records::kParticleFlagMultiTex))continue;
  // The same three-layer donor kernels feed distinct native framebuffer blends.
  // Capture their original identity before the native emitter is constructed.
  ParticleLayerMaterial material{};
  if(!ResolveParticleLayerMaterial(Read<uint8_t>(p,offsetof(M2Particle,m_blendMode)),material))return fail("layer source blend has no native layer material contract");
  // Native shaded/unshaded flags remain intact. Runtime selects the exact
  // retained donor VS for native sunlight plus zero, one or two point lights.
  // Modx4/depth/refraction/TXAC shader variants are not guessed from firsttex.
  // Head and tail are independently authored native quad shapes. Retain both
  // bits; native BuildVertex owns the tail trajectory, clamp and atlas choice.
  const auto blood=bloodKind(i,p);
  if(!(flags&0x60000u) || (flags&0x80100000u) || ((flags&0x20000000u)&&blood==BloodSpriteFamily::None))return fail("layer source requires head/tail quads without modx4/depth variants");
  for(const auto field:{offsetof(M2Particle,m_geometryMdl),offsetof(M2Particle,m_recursionMdl)}){
   const auto n=Read<uint32_t>(p,field),at=Read<uint32_t>(p,field+4);
   if(n && (n!=1 || !at || at>=bodySize || body[at]))return fail("layer source model/child particles are unsupported");
  }
  auto recipe=std::make_shared<ParticleLayerRecipe>();recipe->sourceFlags=flags;recipe->packedIndices=Read<uint16_t>(p,offsetof(M2Particle,m_textureIndex));
  recipe->sourceBlend=material.sourceBlend;recipe->bloodFamily=blood;
  recipe->hasTxac=txac!=nullptr;
  if(txac){const auto row=2*(uint64_t(materials)+i);recipe->txac={txac[row],txac[row+1]};}
  if((recipe->txac[0] || recipe->txac[1]) && (blood==BloodSpriteFamily::None || blood==BloodSpriteFamily::BoilScreen || recipe->txac!=std::array<uint8_t,2>{1,1}))return fail("layer source nonneutral TXAC shader is unimplemented");
  if(recipe->packedIndices&0x8000)return fail("layer source unknown packed texture bit");
  recipe->threeColor=(flags&0x40000000u)!=0;
  for(unsigned layer=0;layer<3;++layer){
   const auto index=uint16_t((recipe->packedIndices>>(5*layer))&31u);recipe->textures[layer]=index;if(blood!=BloodSpriteFamily::None)recipe->sourceTextureIds[layer]=id(index);
   if(index>=textures || Read<uint32_t>(body,texturesAt+size_t(index)*sizeof(M2Texture)+offsetof(M2Texture,m_type))!=0)return fail("layer source texture is not a file texture");
  }
  for(unsigned layer=0;layer<2;++layer){
   if(p[0x2c+layer]&0x80)return fail("layer source signed scale is unimplemented");
   recipe->scale[layer]=p[0x2c+layer]/32.0f;
   const auto at=0x1dc+layer*4;
   recipe->mid[layer]={Scroll(Read<uint16_t>(p,at)),Scroll(Read<uint16_t>(p,at+2))};
   recipe->range[layer]={Scroll(Read<uint16_t>(p,at+8)),Scroll(Read<uint16_t>(p,at+10))};
  }
  if(stride>64u*1024*1024-bytes)return fail("layer source ownership byte budget");bytes+=stride;
  recipe->sourceRecord.assign(p,p+stride);captured[i]=std::move(recipe);
 }
 output.swap(captured);return true;
}
}
