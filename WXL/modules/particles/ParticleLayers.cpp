// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ParticleLayers.hpp"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "shaders/ParticlePrograms.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace wxl::native {
namespace {
namespace programs=wxl::modern::particlelayers::programs;
uint32_t Mix(uint32_t x){x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;return x^(x>>16);}
float Unit(uint32_t x){return float(Mix(x)>>8)*(1.0f/16777216.0f);}
struct ShaderPair {
 CGxDevice& device;CGxShader* vertex{},*pixel{};
 explicit ShaderPair(CGxDevice& value):device(value){}
 // Programs retain one native device-list owner reference until GxDevDestroy.
 // Draws borrow them; no per-emitter GPU program creation/destruction.
 template<size_t N>bool Load(CGxShader*& shader,EGxShTarget target,const char* name,const uint32_t (&code)[N]){
  shader=device.m_shaderList[target].Ptr(name);
  if(shader){
   if(shader->m_code.Count()!=sizeof(code) || std::memcmp(shader->m_code.m_data,code,sizeof(code))){shader=nullptr;return false;}
  }else{
   shader=device.m_shaderList[target].New(name,0,0);if(!shader)return false;
   shader->m_refCount=1;shader->m_target=target;shader->m_code.SetCount(sizeof(code));
   std::memcpy(shader->m_code.m_data,code,sizeof(code));
  }
  return shader->Valid()!=0;
 }
};
// Gx owns the state cache and backend bindings. Restore through those same APIs,
// including constants, before the draw-local registered shader refs are released.
struct DrawScope {
 CGxDevice& device;std::array<CGxTex*,3> textures{};
 std::array<CGxTexFlags,3> flags;
 CGxVertexAttrib attribs[14];CGxBuf* buffers[14];uint32_t mask;CGxBuf* indices;
 float pixel0[4],pixel2[4],vertex2[24],vertex30[4],vertex31[16];C44Matrix view;
 explicit DrawScope(CGxDevice& d):device(d),mask(d.m_primVertexMask),indices(d.m_primIndexBuf){
  std::memcpy(attribs,d.m_primVertexFormatAttrib,sizeof(attribs));std::memcpy(buffers,d.m_primVertexFormatBuf,sizeof(buffers));
  std::memcpy(pixel0,CGxDevice::s_shadowConstants[0].constants,sizeof(pixel0));
  std::memcpy(pixel2,CGxDevice::s_shadowConstants[0].constants+2*4,sizeof(pixel2));
  std::memcpy(vertex2,CGxDevice::s_shadowConstants[1].constants+2*4,sizeof(vertex2));
  std::memcpy(vertex30,CGxDevice::s_shadowConstants[1].constants+30*4,sizeof(vertex30));
  std::memcpy(vertex31,CGxDevice::s_shadowConstants[1].constants+31*4,sizeof(vertex31));
  view=d.m_xforms[GxXform_View].m_mtx[d.m_xforms[GxXform_View].m_level];d.RsPush();
 }
 void SaveTextures(const std::array<CGxTex*,3>& value){textures=value;for(unsigned i=0;i<3;++i)flags[i]=textures[i]->m_flags;}
 ~DrawScope(){
  for(unsigned i=0;i<3;++i)if(textures[i])device.TexSetFlags(textures[i],flags[i]);
  device.ShaderConstantsSet(GxSh_Pixel,0,pixel0,1);
  device.ShaderConstantsSet(GxSh_Pixel,2,pixel2,1);
  device.ShaderConstantsSet(GxSh_Vertex,2,vertex2,6);
  device.ShaderConstantsSet(GxSh_Vertex,30,vertex30,1);
  device.ShaderConstantsSet(GxSh_Vertex,31,vertex31,4);
  device.XformSetView(view);
  for(unsigned i=0;i<14;++i)if((mask&(1u<<i)) && buffers[i])device.PrimVertexFormat(buffers[i],&attribs[i],1);
  device.PrimVertexMask(mask);device.PrimIndexPtr(indices);
  device.RsPop();
  for(unsigned i=0;i<3;++i)device.IRsForceUpdate(static_cast<EGxRenderState>(GxRs_Texture0+i));
  if(device.DeviceApi()==GxApi_D3d9)static_cast<CGxDeviceD3d&>(device).IStateSync();
  else static_cast<CGxDeviceD3d9Ex&>(device).IStateSync();
 }
};
}
ParticleLayerBinding::~ParticleLayerBinding(){for(auto* texture:textures)if(texture)HandleClose(reinterpret_cast<HOBJECT>(texture));}
std::shared_ptr<const ParticleLayerBinding> BindParticleLayers(const CM2Model& model,uint32_t index) noexcept{
 try{
  if(!model.m_shared)return {};const auto& shared=*model.m_shared;
  auto recipe=ParticleLayersFor(shared,index);if(!recipe)return {};
  auto binding=std::make_shared<ParticleLayerBinding>();binding->recipe=std::move(recipe);
  for(unsigned layer=0;layer<3;++layer){
   const auto textureIndex=binding->recipe->textures[layer];
   if(!shared.m_data || textureIndex>=shared.m_data->m_textures.m_count || !model.m_textures || !model.m_textures[textureIndex])return {};
   binding->textures[layer]=reinterpret_cast<CTexture*>(HandleDuplicate(reinterpret_cast<HOBJECT>(model.m_textures[textureIndex])));
   if(!binding->textures[layer])return {};
  }
  return binding;
 }catch(...){return {};}
}
bool AppendParticleLayerQuad(const ParticleLayerRecipe& recipe,const CParticle2& particle,const uint8_t* input,uint32_t stride,std::vector<uint8_t>& output){
 if(!input || stride!=24 || !std::isfinite(particle.m_age) || particle.m_age<0 || output.size()>16384u*40u-160)return false;
 float uv[4][2];for(unsigned i=0;i<4;++i){std::memcpy(uv[i],input+i*stride+16,8);if(!std::isfinite(uv[i][0])||!std::isfinite(uv[i][1]))return false;}
 float lo[2]{uv[0][0],uv[0][1]},hi[2]{lo[0],lo[1]};
 for(auto& v:uv)for(unsigned axis=0;axis<2;++axis){lo[axis]=(std::min)(lo[axis],v[axis]);hi[axis]=(std::max)(hi[axis],v[axis]);}
 if(hi[0]-lo[0]<1e-7f || hi[1]-lo[1]<1e-7f)return false;
 uint8_t quad[160]{};unsigned corners=0;
 // Same deterministic motion equations as the donor adapter, seeded from the
 // native particle's stable random seed rather than advancing emitter RNG.
 for(unsigned i=0;i<4;++i){
  float corner[2]{(uv[i][0]-lo[0])/(hi[0]-lo[0]),(uv[i][1]-lo[1])/(hi[1]-lo[1])};
  const unsigned x=corner[0]>.5f?1:0,y=corner[1]>.5f?1:0;
  if(std::fabs(corner[0]-float(x))>1e-4f || std::fabs(corner[1]-float(y))>1e-4f || (corners&(1u<<(x+2*y))))return false;
  corners|=1u<<(x+2*y);auto* dst=quad+i*40;std::memcpy(dst,input+i*stride,24);
  for(unsigned layer=0;layer<2;++layer){
   const uint32_t salt=particle.m_randomSeed+0x9e3779b9u*(layer+1);const float random=2*Unit(salt+2)-1;float extra[2];
   for(unsigned axis=0;axis<2;++axis){extra[axis]=Unit(salt+axis)+(recipe.mid[layer][axis]+recipe.range[layer][axis]*random)*particle.m_age+corner[axis]*recipe.scale[layer];if(!std::isfinite(extra[axis]))return false;}
   std::memcpy(dst+24+layer*8,extra,8);
  }
 }
 output.insert(output.end(),quad,quad+sizeof(quad));return true;
}
bool DrawParticleLayers(CParticleEmitter2& emitter){
 auto* device=g_theGxDevicePtr;const auto& binding=emitter.m_params.m_wxlLayers;
 if(!device || (device->DeviceApi()!=GxApi_D3d9 && device->DeviceApi()!=GxApi_D3d9Ex) ||
    device->Caps().m_shaderTargets[GxSh_Vertex]<3 || device->Caps().m_shaderTargets[GxSh_Pixel]<4 ||
    !CShaderEffect::s_enableShaders || !CShaderEffect::s_curEffect || !emitter.m_wxlLayerValid ||
    !binding || binding->recipe!=emitter.m_params.m_wxlLayerRecipe || emitter.m_params.m_hasModel ||
    (emitter.m_params.m_materialFlags&1) || emitter.m_params.m_materialBlend!=GxBlend_Alpha ||
    emitter.m_vertsPerParticle!=4 || emitter.m_indicesPerParticle!=6)return false;
 if(emitter.m_renderedParticles>4096)return false;
 const auto vertices=emitter.m_renderedParticles*4;
 if(!vertices || vertices>16384 || emitter.m_wxlLayerVertices.size()!=size_t(vertices)*40 ||
    (emitter.m_params.m_wxlCutoff && emitter.m_wxlRenderedCutoffs.size()!=emitter.m_renderedParticles))return false;
 for(float cutoff:emitter.m_wxlRenderedCutoffs)if(!std::isfinite(cutoff)||cutoff<0||cutoff>1)return false;
 if(!std::isfinite(CShaderEffect::s_fogColorAlphaRef.w)||CShaderEffect::s_fogColorAlphaRef.w<0||CShaderEffect::s_fogColorAlphaRef.w>1)return false;
 try{
  ShaderPair shaders(*device);
  // Capture native bindings before extra texture readiness can upload/bind.
  DrawScope scope(*device);std::array<CGxTex*,3> textures{};
  for(unsigned i=0;i<3;++i){textures[i]=TextureGetGxTex(binding->textures[i],0,nullptr);if(!textures[i] || textures[i]->m_target!=GxTex_2d)return false;}
  scope.SaveTextures(textures);
  if(!shaders.Load(shaders.vertex,GxSh_Vertex,"WXL.NativeLayer.Unlit.v1",programs::Unlit))return false;
  if(binding->recipe->threeColor){if(!shaders.Load(shaders.pixel,GxSh_Pixel,"WXL.NativeLayer.ThreeColor.v1",programs::ThreeColorThreeAlpha))return false;}
  else if(!shaders.Load(shaders.pixel,GxSh_Pixel,"WXL.NativeLayer.TwoColor.v1",programs::TwoColorThreeAlpha))return false;
  if(!CParticleEmitter2::s_indexBuf)return false;
  if(!CParticleEmitter2::s_indexBuf->m_hasData || !CParticleEmitter2::s_indexBuf->m_valid)CParticleEmitter2::RenderIndices(CParticleEmitter2::s_indexBuf);
  auto* buffer=device->BufStream(GxPoolTarget_Vertex,40,vertices);if(!buffer)return false;
  auto* destination=device->BufLock(buffer);if(!destination)return false;
  std::memcpy(destination,emitter.m_wxlLayerVertices.data(),emitter.m_wxlLayerVertices.size());device->BufUnlock(buffer,0);buffer->m_hasData=1;
  for(unsigned i=0;i<3;++i){auto flags=textures[i]->m_flags;flags.m_filter=GxTex_LinearMipNearest;flags.m_wrapU=1;flags.m_wrapV=1;device->TexSetFlags(textures[i],flags);device->RsSet(static_cast<EGxRenderState>(GxRs_Texture0+i),textures[i]);device->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_Texture0+i));}
  C44Matrix identity;device->XformSetView(identity);
  CShaderEffect::UpdateProjMatrix();CShaderEffect::SetTexMtx_Identity(0);CShaderEffect::UpdateWorldViewMatrix();
  // c30 fog visibility and PS c2 fog RGB already belong to native SetupMaterial;
  // preserve them, including the native disabled-fog constant, without guessing.
  device->ShaderConstantsSet(GxSh_Vertex,30,scope.vertex30,1);device->ShaderConstantsSet(GxSh_Pixel,2,scope.pixel2,1);
  const CGxVertexAttrib attribs[]{{GxVA_Position,4,0,40},{GxVA_Color0,0,12,40},{GxVA_TexCoord0,3,16,40},{GxVA_TexCoord1,3,24,40},{GxVA_TexCoord2,3,32,40}};
  device->PrimVertexFormat(buffer,attribs,5);device->PrimVertexMask((1u<<GxVA_Position)|(1u<<GxVA_Color0)|(1u<<GxVA_TexCoord0)|(1u<<GxVA_TexCoord1)|(1u<<GxVA_TexCoord2));
  device->PrimIndexPtr(CParticleEmitter2::s_indexBuf);device->RsSet(GxRs_VertexShader,shaders.vertex);device->RsSet(GxRs_PixelShader,shaders.pixel);
  for(size_t begin=0;begin<emitter.m_renderedParticles;){
   const auto cutoffAt=[&](size_t i){return (std::max)(CShaderEffect::s_fogColorAlphaRef.w,emitter.m_params.m_wxlCutoff?emitter.m_wxlRenderedCutoffs[i]:0.0f);};
   const float cutoff=cutoffAt(begin);if(!std::isfinite(cutoff)||cutoff<0||cutoff>1)return false;
   size_t end=begin+1;while(end<emitter.m_renderedParticles && cutoffAt(end)==cutoff)++end;
   // RGB/alpha extension multipliers were already baked into native keys.
   const float parameters[4]{1,1,cutoff,0};device->ShaderConstantsSet(GxSh_Pixel,0,parameters,1);
   CGxBatch batch;batch.m_primType=GxPrim_Triangles;batch.m_start=uint32_t(begin)*6;batch.m_count=uint32_t(end-begin)*6;
   batch.m_minIndex=uint16_t(begin*4);batch.m_maxIndex=uint16_t(end*4-1);device->Draw(&batch,1);begin=end;
  }
  return true;
 }catch(...){return false;}
}
void DestroyParticleLayerPrograms(CGxDevice& device) noexcept {
 try {
  for(const auto* name:{"WXL.NativeLayer.TwoColor.v1","WXL.NativeLayer.ThreeColor.v1"})
   if(auto* program=device.m_shaderList[GxSh_Pixel].Ptr(name))device.ShaderDestroy(program);
  if(auto* program=device.m_shaderList[GxSh_Vertex].Ptr("WXL.NativeLayer.Unlit.v1"))device.ShaderDestroy(program);
 }catch(...){}
}

}
