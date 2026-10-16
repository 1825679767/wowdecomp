// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/WmoRootBytes.hpp"
#include "platform/waystone/WmoChunkIndex.hpp"
#include <cstring>
#include <map>
namespace wxl::waystone::wmo {
namespace {
uint32_t Read(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
void Write(uint8_t* p,uint32_t v){std::memcpy(p,&v,4);}
bool ValidPath(const std::string& path){
    if(path.empty() || path.size()>900 || path.front()=='\\' || path.front()=='/')return false;
    if(path.find(':')!=std::string::npos || path.find("..")!=std::string::npos)return false;
    for(unsigned char c:path)if(c<32)return false;
    return true;
}
}
bool PrepareNamedRoot(std::span<const uint8_t> source,TextureResolver resolve,
                      bool effects,RootBytes& output,std::string& reason){
    reason.clear();auto fail=[&](const char* why){reason=why;return false;};
    RootIndex index;if(!IndexRoot(source,index) || !index.modern)return fail("modern root required");
    if(index.groupFileIds.present || index.doodadFileIds.present)return fail("FDID child provider required");
    // Unknown root chunks can change native semantics: no silent loss at this gate.
    if(index.unknownChunks)return fail("unsupported root chunk");
    const auto header=index.slots[0];
    constexpr unsigned counts[]{0,4,2,7,11,8,14,12};
    constexpr unsigned offsets[]{0,4,0,8,12,0,20,24};
    constexpr unsigned strides[]{0,32,64,20,48,8,40,32};
    for(unsigned i=1;i<8;++i){
        if(i==5)continue; // portal references have no MOHD count.
        if(Read(source.data()+header.offset+offsets[i])!=index.slots[counts[i]].size/strides[i])return fail("root header/table count mismatch");
    }
    RootBytes result;
    if(!PlanMaterials(source,resolve,effects,result.materials,reason))return false;
    if(result.materials.layered)return fail("layered renderer required");
    std::array<std::vector<uint8_t>,std::size(RootSlots)> slots;
    for(size_t i=0;i<slots.size();++i){const auto range=index.slots[i];
        if(range.present)slots[i].assign(source.begin()+range.offset,source.begin()+range.offset+range.size);
    }
    // Native string consumers require terminated names within the owned blob.
    auto terminated=[&](unsigned slot,uint32_t offset){const auto& blob=slots[slot];
        return offset<blob.size() && std::memchr(blob.data()+offset,0,blob.size()-offset);
    };
    for(size_t at=0;at<slots[4].size();at+=32){const auto name=Read(slots[4].data()+at+28);
        if(name!=UINT32_MAX && !terminated(3,name))return fail("invalid group name offset");
    }
    for(size_t at=0;at<slots[14].size();at+=40)
        if(!terminated(13,Read(slots[14].data()+at)&0xffffff))return fail("invalid doodad name offset");
    for(size_t at=0;at<slots[12].size();at+=32){const auto first=Read(slots[12].data()+at+20),count=Read(slots[12].data()+at+24);
        const auto total=slots[14].size()/40;if(first>total || count>total-first)return fail("invalid doodad set range");
    }
    slots[1].assign(1,0);slots[2].clear();std::map<std::string,uint32_t> names;
    auto appendName=[&](const std::string& name){auto found=names.find(name);if(found!=names.end())return found->second;
        const auto offset=uint32_t(slots[1].size());slots[1].insert(slots[1].end(),name.begin(),name.end());slots[1].push_back(0);names.emplace(name,offset);return offset;
    };
    for(const auto& material:result.materials.records){
        auto disk=material.native;
        if(!ValidPath(material.paths[0]))return fail("invalid primary texture path");
        Write(disk.data()+12,appendName(material.paths[0]));
        const bool second=material.nativeShader==3 || material.nativeShader==5 || material.nativeShader==6;
        if(second){if(!ValidPath(material.paths[1]))return fail("invalid secondary texture path");Write(disk.data()+24,appendName(material.paths[1]));}
        else Write(disk.data()+24,0);
        Write(disk.data()+36,0); // unused third texture FDID must not become a name offset.
        slots[2].insert(slots[2].end(),disk.begin(),disk.end());
    }
    // Stock walker reads MOSB[0] even for an absent skybox: supply an empty string.
    if(slots[5].empty())slots[5].push_back(0);
    auto chunk=[&](const char* tag,std::span<const uint8_t> payload){const auto at=result.bytes.size();result.bytes.resize(at+8);
        for(unsigned i=0;i<4;++i)result.bytes[at+i]=uint8_t(tag[3-i]);Write(result.bytes.data()+at+4,uint32_t(payload.size()));
        result.bytes.insert(result.bytes.end(),payload.begin(),payload.end());
    };
    const uint32_t version=17;chunk("MVER",{reinterpret_cast<const uint8_t*>(&version),4});
    for(size_t i=0;i<slots.size();++i)chunk(RootSlots[i].tag,slots[i]);
    if(result.bytes.size()>64u*1024u*1024u)return fail("prepared root exceeds byte limit");
    output=std::move(result);return true;
}
}
