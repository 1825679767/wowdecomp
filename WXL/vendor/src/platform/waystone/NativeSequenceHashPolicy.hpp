// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/SequenceCompatibility.hpp"
namespace wxl::waystone::nativesequence {
// Pure CPU source validation, not a native admission/capability decision. Preserve
// source-selected variation and supplied table; do not manufacture a new lookup
// to conceal malformed original indices or nonterminating absent queries.
inline bool ValidateProvided(std::span<const uint8_t> body,uint32_t count,uint32_t offset,uint32_t slots,uint32_t lookupOffset) noexcept {
 using namespace sequencecompat;
 if(!count || !slots || count>32767 || slots>65536 || offset>body.size() || uint64_t(count)*64>body.size()-offset || lookupOffset>body.size() || uint64_t(slots)*2>body.size()-lookupOffset)return false;
 if(count && slots && uint64_t(offset)<uint64_t(lookupOffset)+uint64_t(slots)*2 && uint64_t(lookupOffset)<uint64_t(offset)+uint64_t(count)*64)return false;
 const auto* rows=body.data()+offset;const auto* lookup=body.data()+lookupOffset;
 uint32_t high508=0;
 for(uint32_t i=0;i<count;++i){const auto* row=rows+size_t(i)*64;uint32_t flags{};std::memcpy(&flags,row+12,4);
  // Only the two actual native fixtures' flags; inactive alias word+62 is
  // preserved, never followed when 0x40 is clear. No variations/chains admitted.
  if((flags!=0x20 && flags!=0xa1) || Read16(row+2)!=0 || Read16(row+60)!=Missing)return false;
  const auto id=Read16(row);if(id>505 && sequencecompat::Target(id)==id){if(id!=508 || ++high508!=1)return false;uint32_t duration{};std::memcpy(&duration,row+4,4);if(duration!=1000)return false;}
 }
 for(uint32_t i=0;i<slots;++i){const auto row=Read16(lookup+size_t(i)*2);if(row!=Missing && row>=count)return false;}
 uint32_t work=16u*1024u*1024u;
 for(uint32_t i=0;i<count;++i)if(FindNativeHash(rows,count,lookup,slots,Read16(rows+size_t(i)*64),&work)==Missing)return false;
 // This first branch requires a real supplied hash; every absent initial
 // bucket must reach FFFF with the exact native probe.
 for(uint32_t initial=0;initial<slots;++initial){uint32_t slot=initial;bool empty=false;
  for(uint32_t probe=1,visited=0;visited<slots;++visited,++probe){if(!work)return false;--work;if(Read16(lookup+size_t(slot)*2)==Missing){empty=true;break;}slot=uint32_t(slot+uint32_t(probe*probe))%slots;}
  if(!empty)return false;
 }
 return high508==1;
}
inline bool NormalizeProvided(std::span<uint8_t> body,uint32_t count,uint32_t offset,uint32_t slots,uint32_t lookupOffset,sequencecompat::HashChanges& changes){
 if(!ValidateProvided(body,count,offset,slots,lookupOffset))return false;
 bool curated=false;for(uint32_t i=0;i<count;++i){const auto id=sequencecompat::Read16(body.data()+offset+size_t(i)*64);curated|=sequencecompat::Target(id)!=id;}
 if(!curated){changes={};return true;}
 // Only the existing eight curated role changes may rebuild this source table.
 return sequencecompat::NormalizeNativeHash(body,count,offset,slots,lookupOffset,sequencecompat::HashPolicy::CuratedEightOnly,changes);
}
}
