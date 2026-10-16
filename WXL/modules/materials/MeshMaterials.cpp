// Bounded source21/one-bone/two-UV AddAlpha family. All other source IDs stay native/refused.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "MeshMaterials.hpp"
#include "AssetCompat.hpp"
#include "NativeFeatures.hpp"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/CM2SceneRender.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include <windows.h>
#include <d3d9.h>
#include <cstring>
#include <span>
#include "shaders/CombinersModAddAlphaVs.h"
#include "shaders/CombinersModAddAlphaPs.h"
namespace wxl::native::mesh_materials {
namespace {
template<class T>T Read(const uint8_t* p,size_t at){T value{};std::memcpy(&value,p+at,sizeof(T));return value;}
bool Range(size_t size,uint32_t offset,uint32_t count,uint32_t stride){return offset<=size&&uint64_t(count)*stride<=size-offset;}
bool D3D9(){
 if(!g_theGxDevicePtr||(g_theGxDevicePtr->m_api!=GxApi_D3d9&&g_theGxDevicePtr->m_api!=GxApi_D3d9Ex))return false;
 // Source preparation can execute on a loader worker. Native cached caps are
 // immutable device metadata; do not invoke a COM device method from here.
 const auto& caps=g_theGxDevicePtr->Caps();
 return caps.m_shaderTargets[GxSh_Vertex]>=3&&caps.m_shaderTargets[GxSh_Pixel]>=4;
}
uint64_t Fingerprint(std::span<const uint8_t> bytes){uint64_t hash=14695981039346656037ull;for(auto b:bytes){hash^=b;hash*=1099511628211ull;}return hash;}
}
std::shared_ptr<const Source> Prepare(const uint8_t* raw,size_t size,const std::vector<uint8_t>& skin,std::vector<uint8_t>& prepared){
 prepared.clear();
 if(!MaterialsEnabled()||!D3D9()||!raw||size<312||size>64u*1024*1024||skin.size()<48||std::memcmp(raw,"MD21",4)||std::memcmp(skin.data(),"SKIN",4))return {};
 const uint32_t bodySize=Read<uint32_t>(raw,4);if(bodySize<304||bodySize>size-8)return {};
 const auto* body=raw+8;
 if(std::memcmp(body,"MD20",4)||Read<uint32_t>(body,4)!=274||Read<uint32_t>(body,0x2c)!=1||Read<uint32_t>(body,0x44)!=1)return {};
 const auto batches=Read<uint32_t>(skin.data(),36),at=Read<uint32_t>(skin.data(),40),sections=Read<uint32_t>(skin.data(),28),sectionAt=Read<uint32_t>(skin.data(),32);
 if(batches!=1||sections!=1||at<48||sectionAt<48||!Range(skin.size(),at,1,24)||!Range(skin.size(),sectionAt,1,48))return {};
 const auto* batch=skin.data()+at;const auto* section=skin.data()+sectionAt;
 if(Read<uint16_t>(batch,2)!=0x8015||Read<uint16_t>(batch,14)!=2||Read<uint16_t>(batch,4)||Read<uint16_t>(batch,12)||Read<uint16_t>(section,12)!=1||Read<uint16_t>(section,16)!=1)return {};
 const uint32_t boneCombos=Read<uint32_t>(body,0x78),boneAt=Read<uint32_t>(body,0x7c),palette=Read<uint16_t>(section,14);
 if(palette>=boneCombos||!Range(bodySize,boneAt,boneCombos,2)||Read<uint16_t>(body,boneAt+2*palette)!=0)return {};
 const auto vertexCount=Read<uint32_t>(body,0x3c),vertexAt=Read<uint32_t>(body,0x40),lookupCount=Read<uint32_t>(skin.data(),4),lookupAt=Read<uint32_t>(skin.data(),8),propsAt=Read<uint32_t>(skin.data(),24);
 if(!Range(bodySize,vertexAt,vertexCount,48)||!Range(skin.size(),lookupAt,lookupCount,2)||Read<uint32_t>(skin.data(),20)!=lookupCount||!Range(skin.size(),propsAt,lookupCount,4))return {};
 for(uint32_t i=0;i<lookupCount;++i){
  const auto vertex=Read<uint16_t>(skin.data(),lookupAt+2*i);if(vertex>=vertexCount||Read<uint32_t>(skin.data(),propsAt+4*i)!=0)return {};
  const auto* v=body+vertexAt+48*vertex;
  if(v[12]!=255||v[13]||v[14]||v[15]||Read<uint32_t>(v,16)!=0)return {};
 }
 const uint32_t materials=Read<uint32_t>(body,0x70),materialAt=Read<uint32_t>(body,0x74),material=Read<uint16_t>(batch,10);
 if(material>=materials||!Range(bodySize,materialAt,materials,4))return {};
 auto result=std::make_shared<Source>();result->flags=Read<uint16_t>(body,materialAt+material*4);result->blend=Read<uint16_t>(body,materialAt+material*4+2);
 if(result->flags>31||result->blend!=7)return {};
 const uint32_t textures=Read<uint32_t>(body,0x50),textureAt=Read<uint32_t>(body,0x54),combos=Read<uint32_t>(body,0x80),comboAt=Read<uint32_t>(body,0x84),combo=Read<uint16_t>(batch,16);
 if(textures!=2||combo>combos||combos-combo<2||!Range(bodySize,textureAt,textures,16)||!Range(bodySize,comboAt,combos,2))return {};
 std::span<const uint8_t> txid;
 for(size_t off=0;off<size;){if(size-off<8)return {};const auto n=Read<uint32_t>(raw,off+4);if(n>size-off-8)return {};if(!std::memcmp(raw+off,"TXID",4)){if(!txid.empty()||n!=8)return {};txid={raw+off+8,n};}off+=8+n;}
 if(txid.size()!=8)return {};
 for(uint32_t i=0;i<2;++i){
  const auto index=Read<uint16_t>(body,comboAt+2*(combo+i));if(index>=textures||Read<uint32_t>(body,textureAt+index*16)!=0||Read<uint32_t>(body,textureAt+index*16+8)!=0)return {};
  result->textureIndices[i]=index;result->textureIds[i]=Read<uint32_t>(txid.data(),index*4);if(!result->textureIds[i])return {};
  for(uint32_t head:{0x90u,0x98u}){
   const uint32_t count=Read<uint32_t>(body,head),lookup=Read<uint32_t>(body,head+4),first=Read<uint16_t>(batch,head==0x90?20:22);
   if(first>count||count-first<2||!Range(bodySize,lookup,count,2))return {};
   const auto value=Read<uint16_t>(body,lookup+2*(first+i));
   (head==0x90?result->weightIndices:result->transformIndices)[i]=value;
  }
 }
 result->modelFingerprint=Fingerprint({raw,size});result->skinFingerprint=Fingerprint(skin);std::memcpy(result->batch.data(),batch,24);
 prepared=skin;
 // Private conversion clone only: donor source21 decodes to packed4011 UV0,1.
 const uint16_t packed=0x4011;std::memcpy(prepared.data()+at+2,&packed,2);
 return result;
}
bool ConvertedMatches(const Source& source,const std::vector<uint8_t>& skin) noexcept {
 if(skin.size()<48||Read<uint32_t>(skin.data(),36)!=1)return false;const auto at=Read<uint32_t>(skin.data(),40);if(!Range(skin.size(),at,1,24))return false;
 const auto* b=skin.data()+at;const auto* raw=source.batch.data();
 if(Read<uint16_t>(b,2)!=0||Read<uint16_t>(b,14)!=2||Read<uint16_t>(b,18)!=4)return false;
 for(size_t offset:{size_t(4),size_t(8),size_t(10),size_t(12),size_t(16),size_t(20),size_t(22)})if(Read<uint16_t>(b,offset)!=Read<uint16_t>(raw,offset))return false;
 return true;
}
namespace {
constexpr const char* VertexName="WXL_Mesh_AddAlpha21_VS_20261016";
constexpr const char* PixelName="WXL_Mesh_AddAlpha21_PS_20261016";
CGxShader* Program(CGxDevice& device,EGxShTarget target,const char* name,const uint8_t* code,uint32_t size){
 auto* shader=device.m_shaderList[target].Ptr(name);
 if(shader){if(shader->m_target!=target||shader->m_code.Count()!=size||std::memcmp(shader->m_code.m_data,code,size))return nullptr;}
 else{
  shader=device.m_shaderList[target].New(name,0,0);if(!shader)return nullptr;
  shader->m_refCount=1;shader->m_target=target;shader->m_code.SetCount(size);std::memcpy(shader->m_code.m_data,code,size);
 }
 // Native list/reset ownership preserves code and invalidates/recreates API handles.
 return shader->Valid()?shader:nullptr;
}
struct StateScope {
 CGxDevice& device;explicit StateScope(CGxDevice& d):device(d){device.RsPush();}
 ~StateScope(){device.RsPop();if(device.m_api==GxApi_D3d9)static_cast<CGxDeviceD3d&>(device).IStateSync();else static_cast<CGxDeviceD3d9Ex&>(device).IStateSync();}
};
}
bool Draw(CM2SceneRender& context,CGxBatch& draw){
 if(!MaterialsEnabled()||!D3D9()||!CShaderEffect::s_enableShaders||context.m_curType!=0||!context.m_curShared||!context.m_curShared->m_data||!context.m_curShared->m_skinData||!context.m_curModel||!context.m_curBatch||!context.m_curSkinSection||!context.m_curMaterial||context.m_pass!=M2PASS_0)return false;
 const auto* source=MeshSourceFor(*context.m_curShared);if(!source)return false;
 const auto& data=*context.m_curShared->m_data;const auto& skin=*context.m_curShared->m_skinData;const auto& b=*context.m_curBatch;const auto* raw=source->batch.data();
 if(!context.m_curModel->m_boneMatrices||context.m_curSkinSection->m_boneComboIndex>=data.m_boneCombos.m_count||data.m_boneCombos[context.m_curSkinSection->m_boneComboIndex]!=0)return false;
 if(skin.m_batches.m_count!=1||skin.m_batches.Data()!=context.m_curBatch||data.m_bones.m_count!=1||context.m_curSkinSection->m_boneCount!=1||b.m_shader!=0x11||b.m_textureCount!=2||b.m_textureCoordComboIndex!=4||b.m_materialLayer||context.m_curElement->m_flags&1)return false;
 if(b.m_materialIndex!=Read<uint16_t>(raw,10)||b.m_textureComboIndex!=Read<uint16_t>(raw,16)||b.m_colorIndex!=Read<uint16_t>(raw,8)||b.m_textureWeightComboIndex!=Read<uint16_t>(raw,20)||b.m_textureTransformComboIndex!=Read<uint16_t>(raw,22)||context.m_curMaterial->m_blendMode!=source->blend||context.m_curMaterial->m_flags!=source->flags)return false;
 for(uint32_t i=0;i<2;++i){
  if(b.m_textureComboIndex+i>=data.m_textureCombos.m_count||data.m_textureCombos[b.m_textureComboIndex+i]!=source->textureIndices[i]||!context.m_curModel->m_textures[source->textureIndices[i]])return false;
  if(b.m_textureWeightComboIndex+i>=data.m_textureWeightCombos.m_count||data.m_textureWeightCombos[b.m_textureWeightComboIndex+i]!=source->weightIndices[i]||b.m_textureTransformComboIndex+i>=data.m_textureTransformCombos.m_count||data.m_textureTransformCombos[b.m_textureTransformComboIndex+i]!=source->transformIndices[i])return false;
 }
 try{auto* vertex=Program(*g_theGxDevicePtr,GxSh_Vertex,VertexName,kCombinersModAddAlphaVs,sizeof(kCombinersModAddAlphaVs));
  auto* pixel=Program(*g_theGxDevicePtr,GxSh_Pixel,PixelName,kCombinersModAddAlphaPs,sizeof(kCombinersModAddAlphaPs));
  if(!vertex||!pixel)return false;StateScope state(*g_theGxDevicePtr);
  g_theGxDevicePtr->RsSet(GxRs_VertexShader,vertex);g_theGxDevicePtr->RsSet(GxRs_PixelShader,pixel);
  g_theGxDevicePtr->Draw(&draw,1);return true;
 }catch(...){return false;}
}
void DestroyPrograms(CGxDevice& device) noexcept {
 try{auto* pixel=device.m_shaderList[GxSh_Pixel].Ptr(PixelName);if(pixel)device.ShaderDestroy(pixel);
  auto* vertex=device.m_shaderList[GxSh_Vertex].Ptr(VertexName);if(vertex)device.ShaderDestroy(vertex);
 }catch(...){}
}
}
