// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtChunkCompatibility.hpp"
#include <array>
namespace wxl::waystone::adtcompat {
// Normalize all split-root MCNK headers atomically. Inputs follow MCNK ordinal;
// source top-level ordering, non-header payloads and byte offsets stay intact.
inline bool PrepareRootStream(const std::vector<uint8_t>& raw,const std::array<RootInputs,256>& inputs,std::vector<uint8_t>& output){
 if(&raw==&output || raw.empty() || raw.size()>16*1024*1024)return false;
 auto prepared=raw;size_t at=0;unsigned chunks=0;bool header=false;
 while(at<raw.size()){
  if(raw.size()-at<8)return false;auto tag=Word(raw.data()+at),n=Word(raw.data()+at+4);if(n>raw.size()-at-8)return false;
  if(tag==0x4d43494e)return false; // Offset tables belong to native indexing.
  if(tag==0x4d484452){if(header || n<64)return false;header=true;}
  if(tag==0x4d414d50 && (!header || (n!=1 && n!=4)))return false;
  if(tag==0x4d434e4b){
   if(chunks>=256 || n<128)return false;
   std::vector<uint8_t> chunk(raw.begin()+at,raw.begin()+at+8+n),normalized;bool high=false;
   if(!PrepareRootHeader(chunk,inputs[chunks],normalized,high) || normalized.size()!=chunk.size())return false;
   std::copy(normalized.begin(),normalized.end(),prepared.begin()+at);++chunks;
  }at+=8+n;
 }
 if(!header || chunks!=256)return false;output.swap(prepared);return true;
}
}
