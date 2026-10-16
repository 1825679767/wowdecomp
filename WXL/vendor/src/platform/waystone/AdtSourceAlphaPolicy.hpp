// Staged source-only parser; matches native MPHD copy/BatchChunks policy.
#pragma once
#include "platform/waystone/AdtChunkCompatibility.hpp"
namespace wxl::waystone::adtcompat {
struct SourceAlphaPolicy {uint32_t mphdFlags{};bool wideAlpha{};};
inline bool ReadSourceAlphaPolicy(const std::vector<uint8_t>& wdt,SourceAlphaPolicy& output){
 if(wdt.empty() || wdt.size()>16u*1024u*1024u)return false;
 bool found=false;uint32_t flags=0;size_t cursor=0;
 while(cursor<wdt.size()){
  if(wdt.size()-cursor<8)return false;
  const auto tag=Word(wdt.data()+cursor),size=Word(wdt.data()+cursor+4);
  if(size>wdt.size()-cursor-8)return false;
  if(tag==0x4d504844){if(found || size!=32)return false;found=true;flags=Word(wdt.data()+cursor+8);}
  cursor+=8+size;
 }
 // WMO-only maps and height-only maps without native wide layout are outside this subset.
 if(!found || (flags&1u) || ((flags&0x80u) && !(flags&4u)))return false;
 output={flags,(flags&4u)!=0};return true;
}
}
