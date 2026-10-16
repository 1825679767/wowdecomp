#include "ParticleCompat.hpp"
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeFeatures.hpp"
#include "Engine/Source/Model2/M2Data.h"
#include "engine/assets/shared/models/m2/M2RecordWidths.hpp"
#include <cstring>
#include <cmath>
namespace wxl::native {
namespace {
constexpr uint32_t Limit=64u*1024*1024;
template<class T>T Read(const uint8_t* p,size_t at){T v;std::memcpy(&v,p+at,sizeof(v));return v;}
template<class T>void Write(std::vector<uint8_t>& bytes,size_t at,const T& value){std::memcpy(bytes.data()+at,&value,sizeof(value));}
// A modern extension supplies a constant source point, whereas the native M2
// stores that same parameter as an ordinary float animation track. Own a new
// constant track in the converted M2 allocation; relocation, model creation,
// emitter cloning and simulation then use the native path without a registry.
bool BakeZSource(std::vector<uint8_t>& bytes,size_t particle,float value,std::string& reason){
 auto fail=[&](const char* text){reason=text;return false;};
 if(!std::isfinite(value) || value<0)return fail("particle extension zSource requires a finite nonnegative value");
 if(value==0)return true; // Preserve the existing source track for neutral extensions.
 // Native SetZsource snaps smaller magnitudes to zero. Do not claim support for
 // a source point that would silently turn back into the angular emitter path.
 if(value<0.001f)return fail("particle extension zSource is below native directional epsilon");
 const auto track=particle+offsetof(M2Particle,m_zsourceTrack);
 if(Read<uint16_t>(bytes.data(),track+offsetof(M2TrackBase,m_loopIndex))!=uint16_t(-1))return fail("particle extension zSource conflicts with native global track");
 auto fits=[&](uint32_t at,uint32_t count,size_t stride){return at<=bytes.size() && count<=(bytes.size()-at)/stride;};
 // Native M2Init relocates a non-global slot against the model only when its
 // matching sequence is inline. Never leave an appended key as an external
 // .anim offset, or permit a later sequence load to relocate it a second time.
 const auto sequences=Read<uint32_t>(bytes.data(),offsetof(M2Data,m_sequences));
 const auto sequencesAt=Read<uint32_t>(bytes.data(),offsetof(M2Data,m_sequences)+4);
 if(!sequences || !fits(sequencesAt,sequences,sizeof(M2Sequence)) ||
    !(Read<uint32_t>(bytes.data(),sequencesAt+offsetof(M2Sequence,m_flags))&0x20u))return fail("particle extension zSource needs an inline native sequence zero");
 // Never replace source animation, including external sequence slots. Only an
 // empty or neutral constant track can receive the extension's constant value.
 for(const auto field:{offsetof(M2TrackBase,m_sequenceTimes),offsetof(M2Track<float>,m_sequenceKeys)}){
  const auto count=Read<uint32_t>(bytes.data(),track+field),at=Read<uint32_t>(bytes.data(),track+field+4);
  if(count>1 || !fits(at,count,sizeof(M2Array<uint32_t>)))return fail("particle extension zSource conflicts with native sequence track");
  if(!count)continue;
  const auto keys=Read<uint32_t>(bytes.data(),at),keysAt=Read<uint32_t>(bytes.data(),at+4);
  if(keys>1 || !fits(keysAt,keys,4))return fail("particle extension zSource native constant bounds");
  if(keys && Read<uint32_t>(bytes.data(),keysAt)!=0)return fail("particle extension zSource conflicts with nonneutral native constant");
 }
 const auto aligned=(bytes.size()+3)&~size_t(3);
 // Two native array descriptors plus one uint32 timestamp and float key.
 constexpr size_t ownedBytes=2*sizeof(M2Array<uint32_t>)+sizeof(uint32_t)+sizeof(float);
 if(aligned>Limit || ownedBytes>Limit-aligned)return fail("particle extension zSource ownership byte budget");
 bytes.resize(aligned+ownedBytes,0);
 const uint32_t times=uint32_t(aligned),keys=times+uint32_t(sizeof(M2Array<uint32_t>));
 const uint32_t timeValue=keys+uint32_t(sizeof(M2Array<float>)),keyValue=timeValue+uint32_t(sizeof(uint32_t));
 Write(bytes,times,M2Array<uint32_t>{1,timeValue});
 Write(bytes,keys,M2Array<float>{1,keyValue});
 Write(bytes,keyValue,value);
 Write(bytes,track+offsetof(M2TrackBase,m_trackType),uint16_t(0));
 Write(bytes,track+offsetof(M2TrackBase,m_loopIndex),uint16_t(-1));
 Write(bytes,track+offsetof(M2TrackBase,m_sequenceTimes),M2Array<M2Array<uint32_t>>{1,times});
 Write(bytes,track+offsetof(M2Track<float>,m_sequenceKeys),M2Array<M2Array<float>>{1,keys});
 return true;
}
}
// EXPT and EXP2 share constant RGB/alpha multiplier semantics.
// EXPT has emitter-indexed float triples; EXP2 additionally carries cutoff tracks.
// Bake it into private native key arrays before relocation. Native simulation,
// lifetime interpolation, batching and render ownership remain unchanged.
bool ApplyParticleExtensions(std::span<const uint8_t> raw,std::vector<uint8_t>& native,std::string& reason,ParticleCutoffSet* cutoffs,const ParticleLayerSet* layers){
 auto fail=[&](const char* text){reason=text;return false;};
 if(cutoffs)cutoffs->clear();
 if(!ParticlesEnabled())return true;
 if(raw.size()<8 || native.size()<sizeof(M2Data))return fail("particle extension header bounds");
 const auto bodySize=Read<uint32_t>(raw.data(),4);
 if(bodySize>raw.size()-8 || bodySize<304)return fail("particle extension body bounds");
 constexpr auto particleField=offsetof(M2Data,m_particles);
 const auto count=Read<uint32_t>(raw.data()+8,particleField);
 if(!count)return true;
 if(count>65535 || count!=Read<uint32_t>(native.data(),particleField))return fail("particle extension emitter identity");
 const auto materials=Read<uint32_t>(raw.data()+8,offsetof(M2Data,m_materials));
 std::span<const uint8_t> txac,exp2,expt;bool seenTxac=false,seenExp2=false,seenExpt=false;
 for(size_t at=0;at<raw.size();){
  if(raw.size()-at<8)return fail("particle extension chunk header bounds");
  const auto length=Read<uint32_t>(raw.data(),at+4);
  if(length>raw.size()-at-8)return fail("particle extension chunk bounds");
  const auto payload=raw.subspan(at+8,length);
  if(!std::memcmp(raw.data()+at,"TXAC",4)){
   if(seenTxac)return fail("duplicate TXAC");seenTxac=true;txac=payload;
  }else if(!std::memcmp(raw.data()+at,"EXP2",4)){
   if(seenExp2)return fail("duplicate EXP2");seenExp2=true;exp2=payload;
  }else if(!std::memcmp(raw.data()+at,"EXPT",4)){
   if(seenExpt)return fail("duplicate EXPT");seenExpt=true;expt=payload;
  }
  at+=8+length;
 }
 // WhiteoutLib a88224a9 parser.cpp150..168 maps EXPT
 // zSource/colorMult/alphaMult triples to emitter i. EXPT has no cutoff track.
 // Whiteout specification13.4: EXP2 is authoritative when both exist;
 // EXPT reconstructs the extension only when EXP2 is absent.
 const bool useExpt=seenExpt && !seenExp2;
 if(!seenExpt && !seenExp2)return true;
 if(seenTxac){
  if((uint64_t(materials)+count)*2!=txac.size())return fail("particle extension TXAC row count");
  if(useExpt){
   for(const auto value:txac)if(value)return fail("EXPT requires neutral TXAC");
  }else{
   for(uint32_t i=0;i<count;++i){const auto at=2*(uint64_t(materials)+i);
    if(txac[at] || txac[at+1])return fail("EXP2 requires neutral particle TXAC");
   }
  }
 }
 auto fits=[](size_t limit,uint32_t at,uint32_t n,size_t stride){return at<=limit && n<=(limit-at)/stride;};
 const auto extension=useExpt?expt:exp2;
 uint32_t offset=0;const uint32_t recordStride=useExpt?12:28;
 if(useExpt){
  if(expt.size()!=uint64_t(count)*recordStride)return fail("EXPT emitter row count");
 }else{
  if(exp2.size()<8)return fail("EXP2 header bounds");
  const auto records=Read<uint32_t>(exp2.data(),0);offset=Read<uint32_t>(exp2.data(),4);
  if(records!=count || offset<8 || !fits(exp2.size(),offset,count,recordStride))return fail("EXP2 record bounds/count");
 }
 const auto particlesAt=Read<uint32_t>(native.data(),particleField+4);
 if(!fits(native.size(),particlesAt,count,sizeof(M2Particle)))return fail("native particle bounds");
 const auto sourceParticlesAt=Read<uint32_t>(raw.data()+8,particleField+4);
 constexpr auto sourceStride=wxl::structure::m2records::kParticleStrideModern;
 if(!fits(bodySize,sourceParticlesAt,count,sourceStride))return fail("source cutoff emitter bounds");
 ParticleCutoffSet captured(count);size_t cutoffBytes=0;
 for(uint32_t i=0;i<count;++i){
  const auto rec=offset+i*recordStride;
  const float extra=Read<float>(extension.data(),rec),rgb=Read<float>(extension.data(),rec+4),alpha=Read<float>(extension.data(),rec+8);
  if(!BakeZSource(native,particlesAt+i*sizeof(M2Particle),extra,reason))return false;
  if(!std::isfinite(rgb) || !std::isfinite(alpha) || rgb<0 || rgb>16 || alpha<0 || alpha>16)return fail("particle extension multiplier range");
  if(seenExp2){
   const auto times=Read<uint32_t>(exp2.data(),rec+12),timesAt=Read<uint32_t>(exp2.data(),rec+16);
   const auto keys=Read<uint32_t>(exp2.data(),rec+20),keysAt=Read<uint32_t>(exp2.data(),rec+24);
   if(times!=keys || keys>4096 || !fits(exp2.size(),timesAt,times,2) || !fits(exp2.size(),keysAt,keys,2) || (keys && (!timesAt || !keysAt)))return fail("EXP2 cutoff bounds");
   uint16_t previous=0;bool nonzero=false;
   for(uint32_t k=0;k<keys;++k){const auto t=Read<uint16_t>(exp2.data(),timesAt+k*2);
    if(k && t<previous)return fail("EXP2 cutoff timestamps");previous=t;
    nonzero|=Read<uint16_t>(exp2.data(),keysAt+k*2)!=0;
   }
   if(nonzero){
    if(!cutoffs)return fail("EXP2 cutoff owner missing");
    const auto particle=particlesAt+i*sizeof(M2Particle);
    // Layered cutoffs require the independently captured/admitted layer recipe;
    // packed metadata may never disappear through first-texture normalization.
    auto emptyName=[&](size_t field){
     const auto n=Read<uint32_t>(native.data(),particle+field),at=Read<uint32_t>(native.data(),particle+field+4);
     return !n || (n==1 && at<native.size() && native[at]==0);
    };
    const auto sourceParticle=sourceParticlesAt+size_t(i)*sourceStride;
    const auto sourceFlags=Read<uint32_t>(raw.data()+8,sourceParticle+offsetof(M2Particle,m_flags));
    const bool layered=(sourceFlags&wxl::structure::m2records::kParticleFlagMultiTex) ||
       (Read<uint32_t>(native.data(),particle+offsetof(M2Particle,m_flags))&wxl::structure::m2records::kParticleFlagMultiTex);
    const bool admitted=layers && layers->size()==count && (*layers)[i] && (*layers)[i]->sourceFlags==sourceFlags &&
       (*layers)[i]->sourceRecord.size()==sourceStride && !std::memcmp((*layers)[i]->sourceRecord.data(),raw.data()+8+sourceParticle,sourceStride);
    if((layered && !admitted) ||
       !emptyName(offsetof(M2Particle,m_geometryMdl)) || !emptyName(offsetof(M2Particle,m_recursionMdl)))return fail("EXP2 cutoff requires sprite or admitted layer contract");
    if(size_t(keys)*4>Limit-cutoffBytes)return fail("EXP2 cutoff ownership byte budget");cutoffBytes+=size_t(keys)*4;
    auto track=std::make_shared<ParticleCutoffTrack>();track->times.reserve(keys);track->values.reserve(keys);
    for(uint32_t k=0;k<keys;++k){const auto time=Read<uint16_t>(exp2.data(),timesAt+k*2);
     if(k && time<=track->times.back())return fail("EXP2 nonzero cutoff needs strictly increasing timestamps");
     track->times.push_back(time);track->values.push_back(Read<uint16_t>(exp2.data(),keysAt+k*2));
    }
    captured[i]=std::move(track);
   }
  }
  // Every emitter owns its appended copies even if exporters alias key arrays.
  const auto particle=particlesAt+i*sizeof(M2Particle);
  for(unsigned component=0;component<2;++component){
   const float multiplier=component?alpha:rgb;if(multiplier==1)continue;
   const auto track=particle+(component?offsetof(M2Particle,m_alphaTrack):offsetof(M2Particle,m_colorTrack));
   const auto values=track+(component?offsetof(M2PartTrack<fixed16>,m_values):offsetof(M2PartTrack<C3Vector>,m_values));
   const auto n=Read<uint32_t>(native.data(),values),source=Read<uint32_t>(native.data(),values+4);
   const size_t stride=component?sizeof(fixed16):sizeof(C3Vector);
   if(n>65535 || !fits(native.size(),source,n,stride))return fail("native particle key bounds");
   if(!n)continue;
   const auto aligned=(native.size()+3)&~size_t(3);
   const auto bytes=size_t(n)*stride;
   if(aligned>Limit || bytes>Limit-aligned)return fail("particle key byte budget");
   std::vector<uint8_t> copy(native.begin()+source,native.begin()+source+bytes);
   if(component){
    for(uint32_t k=0;k<n;++k){const auto original=Read<int16_t>(copy.data(),k*2);const float value=original*multiplier;
     if(original<0 || value>32767)return fail("particle extension alpha exceeds native range");
     const int16_t result=static_cast<int16_t>(std::lround(value));std::memcpy(copy.data()+k*2,&result,2);
    }
   }else{
    for(size_t k=0;k<bytes;k+=4){const auto original=Read<float>(copy.data(),k);const float value=original*multiplier;
     if(!std::isfinite(original) || original<0 || value>255)return fail("particle extension RGB exceeds native range");
     std::memcpy(copy.data()+k,&value,4);
    }
   }
   native.resize(aligned,0);native.insert(native.end(),copy.begin(),copy.end());
   const auto target=static_cast<uint32_t>(aligned);std::memcpy(native.data()+values+4,&target,4);
  }
 }
 if(cutoffs)cutoffs->swap(captured);
 return true;
}

}
