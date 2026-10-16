// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ParticleLayers.hpp"
#include "ParticleLayerMaterial.hpp"
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
 float pixel0[4],pixel2[4],vertex2[24],vertex10[12],vertex17[44],vertex29[4],vertex30[4],vertex31[16];C44Matrix view;
 explicit DrawScope(CGxDevice& d):device(d),mask(d.m_primVertexMask),indices(d.m_primIndexBuf){
  std::memcpy(attribs,d.m_primVertexFormatAttrib,sizeof(attribs));std::memcpy(buffers,d.m_primVertexFormatBuf,sizeof(buffers));
  std::memcpy(pixel0,CGxDevice::s_shadowConstants[0].constants,sizeof(pixel0));
  std::memcpy(pixel2,CGxDevice::s_shadowConstants[0].constants+2*4,sizeof(pixel2));
  std::memcpy(vertex2,CGxDevice::s_shadowConstants[1].constants+2*4,sizeof(vertex2));
  std::memcpy(vertex10,CGxDevice::s_shadowConstants[1].constants+10*4,sizeof(vertex10));
  std::memcpy(vertex17,CGxDevice::s_shadowConstants[1].constants+17*4,sizeof(vertex17));
  std::memcpy(vertex29,CGxDevice::s_shadowConstants[1].constants+29*4,sizeof(vertex29));
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
  device.ShaderConstantsSet(GxSh_Vertex,10,vertex10,3);
  device.ShaderConstantsSet(GxSh_Vertex,17,vertex17,11);
  device.ShaderConstantsSet(GxSh_Vertex,29,vertex29,1);
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
 const bool lit=!(recipe.sourceFlags&1);const uint32_t extendedStride=lit?52u:40u;
 if(!input || stride!=(lit?36u:24u) || !std::isfinite(particle.m_age) || particle.m_age<0 || output.size()>16384u*extendedStride-4*extendedStride)return false;
 float uv[4][2];for(unsigned i=0;i<4;++i){
  std::memcpy(uv[i],input+i*stride+stride-8,8);if(!std::isfinite(uv[i][0])||!std::isfinite(uv[i][1]))return false;
  if(lit){float normal[3];std::memcpy(normal,input+i*stride+12,sizeof(normal));
   const float lengthSquared=normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2];
   if(!std::isfinite(normal[0])||!std::isfinite(normal[1])||!std::isfinite(normal[2]) ||
      !std::isfinite(lengthSquared)||!(lengthSquared>0))return false;
  }
 }
 float lo[2]{uv[0][0],uv[0][1]},hi[2]{lo[0],lo[1]};
 for(auto& v:uv)for(unsigned axis=0;axis<2;++axis){lo[axis]=(std::min)(lo[axis],v[axis]);hi[axis]=(std::max)(hi[axis],v[axis]);}
 if(hi[0]-lo[0]<1e-7f || hi[1]-lo[1]<1e-7f)return false;
 uint8_t quad[208]{};unsigned corners=0;
 // Same deterministic motion equations as the donor adapter, seeded from the
 // native particle's stable random seed rather than advancing emitter RNG.
 for(unsigned i=0;i<4;++i){
  float corner[2]{(uv[i][0]-lo[0])/(hi[0]-lo[0]),(uv[i][1]-lo[1])/(hi[1]-lo[1])};
  const unsigned x=corner[0]>.5f?1:0,y=corner[1]>.5f?1:0;
  if(std::fabs(corner[0]-float(x))>1e-4f || std::fabs(corner[1]-float(y))>1e-4f || (corners&(1u<<(x+2*y))))return false;
  corners|=1u<<(x+2*y);auto* dst=quad+i*extendedStride;std::memcpy(dst,input+i*stride,stride);
  for(unsigned layer=0;layer<2;++layer){
   const uint32_t salt=particle.m_randomSeed+0x9e3779b9u*(layer+1);const float random=2*Unit(salt+2)-1;float extra[2];
   for(unsigned axis=0;axis<2;++axis){extra[axis]=Unit(salt+axis)+(recipe.mid[layer][axis]+recipe.range[layer][axis]*random)*particle.m_age+corner[axis]*recipe.scale[layer];if(!std::isfinite(extra[axis]))return false;}
   std::memcpy(dst+stride+layer*8,extra,8);
  }
 }
 output.insert(output.end(),quad,quad+4*extendedStride);return true;
}
bool DrawParticleLayers(CParticleEmitter2& emitter){
 auto* device=g_theGxDevicePtr;const auto& binding=emitter.m_params.m_wxlLayers;
 if(!device || (device->DeviceApi()!=GxApi_D3d9 && device->DeviceApi()!=GxApi_D3d9Ex) ||
    device->Caps().m_shaderTargets[GxSh_Vertex]<3 || device->Caps().m_shaderTargets[GxSh_Pixel]<4 ||
    !CShaderEffect::s_enableShaders || !CShaderEffect::s_curEffect || !emitter.m_wxlLayerValid ||
    !binding || binding->recipe!=emitter.m_params.m_wxlLayerRecipe || emitter.m_params.m_hasModel ||
    emitter.m_vertsPerParticle!=4 || emitter.m_indicesPerParticle!=6)return false;
 const bool lit=!(binding->recipe->sourceFlags&1);const uint32_t stride=lit?52u:40u;
 if(bool(emitter.m_params.m_materialFlags&1)!=lit ||
    (lit && (CShaderEffect::s_lightEnabled!=1 || CShaderEffect::s_localLightCount>2 || CShaderEffect::s_shadowValue)))return false;
 if(lit)for(uint32_t i=0;i<CShaderEffect::s_localLightCount;++i){
  // ComputeLocalLights explicitly marks real point slots with color.w=1;
  // nonpoint slots only clear color and do not initialize point-position data.
  if(CGxDevice::s_shadowConstants[1].constants[(17+i)*4+3]!=1.0f)return false;
 }
 ParticleLayerMaterial material{};
 if(!ResolveParticleLayerMaterial(binding->recipe->sourceBlend,material) ||
    emitter.m_params.m_materialBlend!=uint32_t(material.nativeBlend) ||
    device->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]!=int32_t(material.nativeBlend))return false;
 if(emitter.m_renderedParticles>4096)return false;
 const auto vertices=emitter.m_renderedParticles*4;
 if(!vertices || vertices>16384 || emitter.m_wxlLayerVertices.size()!=size_t(vertices)*stride ||
    (emitter.m_params.m_wxlCutoff && emitter.m_wxlRenderedCutoffs.size()!=emitter.m_renderedParticles))return false;
 for(float cutoff:emitter.m_wxlRenderedCutoffs)if(!std::isfinite(cutoff)||cutoff<0||cutoff>1)return false;
 if(!std::isfinite(CShaderEffect::s_fogColorAlphaRef.w)||CShaderEffect::s_fogColorAlphaRef.w<0||CShaderEffect::s_fogColorAlphaRef.w>1)return false;
 try{
  ShaderPair shaders(*device);
  // Capture native bindings before extra texture readiness can upload/bind.
  DrawScope scope(*device);std::array<CGxTex*,3> textures{};
  for(unsigned i=0;i<3;++i){textures[i]=TextureGetGxTex(binding->textures[i],0,nullptr);if(!textures[i] || textures[i]->m_target!=GxTex_2d)return false;}
  scope.SaveTextures(textures);
  if(!lit){if(!shaders.Load(shaders.vertex,GxSh_Vertex,"WXL.NativeLayer.Unlit.v1",programs::Unlit))return false;}
  else switch(CShaderEffect::s_localLightCount){
   case 0:if(!shaders.Load(shaders.vertex,GxSh_Vertex,"WXL.NativeLayer.Directional.v1",programs::Directional))return false;break;
   case 1:if(!shaders.Load(shaders.vertex,GxSh_Vertex,"WXL.NativeLayer.DirectionalPoint.v1",programs::DirectionalPoint))return false;break;
   case 2:if(!shaders.Load(shaders.vertex,GxSh_Vertex,"WXL.NativeLayer.DirectionalTwoPoints.v1",programs::DirectionalTwoPoints))return false;break;
   default:return false;
  }
  if(binding->recipe->threeColor){if(!shaders.Load(shaders.pixel,GxSh_Pixel,"WXL.NativeLayer.ThreeColor.v1",programs::ThreeColorThreeAlpha))return false;}
  else if(!shaders.Load(shaders.pixel,GxSh_Pixel,"WXL.NativeLayer.TwoColor.v1",programs::TwoColorThreeAlpha))return false;
  if(!CParticleEmitter2::s_indexBuf)return false;
  if(!CParticleEmitter2::s_indexBuf->m_hasData || !CParticleEmitter2::s_indexBuf->m_valid)CParticleEmitter2::RenderIndices(CParticleEmitter2::s_indexBuf);
  auto* buffer=device->BufStream(GxPoolTarget_Vertex,stride,vertices);if(!buffer)return false;
  auto* destination=device->BufLock(buffer);if(!destination)return false;
  std::memcpy(destination,emitter.m_wxlLayerVertices.data(),emitter.m_wxlLayerVertices.size());device->BufUnlock(buffer,0);buffer->m_hasData=1;
  for(unsigned i=0;i<3;++i){auto flags=textures[i]->m_flags;flags.m_filter=GxTex_LinearMipNearest;flags.m_wrapU=1;flags.m_wrapV=1;device->TexSetFlags(textures[i],flags);device->RsSet(static_cast<EGxRenderState>(GxRs_Texture0+i),textures[i]);device->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_Texture0+i));}
  C44Matrix identity;device->XformSetView(identity);
  CShaderEffect::UpdateProjMatrix();CShaderEffect::SetTexMtx_Identity(0);CShaderEffect::UpdateWorldViewMatrix();
  // Native SetupLighting/SetupMaterial populated the exact captured light ABI.
  // Reassert its immutable draw snapshot after texture readiness, preserving
  // attenuation packing, coordinate space and emissive bias without recompute.
  if(lit){device->ShaderConstantsSet(GxSh_Vertex,10,scope.vertex10,3);device->ShaderConstantsSet(GxSh_Vertex,17,scope.vertex17,11);device->ShaderConstantsSet(GxSh_Vertex,29,scope.vertex29,1);}
  // c30 fog visibility and PS c2 fog RGB already belong to native SetupMaterial;
  // preserve them, including the native disabled-fog constant, without guessing.
  device->ShaderConstantsSet(GxSh_Vertex,30,scope.vertex30,1);device->ShaderConstantsSet(GxSh_Pixel,2,scope.pixel2,1);
  // Additive and Screen must approach their neutral black source at distance.
  // Leave the native visibility exponent/disabled-fog constant in c30 intact.
  // Do NOT set donor parameters.w: that experimental BlendAdd switch also
  // enables a separate layer0 alpha test, which is not this material contract.
  if(material.blackFog){const float black[4]{0,0,0,scope.pixel2[3]};device->ShaderConstantsSet(GxSh_Pixel,2,black,1);}
  CGxVertexAttrib attribs[6];uint32_t attribCount=0;attribs[attribCount++]={GxVA_Position,4,0,stride};
  if(lit)attribs[attribCount++]={GxVA_Normal,4,12,stride};
  const uint32_t nativeStride=stride-16;
  attribs[attribCount++]={GxVA_Color0,0,nativeStride-12,stride};
  attribs[attribCount++]={GxVA_TexCoord0,3,nativeStride-8,stride};
  attribs[attribCount++]={GxVA_TexCoord1,3,nativeStride,stride};
  attribs[attribCount++]={GxVA_TexCoord2,3,nativeStride+8,stride};
  device->PrimVertexFormat(buffer,attribs,attribCount);device->PrimVertexMask((1u<<GxVA_Position)|(lit?(1u<<GxVA_Normal):0u)|(1u<<GxVA_Color0)|(1u<<GxVA_TexCoord0)|(1u<<GxVA_TexCoord1)|(1u<<GxVA_TexCoord2));
  device->PrimIndexPtr(CParticleEmitter2::s_indexBuf);device->RsSet(GxRs_VertexShader,shaders.vertex);device->RsSet(GxRs_PixelShader,shaders.pixel);
  for(size_t begin=0;begin<emitter.m_renderedParticles;){
   const auto cutoffAt=[&](size_t i){return (std::max)(CShaderEffect::s_fogColorAlphaRef.w,emitter.m_params.m_wxlCutoff?emitter.m_wxlRenderedCutoffs[i]:0.0f);};
   const float cutoff=cutoffAt(begin);if(!std::isfinite(cutoff)||cutoff<0||cutoff>1)return false;
   size_t end=begin+1;while(end<emitter.m_renderedParticles && cutoffAt(end)==cutoff)++end;
   // RGB/alpha extension multipliers were already baked into native keys.
   // Weighted additive applies alpha exactly once in the native blend stage;
   // unweighted additive/Screen preserve independent RGB and opacity.
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
  for(const auto* name:{"WXL.NativeLayer.Unlit.v1","WXL.NativeLayer.Directional.v1","WXL.NativeLayer.DirectionalPoint.v1","WXL.NativeLayer.DirectionalTwoPoints.v1"})
   if(auto* program=device.m_shaderList[GxSh_Vertex].Ptr(name))device.ShaderDestroy(program);
 }catch(...){}
}

}
