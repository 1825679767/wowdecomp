// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/AdtChunkCompatibility.hpp"
#include <string>
namespace wxl::waystone::adtcompat {
struct TextureNameStats {uint32_t diffuseCount{},chunks{},droppedLayers{},disabledAlpha{};std::vector<uint32_t> heightFdids;};
using ResolveTexture=const char*(*)(uint32_t);
inline bool SafeTextureName(const char* name,std::string& result){
 if(!name)return false;size_t n=0;while(n<1024 && name[n])++n;if(!n || n>=1024)return false;std::string path(name,n);for(auto& c:path)if(c=='/')c=char(92);
 if(path.find("..")!=std::string::npos || path.find(':')!=std::string::npos || path[0]==char(92) || !path.ends_with(".blp") || (!path.starts_with(std::string("tileset")+char(92)) && !path.starts_with(std::string("world")+char(92))))return false;result=std::move(path);return true;
}
inline bool PrepareTextureNames(const std::vector<uint8_t>& raw,bool wideAlpha,ResolveTexture resolve,std::vector<uint8_t>& output,TextureNameStats& stats){
 if(&raw==&output || !resolve || raw.size()<8 || raw.size()>16*1024*1024)return false;struct Part{uint32_t tag,n;size_t at;};std::vector<Part> parts;size_t at=0;const uint8_t* ids=nullptr;uint32_t count=0;TextureNameStats next;
 while(at<raw.size()){if(raw.size()-at<8)return false;const auto tag=Word(raw.data()+at),n=Word(raw.data()+at+4);if(n>raw.size()-at-8)return false;parts.push_back({tag,n,at});if(tag==0x4d544558)return false;if(tag==0x4d444944){if(ids || !n || n%4 || n/4>4096)return false;ids=raw.data()+at+8;count=n/4;}if(tag==0x4d484944){if(!next.heightFdids.empty() || n%4 || n/4>4096)return false;for(size_t i=0;i<n;i+=4)next.heightFdids.push_back(Word(raw.data()+at+8+i));}at+=8+n;}
 if(!ids)return false;std::vector<uint8_t> names;for(unsigned i=0;i<count;++i){const auto id=Word(ids+i*4);std::string path;if(!id || !SafeTextureName(resolve(id),path))return false;names.insert(names.end(),path.begin(),path.end());names.push_back(0);}next.diffuseCount=count;std::vector<uint8_t> prepared;
 for(const auto& part:parts){const auto dst=prepared.size();if(part.tag==0x4d444944){prepared.resize(dst+8+names.size());Put(prepared.data()+dst,0x4d544558);Put(prepared.data()+dst+4,uint32_t(names.size()));std::memcpy(prepared.data()+dst+8,names.data(),names.size());}
  else if(part.tag==0x4d434e4b){std::vector<uint8_t> chunk(raw.begin()+part.at+8,raw.begin()+part.at+8+part.n),fixed;TextureStats t;if(!PrepareTexture(chunk,wideAlpha,fixed,t))return false;size_t k=0;while(k<fixed.size()){const auto tag=Word(fixed.data()+k),n=Word(fixed.data()+k+4);if(tag==0x4d434c59)for(size_t i=0;i<n;i+=16)if(Word(fixed.data()+k+8+i)>=count)return false;k+=8+n;}prepared.resize(dst+8+fixed.size());Put(prepared.data()+dst,part.tag);Put(prepared.data()+dst+4,uint32_t(fixed.size()));std::memcpy(prepared.data()+dst+8,fixed.data(),fixed.size());++next.chunks;next.droppedLayers+=t.droppedLayers;next.disabledAlpha+=t.disabledAlpha;}
  else prepared.insert(prepared.end(),raw.begin()+part.at,raw.begin()+part.at+8+part.n);}
 if(!next.chunks)return false;output.swap(prepared);stats=std::move(next);return true;
}
}
