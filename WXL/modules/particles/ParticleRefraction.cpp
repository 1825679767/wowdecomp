// Exact-source donor Blood Boil refraction approximation. No general refraction grant.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ParticleRefraction.hpp"
#include "NativeFeatures.hpp"
#include "MaterialBackend.hpp"
#ifdef WXL_ENABLE_DX11
#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#endif
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "engine/assets/shared/models/m2/M2RecordWidths.hpp"
#include "common/Log.hpp"
#include "shaders/ParticlePrograms.hpp"
#include "shaders/BloodRefractionPs.h"
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <string_view>
namespace wxl::native {
// No saved device pointer. Resources belong to the native CGxDevice instance and
// are released by its backend's existing Reset/resource-release boundary.
struct ParticleRefractionCapture {
 IDirect3DTexture9* texture=nullptr;
 IDirect3DSurface9* surface=nullptr;
 UINT width{},height{};D3DFORMAT format=D3DFMT_UNKNOWN;
 ~ParticleRefractionCapture(){if(surface)surface->Release();if(texture)texture->Release();}
};
namespace {
namespace programs=wxl::modern::particlelayers::programs;
constexpr const char* PixelName="WXL_BloodRefraction_PS_20261016";
std::atomic<bool> enabled{WXL_ENABLE_BLOOD_REFRACTION!=0};
std::atomic<bool> restoreFailed{false};
bool Changed(CVar*,const char*,const char* text,void*){char* end=nullptr;const long n=text?std::strtol(text,&end,10):0;enabled.store(WXL_ENABLE_BLOOD_REFRACTION&&text&&end!=text&&!*end&&n==1,std::memory_order_release);return true;}
bool Backend(CGxDevice& d){return LegacyMaterialBackend(&d);}
void Sync(CGxDevice& d){SyncMaterialState(d);}
IDirect3DDevice9* Native(CGxDevice& d){return d.m_api==GxApi_D3d9?static_cast<CGxDeviceD3d&>(d).m_d3dDevice:static_cast<CGxDeviceD3d9Ex&>(d).m_d3dDevice;}
template<class T>T Read(const uint8_t* p,size_t at){T v{};std::memcpy(&v,p+at,sizeof(v));return v;}
bool Range(size_t n,uint32_t at,uint32_t count,size_t stride){return at<=n&&count<=(n-at)/stride;}
bool Path(const char* name){
 if(!name)return false;constexpr std::string_view target="spells\\cfx_deathknight_bloodboil_castworld.m2";const std::string_view p(name);if(p.size()!=target.size())return false;
 for(size_t i=0;i<p.size();++i){char c=p[i];if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c+'a'-'A');if(c!=target[i])return false;}return true;
}
template<class T>struct Com {T* value=nullptr;~Com(){if(value)value->Release();}};
template<class T,size_t N>bool Exact(T* shader,const uint32_t(&code)[N]){if(!shader)return false;UINT size=0;if(FAILED(shader->GetFunction(nullptr,&size))||size!=sizeof(code))return false;std::array<uint32_t,N> bytes{};return SUCCEEDED(shader->GetFunction(bytes.data(),&size))&&size==sizeof(code)&&!std::memcmp(bytes.data(),code,size);}
bool Contract(IDirect3DDevice9& d){
 Com<IDirect3DVertexShader9> vs;Com<IDirect3DPixelShader9> ps;Com<IDirect3DVertexBuffer9> vb;Com<IDirect3DBaseTexture9> height;
 UINT offset=0,stride=0;DWORD zwrite=1,srgbRead=1,srgbWrite=1;
 return SUCCEEDED(d.GetVertexShader(&vs.value))&&Exact(vs.value,programs::NativeUnlit)&&
  SUCCEEDED(d.GetPixelShader(&ps.value))&&Exact(ps.value,programs::NativePixel)&&
  SUCCEEDED(d.GetStreamSource(0,&vb.value,&offset,&stride))&&vb.value&&stride==24&&
  SUCCEEDED(d.GetTexture(0,&height.value))&&height.value&&
  SUCCEEDED(d.GetRenderState(D3DRS_ZWRITEENABLE,&zwrite))&&!zwrite&&
  SUCCEEDED(d.GetSamplerState(0,D3DSAMP_SRGBTEXTURE,&srgbRead))&&!srgbRead&&
  SUCCEEDED(d.GetRenderState(D3DRS_SRGBWRITEENABLE,&srgbWrite))&&!srgbWrite;
}
CGxShader* Program(CGxDevice& d){
 auto* shader=d.m_shaderList[GxSh_Pixel].Ptr(PixelName);
 if(shader){if(shader->m_target!=GxSh_Pixel||shader->m_code.Count()!=sizeof(kBloodRefractionShader)||std::memcmp(shader->m_code.m_data,kBloodRefractionShader,sizeof(kBloodRefractionShader)))return nullptr;}
 else{shader=d.m_shaderList[GxSh_Pixel].New(PixelName,0,0);if(!shader)return nullptr;shader->m_refCount=1;shader->m_target=GxSh_Pixel;shader->m_code.SetCount(sizeof(kBloodRefractionShader));std::memcpy(shader->m_code.m_data,kBloodRefractionShader,sizeof(kBloodRefractionShader));}
 return shader->Valid()?shader:nullptr;
}
bool Capture(CGxDevice& owner,IDirect3DDevice9& d){
 Com<IDirect3DSurface9> source;D3DSURFACE_DESC desc{};D3DVIEWPORT9 viewport{};
 if(FAILED(d.GetRenderTarget(0,&source.value))||!source.value||FAILED(source.value->GetDesc(&desc))||FAILED(d.GetViewport(&viewport))||
    !desc.Width||!desc.Height||viewport.X||viewport.Y||viewport.Width!=desc.Width||viewport.Height!=desc.Height)return false;
 auto& saved=owner.m_wxlParticleRefractionCapture;
 if(!saved||saved->width!=desc.Width||saved->height!=desc.Height||saved->format!=desc.Format){
  // Create the replacement before dropping the previous capture; allocation
  // cannot leave an old-size texture usable as the current frame's contents.
  auto next=std::make_shared<ParticleRefractionCapture>();
  if(FAILED(d.CreateTexture(desc.Width,desc.Height,1,D3DUSAGE_RENDERTARGET,desc.Format,D3DPOOL_DEFAULT,&next->texture,nullptr))||!next->texture||
     FAILED(next->texture->GetSurfaceLevel(0,&next->surface))||!next->surface)return false;
  next->width=desc.Width;next->height=desc.Height;next->format=desc.Format;saved=std::move(next);
 }
 // Deliberately not Gx::TexCopy: that native helper reads m_defColorSurface.
 // This resolves ACTIVE RT0 immediately before this emitter, including MSAA.
 return SUCCEEDED(d.StretchRect(source.value,nullptr,saved->surface,nullptr,D3DTEXF_NONE));
}
struct NativeState {
 CGxDevice& device;float pixel0[4];
 explicit NativeState(CGxDevice& d):device(d){ReadMaterialConstants(d,GxSh_Pixel,0,1,pixel0);device.RsPush();}
 ~NativeState(){device.ShaderConstantsSet(GxSh_Pixel,0,pixel0,1);device.RsPop();Sync(device);}
};
#ifdef WXL_ENABLE_DX11
ParticleRefractionDraw DrawDx11(CGxDeviceD3d11& d,CGxBatch& batch){
 // DX11 consumes only the Gx-owned bytecode/state; no module writes its raw
 // context. Validate the same donor program/stride/depth/texture contract.
 if(!ExactMaterialProgram(d,GxRs_VertexShader,programs::NativeUnlit)||
    !ExactMaterialProgram(d,GxRs_PixelShader,programs::NativePixel)||
    d.m_primVertexFormat!=GxVBF_PCT||d.m_primVertexSize!=24||
    !d.m_primVertexBuf||!d.m_appRenderStates[GxRs_Texture0].m_value.m_data.p||
    d.m_appRenderStates[GxRs_DepthWrite].m_value.m_data.i[0])return ParticleRefractionDraw::Native;
 auto* shader=Program(d);if(!shader)return ParticleRefractionDraw::Native;
 auto* scene=d.CaptureCurrentColor();if(!scene)return ParticleRefractionDraw::Native;
 NativeState state(d);
 const float parameters[]={1.0f/scene->m_width,1.0f/scene->m_height,64.0f,4.0f};
 d.RsSet(GxRs_PixelShader,shader);d.RsSet(GxRs_Texture1,scene);d.ShaderConstantsSet(GxSh_Pixel,0,parameters,1);
 d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_AlphaRef,0);d.RsSet(GxRs_Fog,0);
 d.RsSet(GxRs_ColorWrite,d.m_appRenderStates[GxRs_ColorWrite].m_value.m_data.i[0]&7);
 struct SamplerScope {CGxDeviceD3d11& d;CGxDeviceD3d11::SamplerOverride old;~SamplerScope(){d.ExchangeSamplerOverride(1,old);}} sampler{d,d.ExchangeSamplerOverride(1,{true,false})};
 d.Draw(&batch,1);return ParticleRefractionDraw::Drawn;
}
#endif
// These backend-only states have no complete native Gx state representation.
// Their Gx caches stay untouched; capture and restore actual hardware values.
struct BackendState {
 IDirect3DDevice9& device;Com<IDirect3DBaseTexture9> texture;
 static constexpr D3DSAMPLERSTATETYPE samplers[]={D3DSAMP_ADDRESSU,D3DSAMP_ADDRESSV,D3DSAMP_MINFILTER,D3DSAMP_MAGFILTER,D3DSAMP_MIPFILTER,D3DSAMP_SRGBTEXTURE};
 static constexpr D3DRENDERSTATETYPE renders[]={D3DRS_ALPHABLENDENABLE,D3DRS_ALPHATESTENABLE,D3DRS_FOGENABLE,D3DRS_COLORWRITEENABLE};
 DWORD sampler[6]{},render[4]{};bool captured=false,mutated=false,restored=true;
 explicit BackendState(IDirect3DDevice9& d):device(d){
  if(FAILED(device.GetTexture(1,&texture.value)))return;
  for(unsigned i=0;i<6;++i)if(FAILED(device.GetSamplerState(1,samplers[i],sampler+i)))return;
  for(unsigned i=0;i<4;++i)if(FAILED(device.GetRenderState(renders[i],render+i)))return;
  captured=true;
 }
 bool Apply(IDirect3DTexture9* scene){
  if(!captured)return false;mutated=true;
  if(FAILED(device.SetTexture(1,scene)))return false;
  const DWORD values[]={D3DTADDRESS_CLAMP,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_NONE,FALSE};
  for(unsigned i=0;i<6;++i)if(FAILED(device.SetSamplerState(1,samplers[i],values[i])))return false;
  const DWORD rs[]={FALSE,FALSE,FALSE,render[3]&(D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE)};
  for(unsigned i=0;i<4;++i)if(FAILED(device.SetRenderState(renders[i],rs[i])))return false;
  return true;
 }
 bool Restore(){
  if(!mutated)return restored;mutated=false;
  restored=SUCCEEDED(device.SetTexture(1,texture.value));
  for(unsigned i=0;i<6;++i)restored=SUCCEEDED(device.SetSamplerState(1,samplers[i],sampler[i]))&&restored;
  for(unsigned i=0;i<4;++i)restored=SUCCEEDED(device.SetRenderState(renders[i],render[i]))&&restored;
  if(!restored){restoreFailed.store(true,std::memory_order_release);WLOG_WARN("native-blood-refraction: backend restore failed; disabled for this session");}
  return restored;
 }
 ~BackendState(){Restore();}
};
}
void RegisterParticleRefraction(){auto* cv=CVar::Register("wxlBloodRefraction","Experimental exact-source Blood Boil donor refraction: 0 off, 1 on",2,WXL_ENABLE_BLOOD_REFRACTION?"1":"0",Changed,0,false,nullptr,false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,nullptr);}
bool ParticleRefractionEnabled() noexcept {return enabled.load(std::memory_order_acquire)||restoreFailed.load(std::memory_order_acquire);}
std::shared_ptr<const ParticleRefractionRecipe> CaptureParticleRefraction(const char* path,const uint8_t* raw,size_t size){
 if(!WXL_ENABLE_BLOOD_REFRACTION||!ParticlesEnabled()||!Path(path)||!raw||size<312||size>64u*1024*1024||std::memcmp(raw,"MD21",4))return {};
 const auto bodySize=Read<uint32_t>(raw,4);if(bodySize<304||bodySize>size-8)return {};const auto* body=raw+8;
 constexpr auto stride=wxl::structure::m2records::kParticleStrideModern;
 const auto count=Read<uint32_t>(body,offsetof(M2Data,m_particles)),at=Read<uint32_t>(body,offsetof(M2Data,m_particles)+4);
 const auto textures=Read<uint32_t>(body,offsetof(M2Data,m_textures)),texAt=Read<uint32_t>(body,offsetof(M2Data,m_textures)+4);
 if(std::memcmp(body,"MD20",4)||Read<uint32_t>(body,4)!=274||Read<uint32_t>(body,offsetof(M2Data,m_flags))!=0x3090||
    count!=7||textures!=6||!Range(bodySize,at,count,stride)||!Range(bodySize,texAt,textures,sizeof(M2Texture)))return {};
 const uint8_t* ids=nullptr;unsigned md21=0;
 for(size_t p=0;p<size;){if(size-p<8)return {};const auto bytes=Read<uint32_t>(raw,p+4);if(bytes>size-p-8)return {};
  if(!std::memcmp(raw+p,"MD21",4)){if(++md21!=1)return {};}
  if(!std::memcmp(raw+p,"TXID",4)){if(ids||bytes!=6*4)return {};ids=raw+p+8;}p+=8+bytes;
 }
 const auto* particle=body+at;
 if(!ids||Read<uint32_t>(ids,0)!=1601218||Read<uint32_t>(particle,offsetof(M2Particle,m_flags))!=0x60931011||
    Read<uint8_t>(particle,offsetof(M2Particle,m_blendMode))!=4||Read<uint16_t>(particle,offsetof(M2Particle,m_textureIndex))!=0)return {};
 auto recipe=std::make_shared<ParticleRefractionRecipe>();recipe->sourceFlags=0x60931011;recipe->sourceBlend=4;recipe->textureIndex=0;recipe->heightFileDataId=1601218;recipe->sourceRecord.assign(particle,particle+stride);return recipe;
}
ParticleRefractionDraw DrawParticleRefraction(CParticleEmitter2& emitter,CGxBatch& batch,EGxVertexBufferFormat format,uint32_t vertices){
 auto* d=g_theGxDevicePtr;const auto& recipe=emitter.m_params.m_wxlRefraction;
 if(restoreFailed.load(std::memory_order_acquire))return ParticleRefractionDraw::Suppressed;
 if(!enabled.load(std::memory_order_acquire)||!ParticlesEnabled()||!d||!Backend(*d)||!recipe||!CShaderEffect::s_enableShaders||
    recipe->sourceFlags!=0x60931011||recipe->sourceBlend!=4||recipe->textureIndex||recipe->heightFileDataId!=1601218||
    emitter.m_params.m_hasModel||(emitter.m_params.m_materialFlags&1)||!emitter.m_texture||emitter.m_vertsPerParticle!=4||emitter.m_indicesPerParticle!=6||
    format!=GxVBF_PCT||vertices<4||vertices>4096||vertices%4||batch.m_primType!=GxPrim_Triangles||batch.m_count!=vertices/2*3||
    batch.m_start||batch.m_minIndex||batch.m_maxIndex!=vertices-1)return ParticleRefractionDraw::Native;
 bool submitted=false;
 try{
#ifdef WXL_ENABLE_DX11
  if(d->m_api==GxApi_D3d11)return DrawDx11(static_cast<CGxDeviceD3d11&>(*d),batch);
#endif
  Sync(*d);auto* native=Native(*d);if(!native||!Contract(*native))return ParticleRefractionDraw::Native;
  // All allocating work precedes the first custom state mutation.
  auto* shader=Program(*d);if(!shader||!Capture(*d,*native))return ParticleRefractionDraw::Native;
  NativeState state(*d);BackendState backend(*native);if(!backend.captured)return ParticleRefractionDraw::Native;
  const auto& capture=*d->m_wxlParticleRefractionCapture;const float parameters[]={1.0f/capture.width,1.0f/capture.height,64.0f,4.0f};
  d->RsSet(GxRs_PixelShader,shader);d->ShaderConstantsSet(GxSh_Pixel,0,parameters,1);Sync(*d);
  if(!backend.Apply(capture.texture))return backend.Restore()?ParticleRefractionDraw::Native:ParticleRefractionDraw::Suppressed;
  submitted=true;d->Draw(&batch,1);
  const bool restored=backend.Restore();
  static unsigned reports=0;if(reports++<8)WLOG_INFO("native-blood-refraction: exactSource=1 activeScene=1 maxPixels=4 restored=%u; donor approximation, no pixel acceptance",unsigned(restored));
  return restored?ParticleRefractionDraw::Drawn:ParticleRefractionDraw::Suppressed;
 }catch(...){return restoreFailed.load(std::memory_order_acquire)?ParticleRefractionDraw::Suppressed:submitted?ParticleRefractionDraw::Drawn:ParticleRefractionDraw::Native;}
}
void ReleaseParticleRefractionCapture(CGxDevice& d) noexcept {d.m_wxlParticleRefractionCapture.reset();}
void DestroyParticleRefraction(CGxDevice& d) noexcept {
 ReleaseParticleRefractionCapture(d);
 try{if(auto* shader=d.m_shaderList[GxSh_Pixel].Ptr(PixelName))d.ShaderDestroy(shader);}catch(...){}
}
}
