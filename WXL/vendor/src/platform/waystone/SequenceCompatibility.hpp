// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <span>
#include <cstdint>
#include <cstring>
#include <vector>
#include <algorithm>
namespace wxl::waystone::sequencecompat {
inline constexpr uint16_t Missing=65535;
inline uint16_t Target(uint16_t id){switch(id){case 564:return 37;case 548:return 41;case 556:return 42;case 552:return 43;case 554:return 44;case 562:return 45;case 572:return 39;case 574:return 187;default:return id;}}
inline uint16_t Read16(const uint8_t* p) noexcept {uint16_t v;std::memcpy(&v,p,2);return v;}
// Exact build-4 HasSequence/FindSequenceIndex/SDK55 probe. The serialized
// lookup is a hash table of row indices, never a table of classic action roles.
// Native consumers do not bound collisions; owned producers additionally prove
// termination. This reader bounds them and checks an index BEFORE dereferencing.
inline uint16_t FindNativeHash(const uint8_t* rows,uint32_t count,const uint8_t* lookup,uint32_t slots,uint32_t id,uint32_t* remainingWork=nullptr) noexcept {
 if(!rows || !count || count>32767 || slots>65536 || id>65535 || (slots && !lookup))return Missing;
 if(!slots){for(uint32_t i=0;i<count;++i){if(remainingWork){if(!*remainingWork)return Missing;--*remainingWork;}if(Read16(rows+size_t(i)*64)==id)return uint16_t(i);}return Missing;}
 uint32_t slot=id%slots;
 for(uint32_t probe=1,visited=0;visited<slots;++visited,++probe){
  if(remainingWork){if(!*remainingWork)return Missing;--*remainingWork;}
  const auto index=Read16(lookup+size_t(slot)*2);
  if(index==Missing || index>=count)return Missing;
  if(Read16(rows+size_t(index)*64)==id)return index;
  // uint32 arithmetic deliberately matches native imul/add before unsigned div.
  slot=uint32_t(slot+uint32_t(probe*probe))%slots;
 }
 return Missing;
}
enum class HashPolicy {CuratedEightOnly,RetailCharacterClassicApproximation};
struct HashChanges {uint32_t curated{},classicApproximation{};};
// Transactional in-place normalization. Bounds, chosen source primaries, the
// whole replacement table and all absent-key probe paths are checked first.
// Other than the eight donor mappings, generic IDs/timing/track/variation bytes
// stay exact. Retail's separately named old >505->0 approximation is retained.
inline bool NormalizeNativeHash(std::span<uint8_t> body,uint32_t count,uint32_t offset,uint32_t slots,uint32_t lookupOffset,HashPolicy policy,HashChanges& changes){
 if(count>32767 || slots>65536 || offset>body.size() || uint64_t(count)*64>body.size()-offset || lookupOffset>body.size() || uint64_t(slots)*2>body.size()-lookupOffset)return false;
 if(count && slots && uint64_t(offset)<uint64_t(lookupOffset)+uint64_t(slots)*2 && uint64_t(lookupOffset)<uint64_t(offset)+uint64_t(count)*64)return false;
 if(policy!=HashPolicy::CuratedEightOnly && policy!=HashPolicy::RetailCharacterClassicApproximation)return false;
 try {
  const auto* rows=body.data()+offset;const auto* old=body.data()+lookupOffset;
  for(uint32_t i=0;i<slots;++i){const auto index=Read16(old+size_t(i)*2);if(index!=Missing && index>=count)return false;}
  std::vector<uint16_t> ids(count),sourcePrimary(65536,Missing),chosen(65536,Missing);
  std::vector<uint8_t> priority(65536,0);
  HashChanges pending;uint32_t remainingWork=16u*1024u*1024u;
  for(uint32_t i=0;i<count;++i){
   const auto source=Read16(rows+size_t(i)*64);auto target=Target(source);
   if(sourcePrimary[source]==Missing){sourcePrimary[source]=FindNativeHash(rows,count,old,slots,source,&remainingWork);if(sourcePrimary[source]==Missing)return false;}
   if(target!=source)++pending.curated;
   if(policy==HashPolicy::RetailCharacterClassicApproximation && target>505){target=0;++pending.classicApproximation;}
   ids[i]=target;
  }
  // Preserve the exact source-selected variation. A curated newer action wins
  // over an existing classic action, even if that older row appears first.
  for(uint32_t source=0;source<65536;++source)if(sourcePrimary[source]!=Missing){
   const auto index=sourcePrimary[source];const auto target=ids[index];
   const uint8_t rank=Target(uint16_t(source))!=source?3:(target==source?2:1);
   if(rank>priority[target] || (rank==priority[target] && index<chosen[target])){chosen[target]=index;priority[target]=rank;}
  }
  std::vector<uint16_t> table(slots,Missing);
  if(!slots){
   for(uint32_t i=0;i<count;++i)if(chosen[ids[i]]!=Missing){if(chosen[ids[i]]!=i)return false;chosen[ids[i]]=Missing;}
  }else{
   for(uint32_t id=0;id<65536;++id)if(chosen[id]!=Missing){
    uint32_t slot=id%slots;bool inserted=false;
    for(uint32_t probe=1,visited=0;visited<slots;++visited,++probe){
     if(!remainingWork)return false;--remainingWork;
     if(table[slot]==Missing){table[slot]=chosen[id];inserted=true;break;}
     slot=uint32_t(slot+uint32_t(probe*probe))%slots;
    }
    if(!inserted)return false;
   }
   // An absent query must reach FFFF too. Prove every possible initial bucket
   // with the exact native probe and a bounded CPU budget; no prime assumption.
   for(uint32_t initial=0;initial<slots;++initial){uint32_t slot=initial;bool empty=false;
    for(uint32_t probe=1,visited=0;visited<slots;++visited,++probe){
     if(!remainingWork)return false;--remainingWork;
     if(table[slot]==Missing){empty=true;break;}
     slot=uint32_t(slot+uint32_t(probe*probe))%slots;
    }
    if(!empty)return false;
   }
  }
  // No allocation, callback or failure remains after the first mutation.
  for(uint32_t i=0;i<count;++i)std::memcpy(body.data()+offset+size_t(i)*64,&ids[i],2);
  if(slots)std::memcpy(body.data()+lookupOffset,table.data(),size_t(slots)*2);
  changes=pending;return true;
 }catch(...){return false;}
}
}
