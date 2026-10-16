// Staged only: explicit-input, transactional CPU producer. No production route enabled.
#pragma once
#include "platform/waystone/AdtTextureNames.hpp"
#include "platform/waystone/AdtObjectNames.hpp"
#include "platform/waystone/AdtCompanionInputs.hpp"
#include "platform/waystone/AdtSourceAlphaPolicy.hpp"
#include <array>
#include <string>
namespace wxl::waystone::adtcompat {
struct PreparedByteFamily {
 std::array<std::string,3> paths;
 std::array<std::vector<uint8_t>,3> bytes;
};
// Caller supplies canonical names from a verified loader contract and native alpha policy.
// This helper owns no native objects and does not publish partially converted members.
inline bool PrepareByteFamily(const std::array<std::string,3>& paths,
 const std::array<std::vector<uint8_t>,3>& source,const SourceAlphaPolicy& policy,
 ResolveTexture textureResolver,ResolveModel modelResolver,PreparedByteFamily& output){
 if(!textureResolver || !modelResolver || paths[0].size()<5 || paths[0].size()>240 || !paths[0].ends_with(".adt"))return false;
 const auto stem=paths[0].substr(0,paths[0].size()-4);
 if(paths[1]!=stem+"_tex0.adt" || paths[2]!=stem+"_obj0.adt")return false;
 for(const auto& path:paths)if(!path.starts_with("world\\maps\\") || path.find("..")!=std::string::npos || path.find(':')!=std::string::npos)return false;
 for(const auto& bytes:source)if(bytes.empty() || bytes.size()>16u*1024u*1024u)return false;
 const bool wideAlpha=policy.wideAlpha;
 if(wideAlpha!=bool(policy.mphdFlags&4u) || (policy.mphdFlags&1u) || ((policy.mphdFlags&0x80u) && !wideAlpha))return false;
 // Width comes from MPHD4 -> native renderer flag100 -> native format2.
 // The independent source flag8000 is required for the native wide reader,
 // not used to derive width. Narrow inputs preserve this flag unchanged.
 unsigned chunks=0;size_t cursor=0;
 while(cursor<source[0].size()){
  const auto& root=source[0];if(root.size()-cursor<8)return false;
  const auto tag=Word(root.data()+cursor),size=Word(root.data()+cursor+4);
  if(size>root.size()-cursor-8)return false;
  if(tag==0x4d434e4b){if(size<128 || ++chunks>256 || (wideAlpha && !(Word(root.data()+cursor+8)&0x8000u)))return false;}
  cursor+=8+size;
 }
 if(chunks!=256)return false;
 PreparedByteFamily pending;pending.paths=paths;TextureNameStats textureStats;ObjectNameStats objectStats;
 std::array<RootInputs,256> inputs;
 if(!PrepareTextureNames(source[1],wideAlpha,textureResolver,pending.bytes[1],textureStats) ||
    !PrepareObjectNames(source[2],modelResolver,pending.bytes[2],objectStats) ||
    !CompanionInputs(pending.bytes[1],pending.bytes[2],inputs) ||
    !PrepareRootStream(source[0],inputs,pending.bytes[0]))return false;
 // Native source stage refuses unresolved/dropped placements rather than
 // publishing an incomplete world or inventing fallback asset names.
 if(objectStats.doodads.unresolved || objectStats.wmos.suppressed)return false;
 output=std::move(pending);return true;
}
}
