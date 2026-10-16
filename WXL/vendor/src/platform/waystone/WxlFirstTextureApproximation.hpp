// Copyright (C)2026 WarcraftXL. GPLv3.
#pragma once
#include <span>
#include <vector>
#include <string>
#include <cstring>
#include <array>
namespace wxl::waystone::wxlfirsttextureapproximation {
inline uint16_t U16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
inline uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
inline void W16(uint8_t* p,uint16_t v){std::memcpy(p,&v,2);}
// Explicit CPU compatibility policy for two verified source row shapes. This
// does NOT admit the shader to SupportsSourceMaterial or the native renderer.
// Source bytes and output/counter remain unchanged on refusal.
inline bool Apply(std::span<const uint8_t> body,std::span<const uint8_t> source,
 std::vector<uint8_t>& output,uint32_t& converted,std::string& reason){
 auto fail=[&](const char* why){reason=why;return false;};
 if(body.size()<312 || source.size()<48 || std::memcmp(body.data(),"MD20",4) || std::memcmp(source.data(),"SKIN",4))return fail("WxlFirstTextureApproximation header");
 auto fits=[](size_t bytes,uint32_t first,uint32_t count,uint32_t stride,uint32_t minimum){return first>=minimum && first<=bytes && uint64_t(count)*stride<=bytes-first;};
 const auto materials=U32(body.data()+0x70),materialAt=U32(body.data()+0x74),textures=U32(body.data()+0x50),textureAt=U32(body.data()+0x54),combos=U32(body.data()+0x80),comboAt=U32(body.data()+0x84);
 const auto batches=U32(source.data()+36),batchAt=U32(source.data()+40),sections=U32(source.data()+28),sectionAt=U32(source.data()+32);
 if(!materials || !textures || !combos || !batches || batches>65535 || !sections || !fits(body.size(),materialAt,materials,4,312) || !fits(body.size(),textureAt,textures,16,312) || !fits(body.size(),comboAt,combos,2,312) ||
  !fits(source.size(),batchAt,batches,24,48) || !fits(source.size(),sectionAt,sections,48,48))return fail("WxlFirstTextureApproximation arrays");
 std::vector<uint8_t> prepared(source.begin(),source.end());uint32_t accepted=0;
 for(uint32_t i=0;i<batches;++i){const auto* p=source.data()+batchAt+size_t(i)*24;const auto shader=U16(p+2),count=U16(p+14);
  if(shader!=0x801c){if(count>3)return fail("WxlFirstTextureApproximation unknown multistage shader");continue;}
  const auto section=U16(p+4),combo=U16(p+16),transform=U16(p+22);
  const bool nightborne=section==73 && combo==3 && transform==0;
  const bool voidelf=section==72 && combo==5 && transform==2;
  if(i || accepted || count!=6 || p[0]!=0x90 || p[1] || !((nightborne)||(voidelf)) || section>=sections || U16(p+6) || U16(p+8)!=0xffff || U16(p+10) || U16(p+12) || U16(p+18)!=0xffff || U16(p+20) ||
   U16(body.data()+materialAt) || U16(body.data()+materialAt+2) || uint32_t(combo)+6>combos)return fail("WxlFirstTextureApproximation exact shader28 selector");
  const std::array<uint16_t,6> expected{uint16_t(nightborne?2:1),5,uint16_t(nightborne?2:1),3,4,6};
  constexpr std::array<uint32_t,6> types{2,18,2,15,16,17};
  for(unsigned stage=0;stage<6;++stage){const auto index=U16(body.data()+comboAt+size_t(combo+stage)*2);
   if(index!=expected[stage] || index>=textures || U32(body.data()+textureAt+size_t(index)*16)!=types[stage] || U32(body.data()+textureAt+size_t(index)*16+4))return fail("WxlFirstTextureApproximation texture selector");}
  auto* row=prepared.data()+batchAt+size_t(i)*24;
  // Current x32 DownConvertBatch default(low28): packed0x0010/count1;
  // subsequent existing ConvertSkin decodes shader0,count1,primary UV0.
  row[0]&=0x10;W16(row+2,0x0010);W16(row+14,1);++accepted;
 }
 if(accepted!=1)return fail("WxlFirstTextureApproximation missing exact shader28 row");
 output.swap(prepared);converted=accepted;reason.clear();return true;
}
}
