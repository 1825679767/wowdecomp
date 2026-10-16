// Copyright (C) 2026 WarcraftXL. GPLv3.
// Byte adaptation only. Native Waystone retains ownership of the parsed object,
// offset relocation, animation evaluator, skin loader and GPU resources.
#include "platform/waystone/M2Bytes.hpp"
#include "platform/waystone/WxlFirstTextureApproximation.hpp"
#include "platform/waystone/SkinPartition.hpp"
#include "platform/waystone/SequenceCompatibility.hpp"
#include "platform/waystone/NativeSequenceHashPolicy.hpp"
#include "platform/waystone/PrivateRacePolicy.hpp"
#include "platform/waystone/SkeletonEmbed.hpp"
#include "platform/waystone/SkeletonParentPaths.hpp"
#include "../../../extensions/wxl-modern-m2/src/load/M2NativeInternal.hpp"
#include "../../../extensions/wxl-modern-m2/src/load/M2WalkLayout.hpp"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <set>
#include <map>
#include <cstdio>
namespace wxl::waystone::m2bytes {
namespace {
namespace f=wxl::structure::m2;
namespace d=wxl::runtime::m2native::detail;
constexpr uint32_t Limit=64u*1024u*1024u;
struct AnimationContext {std::string stem;Read read;std::string animationStem;};
thread_local AnimationContext* animationContext{};
bool ReadAnimation(uint16_t id,uint16_t variation,uint32_t,std::vector<uint8_t>& bytes){
    char suffix[32]{};std::snprintf(suffix,sizeof(suffix),"%04u-%02u.anim",id,variation);
    return animationContext && animationContext->read(((animationContext->animationStem.empty()?animationContext->stem:animationContext->animationStem)+suffix).c_str(),bytes);
}
// Model-owned AFM2 tracks retain the child stem even when bone/attachment
// animation comes from the exact one-hop parent skeleton.
bool ReadModelAnimation(uint16_t id,uint16_t variation,uint32_t,std::vector<uint8_t>& bytes){
 char suffix[32]{};std::snprintf(suffix,sizeof(suffix),"%04u-%02u.anim",id,variation);
 return animationContext && animationContext->read((animationContext->stem+suffix).c_str(),bytes);
}
bool SameNamedPath(std::string a,std::string b){
 auto normalize=[](std::string& s){for(auto& c:s){if(c=='/')c='\\';if(c>='A' && c<='Z')c=char(c-'A'+'a');}};
 normalize(a);normalize(b);return a==b;
}
uint32_t Tag(const char* s){return uint32_t(uint8_t(s[0]))|(uint32_t(uint8_t(s[1]))<<8)|(uint32_t(uint8_t(s[2]))<<16)|(uint32_t(uint8_t(s[3]))<<24);}
bool Array(std::span<const uint8_t> b,uint32_t at,uint32_t stride) {
    if(at>b.size() || b.size()-at<8) return false;
    uint32_t n=d::Rd32(b.data()+at),o=d::Rd32(b.data()+at+4);
    return !n || (o>=((d::Rd32(b.data()+0x10)&f::kFlagUseTextureCombinerCombos)?sizeof(f::M2Header):offsetof(f::M2Header,textureCombinerCombos)) && d::FitsInBody(uint32_t(b.size()),o,n,stride));
}
bool Track(std::span<const uint8_t> b,uint32_t at,uint32_t stride) {
    if(at>b.size() || b.size()-at<(stride?20u:12u)) return false;
    if(d::Rd16(b.data()+at)>3) return false;
    const int16_t loop=static_cast<int16_t>(d::Rd16(b.data()+at+2));
    auto* h=reinterpret_cast<const f::M2Header*>(b.data());
    if(loop!=-1 && (loop<0 || uint32_t(loop)>=h->globalLoops.count)) return false;
    for(uint32_t pair=at+4;pair<=at+(stride?12:4);pair+=8) {
        if(!Array(b,pair,8)) return false;
        uint32_t n=d::Rd32(b.data()+pair),o=d::Rd32(b.data()+pair+4);
        for(uint32_t i=0;i<n;++i)
            if(!Array(b,o+i*8,pair==at+4?4:stride)) return false;
    }
    return true;
}
bool Validate(std::span<const uint8_t> b,bool modern,std::string& reason,bool screenBlend=false,bool nativeSequenceHash=false) {
    auto detail=[&](const char* label,uint32_t at) {char text[100]{};std::snprintf(text,sizeof(text),"%s at=0x%x",label,at);reason=text;return false;};
    if(b.size()<sizeof(f::M2Header)) return false;
    auto* h=reinterpret_cast<const f::M2Header*>(b.data());
    for(const auto& a:d::kHeaderArrays) {
        if((a.traits&d::kArrayCombinerGated) && !(h->globalFlags&f::kFlagUseTextureCombinerCombos)) continue;
        uint32_t stride=a.stride;
        if(modern && a.at==offsetof(f::M2Header,cameras)) stride=0x74;
        if(modern && a.at==offsetof(f::M2Header,particleEmitters)) stride=0x1ec;
        if(!Array(b,a.at,stride)) return detail("invalid header array",a.at);
        uint32_t n=d::Rd32(b.data()+a.at),o=d::Rd32(b.data()+a.at+4);
        for(uint32_t i=0;i<n;++i) {
            for(uint32_t j=0;j<a.stepCount;++j) {
                const auto& s=a.steps[j];
                uint32_t at=o+i*stride+s.at;
                if(modern && a.at==offsetof(f::M2Header,cameras)) at-=4;
                if(s.kind==d::kStepArray ? !Array(b,at,s.stride) : !Track(b,at,s.stride)) return detail("invalid nested array/track",at);
            }
            if(modern && a.at==offsetof(f::M2Header,cameras) && !Track(b,o+i*stride+0x60,12)) return detail("invalid animated FOV",o+i*stride+0x60);
        }
    }
    for(uint32_t i=0;i<h->sequences.count;++i) {
        auto* seq=reinterpret_cast<const f::M2Sequence*>(b.data()+h->sequences.offset+i*64);
        // Modern high IDs are checked after the explicit curated-eight mapper.
        // Final classic validation still rejects every unresolved high ID.
        if(!(seq->flags&0x20) || (seq->flags&0x40) || (!modern && !nativeSequenceHash && seq->id>505)) return detail("unsupported external/alias/high-ID sequence",h->sequences.offset+i*64);
        if(seq->variationNext!=-1 && (seq->variationNext<0 || uint32_t(seq->variationNext)>=h->sequences.count)) return false;
    }
    // Native blend admission has not yet been widened for modern emitters.
    for(uint32_t i=0;i<h->particleEmitters.count;++i) {
        auto* p=b.data()+h->particleEmitters.offset+i*(modern?0x1ec:0x1dc);
        if(p[0x28]>((modern || screenBlend)?7:6)) return false;
    }
    return true;
}
// This is the single-stage branch of WXL Skin.cpp::DecodeBlendBits.
// The separate named/two-stage branches below admit only binary-verified cases.
bool SingleStage(uint32_t shader,uint32_t textures) {
    return shader<0x8000 && textures>=1 && textures<=2 &&
        !(textures>1 && (shader&0x4000) && ((shader>>4)&7) && (shader&7));
}
// The optional combiner fields occupy bytes 304..311. A flag-less source can
// place its first payload at 304; insert owned bytes and rebase every described
// body-relative array rather than overwriting that payload.
void ExpandCombinerHeader(std::span<const uint8_t> body,std::vector<uint8_t>& result) {
    const auto* h=reinterpret_cast<const f::M2Header*>(body.data());
    if(h->globalFlags&f::kFlagUseTextureCombinerCombos)return;
    constexpr uint32_t start=offsetof(f::M2Header,textureCombinerCombos);
    result.insert(result.begin()+start,8,0);
    auto shifted=[](uint32_t at){return at>=start?at+8:at;};
    auto pair=[&](uint32_t at) {
        const uint32_t count=d::Rd32(body.data()+at),offset=d::Rd32(body.data()+at+4);
        d::Wr32(result.data()+shifted(at)+4,count?shifted(offset):0);
    };
    auto track=[&](uint32_t at,uint32_t stride) {
        for(uint32_t field=at+4;field<=at+(stride?12:4);field+=8) {
            pair(field);
            const uint32_t count=d::Rd32(body.data()+field),offset=d::Rd32(body.data()+field+4);
            for(uint32_t i=0;i<count;++i)pair(offset+8*i);
        }
    };
    for(const auto& a:d::kHeaderArrays) {
        if(a.traits&d::kArrayCombinerGated)continue;
        pair(a.at);
        uint32_t stride=a.stride;
        if(a.at==offsetof(f::M2Header,cameras))stride=0x74;
        if(a.at==offsetof(f::M2Header,particleEmitters))stride=0x1ec;
        const uint32_t count=d::Rd32(body.data()+a.at),offset=d::Rd32(body.data()+a.at+4);
        for(uint32_t i=0;i<count;++i) {
            for(uint32_t j=0;j<a.stepCount;++j) {
                const auto& step=a.steps[j];uint32_t at=offset+i*stride+step.at;
                if(a.at==offsetof(f::M2Header,cameras))at-=4;
                if(step.kind==d::kStepArray)pair(at);else track(at,step.stride);
            }
            if(a.at==offsetof(f::M2Header,cameras))track(offset+i*stride+0x60,12);
        }
    }
}
bool TwoStage(uint32_t shader,uint32_t textures) {
    return shader<0x8000 && textures==2 && (shader&0x4000) &&
        ((shader>>4)&7)==1 && ((shader&7)==1 || (shader&7)==3 || (shader&7)==4);
}
uint16_t UVIndex(uint16_t shader) {
    if(shader&0x80)return (shader&8)?1:3; // (-1,-1) or (-1,0)
    return (shader&8)?0:4;             // (0,-1) or (0,1)
}
bool EnvSplit(uint32_t shader,uint32_t textures){
    if(shader<0x8000 || textures<2 || textures>3)return false;
    switch(shader&0x7fff){case 3:case 9:case 17:case 24:return true;default:return false;}
}
bool CollapsedNamed(uint32_t shader,uint32_t textures){
    // Exact current x32 DownConvertBatch cases that use a single diffuse pass.
    // Unknown named effects and environment splits remain separate capabilities.
    if(shader<0x8000 || textures<1 || textures>3)return false;
    switch(shader&0x7fff){case 5:case 8:case 10:case 12:case 16:case 23:return true;default:return false;}
}
bool NamedEffect(uint32_t shader,uint32_t textures) {
    // Native RVA 0x123910 / 0x123e90 dispatch low effect IDs 1,2,3.
    // WXL's source IDs 0,1,2 map to those legacy Diffuse_T1_Env effects.
    return shader>=0x8000 && shader<=0x8002 && textures==2;
}
bool Skin(const std::vector<uint8_t>& b,const f::M2Header& h,std::span<const uint8_t> body,Stats& stats,bool extendedIndices,uint32_t paletteLimit) {
    auto refuse=[&](const char* why){stats.reason=why;return false;};
    if(b.size()<48 || d::Rd32(b.data())!=Tag("SKIN")) return refuse("skin header");
    auto array=[&](uint32_t at,uint32_t stride) {
        uint32_t n=d::Rd32(b.data()+at),o=d::Rd32(b.data()+at+4);
        return !n || (o>=48 && d::FitsInBody(uint32_t(b.size()),o,n,stride));
    };
    for(auto pair:{std::pair{4u,2u},{12u,2u},{20u,4u},{28u,48u},{36u,24u}})
        if(!array(pair.first,pair.second)) return refuse("skin array bounds");
    uint32_t vertices=d::Rd32(b.data()+4),vo=d::Rd32(b.data()+8),indices=d::Rd32(b.data()+12),io=d::Rd32(b.data()+16);
    for(uint32_t i=0;i<vertices;++i) if(d::Rd16(b.data()+vo+2*i)>=h.vertices.count) return refuse("skin vertex reference");
    for(uint32_t i=0;i<indices;++i) if(d::Rd16(b.data()+io+2*i)>=vertices) return refuse("skin index reference");
    uint32_t sections=d::Rd32(b.data()+28),so=d::Rd32(b.data()+32);
    for(uint32_t i=0;i<sections;++i) {
        auto* s=b.data()+so+i*48;
        // Native palette upload at 0x127870 starts at VS register 31 and
        // writes three registers per bone into the 256-register cache.
        // Owned fixtures partition palettes before admission; ordinary inputs must fit.
        if(d::Rd16(s+12)>paletteLimit || d::Rd16(s+16)>4 ||
           (d::Rd16(s+10) && !d::Rd16(s+12)))return refuse("skin palette/influence budget");
        const uint32_t first=d::Rd16(s+8)|(extendedIndices?uint32_t(d::Rd16(s+2))<<16:0);
        if((!extendedIndices && d::Rd16(s+2)!=0) || uint32_t(d::Rd16(s+4))+d::Rd16(s+6)>vertices ||
           uint64_t(first)+d::Rd16(s+10)>indices ||
           uint32_t(d::Rd16(s+14))+d::Rd16(s+12)>h.boneCombos.count) return refuse("skin section range");
    }
    uint32_t batches=d::Rd32(b.data()+36),bo=d::Rd32(b.data()+40);
    for(uint32_t i=0;i<batches;++i) {
        auto* p=b.data()+bo+i*24;uint32_t shader=d::Rd16(p+2),textures=d::Rd16(p+14);
        if(!SupportsSourceMaterial(shader,textures) || d::Rd16(p+4)>=sections ||
           d::Rd16(p+10)>=h.materials.count || uint32_t(d::Rd16(p+16))+textures>h.textureCombos.count) return refuse("skin material reference");
        if(EnvSplit(shader,textures) && uint32_t(d::Rd16(p+10))+1>=h.materials.count)return refuse("environment follower material");
        uint16_t color=d::Rd16(p+8);
        if(color!=0xffff && color>=h.colors.count)return refuse("skin color reference");
        for(uint32_t j=0;j<textures;++j)
            if(d::Rd16(body.data()+h.textureCombos.offset+2*(d::Rd16(p+16)+j))>=h.textures.count)return refuse("skin texture reference");
        // Validate every retained stage. Optional -1 lookups mean
        // no weight/transform; populated lookup tables must reference real data.
        auto lookup=[&](const f::M2Array& array,uint32_t count,uint16_t index,uint32_t stage,bool weight) {
            if(!array.count)return true;
            if(index>=array.count){
                // x32 NextLookup reuses the primary when its secondary entry is
                // absent. Serialize that same shared stage explicitly for x64.
                if(stage!=1 || index!=array.count || array.count>=65535)return refuse("skin stage lookup index");
                const auto primary=d::Rd16(body.data()+array.offset+2*(array.count-1));
                if(primary!=0xffff && primary>=count)return false;
                if(weight)stats.sharedWeightStage=1;else stats.sharedTransformStage=1;
                return true;
            }
            uint16_t value=d::Rd16(body.data()+array.offset+2*index);
            return value==0xffff || value<count;
        };
        for(uint32_t stage=0;stage<((NamedEffect(shader,textures)||TwoStage(shader,textures)||EnvSplit(shader,textures))?2u:1u);++stage) {
            const uint32_t weight=uint32_t(d::Rd16(p+20))+stage,transform=uint32_t(d::Rd16(p+22))+stage;
            if(weight>0xffff || transform>0xffff ||
               !lookup(h.textureWeightCombos,h.textureWeights.count,uint16_t(weight),stage,true) ||
               !lookup(h.textureTransformCombos,h.textureTransforms.count,uint16_t(transform),stage,false))return refuse("skin stage weight/transform reference");
        }
        if(TwoStage(shader,textures) || EnvSplit(shader,textures))++stats.twoStageBatches;
        if(TwoStage(shader,textures) && (shader&7)==3)++stats.stage13Batches;
        if(EnvSplit(shader,textures))++stats.envSplitBatches;
        if(CollapsedNamed(shader,textures))++stats.collapsedNamedBatches;
    }
    return true;
}
}
bool SupportsSourceMaterial(uint32_t shader,uint32_t textures){return SingleStage(shader,textures) || NamedEffect(shader,textures) || TwoStage(shader,textures) || CollapsedNamed(shader,textures) || EnvSplit(shader,textures);}
uint64_t Fingerprint(std::span<const uint8_t> bytes) {
    uint64_t value=UINT64_C(14695981039346656037);
    for(uint8_t byte:bytes)value=(value^byte)*UINT64_C(1099511628211);
    return value;
}
namespace {
bool ValidateClassicHandSourceImpl(std::span<const uint8_t> body,std::span<const uint8_t> skinBytes,uint32_t paletteLimit,unsigned contract){
 const bool namedStage=contract==1,currentGlaive=contract==2;
 try{std::string reason;Stats stats;
  if(body.size()<sizeof(f::M2Header) || body.size()>16u*1024*1024 || skinBytes.size()>8u*1024*1024 ||
   d::Rd32(body.data())!=f::kMagicMD20 || d::Rd32(body.data()+4)!=264 || !paletteLimit || paletteLimit>75 || !Validate(body,false,reason))return false;
  const auto& h=*reinterpret_cast<const f::M2Header*>(body.data());
  if(namedStage && h.textures.count!=2)return false;
  if(currentGlaive && (h.bones.count!=8 || h.vertices.count!=680 || h.textures.count!=3 || h.materials.count!=4))return false;
  if(!h.bones.count || !h.vertices.count || h.vertices.count>65535 || h.numSkinProfiles!=1 || h.particleEmitters.count || h.ribbonEmitters.count ||
     !h.textures.count || h.textures.count>32 || !h.materials.count)return false;
  std::vector<uint8_t> skinCopy(skinBytes.begin(),skinBytes.end());
  if(!Skin(skinCopy,h,body,stats,false,paletteLimit) || stats.twoStageBatches || stats.sharedWeightStage || stats.sharedTransformStage || stats.collapsedNamedBatches)return false;
  skin::Plan plan;if(!skin::Partition(body,skinBytes,h.bones.count,false,plan,reason,paletteLimit) || plan.splitSections || !plan.sourceTriangles)return false;
  const auto sections=d::Rd32(skinBytes.data()+28),sectionAt=d::Rd32(skinBytes.data()+32);
  for(uint32_t i=0;i<sections;++i){const auto* section=skinBytes.data()+sectionAt+i*48;
   if(!d::Rd16(section+12) || d::Rd16(section+12)>paletteLimit || !d::Rd16(section+16))return false;
  }
  for(uint32_t i=0;i<h.boneCombos.count;++i)if(d::Rd16(body.data()+h.boneCombos.offset+i*2)>=h.bones.count)return false;
  for(uint32_t i=0;i<h.vertices.count;++i){const auto* v=body.data()+h.vertices.offset+i*48;unsigned weights=0;
   for(unsigned k=0;k<4;++k)weights+=v[12+k];if(weights!=0 && weights!=255)return false;
   for(unsigned offset:{0u,4u,8u,20u,24u,28u,32u,36u,40u,44u}){float value=0;std::memcpy(&value,v+offset,4);if(!std::isfinite(value))return false;}
  }
  bool replacement=false;for(uint32_t i=0;i<h.textures.count;++i){const auto* t=body.data()+h.textures.offset+i*16;const auto type=d::Rd32(t);
   if(type!=0 && type!=2)return false;replacement=replacement || type==2;
  }
  if(!replacement)return false;
  for(uint32_t i=0;i<h.materials.count;++i)if(d::Rd16(body.data()+h.materials.offset+i*4+2)>6)return false;
  const auto batches=d::Rd32(skinBytes.data()+36),batchAt=d::Rd32(skinBytes.data()+40);
  uint32_t named=0;
  if(!batches)return false;for(uint32_t i=0;i<batches;++i){const auto* batch=skinBytes.data()+batchAt+i*24;
   const auto shader=d::Rd16(batch+2),stages=d::Rd16(batch+14);
   if(shader<0x8000){if(stages!=1)return false;continue;}
   // Current sword1542 converts source0x8001 into exact native effect2.
   // No source-era named IDs, other capability-dependent effects or generic
   // two-stage shader policy are inferred from this narrow prepared contract.
   if((!namedStage && !currentGlaive) || shader!=(currentGlaive?0x8001:0x8002) || stages!=2 || d::Rd16(batch+18)!=0)return false;
   const auto material=d::Rd16(batch+10),texture=d::Rd16(batch+16),weight=d::Rd16(batch+20),transform=d::Rd16(batch+22);
   if(d::Rd16(body.data()+h.materials.offset+material*4+2)!=0)return false;
   if(!h.textureWeightCombos.count || uint32_t(weight)+2>h.textureWeightCombos.count ||
      !h.textureTransformCombos.count || uint32_t(transform)+2>h.textureTransformCombos.count)return false;
   const auto primary=d::Rd16(body.data()+h.textureCombos.offset+texture*2),secondary=d::Rd16(body.data()+h.textureCombos.offset+(texture+1)*2);
   if(primary==secondary || d::Rd32(body.data()+h.textures.offset+primary*16)!=(currentGlaive?0u:2u) || d::Rd32(body.data()+h.textures.offset+secondary*16)!=0)return false;
   ++named;
  }
  if(currentGlaive){
   if(batches!=2 || named!=1 || h.textureCombos.count<4)return false;
   constexpr uint16_t flags[]{0,16,16,16},blends[]{0,6,2,4};for(unsigned i=0;i<4;++i)if(d::Rd16(body.data()+h.materials.offset+i*4)!=flags[i] || d::Rd16(body.data()+h.materials.offset+i*4+2)!=blends[i])return false;
   const char* paths[]{"item\\objectcomponents\\weapon\\glave_1h_dualblade_d_02.blp","item\\objectcomponents\\weapon\\armorreflect4.blp"};
   for(unsigned i=0;i<3;++i){const auto* row=body.data()+h.textures.offset+i*16;if(d::Rd32(row)!=(i==2?2u:0u) || d::Rd32(row+4)!=(i==2?3u:0u))return false;if(i<2){const auto count=d::Rd32(row+8),at=d::Rd32(row+12);if(count!=std::strlen(paths[i])+1 || at>body.size() || count>body.size()-at || std::memcmp(body.data()+at,paths[i],count))return false;}}
   const auto* a=skinBytes.data()+batchAt;const auto* b=a+24;
   if(d::Rd16(a+2)!=0x8001 || d::Rd16(a+10)!=0 || d::Rd16(a+14)!=2 || d::Rd16(a+16)!=2 || d::Rd16(a+18)!=0 || d::Rd16(a+20)!=0 || d::Rd16(a+22)!=2 || d::Rd16(b+2)!=0 || d::Rd16(b+10)!=3 || d::Rd16(b+14)!=1 || d::Rd16(b+16)!=0 || d::Rd16(b+18)!=0 || d::Rd16(b+20)!=0 || d::Rd16(b+22)!=0 || d::Rd16(body.data()+h.textureCombos.offset)!=2 || d::Rd16(body.data()+h.textureCombos.offset+4)!=0 || d::Rd16(body.data()+h.textureCombos.offset+6)!=1)return false;
  }
  return (!namedStage && !currentGlaive) || named!=0;
 }catch(...){return false;}
}
}
bool ValidateClassicHandSource(std::span<const uint8_t> body,std::span<const uint8_t> skinBytes,uint32_t paletteLimit){
 return ValidateClassicHandSourceImpl(body,skinBytes,paletteLimit,0);
}
bool ValidateClassicNamedHandSource(std::span<const uint8_t> body,std::span<const uint8_t> skinBytes,uint32_t paletteLimit){
 return ValidateClassicHandSourceImpl(body,skinBytes,paletteLimit,1);
}
bool ValidateClassicCurrentGlaiveSource(std::span<const uint8_t> body,std::span<const uint8_t> skinBytes,uint32_t paletteLimit){return ValidateClassicHandSourceImpl(body,skinBytes,paletteLimit,2);}

bool ConvertSkin(std::span<const uint8_t> input,std::vector<uint8_t>& output,bool extendedIndices) {
    if(input.size()<48 || d::Rd32(input.data())!=Tag("SKIN")) return false;
    uint32_t n=d::Rd32(input.data()+36),off=d::Rd32(input.data()+40);
    if(n && (off<48 || !d::FitsInBody(uint32_t(input.size()),off,n,24))) return false;
    std::vector<uint8_t> result(input.begin(),input.end());
    const uint32_t sections=d::Rd32(input.data()+28),sectionOffset=d::Rd32(input.data()+32);
    if(sections && (sectionOffset<48 || !d::FitsInBody(uint32_t(input.size()),sectionOffset,sections,48)))return false;
    for(uint32_t i=0;i<sections;++i) {
        auto* section=result.data()+sectionOffset+i*48;
        if(!extendedIndices && d::Rd16(section+2))return false;
        if(d::Rd16(section+12)>75 || d::Rd16(section+16)>4 ||
           (d::Rd16(section+10) && !d::Rd16(section+12)))return false;
        // Native finalize divides its budget by every section's bone count,
        // including empty geometry. A parked section needs the neutral divisor.
        if(!d::Rd16(section+12))d::Wr16(section+12,1);
        if(d::Rd16(section+10) && !d::Rd16(section+16))d::Wr16(section+16,1);
    }
    std::vector<uint8_t> batches;bool split=false;
    if(uint64_t(n)*48+result.size()>Limit)return false;batches.reserve(n*48);
    for(uint32_t i=0;i<n;++i) {
        auto* b=result.data()+off+i*24;
        if(EnvSplit(d::Rd16(b+2),d::Rd16(b+14))){
            if(d::Rd16(b+10)==65535)return false;
            b[0]&=0x10;d::Wr16(b+2,2);d::Wr16(b+14,2);d::Wr16(b+18,0);
            batches.insert(batches.end(),b,b+24);std::array<uint8_t,24> follower{};std::memcpy(follower.data(),b,24);
            d::Wr16(follower.data()+10,d::Rd16(b+10)+1);d::Wr16(follower.data()+12,1);d::Wr16(follower.data()+14,1);
            batches.insert(batches.end(),follower.begin(),follower.end());split=true;continue;
        }
        const uint16_t shader=d::Rd16(b+2);
        const bool named=NamedEffect(shader,d::Rd16(b+14)),two=TwoStage(shader,d::Rd16(b+14));
        if(!SingleStage(shader,d::Rd16(b+14)) && !named && !two && !CollapsedNamed(shader,d::Rd16(b+14))) return false;
        b[0]&=0x10;d::Wr16(b+2,named?shader+1:(two?((shader&7)==1?0:((shader&7)==3?4:2)):0));
        d::Wr16(b+14,(named||two)?2:1);
        d::Wr16(b+18,named?0:(two?UVIndex(shader):((shader&0x80)?1:0)));
        batches.insert(batches.end(),b,b+24);
    }
    if(split){if(result.size()%2)result.push_back(0);const auto newOffset=uint32_t(result.size());result.insert(result.end(),batches.begin(),batches.end());d::Wr32(result.data()+36,uint32_t(batches.size()/24));d::Wr32(result.data()+40,newOffset);}
    output.swap(result);return true;
}
static bool ConvertImpl(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend,bool extendedIndices,uint32_t paletteLimit,PartitionedPair* partitioned,bool wxlFirstTextureApproximation=false,bool nativeSequenceHash=false,bool nativeRuntimeSequences=false,std::vector<std::vector<uint8_t>>* nativeSkins=nullptr,std::vector<std::vector<uint32_t>>* nativeOrigins=nullptr) {
    stats={};auto fail=[&](const char* why){stats.reason=why;return false;};
    if(paletteLimit<3 || paletteLimit>75)return fail("invalid native palette budget");
    if(!path || !resolve || !read || input.size()<8 || input.size()>Limit || d::Rd32(input.data())!=f::kMagicMD21) return false;
    std::span<const uint8_t> body,txid,afid;uint32_t skeletonId{};std::set<uint32_t> chunks;
    size_t off=0;
    while(off<input.size()) {
        if(input.size()-off<8) return fail("truncated chunk header");
        uint32_t tag=d::Rd32(input.data()+off),size=d::Rd32(input.data()+off+4);
        if(size>input.size()-off-8 || !chunks.insert(tag).second) return fail("invalid or repeated chunk");
        if(tag==Tag("SKID")){if(size!=4 || !(skeletonId=d::Rd32(input.data()+off+8)))return fail("invalid skeleton ID");}
        if(tag==Tag("AFID")){if(size%8)return fail("invalid animation mapping");afid=input.subspan(off+8,size);}
        if(tag==f::kMagicMD21) body=input.subspan(off+8,size);
        if(tag==Tag("TXID")) {if(size%4) return fail("invalid TXID");txid=input.subspan(off+8,size);}
        off+=8+size;
    }
    if(body.size()<sizeof(f::M2Header) || d::Rd32(body.data())!=f::kMagicMD20) return fail("missing MD20 body");
    auto* source=reinterpret_cast<const f::M2Header*>(body.data());stats.version=source->version;
    if(source->version<272 || source->version>274) return fail("unsupported source era");
    // Inline-skeleton models can use the same owned stream/alias assembly as
    // retail characters without topology partitioning or material approximation.
    // Validate the sequence span before inspecting flags in untrusted bytes.
    skeleton::Embedded embedded;std::vector<uint8_t> namedMappings;
    if(source->sequences.count && !d::FitsInBody(uint32_t(body.size()),source->sequences.offset,source->sequences.count,64))return fail("invalid sequence array");
    bool needsAssembly=false;
    for(uint32_t i=0;i<source->sequences.count;++i){const auto flags=d::Rd32(body.data()+source->sequences.offset+i*64+12);needsAssembly|=!(flags&0x20) || (flags&0x40);}
    // Explicit CPU policy never assembles external/aliased source animation.
    if(nativeSequenceHash && (needsAssembly || skeletonId || !nativesequence::ValidateProvided(body,source->sequences.count,source->sequences.offset,source->sequenceLookup.count,source->sequenceLookup.offset)))return fail("native sequence policy requires embedded source and valid supplied native hash");
    if(needsAssembly || skeletonId){
        AnimationContext context{path,read};auto dot=context.stem.find_last_of('.');if(dot==std::string::npos || context.stem.size()>900)return fail("invalid animation model path");context.stem.resize(dot);
        // Older named exports omit AFID. The exact ID/variation sibling remains
        // the storage authority; never substitute a different animation file.
        if(afid.empty())for(uint32_t i=0;i<source->sequences.count;++i){const auto* seq=body.data()+source->sequences.offset+i*64;if(d::Rd32(seq+12)&0x60)continue;const auto at=namedMappings.size();namedMappings.resize(at+8);d::Wr32(namedMappings.data()+at,d::Rd32(seq));d::Wr32(namedMappings.data()+at+4,0);}
        struct Guard {AnimationContext* old;Guard(AnimationContext* next):old(animationContext){animationContext=next;}~Guard(){animationContext=old;}} guard(&context);
        if(skeletonId) {
            // The supplied named exports use the exact model sibling, just as
            // the existing character adapter does. No guessed FDID paths or
            // different model skeletons are accepted.
            std::vector<uint8_t> skeletonBytes;
            if(!read((context.stem+".skel").c_str(),skeletonBytes))return fail("missing named skeleton");
            skeleton::Index index;
            if(!skeleton::Read(skeletonBytes,index,stats.reason))return false;
            std::vector<uint8_t> modelSkeleton;uint32_t skeletonFiles=1;
            if(index.parentId){
                const char* parentPath=skeleton::KnownParentPath(index.parentId);
                if(!parentPath)return fail("generic parent skeleton unresolved");
                if(index.parentId==skeletonId || SameNamedPath(context.stem+".skel",parentPath))return fail("generic parent skeleton cycle");
                std::vector<uint8_t> parent,merged;
                if(!read(parentPath,parent))return fail("missing named parent skeleton");
                // InheritBindPose re-reads both sources, requires a parent with
                // no parent of its own and identical nonzero bone counts, then
                // validates the child's bind pose on the parent's track set.
                if(!skeleton::InheritBindPose(skeletonBytes,parent,merged,stats.reason))return false;
                modelSkeleton=std::move(skeletonBytes);skeletonBytes=std::move(merged);
                context.animationStem=parentPath;context.animationStem.resize(context.animationStem.size()-5);
                skeletonFiles=2;
            }
            if(!skeleton::Embed(body,skeletonBytes,&ReadAnimation,embedded,stats.reason,modelSkeleton,&ReadModelAnimation))return false;
            stats.skeletonFiles=skeletonFiles;
        }else if(!skeleton::EmbedInline(body,afid.empty()?std::span<const uint8_t>(namedMappings):afid,&ReadAnimation,embedded,stats.reason))return false;
        stats.animationFiles=embedded.animationFiles;stats.aliases=embedded.aliases;body=embedded.body;source=reinterpret_cast<const f::M2Header*>(body.data());
    }
    if(!source->numSkinProfiles || source->numSkinProfiles>4 || !Validate(body,true,stats.reason)) {if(stats.reason.empty()) stats.reason="unsupported body records";return false;}
    // Require real named siblings before changing SFID-only metadata into the
    // native filename convention. No archive/provider redirection is implied.
    std::string stem=path;auto dot=stem.find_last_of('.');
    if(dot==std::string::npos || stem.size()>900) return fail("invalid model path");
    stem.resize(dot);std::vector<uint8_t> skin;skin::Owned rebuilt;
    std::vector<uint8_t> nativeBody;std::vector<std::vector<uint8_t>> nativeSourceSkins,nativeConvertedSkins;std::vector<std::vector<uint32_t>> nativeConvertedOrigins;
    if(nativeRuntimeSequences && nativeSkins && !partitioned){
        bool needsPartition=false;nativeSourceSkins.resize(source->numSkinProfiles);
        for(uint32_t i=0;i<source->numSkinProfiles;++i){
            char suffix[16]{};std::snprintf(suffix,sizeof(suffix),"%02u.skin",i);
            auto& bytes=nativeSourceSkins[i];
            if(!read((stem+suffix).c_str(),bytes)||bytes.size()>Limit)return fail("native topology missing/overbudget SKIN");
            Stats probe;
            if(!Skin(bytes,*source,body,probe,extendedIndices,paletteLimit)){
                if(probe.reason!="skin palette/influence budget"){stats.reason=probe.reason;return false;}
                needsPartition=true;
            }
        }
        if(needsPartition){
            if(!extendedIndices)return fail("native topology requires extended section starts");
            // Rebuild every LOD from the same assembled skeleton, then join
            // only the vertex/combo arrays. Each SKIN retains its complete
            // triangle stream, section IDs, order and duplicated batch rows.
            // Native uint16 lookup/combos bound the joined arrays explicitly.
            std::vector<uint8_t> vertices,combos;
            std::map<std::array<uint8_t,48>,uint16_t> vertexMap;
            std::vector<std::vector<uint8_t>> candidates;
            candidates.reserve(nativeSourceSkins.size());
            if(nativeOrigins)nativeConvertedOrigins.reserve(nativeSourceSkins.size());
            for(const auto& original:nativeSourceSkins){
                skin::Owned current;
                if(!skin::Rebuild(body,original,source->bones.count,true,current,stats.reason,paletteLimit))return false;
                if(current.plan.draws.empty()||current.plan.draws.size()>16384||!current.plan.sourceTriangles)return fail("native topology empty/overbudget geometry");
                const auto* header=reinterpret_cast<const f::M2Header*>(current.body.data());
                const auto comboBase=uint32_t(combos.size()/2);
                if(uint64_t(comboBase)+header->boneCombos.count>65535)return fail("native topology joined combo budget");
                std::vector<uint16_t> remap(header->vertices.count);
                for(uint32_t j=0;j<header->vertices.count;++j){
                    std::array<uint8_t,48> value;std::memcpy(value.data(),current.body.data()+header->vertices.offset+size_t(j)*48,48);
                    auto found=vertexMap.find(value);
                    if(found==vertexMap.end()){
                        if(vertices.size()/48>=65535)return fail("native topology joined vertex budget");
                        const auto index=uint16_t(vertices.size()/48);vertices.insert(vertices.end(),value.begin(),value.end());found=vertexMap.emplace(value,index).first;
                    }
                    remap[j]=found->second;
                }
                const auto lookupN=d::Rd32(current.skin.data()+4),lookupAt=d::Rd32(current.skin.data()+8);
                for(uint32_t j=0;j<lookupN;++j){auto* value=current.skin.data()+lookupAt+2*j;const auto originalIndex=d::Rd16(value);if(originalIndex>=remap.size())return fail("native topology rebuilt lookup");d::Wr16(value,remap[originalIndex]);}
                const auto sectionN=d::Rd32(current.skin.data()+28),sectionAt=d::Rd32(current.skin.data()+32);
                for(uint32_t j=0;j<sectionN;++j){auto* value=current.skin.data()+sectionAt+j*48+14;d::Wr16(value,uint16_t(comboBase+d::Rd16(value)));}
                combos.insert(combos.end(),current.body.begin()+header->boneCombos.offset,current.body.begin()+header->boneCombos.offset+size_t(header->boneCombos.count)*2);
                if(nativeOrigins){
                    const auto sourceN=d::Rd32(original.data()+36),sourceAt=d::Rd32(original.data()+40);
                    const auto rebuiltN=d::Rd32(current.skin.data()+36),rebuiltAt=d::Rd32(current.skin.data()+40);
                    if(current.batchOrigins.size()!=rebuiltN)return fail("native topology batch provenance count");
                    std::vector<uint32_t> base(sourceN);uint32_t convertedN=0;
                    for(uint32_t batch=0;batch<sourceN;++batch){const auto* row=original.data()+sourceAt+24*batch;base[batch]=convertedN;convertedN+=EnvSplit(d::Rd16(row+2),d::Rd16(row+14))?2:1;}
                    std::vector<uint32_t> origins;origins.reserve(size_t(rebuiltN)*2);
                    for(uint32_t batch=0;batch<rebuiltN;++batch){
                        const auto origin=current.batchOrigins[batch];if(origin>=sourceN)return fail("native topology batch provenance reference");
                        const auto* row=current.skin.data()+rebuiltAt+24*batch;origins.push_back(base[origin]);
                        if(EnvSplit(d::Rd16(row+2),d::Rd16(row+14)))origins.push_back(base[origin]+1);
                    }
                    nativeConvertedOrigins.push_back(std::move(origins));
                }
                candidates.push_back(std::move(current.skin));
            }
            if(body.size()+vertices.size()+combos.size()+6>Limit)return fail("native topology joined body budget");
            nativeBody.assign(body.begin(),body.end());
            while(nativeBody.size()%4)nativeBody.push_back(0);
            auto offset=uint32_t(nativeBody.size());nativeBody.insert(nativeBody.end(),vertices.begin(),vertices.end());
            d::Wr32(nativeBody.data()+0x3c,uint32_t(vertices.size()/48));d::Wr32(nativeBody.data()+0x40,offset);
            while(nativeBody.size()%4)nativeBody.push_back(0);
            offset=uint32_t(nativeBody.size());nativeBody.insert(nativeBody.end(),combos.begin(),combos.end());
            d::Wr32(nativeBody.data()+0x78,uint32_t(combos.size()/2));d::Wr32(nativeBody.data()+0x7c,offset);
            body=nativeBody;source=reinterpret_cast<const f::M2Header*>(body.data());
            if(!Validate(body,true,stats.reason))return false;
            nativeSourceSkins.swap(candidates);
            nativeConvertedSkins.resize(nativeSourceSkins.size());
        }
    }
    if(partitioned){
        if(source->numSkinProfiles!=1)return fail("partitioned pair requires exactly one named SKIN");
        // Preserve the existing generic material conversion, and expose its
        // fallback/masking counts explicitly. This is CPU data, not parity.
        for(uint32_t i=0;i<source->materials.count;++i){
            const auto* row=body.data()+source->materials.offset+i*4;
            partitioned->materialFlagMasks+=d::Rd16(row)>31;
            partitioned->materialBlendFallbacks+=d::Rd16(row+2)>6;
        }
        partitioned->skinPath=stem+"00.skin";
        if(!read(partitioned->skinPath.c_str(),skin) || skin.size()<48 || skin.size()>Limit)return fail("partitioned pair missing/overbudget named SKIN");
        partitioned->sourceSkin=skin;partitioned->sourceSkinFingerprint=Fingerprint(skin);
        if(wxlFirstTextureApproximation){std::vector<uint8_t> compatible;
            if(!wxlfirsttextureapproximation::Apply(body,skin,compatible,partitioned->wxlFirstTextureApproximationBatches,stats.reason))return false;
            skin.swap(compatible); // prepared clone only; sourceSkin remains exact
        }
        if(!skin::Rebuild(body,skin,source->bones.count,true,rebuilt,stats.reason,paletteLimit))return false;
        if(rebuilt.plan.draws.empty() || rebuilt.plan.draws.size()>16384 || !rebuilt.plan.sourceTriangles)return fail("partitioned pair empty/overbudget geometry");
        partitioned->sourceTriangles=rebuilt.plan.sourceTriangles;partitioned->sourceSections=rebuilt.plan.sourceSections;partitioned->splitSections=rebuilt.plan.splitSections;
        partitioned->vertexOrigins=rebuilt.vertexOrigins;
        body=rebuilt.body;source=reinterpret_cast<const f::M2Header*>(body.data());
        skin=rebuilt.skin;
        if(!Skin(skin,*source,body,stats,true,paletteLimit))return false;
        if(!ConvertSkin(skin,partitioned->skin,true))return fail("partitioned pair strict SKIN conversion");
        stats.skinFingerprints[0]=partitioned->sourceSkinFingerprint;stats.skins=1;
    }
    for(uint32_t i=0;!partitioned && i<source->numSkinProfiles;++i) {
        char suffix[16]{};std::snprintf(suffix,sizeof(suffix),"%02u.skin",i);
        if(!nativeSourceSkins.empty())skin=nativeSourceSkins[i];
        else if(!read((stem+suffix).c_str(),skin))return fail("missing named skin");
        if(!Skin(skin,*source,body,stats,extendedIndices,paletteLimit)) {if(stats.reason.empty())stats.reason="unsupported named skin";return false;}
        if(!nativeConvertedSkins.empty()&&!ConvertSkin(skin,nativeConvertedSkins[i],true))return fail("native topology SKIN conversion");
        if(!nativeConvertedOrigins.empty() && nativeConvertedOrigins[i].size()!=d::Rd32(nativeConvertedSkins[i].data()+36))return fail("native topology converted provenance count");
        stats.skinFingerprints[i]=Fingerprint(skin);++stats.skins;
    }
    if(stats.twoStageBatches && body.size()+8>Limit)return fail("expanded header exceeds byte limit");
    std::vector<uint8_t> result(body.begin(),body.end());
    if(stats.twoStageBatches)ExpandCombinerHeader(body,result);
    for(uint32_t head:{0x90u,0x98u}){
        if(!(head==0x90?stats.sharedWeightStage:stats.sharedTransformStage))continue;
        const auto n=d::Rd32(result.data()+head),at=d::Rd32(result.data()+head+4);
        if(!n || n>=65535 || result.size()+2*(n+1)>Limit)return fail("shared stage lookup limit");
        std::vector<uint8_t> lookup(result.begin()+at,result.begin()+at+n*2);
        lookup.push_back(lookup[(n-1)*2]);lookup.push_back(lookup[(n-1)*2+1]);
        if(result.size()%2)result.push_back(0);const auto offset=uint32_t(result.size());
        result.insert(result.end(),lookup.begin(),lookup.end());d::Wr32(result.data()+head,n+1);d::Wr32(result.data()+head+4,offset);
    }
    const uint32_t textureCount=d::Rd32(result.data()+0x50),textureOffset=d::Rd32(result.data()+0x54);
    for(uint32_t i=0;i<textureCount;++i) {
        const uint32_t at=textureOffset+i*16;
        const auto type=d::Rd32(result.data()+at);
        const auto fdid=txid.size()/4>i?d::Rd32(txid.data()+i*4):0;
        const auto replacement=appearance::PrivateCreatureTextureSlot(path,fdid,type);
        if(replacement!=type){d::Wr32(result.data()+at,replacement);d::Wr32(result.data()+at+8,0);d::Wr32(result.data()+at+12,0);++stats.privateTextureSlots;continue;}
        if(type!=0) continue;
        uint32_t n=d::Rd32(result.data()+at+8),o=d::Rd32(result.data()+at+12);
        if(n) {if(result[o+n-1]!=0) return fail("unterminated texture path");continue;}
        if(txid.size()/4<=i) return fail("missing texture ID");
        const char* name=resolve(d::Rd32(txid.data()+i*4));
        if(!name || !*name) return fail("unresolved texture ID");
        size_t length=strnlen_s(name,1024);if(!length || length>=1024 || result.size()+length+1>Limit) return fail("invalid resolved texture path");
        uint32_t offset=uint32_t(result.size());result.insert(result.end(),name,name+length+1);
        d::Wr32(result.data()+at+8,uint32_t(length+1));d::Wr32(result.data()+at+12,offset);++stats.textures;
    }
    // Admitted packed IDs collapse to a single stage under the existing WXL
    // policy. Lookup 0 is primary UV; lookup 1 is native environment UV (-1).
    if(result.size()+21>Limit)return fail("lookup exceeds byte limit");
    if(result.size()&1)result.push_back(0);
    const uint32_t unitOffset=uint32_t(result.size());
    // Singles plus every two-stage UV pair used by the admitted packed policy.
    result.insert(result.end(),{0,0,0xff,0xff,0xff,0xff,0xff,0xff,0,0,1,0});
    uint32_t combinerOffset=0;
    if(stats.twoStageBatches) {
        combinerOffset=uint32_t(result.size());result.insert(result.end(),{1,0,1,0,1,0,4,0});if(stats.stage13Batches)result.insert(result.end(),{1,0,3,0});
    }
    // Modern spell exports often omit the debug name. Give the owned adapted
    // buffer an exact path identity so an asynchronous actor can be matched to
    // the admitted bytes without borrowing another model or accepting aliases.
    if(!d::Rd32(result.data()+8)) {
        const size_t length=strnlen_s(path,256);
        if(!length || length>=256 || result.size()+length+1>Limit)return fail("invalid model identity");
        const uint32_t offset=uint32_t(result.size());
        result.insert(result.end(),path,path+length+1);
        d::Wr32(result.data()+8,uint32_t(length+1));d::Wr32(result.data()+12,offset);
    }
    auto* h=reinterpret_cast<f::M2Header*>(result.data());
    h->textureUnitLookup={6,unitOffset};
    h->globalFlags&=~f::kFlagUseTextureCombinerCombos;
    if(stats.twoStageBatches) {h->textureCombinerCombos={stats.stage13Batches?6u:4u,combinerOffset};h->globalFlags|=f::kFlagUseTextureCombinerCombos;}
    sequencecompat::HashChanges sequenceChanges;
    if(nativeSequenceHash){
        if(!nativesequence::NormalizeProvided(result,h->sequences.count,h->sequences.offset,h->sequenceLookup.count,h->sequenceLookup.offset,sequenceChanges))return fail("native sequence policy hash normalization refused");
    }else if(!sequencecompat::NormalizeNativeHash(result,h->sequences.count,h->sequences.offset,h->sequenceLookup.count,h->sequenceLookup.offset,sequencecompat::HashPolicy::CuratedEightOnly,sequenceChanges))return fail("source sequence hash bounds, primaries or termination");
    for(uint32_t i=0;i<h->sequences.count;++i) {
        auto* s=reinterpret_cast<f::M2Sequence*>(result.data()+h->sequences.offset+i*64);if(!nativeSequenceHash)s->blendTime&=0xffff;
    }
    for(uint32_t i=0;i<h->materials.count;++i) {
        auto* m=result.data()+h->materials.offset+4*i;uint16_t flags=d::Rd16(m),blend=d::Rd16(m+2);
        if(blend>6) {blend=4;flags|=5;}d::Wr16(m,flags&31);d::Wr16(m+2,blend);
    }
    // Mode 7 is retained only after the guarded native ScreenAdd hook is ready.
    // The fallback omits those emitters if native installation was refused.
    uint32_t retained=0;
    for(uint32_t i=0;i<h->particleEmitters.count;++i) {
        auto* sourceRecord=result.data()+h->particleEmitters.offset+i*0x1ec;
        if(sourceRecord[0x28]>6 && !screenBlend) {++stats.omittedEmitters;continue;}
        std::memmove(result.data()+h->particleEmitters.offset+retained*0x1ec,sourceRecord,0x1ec);++retained;
    }
    h->particleEmitters.count=retained;
    d::NormalizeReport report{};
    if(!d::NormalizeRecords(result.data(),uint32_t(result.size()),h,report)) return fail("record normalization refused");
    stats.particles=h->particleEmitters.count;stats.cameras=h->cameras.count;
    h->globalFlags&=~0x60u;h->version=f::kClientVersion;
    if(nativeSequenceHash && !nativesequence::ValidateProvided(result,h->sequences.count,h->sequences.offset,h->sequenceLookup.count,h->sequenceLookup.offset))return fail("normalized native sequence policy refused");
    if(!Validate(result,false,stats.reason,screenBlend,nativeSequenceHash||nativeRuntimeSequences)) return false;
    if(partitioned){
        partitioned->omittedEmitters=stats.omittedEmitters;
        const auto* finalHeader=reinterpret_cast<const f::M2Header*>(result.data());
        partitioned->vertices=finalHeader->vertices.count;partitioned->skeletonBones=finalHeader->bones.count;
        if(!partitioned->vertices || partitioned->vertices>65535 || !partitioned->skeletonBones || partitioned->skeletonBones>4096 ||
           partitioned->vertexOrigins.size()!=partitioned->vertices)return fail("partitioned pair final vertex/skeleton budget");
        const auto comboN=finalHeader->boneCombos.count,comboAt=finalHeader->boneCombos.offset;
        if(!comboN || comboN>65535 || !d::FitsInBody(uint32_t(result.size()),comboAt,comboN,2))return fail("partitioned pair combo bounds");
        partitioned->bones.resize(comboN);std::memcpy(partitioned->bones.data(),result.data()+comboAt,comboN*2);
        const auto sectionN=d::Rd32(partitioned->skin.data()+28),sectionAt=d::Rd32(partitioned->skin.data()+32);
        if(!sectionN || sectionN>16384 || !d::FitsInBody(uint32_t(partitioned->skin.size()),sectionAt,sectionN,48))return fail("partitioned pair section bounds");
        uint64_t triangles=0;std::vector<uint8_t> used(comboN);
        for(uint32_t i=0;i<sectionN;++i){const auto* section=partitioned->skin.data()+sectionAt+i*48;
            const auto start=d::Rd16(section+14),size=d::Rd16(section+12),count=d::Rd16(section+10);
            if(!size || size>paletteLimit || start>comboN || size>comboN-start || count%3)return fail("partitioned pair palette/triangle bounds");
            partitioned->starts.push_back(start);partitioned->sizes.push_back(size);triangles+=count/3;
            for(uint32_t j=0;j<size;++j){if(used[start+j] || partitioned->bones[start+j]>=partitioned->skeletonBones)return fail("partitioned pair palette overlap/reference");used[start+j]=1;}
        }
        if(triangles!=partitioned->sourceTriangles || std::find(used.begin(),used.end(),uint8_t(0))!=used.end())return fail("partitioned pair incomplete geometry/palette");
        char marker[96]{};const auto pathBytes=std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(path),std::strlen(path));
        std::snprintf(marker,sizeof(marker),"WXL64_OWNED_%016llx_%016llx",static_cast<unsigned long long>(Fingerprint(result)^Fingerprint(partitioned->skin)),static_cast<unsigned long long>(Fingerprint(pathBytes)));
        partitioned->marker=marker;const auto length=partitioned->marker.size()+1;
        if(result.size()+length>Limit)return fail("partitioned pair marker limit");
        const auto at=uint32_t(result.size());result.insert(result.end(),partitioned->marker.begin(),partitioned->marker.end());result.push_back(0);
        d::Wr32(result.data()+8,uint32_t(length));d::Wr32(result.data()+12,at);
        if(!Validate(result,false,stats.reason,screenBlend,nativeSequenceHash||nativeRuntimeSequences))return false;
    }
    if(nativeSkins)nativeSkins->swap(nativeConvertedSkins);
    if(nativeOrigins)nativeOrigins->swap(nativeConvertedOrigins);
    output.swap(result);return true;
}
bool Convert(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend,bool extendedIndices,uint32_t paletteLimit){
 return ConvertImpl(path,input,resolve,read,output,stats,screenBlend,extendedIndices,paletteLimit,nullptr);
}
// Native source owns the complete typed sequence consumer. Keep unresolved
// modern action IDs after owned external/alias assembly and the bounded native
// hash rebuild; all geometry/material/track validations still apply. The old
// bridge/generic Convert entry remains strict.
bool ConvertForNativeRuntime(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend,bool extendedIndices,uint32_t paletteLimit,std::vector<std::vector<uint8_t>>* rebuiltSkins,std::vector<std::vector<uint32_t>>* rebuiltBatchOrigins){
 return ConvertImpl(path,input,resolve,read,output,stats,screenBlend,extendedIndices,paletteLimit,nullptr,false,false,true,rebuiltSkins,rebuiltBatchOrigins);
}
bool ConvertWithNativeSequenceHash(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend,bool extendedIndices,uint32_t paletteLimit){
 return ConvertImpl(path,input,resolve,read,output,stats,screenBlend,extendedIndices,paletteLimit,nullptr,false,true);
}
static bool PreparePartitionedPolicy(bool compatibility,const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::shared_ptr<const PartitionedPair>& output,Stats& stats,bool screenBlend,uint32_t paletteLimit){
 // All callbacks and construction finish before publishing any pointer. Failed
 // inputs and exceptions preserve the caller's previously published pair.
 stats={};if(!path || strnlen_s(path,901)>=901 || input.size()<8 || input.size()>Limit){stats.reason="partitioned pair path";return false;}
 const std::string sourcePath=path;PartitionedPair next;std::vector<uint8_t> model;
 next.sourceModel.assign(input.begin(),input.end());next.sourceModelFingerprint=Fingerprint(next.sourceModel);
 if(!ConvertImpl(sourcePath.c_str(),next.sourceModel,resolve,read,model,stats,screenBlend,true,paletteLimit,&next,compatibility))return false;
 next.model=std::move(model);
 auto immutable=std::make_shared<const PartitionedPair>(std::move(next));output=std::move(immutable);return true;
}
bool PreparePartitioned(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::shared_ptr<const PartitionedPair>& output,Stats& stats,bool screenBlend,uint32_t paletteLimit){return PreparePartitionedPolicy(false,path,input,resolve,read,output,stats,screenBlend,paletteLimit);}
bool PreparePartitionedWxlFirstTextureApproximation(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::shared_ptr<const PartitionedPair>& output,Stats& stats,bool screenBlend,uint32_t paletteLimit){return PreparePartitionedPolicy(true,path,input,resolve,read,output,stats,screenBlend,paletteLimit);}
}
