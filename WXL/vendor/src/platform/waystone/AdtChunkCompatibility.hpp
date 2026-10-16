// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
namespace wxl::waystone::adtcompat {
struct TextureStats {uint32_t sourceLayers{},nativeLayers{},droppedLayers{},disabledAlpha{};};
inline uint32_t Word(const uint8_t* p){uint32_t n{};std::memcpy(&n,p,4);return n;}
inline void Put(uint8_t* p,uint32_t n){std::memcpy(p,&n,4);}
inline bool PackedFits(const uint8_t* b,size_t size,size_t at){
 for(unsigned row=0;row<64;++row){unsigned produced=0;while(produced<64){if(at>=size)return false;const auto control=b[at++];const unsigned count=control&127;if(!count)return false;if(control&128){if(at>=size)return false;++at;}else{if(count>size-at)return false;at+=count;}produced+=count;}}
 return true;
}
// Mirror the Win32 four-layer compatibility ceiling and alpha-span guards.
// Work only on a private output; never change caller bytes on failure or success.
inline bool PrepareTexture(const std::vector<uint8_t>& raw,bool wideAlpha,std::vector<uint8_t>& output,TextureStats& stats){
 if(&raw==&output || raw.empty() || raw.size()>0x100000)return false;struct Part{uint32_t tag,n;size_t at;};std::vector<Part> parts;size_t at=0;const uint8_t* alpha=nullptr;uint32_t alphaSize=0,layers=0;bool hasLayers=false;
 while(at<raw.size()){if(raw.size()-at<8)return false;const auto tag=Word(raw.data()+at),n=Word(raw.data()+at+4);if(n>raw.size()-at-8)return false;for(const auto& part:parts)if(part.tag==tag)return false;
  parts.push_back({tag,n,at});if(tag==0x4d434c59){if(!n || n%16 || n/16>65535)return false;layers=n/16;hasLayers=true;}if(tag==0x4d43414c){alpha=raw.data()+at+8;alphaSize=n;}at+=8+n;}
 if(!hasLayers)return false;TextureStats next{layers,std::min(layers,4u),layers>4?layers-4:0,0};std::vector<uint8_t> prepared;
 for(const auto& part:parts){if(part.tag==0x4d43414c && !part.n)continue;const auto n=part.tag==0x4d434c59?next.nativeLayers*16:part.n;const auto dst=prepared.size();prepared.insert(prepared.end(),raw.begin()+part.at,raw.begin()+part.at+8+n);Put(prepared.data()+dst+4,n);
  if(part.tag==0x4d434c59)for(unsigned i=0;i<next.nativeLayers;++i){auto* row=prepared.data()+dst+8+i*16;const auto flags=Word(row+4),start=Word(row+8);if(!(flags&0x100))continue;bool fits=alpha && start<alphaSize;if(fits)fits=wideAlpha && (flags&0x200)?PackedFits(alpha,alphaSize,start):(wideAlpha?4096u:2048u)<=alphaSize-start;if(!fits){Put(row+4,flags&~0x100u);++next.disabledAlpha;}}}
 output.swap(prepared);stats=next;return true;
}
struct RootInputs {uint32_t layers{},doodads{},wmos{},alphaBytes{},soundEmitters{};bool shadowWhole{};};
inline uint16_t CollapseHoles(uint64_t high){uint16_t low=0;for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){const auto shift=r*16+c*2;const uint64_t mask=(uint64_t(3)<<shift)|(uint64_t(3)<<(shift+8));if(high&mask)low|=uint16_t(1u<<(r*4+c));}return low;}
inline bool PrepareRootHeader(const std::vector<uint8_t>& raw,const RootInputs& inputs,std::vector<uint8_t>& output,bool& highHoles){
 if(&raw==&output || raw.size()<136 || raw.size()>0x100000 || Word(raw.data())!=0x4d434e4b || Word(raw.data()+4)!=raw.size()-8 || inputs.layers>4 || inputs.doodads>65535 || inputs.wmos>65535 || inputs.soundEmitters>65535 || inputs.alphaBytes>65536)return false;
 // Active root alpha/liquid payload rewriting is separately gated.
 // Retail split roots may retain inactive legacy offsets; size<=8 cannot
 // enter the native legacy liquid walk, so normalize those dead fields.
 if(Word(raw.data()+8+0x24) || (Word(raw.data()+8+0x60) && Word(raw.data()+8+0x64)>8))return false;
 auto prepared=raw;auto* h=prepared.data()+8;auto flags=Word(h);const bool high=(flags&0x10000)!=0;
 if(high){uint64_t mask{};std::memcpy(&mask,h+0x14,8);const auto low=CollapseHoles(mask);std::memcpy(h+0x3c,&low,2);std::memset(h+0x14,0,8);flags&=~0x10000u;}
 flags=inputs.shadowWhole?flags|1u:flags&~1u;Put(h,flags);Put(h+0x0c,inputs.layers);Put(h+0x10,inputs.doodads);Put(h+0x28,inputs.alphaBytes+8);Put(h+0x38,inputs.wmos);Put(h+0x5c,inputs.soundEmitters);Put(h+0x60,0);Put(h+0x64,8);output.swap(prepared);highHoles=high;return true;
}

}
