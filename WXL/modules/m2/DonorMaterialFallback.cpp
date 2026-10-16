// Port of wxl-modern-m2/src/compat/Skin.cpp::DownConvertBatch donor policy.
// These are explicit presentation approximations, not retail shader equations.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "DonorMaterialFallback.hpp"
#include "NativeFeatures.hpp"
#include "platform/waystone/M2Bytes.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <cstdlib>
namespace wxl::native {
namespace {
constexpr size_t Limit=64u*1024u*1024u;
std::atomic<bool> enabled{WXL_ENABLE_DONOR_MATERIAL_FALLBACK!=0};
bool Changed(CVar*,const char*,const char* value,void*) {
 enabled.store(WXL_ENABLE_DONOR_MATERIAL_FALLBACK && value && std::strtol(value,nullptr,10)!=0,std::memory_order_release);return true;
}
uint16_t U16(const uint8_t* p){uint16_t value;std::memcpy(&value,p,2);return value;}
uint32_t U32(const uint8_t* p){uint32_t value;std::memcpy(&value,p,4);return value;}
void W16(uint8_t* p,uint16_t value){std::memcpy(p,&value,2);}
void W32(uint8_t* p,uint32_t value){std::memcpy(p,&value,4);}
bool Range(size_t size,uint32_t at,uint32_t count,uint32_t stride){return at<=size&&uint64_t(count)*stride<=size-at;}
bool StrictSplit(uint16_t shader,uint16_t count){
 if(shader<0x8000||count<2||count>3)return false;
 switch(shader&0x7fff){case 3:case 9:case 17:case 24:return true;default:return false;}
}
uint16_t Next(uint16_t base,uint32_t count){return uint32_t(base)+1<count?uint16_t(base+1):base;}
}
void RegisterDonorMaterialFallback(){
 auto* variable=CVar::Register("wxlDonorMaterialFallback","WXL donor SKIN diffuse/split approximation for unsupported source shaders; reload models after changing",0,WXL_ENABLE_DONOR_MATERIAL_FALLBACK?"1":"0",&Changed,0,false,nullptr,false);
 if(variable)Changed(variable,nullptr,variable->m_stringValue.m_str,nullptr);
}
bool DonorMaterialFallbackEnabled() noexcept {return enabled.load(std::memory_order_acquire)&&AssetsEnabled();}
bool PrepareDonorMaterialSkin(const uint8_t* model,size_t modelSize,const std::vector<uint8_t>& source,std::vector<uint8_t>& prepared,std::shared_ptr<const DonorMaterialRecipe>& recipe,std::string& reason){
 prepared.clear();recipe.reset();reason.clear();
 auto fail=[&](const char* text){reason=text;return false;};
 if(!model||modelSize<8||modelSize>Limit||std::memcmp(model,"MD21",4)||source.size()<48||source.size()>Limit||std::memcmp(source.data(),"SKIN",4))return fail("donor fallback source header");
 const uint8_t* body=nullptr;uint32_t bodySize=0;
 for(size_t offset=0;offset<modelSize;){
  if(modelSize-offset<8)return fail("donor fallback source chunk header");
  const uint32_t size=U32(model+offset+4);if(size>modelSize-offset-8)return fail("donor fallback source chunk bounds");
  if(!std::memcmp(model+offset,"MD21",4)){if(body)return fail("donor fallback repeated MD21");body=model+offset+8;bodySize=size;}
  offset+=8+size;
 }
 if(!body||bodySize<304||std::memcmp(body,"MD20",4)||U32(body+4)<272||U32(body+4)>274)return fail("donor fallback MD20 era");
 const auto count=U32(source.data()+36),at=U32(source.data()+40);
 if(count&&(at<48||!Range(source.size(),at,count,24)))return fail("donor fallback batch bounds");
 // Inspect first; ordinary strict models are unaffected by compatibility budgets.
 bool needed=false;for(uint32_t i=0;i<count;++i){const auto* batch=source.data()+at+i*24;const auto shader=U16(batch+2),textures=U16(batch+14);if(shader>=0x8000&&!wxl::waystone::m2bytes::SupportsSourceMaterial(shader,textures))needed=true;}
 if(!needed)return true;
 if(count>16384)return fail("donor fallback batch budget");
 const uint32_t materialCount=U32(body+0x70),materialAt=U32(body+0x74),textureCount=U32(body+0x50),textureAt=U32(body+0x54),comboCount=U32(body+0x80),comboAt=U32(body+0x84);
 const uint32_t weights=U32(body+0x90),transforms=U32(body+0x98);
 if(!Range(bodySize,materialAt,materialCount,4)||!Range(bodySize,textureAt,textureCount,16)||!Range(bodySize,comboAt,comboCount,2))return fail("donor fallback model material arrays");
 auto result=std::make_shared<DonorMaterialRecipe>();result->sourceBatches=count;
 result->modelFingerprint=wxl::waystone::m2bytes::Fingerprint({model,modelSize});result->sourceSkinFingerprint=wxl::waystone::m2bytes::Fingerprint(source);
 std::vector<uint8_t> batches;batches.reserve(size_t(count)*48);uint32_t nativeIndex=0;
 auto append=[&](const std::array<uint8_t,24>& batch){batches.insert(batches.end(),batch.begin(),batch.end());};
 for(uint32_t i=0;i<count;++i){
  const auto* raw=source.data()+at+i*24;std::array<uint8_t,24> primary;std::memcpy(primary.data(),raw,24);
  const auto shader=U16(raw+2),textures=U16(raw+14);
  if(wxl::waystone::m2bytes::SupportsSourceMaterial(shader,textures)||shader<0x8000){append(primary);nativeIndex+=StrictSplit(shader,textures)?2:1;continue;}
  // The existing native21/33/35 and DnD/Rift recipes have first refusal. Their
  // missing capabilities cannot be replaced with a fake generic ModMod equation.
  const uint16_t low=shader&0xff,index=shader&0x7fff;
  if(low==21||low==33||low==35)return fail("source21/33/35 requires prepared native material recipe");
  if(!textures)return fail("donor fallback needs a primary source texture");
  const uint32_t combo=U16(raw+16),material=U16(raw+10);
  if(material>=materialCount||combo>comboCount||textures>comboCount-combo)return fail("donor fallback source material reference");
  for(uint32_t stage=0;stage<textures;++stage)if(U16(body+comboAt+2*(combo+stage))>=textureCount)return fail("donor fallback source texture reference");
  DonorMaterialBatch audit;audit.sourceBatch=i;audit.firstNativeBatch=nativeIndex;audit.sourceShader=shader;audit.sourceTextureCount=textures;audit.source=primary;
  primary[0]&=0x10;
  if(index<=2){
   if(textures<2)return fail("named effect needs two real source stages");
   W16(primary.data()+14,2);audit.rule=DonorMaterialRule::NamedRebase;append(primary);++nativeIndex;
  }else if(low==0||low==2||low==3||low==9||low==17||low==24||low==1||low==15){
   if(textures<2||material+1>=materialCount||material==65535)return fail("donor split follower material/texture");
   // EnvSplit case2 leaves its primary count untouched in the donor. Native
   // shader evaluation consumes at most two stages; do not silently clamp it.
   if(low==2&&textures!=2)return fail("donor case2 primary exceeds native stages");
   auto follower=primary;W16(follower.data()+10,uint16_t(material+1));W16(follower.data()+12,1);W16(follower.data()+14,1);
   if(low==1||low==15){
    // Donor NextLookup shares the primary if a second weight/transform is absent.
    W16(primary.data()+2,0);W16(primary.data()+14,1);
    W16(follower.data()+16,uint16_t(combo+1));
    W16(follower.data()+20,Next(U16(raw+20),weights));W16(follower.data()+22,Next(U16(raw+22),transforms));
    // The strict serializer's environment single is a temporary representable
    // carrier. Finish verifies that contract and selects the real UV1 entry.
    W16(follower.data()+2,0x80);
    result->secondaryUV.push_back({nativeIndex+1,uint16_t(material+1),uint16_t(combo+1)});
    audit.rule=DonorMaterialRule::SecondaryUVSplit;
   }else{
    W16(primary.data()+2,low==2?0x401b:0x401c);W16(primary.data()+14,2);
    W16(follower.data()+2,0x10);audit.rule=DonorMaterialRule::EnvironmentSplit;
   }
   append(primary);append(follower);nativeIndex+=2;
  }else{
   switch(low){case 5:case 8:case 10:case 12:case 16:case 23:W16(primary.data()+2,0);audit.rule=DonorMaterialRule::DiffuseCollapse;break;
    default:W16(primary.data()+2,0x10);audit.rule=DonorMaterialRule::DefaultDiffuse;break;}
   W16(primary.data()+14,1);append(primary);++nativeIndex;
  }
  result->batches.push_back(audit);
 }
 if(source.size()+1+batches.size()>Limit)return fail("donor fallback output budget");
 std::vector<uint8_t> output=source;if(output.size()&1)output.push_back(0);const auto offset=uint32_t(output.size());
 output.insert(output.end(),batches.begin(),batches.end());W32(output.data()+36,uint32_t(batches.size()/24));W32(output.data()+40,offset);
 result->preparedBatches=uint32_t(batches.size()/24);result->nativeBatches=nativeIndex;result->preparedSkinFingerprint=wxl::waystone::m2bytes::Fingerprint(output);
 prepared.swap(output);recipe=std::move(result);return true;
}
bool FinishDonorMaterialSkin(const DonorMaterialRecipe& recipe,const std::vector<uint8_t>& model,std::vector<uint8_t>& converted,std::string& reason){
 auto fail=[&](const char* text){reason=text;return false;};
 if(model.size()<304||std::memcmp(model.data(),"MD20",4)||U32(model.data()+4)!=264||converted.size()<48||std::memcmp(converted.data(),"SKIN",4))return fail("donor fallback converted header");
 const auto count=U32(converted.data()+36),at=U32(converted.data()+40);
 if(count!=recipe.nativeBatches||(count&&(at<48||!Range(converted.size(),at,count,24))))return fail("donor fallback converted batch identity");
 if(recipe.topologyParent){
  if(recipe.nativeBatchOrigins.size()!=count||recipe.topologyBatchTemplate.size()!=recipe.topologyParent->nativeBatches)return fail("donor topology provenance count");
  for(uint32_t batch=0;batch<count;++batch){
   const auto origin=recipe.nativeBatchOrigins[batch];if(origin>=recipe.topologyBatchTemplate.size())return fail("donor topology provenance reference");
   const auto* actual=converted.data()+at+24*batch;const auto& expected=recipe.topologyBatchTemplate[origin];
   // Only the section identity changes during native full-geometry partition.
   for(uint32_t byte=0;byte<24;++byte)if(byte!=4&&byte!=5&&actual[byte]!=expected[byte])return fail("donor topology material batch changed");
  }
 }
 if(recipe.secondaryUV.empty())return true;
 const auto unitCount=U32(model.data()+0x88),unitAt=U32(model.data()+0x8c);
 if(unitCount!=6||!Range(model.size(),unitAt,unitCount,2)||U16(model.data()+unitAt+2)!=0xffff||U16(model.data()+unitAt+10)!=1)return fail("donor fallback native UV lookup contract");
 for(const auto& fix:recipe.secondaryUV){
  if(fix.nativeBatch>=count)return fail("donor fallback secondary UV batch");
  auto* batch=converted.data()+at+fix.nativeBatch*24;
  if(U16(batch+2)!=0||U16(batch+14)!=1||U16(batch+18)!=1||U16(batch+10)!=fix.material||U16(batch+16)!=fix.textureCombo)return fail("donor fallback secondary UV identity");
 }
 for(const auto& fix:recipe.secondaryUV)W16(converted.data()+at+fix.nativeBatch*24+18,5);
 return true;
}
}
