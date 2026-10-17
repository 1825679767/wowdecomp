// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
#include <cstddef>
#include "WXL/NativeFeatures.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "WXL/VideoFrameRateUi.hpp"
namespace wxl::native::ui_compat {
inline unsigned VideoPanelsKind(std::string_view path){
 try{std::string key(path);for(char& c:key){if(c=='/')c='\\';if(c>='A'&&c<='Z')c+=32;}
  return key=="interface\\framexml\\videooptionspanels.xml"?1u:key=="interface\\framexml\\videooptionspanels.lua"?2u:0u;
 }catch(...){return false;}
}
inline bool VideoPanelsPath(std::string_view path){return VideoPanelsKind(path)!=0;}
// Early Apply repair remains fingerprint-bound. The independent optional FPS
// control is appended only to the canonical panel Lua, preserving its source.
inline bool GuardVideoApply(std::string_view path,const uint8_t* bytes,size_t size,std::vector<uint8_t>& output){
 if(!NativeEnabled() || !bytes || !VideoPanelsPath(path) || size>65536)return false;
 uint64_t hash=14695981039346656037ull;for(size_t i=0;i<size;++i){hash^=bytes[i];hash*=1099511628211ull;}
 const bool xml=VideoPanelsKind(path)==1 && size==38933 && hash==0xae448ae8123744c8ull;
 const bool lua=VideoPanelsKind(path)==2 && size==28971 && hash==0x2313e9ba430f7a6full;
 if(!xml && !lua){
  // A safe independent extension of the canonical panel script, including
  // mounted UI revisions other than the historical build4 guard fingerprint.
  if(VideoPanelsKind(path)!=2)return false;
  try{std::vector<uint8_t> next(bytes,bytes+size);const std::string_view ui=FrameRateUi;
   next.push_back('\n');next.insert(next.end(),ui.begin(),ui.end());output=std::move(next);return true;
  }catch(...){return false;}
 }
 try{std::string text(reinterpret_cast<const char*>(bytes),size);constexpr std::string_view needle="VideoOptionsFrameApply:Enable();",replacement="if VideoOptionsFrameApply then VideoOptionsFrameApply:Enable(); end";
  size_t count=0,at=0;while((at=text.find(needle,at))!=std::string::npos){text.replace(at,needle.size(),replacement);at+=replacement.size();++count;}
  if(count!=(xml?6u:4u))return false;if(lua){text+='\n';text+=FrameRateUi;}
  std::vector<uint8_t> next(text.begin(),text.end());output=std::move(next);return true;
 }catch(...){return false;}
}
}
