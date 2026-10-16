// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "BlpCompat.hpp"
#include "NativeFeatures.hpp"
#include "engine/assets/shared/textures/blp/BlpTranscode.hpp"
#include <algorithm>
#include <cstring>
#include <span>
namespace wxl::native {
constexpr uint32_t Limit=64u*1024*1024;
namespace {
uint32_t Read32(const uint8_t* p){uint32_t value;std::memcpy(&value,p,4);return value;}
bool PowerOfTwo(uint32_t value){return value && !(value&(value-1));}
// Inspect only byte-owned input. No native header or palette is dereferenced
// until its complete extent has been checked against the async buffer size.
bool Inspect(std::span<const uint8_t> bytes,uint8_t encoding,ComponentBlpInfo& info){
 if(bytes.size()<148||bytes.size()>Limit||Read32(bytes.data())!=0x32504c42||Read32(bytes.data()+4)!=1||bytes[8]!=encoding)return false;
 const auto width=Read32(bytes.data()+12),height=Read32(bytes.data()+16);const auto alpha=bytes[9];
 // Native Paste selects mip levels by repeated halving; arbitrary dimensions
 // can never reach its native section widths. Header dimensions also fit uint16.
 if(!PowerOfTwo(width)||!PowerOfTwo(height)||width>4096||height>4096||bytes[11]>1)return false;
 const uint32_t minimum=encoding==1?1172u:148u;
 if(bytes.size()<minimum)return false;
 if(encoding==1 && alpha!=0&&alpha!=1&&alpha!=4&&alpha!=8)return false;
 if(encoding==2 && (bytes[10]!=0&&bytes[10]!=1&&bytes[10]!=7))return false;
 if(encoding==2 && alpha!=0&&alpha!=1&&alpha!=4&&alpha!=8)return false;
 if(encoding==3 && alpha!=0&&alpha!=1&&alpha!=4&&alpha!=8)return false;
 const uint64_t basePixels=uint64_t(width)*height;
 const uint32_t bpp=encoding==3?uint32_t(Read32(bytes.data()+84)/basePixels):0;
 if(encoding==3 && bpp!=3&&bpp!=4)return false;
 uint32_t expected=1,w=width,h=height;
 while(w>1||h>1){w=(std::max)(1u,w>>1);h=(std::max)(1u,h>>1);++expected;}
 uint32_t count=0;bool ended=false;
 for(uint32_t mip=0;mip<16;++mip){
  const auto offset=Read32(bytes.data()+20+4*mip),size=Read32(bytes.data()+84+4*mip);
  if(!offset&&!size){ended=true;continue;}
  if(!offset||!size||ended||mip>=expected||offset<minimum||offset>bytes.size()||size>bytes.size()-offset)return false;
  const uint32_t mw=(std::max)(1u,width>>mip),mh=(std::max)(1u,height>>mip);const uint64_t pixels=uint64_t(mw)*mh;
  const uint64_t needed=encoding==1?pixels+(pixels*alpha+7)/8:
   encoding==2?uint64_t((mw+3)/4)*((mh+3)/4)*(bytes[10]==0?8u:16u):pixels*bpp;
  if(size<needed)return false;
  for(uint32_t prior=0;prior<mip;++prior){const auto previous=Read32(bytes.data()+20+4*prior),length=Read32(bytes.data()+84+4*prior);if(uint64_t(offset)<uint64_t(previous)+length&&uint64_t(previous)<uint64_t(offset)+size)return false;}
  ++count;
 }
 if(!count)return false;
 info={width,height,count,alpha};return true;
}
bool HasBoundedComponentMip(const ComponentBlpInfo& info){
 uint32_t mip=0,w=info.width,h=info.height;
 while((std::max)(w,h)>256){w=(std::max)(1u,w>>1);h=(std::max)(1u,h>>1);++mip;}
 // The donor decoder otherwise falls back to the large base mip. Refuse that
 // fallback so component quantization stays at the requested256px budget.
 return mip<info.mipCount;
}
}
bool PrepareComponentBlp(const void* raw,uint32_t size,std::vector<uint8_t>& replacement,ComponentBlpInfo& output) noexcept {
 replacement.clear();output={};if(!raw||size<148||size>Limit)return false;
 try{
  namespace blp=wxl::modern::assets::textures::blp;
  const std::span<const uint8_t> input(static_cast<const uint8_t*>(raw),size);ComponentBlpInfo info;
  if(input[8]==1){if(!Inspect(input,1,info))return false;output=info;return true;}
  if(!BlpEnabled()||(input[8]!=2&&input[8]!=3)||!Inspect(input,input[8],info)||!HasBoundedComponentMip(info))return false;
  std::vector<uint8_t> transcoded;std::span<const uint8_t> decoded=input;
  if(input[8]==3){
   if(!blp::TranscodeBlp(input,transcoded)||transcoded.size()>Limit||!Inspect(transcoded,2,info)||!HasBoundedComponentMip(info))return false;
   decoded=transcoded;
  }
  std::vector<uint8_t> candidate;
  if(!blp::TextureComponentToPaletted(decoded,candidate,256)||candidate.size()>Limit||!Inspect(candidate,1,info)||info.width>256||info.height>256)return false;
  replacement.swap(candidate);output=info;return true;
 }catch(...){replacement.clear();output={};return false;}
}
bool ConvertBlp(const void* raw,uint32_t size,std::vector<uint8_t>& output){
 if(!BlpEnabled() || !raw || size>Limit)return false;
 try{return wxl::modern::assets::textures::blp::TranscodeBlp({static_cast<const uint8_t*>(raw),size},output);}catch(...){return false;}
}
}
