// Exact named donor DnD compatibility: Fade/Colour/Mask/Detail, not generic TXAC.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "DndMaterials.hpp"
#include "NativeFeatures.hpp"
#include "MaterialBackend.hpp"
#ifdef WXL_ENABLE_DX11
#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#endif
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Model2/CM2SceneRender.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "common/Log.hpp"
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <span>
#include <string_view>
#include "shaders/DeathDecayNative.hpp"
#include "shaders/DeathDecayMaskVs.hpp"
#include "shaders/DeathDecayFadePs.h"
#include "shaders/DeathDecayColourPs.h"
#include "shaders/DeathDecayMaskPs.h"
#include "shaders/DeathDecayDetailPs.h"
namespace wxl::native::mesh_materials {
namespace {
std::atomic<uint32_t> mode{WXL_ENABLE_DND_MATERIALS?4u:0u};
bool Changed(CVar*,const char*,const char* text,void*){char* end=nullptr;const long value=text?std::strtol(text,&end,10):0;mode.store(WXL_ENABLE_DND_MATERIALS&&text&&end!=text&&!*end&&value>=0&&value<=4?uint32_t(value):0,std::memory_order_release);return true;}
template<class T>T Read(const uint8_t* p,size_t at){T v{};std::memcpy(&v,p+at,sizeof(v));return v;}
bool Range(size_t size,uint32_t at,uint32_t count,uint32_t stride){return at<=size&&uint64_t(count)*stride<=size-at;}
uint64_t Hash(std::span<const uint8_t> bytes){uint64_t h=14695981039346656037ull;for(auto b:bytes){h^=b;h*=1099511628211ull;}return h;}
bool Path(const char* name){
 if(!name)return false;constexpr std::string_view target="spells\\cfx_deathknight_deathanddecay_state.m2";const std::string_view p(name);if(p.size()!=target.size())return false;
 for(size_t i=0;i<p.size();++i){char c=p[i];if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c+'a'-'A');if(c!=target[i])return false;}return true;
}
bool Backend(){return LegacyMaterialBackend(g_theGxDevicePtr,3);}
}
void RegisterDndMaterials(){auto* cv=CVar::Register("wxlDndMaterialMode","Experimental exact-source DnD donor kernel: 0 off, 1 fade, 2 colour, 3 mask, 4 detail; reload world after changing",2,WXL_ENABLE_DND_MATERIALS?"4":"0",Changed,0,false,nullptr,false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,nullptr);}
std::shared_ptr<const Source> PrepareDnd(const char* path,const uint8_t* raw,size_t size,const std::vector<uint8_t>& skin,std::vector<uint8_t>& prepared){
 const auto selected=mode.load(std::memory_order_acquire);
 if(!selected||!MaterialsEnabled()||!Backend()||!Path(path)||!raw||size<312||size>64u*1024*1024||skin.size()<48||skin.size()>64u*1024*1024||std::memcmp(raw,"MD21",4)||std::memcmp(skin.data(),"SKIN",4))return {};
 const uint32_t bodySize=Read<uint32_t>(raw,4);if(bodySize<304||bodySize>size-8)return {};const auto* body=raw+8;
 if(std::memcmp(body,"MD20",4)||Read<uint32_t>(body,4)!=274||Read<uint32_t>(body,0x10)!=0x3090||Read<uint32_t>(body,0x44)!=1||Read<uint32_t>(body,0x50)!=18)return {};
 const uint32_t count=Read<uint32_t>(skin.data(),36),at=Read<uint32_t>(skin.data(),40),sections=Read<uint32_t>(skin.data(),28);
 const uint32_t materials=Read<uint32_t>(body,0x70),materialAt=Read<uint32_t>(body,0x74),textureAt=Read<uint32_t>(body,0x54),combos=Read<uint32_t>(body,0x80),comboAt=Read<uint32_t>(body,0x84),particles=Read<uint32_t>(body,0x128);
 if(count!=3||!sections||sections>65535||at<48||materials>65536||particles>65536||!Range(skin.size(),at,3,24)||!Range(bodySize,materialAt,materials,4)||!Range(bodySize,textureAt,18,16)||!Range(bodySize,comboAt,combos,2))return {};
 std::span<const uint8_t> txid,txac;
 for(size_t off=0;off<size;){if(size-off<8)return {};const auto n=Read<uint32_t>(raw,off+4);if(n>size-off-8)return {};
  if(!std::memcmp(raw+off,"TXID",4)){if(!txid.empty()||n!=72)return {};txid={raw+off+8,n};}
  if(!std::memcmp(raw+off,"TXAC",4)){if(!txac.empty()||n!=2*(materials+particles))return {};txac={raw+off+8,n};}off+=8+n;
 }
 if(txid.size()!=72||(selected>=3&&txac.empty()))return {};
 auto source=std::make_shared<Source>();auto detail=std::make_shared<DndSource>();detail->mode=selected;detail->materialCount=materials;
 source->modelPath=path;source->modelFingerprint=Hash({raw,size});source->skinFingerprint=Hash(skin);source->sectionCount=sections;source->textureCount=18;
 const uint32_t expected[2][3]={{1603902,1522978,1603903},{1601200,1620780,1612964}};
 for(uint32_t b=0;b<3;++b){const auto* batch=skin.data()+at+24*b;if(Read<uint16_t>(batch,12)||Read<uint16_t>(batch,4)>=sections)return {};if(b==2){std::memcpy(detail->nativeBatch.data(),batch,24);break;}
  auto& record=detail->batches[b];std::memcpy(record.original.data(),batch,24);const uint32_t material=Read<uint16_t>(batch,10),combo=Read<uint16_t>(batch,16);
  if(Read<uint16_t>(batch,2)!=0x4013||Read<uint16_t>(batch,14)!=3||material>=materials||Read<uint16_t>(body,materialAt+4*material)!=0x13||Read<uint16_t>(body,materialAt+4*material+2)!=7||combo>combos||combos-combo<3)return {};
  if(!txac.empty())record.txac={txac[2*material],txac[2*material+1]};
  if(selected>=3){if(batch[0]!=0x8c||record.txac!=std::array<uint8_t,2>{1,1})return {};}
  for(uint32_t i=0;i<3;++i){const auto index=Read<uint16_t>(body,comboAt+2*(combo+i));if(index>=18||Read<uint32_t>(body,textureAt+16*index)!=0)return {};
   record.textures[i]=index;record.ids[i]=Read<uint32_t>(txid.data(),4*index);record.textureFlags[i]=Read<uint32_t>(body,textureAt+16*index+4);if(record.ids[i]!=expected[b][i])return {};
   for(uint32_t head:{0x90u,0x98u}){const uint32_t n=Read<uint32_t>(body,head),o=Read<uint32_t>(body,head+4),first=Read<uint16_t>(batch,head==0x90?20:22);if(!Range(bodySize,o,n,2))return {};
    // Donor permits an absent third weight, but never an absent transform.
    const uint32_t entry=first+i;if(entry>=n){if(i==2&&head==0x90){record.weights[i]=0xffff;continue;}return {};}
    (head==0x90?record.weights:record.transforms)[i]=Read<uint16_t>(body,o+2*entry);
   }
  }
  if(selected>=3){const auto transforms=Read<uint32_t>(body,0x60);if(record.textureFlags[0]!=3||record.textureFlags[1]!=3||record.textureFlags[2]!=4||record.transforms[2]!=0xffff||record.weights[2]!=0xffff||record.transforms[0]>=transforms||record.transforms[1]>=transforms)return {};}
 }
 // Material rewrite is restricted to the custom passes, never the native third.
 const auto thirdMaterial=Read<uint16_t>(detail->nativeBatch.data(),10);
 for(const auto& record:detail->batches)if(Read<uint16_t>(record.original.data(),10)==thirdMaterial)return {};
 prepared=skin;for(uint32_t i=0;i<2;++i){const uint16_t stages=2;std::memcpy(prepared.data()+at+24*i+14,&stages,2);}
 source->dnd=detail;return source;
}
bool ApplyPreparedMaterials(const Source& source,std::vector<uint8_t>& native){
 if(!source.dnd)return true;if(native.size()<304)return false;
 const uint32_t n=Read<uint32_t>(native.data(),0x70),o=Read<uint32_t>(native.data(),0x74);if(n!=source.dnd->materialCount||!Range(native.size(),o,n,4))return false;
 const auto thirdMaterial=Read<uint16_t>(source.dnd->nativeBatch.data(),10);
 for(const auto& record:source.dnd->batches)if(Read<uint16_t>(record.original.data(),10)==thirdMaterial)return false;
 for(const auto& record:source.dnd->batches){const uint32_t material=Read<uint16_t>(record.original.data(),10);if(material>=n||Read<uint16_t>(native.data(),o+4*material)!=0x13)return false;const auto blend=Read<uint16_t>(native.data(),o+4*material+2);if(blend!=7&&blend!=4)return false;const uint16_t replacement=4;std::memcpy(native.data()+o+4*material+2,&replacement,2);}
 return true;
}
bool DndConvertedMatches(const Source& source,const std::vector<uint8_t>& skin) noexcept {
 if(!source.dnd||skin.size()<48||Read<uint32_t>(skin.data(),36)!=3||Read<uint32_t>(skin.data(),28)!=source.sectionCount)return false;const auto at=Read<uint32_t>(skin.data(),40);if(!Range(skin.size(),at,3,24))return false;
 for(uint32_t i=0;i<2;++i){const auto* b=skin.data()+at+24*i;const auto* raw=source.dnd->batches[i].original.data();if(b[0]!=(raw[0]&0x10)||b[1]!=raw[1]||Read<uint16_t>(b,2)!=4||Read<uint16_t>(b,14)!=2||Read<uint16_t>(b,18)!=4)return false;
  for(size_t offset:{size_t(4),size_t(6),size_t(8),size_t(10),size_t(12),size_t(16),size_t(20),size_t(22)})if(Read<uint16_t>(b,offset)!=Read<uint16_t>(raw,offset))return false;
 }return true;
}
namespace {
void Sync(CGxDevice& device){SyncMaterialState(device);}
IDirect3DDevice9* NativeDevice(CGxDevice& device){return device.m_api==GxApi_D3d9?static_cast<CGxDeviceD3d&>(device).m_d3dDevice:static_cast<CGxDeviceD3d9Ex&>(device).m_d3dDevice;}
template<class T>struct ComRead {T* value=nullptr;~ComRead(){if(value)value->Release();}};
template<class T,size_t N>bool Exact(T* shader,const uint32_t(&expected)[N]){if(!shader)return false;UINT bytes=0;if(FAILED(shader->GetFunction(nullptr,&bytes))||bytes!=sizeof(expected))return false;std::array<uint32_t,N> code{};return SUCCEEDED(shader->GetFunction(code.data(),&bytes))&&bytes==sizeof(expected)&&!std::memcmp(code.data(),expected,sizeof(expected));}
const char* PixelNames[]={"WXL_Dnd_Fade_PS_20261016","WXL_Dnd_Colour_PS_20261016","WXL_Dnd_Mask_PS_20261016","WXL_Dnd_Detail_PS_20261016"};
constexpr const char* MaskVertexName="WXL_Dnd_Mask_VS_20261016";
CGxShader* Program(CGxDevice& device,EGxShTarget target,const char* name,const void* bytes,uint32_t size){
 auto* shader=device.m_shaderList[target].Ptr(name);
 if(shader){if(shader->m_target!=target||shader->m_code.Count()!=size||std::memcmp(shader->m_code.m_data,bytes,size))return nullptr;}
 else{shader=device.m_shaderList[target].New(name,0,0);if(!shader)return nullptr;shader->m_refCount=1;shader->m_target=target;shader->m_code.SetCount(size);std::memcpy(shader->m_code.m_data,bytes,size);}
 return shader->Valid()?shader:nullptr;
}
struct State {CGxDevice& device;explicit State(CGxDevice& d):device(d){device.RsPush();}~State(){device.RsPop();Sync(device);}};
struct Sampler {
 IDirect3DDevice9& device;
 static constexpr D3DSAMPLERSTATETYPE states[]={D3DSAMP_ADDRESSU,D3DSAMP_ADDRESSV,D3DSAMP_MINFILTER,D3DSAMP_MAGFILTER,D3DSAMP_MIPFILTER,D3DSAMP_MAXMIPLEVEL,D3DSAMP_MIPMAPLODBIAS,D3DSAMP_SRGBTEXTURE};
 DWORD saved[8]{};uint32_t captured=0;
 explicit Sampler(IDirect3DDevice9& d):device(d){for(;captured<8;++captured)if(FAILED(device.GetSamplerState(2,states[captured],saved+captured)))break;}
 ~Sampler(){bool restored=true;for(uint32_t i=0;i<captured;++i)restored=SUCCEEDED(device.SetSamplerState(2,states[i],saved[i]))&&restored;if(!restored){mode.store(0,std::memory_order_release);WLOG_WARN("native-dnd: sampler restore failed; custom passes disabled for this session");}}
 bool Apply(){const DWORD desired[]={D3DTADDRESS_CLAMP,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_LINEAR,0,0,FALSE};if(captured!=8)return false;for(uint32_t i=0;i<8;++i)if(FAILED(device.SetSamplerState(2,states[i],desired[i])))return false;return true;}
};
bool NativeContract(CGxDevice& device){
 auto* vs=static_cast<CGxShader*>(device.m_appRenderStates[GxRs_VertexShader].m_value.m_data.p);
 auto* ps=static_cast<CGxShader*>(device.m_appRenderStates[GxRs_PixelShader].m_value.m_data.p);
 if(!vs||!ps||!vs->m_valid||!ps->m_valid||!device.m_appRenderStates[GxRs_Texture0].m_value.m_data.p||!device.m_appRenderStates[GxRs_Texture1].m_value.m_data.p)return false;
#ifdef WXL_ENABLE_DX11
 if(device.m_api==GxApi_D3d11)return
  ExactMaterialProgram(device,GxRs_VertexShader,wxl::modern::materialblend::kDeathDecayNativeVS)&&
  ExactMaterialProgram(device,GxRs_PixelShader,wxl::modern::materialblend::kDeathDecayNativePS)&&
  device.m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]==GxBlend_Add&&
  !device.m_appRenderStates[GxRs_DepthWrite].m_value.m_data.i[0];
#endif
 auto* native=NativeDevice(device);if(!native)return false;
 ComRead<IDirect3DVertexShader9> liveVS;ComRead<IDirect3DPixelShader9> livePS;
 if(FAILED(native->GetVertexShader(&liveVS.value))||FAILED(native->GetPixelShader(&livePS.value))||!Exact(liveVS.value,wxl::modern::materialblend::kDeathDecayNativeVS)||!Exact(livePS.value,wxl::modern::materialblend::kDeathDecayNativePS))return false;
 DWORD src=0,dst=0,on=0,operation=0,zwrite=1;
 return SUCCEEDED(native->GetRenderState(D3DRS_SRCBLEND,&src))&&src==D3DBLEND_SRCALPHA&&SUCCEEDED(native->GetRenderState(D3DRS_DESTBLEND,&dst))&&dst==D3DBLEND_ONE&&SUCCEEDED(native->GetRenderState(D3DRS_ALPHABLENDENABLE,&on))&&on&&SUCCEEDED(native->GetRenderState(D3DRS_BLENDOP,&operation))&&operation==D3DBLENDOP_ADD&&SUCCEEDED(native->GetRenderState(D3DRS_ZWRITEENABLE,&zwrite))&&!zwrite;
}
}
bool DrawDnd(CM2SceneRender& context,CGxBatch& draw,const Source& source,uint32_t index){
 if(!source.dnd||index>=2||mode.load(std::memory_order_acquire)!=source.dnd->mode||!MaterialsEnabled()||!Backend()||!CShaderEffect::s_enableShaders||context.m_curType!=0||context.m_pass!=M2PASS_0||!context.m_curModel||!context.m_curShared||!context.m_curShared->m_data||!context.m_curShared->m_skinData||!context.m_curBatch||!context.m_curSkinSection||!context.m_curMaterial||!context.m_curElement||(context.m_curElement->m_flags&1))return false;
 const auto& skin=*context.m_curShared->m_skinData;const auto& data=*context.m_curShared->m_data;const auto& batch=*context.m_curBatch;const auto& record=source.dnd->batches[index];const auto* raw=record.original.data();
 if(skin.m_batches.m_count!=3||&skin.m_batches[index]!=context.m_curBatch||skin.m_skinSections.m_count!=source.sectionCount||batch.m_skinSectionIndex!=Read<uint16_t>(raw,4)||batch.m_skinSectionIndex>=skin.m_skinSections.m_count||!context.m_curShared->m_skinSections||&context.m_curShared->m_skinSections[batch.m_skinSectionIndex]!=context.m_curSkinSection||batch.m_shader!=0x13||batch.m_textureCount!=2||batch.m_textureCoordComboIndex!=4||batch.m_materialLayer||context.m_curMaterial->m_flags!=0x13||context.m_curMaterial->m_blendMode!=4||data.m_textures.m_count!=18)return false;
 if(batch.m_materialIndex!=Read<uint16_t>(raw,10)||batch.m_colorIndex!=Read<uint16_t>(raw,8)||batch.m_textureComboIndex!=Read<uint16_t>(raw,16)||batch.m_textureWeightComboIndex!=Read<uint16_t>(raw,20)||batch.m_textureTransformComboIndex!=Read<uint16_t>(raw,22))return false;
 for(uint32_t i=0;i<3;++i){const uint32_t texture=uint32_t(batch.m_textureComboIndex)+i;if(texture>=data.m_textureCombos.m_count||data.m_textureCombos[texture]!=record.textures[i]||record.textures[i]>=18||data.m_textures[record.textures[i]].m_type!=0||data.m_textures[record.textures[i]].m_flags!=record.textureFlags[i]||!context.m_curModel->m_textures[record.textures[i]])return false;
  if(i<2&&(uint32_t(batch.m_textureWeightComboIndex)+i>=data.m_textureWeightCombos.m_count||data.m_textureWeightCombos[batch.m_textureWeightComboIndex+i]!=record.weights[i]||uint32_t(batch.m_textureTransformComboIndex)+i>=data.m_textureTransformCombos.m_count||data.m_textureTransformCombos[batch.m_textureTransformComboIndex+i]!=record.transforms[i]))return false;
 }
 try{
  auto& device=*g_theGxDevicePtr;Sync(device);if(!NativeContract(device)){static uint32_t reports=0;if(reports++<8)WLOG_WARN("native-dnd: suppressed batch=%u because exact native GPU shader/blend/depth contract differs",index);return false;}
  const auto selected=source.dnd->mode;CGxShader* pixel=nullptr;
  if(selected==1)pixel=Program(device,GxSh_Pixel,PixelNames[0],kDeathDecayFadeShader,sizeof(kDeathDecayFadeShader));
  if(selected==2)pixel=Program(device,GxSh_Pixel,PixelNames[1],kDeathDecayColourShader,sizeof(kDeathDecayColourShader));
  if(selected==3)pixel=Program(device,GxSh_Pixel,PixelNames[2],kDeathDecayMaskPS,sizeof(kDeathDecayMaskPS));
  if(selected==4)pixel=Program(device,GxSh_Pixel,PixelNames[3],kDeathDecayDetailPS,sizeof(kDeathDecayDetailPS));
  if(!pixel)return false;
  CGxShader* vertex=nullptr;CGxTex* mask=nullptr;
  if(selected>=3){vertex=Program(device,GxSh_Vertex,MaskVertexName,kDeathDecayMaskVS,sizeof(kDeathDecayMaskVS));mask=TextureGetGxTex(context.m_curModel->m_textures[record.textures[2]],1,nullptr);if(!vertex||!mask||mask->m_target!=GxTex_2d)return false;}
  State state(device);device.RsSet(GxRs_PixelShader,pixel);
  if(selected>=3){device.RsSet(GxRs_VertexShader,vertex);device.RsSet(GxRs_Texture2,mask);
#ifdef WXL_ENABLE_DX11
   if(device.m_api==GxApi_D3d11){
    auto& dx=static_cast<CGxDeviceD3d11&>(device);
    struct SamplerScope {CGxDeviceD3d11& d;CGxDeviceD3d11::SamplerOverride old;~SamplerScope(){d.ExchangeSamplerOverride(2,old);}} sampler{dx,dx.ExchangeSamplerOverride(2,{true,true})};
    device.Draw(&draw,1);
   }else
#endif
   {Sync(device);Sampler sampler(*NativeDevice(device));if(!sampler.Apply())return false;device.Draw(&draw,1);}
  }
  else device.Draw(&draw,1);
  return true;
 }catch(...){return false;}
}
void DestroyDndPrograms(CGxDevice& device) noexcept {try{for(const auto* name:PixelNames){auto* shader=device.m_shaderList[GxSh_Pixel].Ptr(name);if(shader)device.ShaderDestroy(shader);}auto* vertex=device.m_shaderList[GxSh_Vertex].Ptr(MaskVertexName);if(vertex)device.ShaderDestroy(vertex);}catch(...){} }
}
