// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtRootStream.hpp"
namespace wxl::waystone::adtcompat {
// Inputs are already transformed companion streams. This derives header counts,
// not reference resolution or active sound/liquid ownership.
inline bool CompanionInputs(const std::vector<uint8_t>& texture,const std::vector<uint8_t>& objects,std::array<RootInputs,256>& output){
 std::array<RootInputs,256> prepared{};
 const std::vector<uint8_t>* streams[]{&texture,&objects};
 for(unsigned stream=0;stream<2;++stream){const auto& b=*streams[stream];if(b.empty() || b.size()>16*1024*1024)return false;size_t at=0;unsigned chunks=0;
  while(at<b.size()){if(b.size()-at<8)return false;auto tag=Word(b.data()+at),n=Word(b.data()+at+4);if(n>b.size()-at-8)return false;
   if(tag==0x4d434e4b){if(chunks>=256)return false;size_t pos=at+8,end=pos+n;std::vector<uint32_t> seen;auto& v=prepared[chunks++];
    while(pos<end){if(end-pos<8)return false;auto sub=Word(b.data()+pos),size=Word(b.data()+pos+4);if(size>end-pos-8 || std::find(seen.begin(),seen.end(),sub)!=seen.end())return false;seen.push_back(sub);
     if(!stream){
      if(sub==0x4d434c59){if(size%16 || size/16>4)return false;v.layers=size/16;}
      if(sub==0x4d43414c){if(size>65536)return false;v.alphaBytes=size;}
      if(sub==0x4d435348){if(size!=512)return false;v.shadowWhole=true;}
     }else{
      if(sub==0x4d435244){if(size%4 || size/4>65535)return false;v.doodads=size/4;}
      if(sub==0x4d435257){if(size%4 || size/4>65535)return false;v.wmos=size/4;}
     }pos+=8+size;
    }
   }at+=8+n;
  }if(chunks!=256)return false;
 }output=prepared;return true;
}
}
