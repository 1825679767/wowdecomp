// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <map>
#include <stdexcept>
namespace wxl::native::world::bytes {
using Blob = std::vector<uint8_t>;
inline uint32_t Read(const uint8_t* p) { uint32_t v; std::memcpy(&v,p,4); return v; }
inline void Write(uint8_t* p,uint32_t v) { std::memcpy(p,&v,4); }
inline uint32_t Tag(const char* p) { return uint32_t(uint8_t(p[3]))|(uint32_t(uint8_t(p[2]))<<8)|(uint32_t(uint8_t(p[1]))<<16)|(uint32_t(uint8_t(p[0]))<<24); }
struct Part { uint32_t tag; size_t at,size; };
inline bool Walk(const Blob& b,size_t begin,size_t end,std::vector<Part>& parts) {
 if(begin>end || end>b.size())return false;
 while(begin<end){if(end-begin<8 || parts.size()>=65536)return false;auto n=Read(b.data()+begin+4);if(n>end-begin-8)return false;parts.push_back({Read(b.data()+begin),begin+8,n});begin+=8+n;}return true;
}
inline size_t Append(Blob& out,uint32_t tag,const uint8_t* p,size_t size) {
 constexpr size_t max=64u*1024u*1024u;
 if(out.size()>max-8 || size>max-out.size()-8)throw std::length_error("native world buffer exceeds limit");
 const auto at=out.size();out.resize(at+8+size);Write(out.data()+at,tag);Write(out.data()+at+4,uint32_t(size));if(size)std::memcpy(out.data()+at+8,p,size);return at;
}
inline size_t Append(Blob& out,const char* tag,const Blob& b){return Append(out,Tag(tag),b.data(),b.size());}
inline Blob Copy(const Blob& b,const Part& p){return {b.begin()+p.at,b.begin()+p.at+p.size};}
}
