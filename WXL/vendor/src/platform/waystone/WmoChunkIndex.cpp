// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/WmoChunkIndex.hpp"
#include <cstring>
namespace wxl::waystone::wmo {
namespace {
uint32_t Read(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
uint32_t Tag(const char* p){return uint32_t(uint8_t(p[3]))|(uint32_t(uint8_t(p[2]))<<8)|(uint32_t(uint8_t(p[1]))<<16)|(uint32_t(uint8_t(p[0]))<<24);}
}
bool IndexRoot(std::span<const uint8_t> bytes,RootIndex& output) {
    if(bytes.size()<12 || bytes.size()>64u*1024u*1024u)return false;
    RootIndex result;bool version=false,gfid=false;
    size_t at=0;
    while(at<bytes.size()) {
        if(bytes.size()-at<8)return false;
        const uint32_t tag=Read(bytes.data()+at),size=Read(bytes.data()+at+4);
        if(size>bytes.size()-at-8)return false;
        const uint32_t content=uint32_t(at+8);
        if(tag==Tag("MVER")) {
            if(version || at!=0 || size!=4 || Read(bytes.data()+content)!=17)return false;
            version=true;
        } else {
            bool known=false;
            for(size_t i=0;i<std::size(RootSlots);++i) {
                const auto& slot=RootSlots[i];if(tag!=Tag(slot.tag))continue;
                if(result.slots[i].present || (slot.stride && size%slot.stride))return false;
                if(i==0 && size!=64)return false;
                if(i==5 && size && !std::memchr(bytes.data()+content,0,size))return false;
                result.slots[i]={content,size,true};known=true;break;
            }
            if(!known) {
                ++result.unknownChunks;
                if(tag==Tag("GFID")){if(gfid || size%4)return false;gfid=true;result.groupFileIds={content,size,true};}
                if(tag==Tag("MODI")){if(result.doodadIds || size%4)return false;result.doodadIds=true;result.doodadFileIds={content,size,true};}
            }
        }
        at+=8+size;
    }
    if(!version || !result.slots[0].present)return false;
    result.modern=gfid || !result.slots[1].present;
    output=result;return true;
}
namespace {
uint16_t Read16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
bool Add(Range& range,uint32_t offset,uint32_t size,uint32_t stride) {
    if(range.present || (stride && size%stride))return false;
    range={offset,size,true};return true;
}
}
bool IndexGroup(std::span<const uint8_t> bytes,GroupIndex& output) {
    if(bytes.size()<12 || bytes.size()>64u*1024u*1024u)return false;
    GroupIndex result;bool version=false,group=false;
    size_t at=0;
    while(at<bytes.size()) {
        if(bytes.size()-at<8)return false;
        const auto tag=Read(bytes.data()+at),size=Read(bytes.data()+at+4);
        if(size>bytes.size()-at-8)return false;
        const auto content=uint32_t(at+8);
        if(tag==Tag("MVER")) {
            if(version || at!=0 || size!=4 || Read(bytes.data()+content)!=17)return false;
            version=true;
        }else if(tag==Tag("MOGP")) {
            if(group || size<68)return false;
            group=true;result.header={content,68,true};
            result.flags=Read(bytes.data()+content+8);
            result.groupId=Read(bytes.data()+content+56);
            size_t cursor=content+68,end=content+size;
            while(cursor<end) {
                if(end-cursor<8)return false;
                const auto child=Read(bytes.data()+cursor),length=Read(bytes.data()+cursor+4);
                if(length>end-cursor-8)return false;
                const auto data=uint32_t(cursor+8);
                bool ok=true;
                if(child==Tag("MOTV")) {
                    if(result.uvSets==result.uv.size())return false;
                    ok=Add(result.uv[result.uvSets++],data,length,8);
                }else if(child==Tag("MOCV")) {
                    if(result.colorSets==result.colors.size())return false;
                    ok=Add(result.colors[result.colorSets++],data,length,4);
                }else if(child==Tag("MOPY"))ok=Add(result.triangles,data,length,2);
                else if(child==Tag("MOVI"))ok=Add(result.indices,data,length,2);
                else if(child==Tag("MOVT"))ok=Add(result.vertices,data,length,12);
                else if(child==Tag("MONR"))ok=Add(result.normals,data,length,12);
                else if(child==Tag("MOBA"))ok=Add(result.batches,data,length,24);
                else if(child==Tag("MOLR"))ok=Add(result.lights,data,length,2);
                else if(child==Tag("MODR"))ok=Add(result.doodads,data,length,2);
                else if(child==Tag("MOBN"))ok=Add(result.bspNodes,data,length,16);
                else if(child==Tag("MOBR"))ok=Add(result.bspFaces,data,length,2);
                else if(child==Tag("MORB"))ok=Add(result.replacementBatches,data,length,0);
                else if(child==Tag("MLIQ"))ok=Add(result.liquid,data,length,0);
                else if(child==Tag("MOVX"))ok=Add(result.indices32,data,length,4);
                else if(child==Tag("MOC2"))ok=Add(result.layerWeights,data,length,4);
                else ++result.unknownChunks;
                if(!ok)return false;
                cursor+=8+length;
            }
        }else ++result.unknownChunks;
        at+=8+size;
    }
    if(!version || !group)return false;
    output=result;return true;
}
bool ValidateGroupGeometry(std::span<const uint8_t> bytes,const GroupIndex& index,uint32_t materials) {
    // Do not trust an index from another file or a caller-modified index.
    GroupIndex parsed;
    if(!IndexGroup(bytes,parsed))return false;
    auto same=[](const Range& a,const Range& b){return a.offset==b.offset && a.size==b.size && a.present==b.present;};
    if(!same(index.header,parsed.header) || !same(index.vertices,parsed.vertices) ||
       !same(index.indices,parsed.indices) || !same(index.batches,parsed.batches))return false;
    const auto& g=parsed;
    if(g.indices32.present || !g.vertices.present || !g.indices.present ||
       !g.triangles.present || !g.normals.present)return false;
    const uint32_t vertices=g.vertices.size/12,indices=g.indices.size/2,faces=g.triangles.size/2;
    if(indices%3 || faces!=indices/3 || g.normals.size/12!=vertices)return false;
    for(const auto& uv:g.uv)if(uv.present && uv.size/8!=vertices)return false;
    for(const auto& colors:g.colors)if(colors.present && colors.size/4!=vertices)return false;
    if(g.layerWeights.present && g.layerWeights.size/4!=vertices)return false;
    if(g.batches.size && (!g.uvSets || !g.uv[0].size))return false;
    for(uint32_t i=0;i<indices;++i)if(Read16(bytes.data()+g.indices.offset+i*2)>=vertices)return false;
    for(uint32_t i=0;i<g.batches.size/24;++i) {
        const auto* batch=bytes.data()+g.batches.offset+i*24;
        const auto first=Read(batch+12);const auto count=Read16(batch+16);
        const auto minVertex=Read16(batch+18),maxVertex=Read16(batch+20);
        const uint32_t material=(batch[22]&2)?Read16(batch+10):batch[23];
        if(first>indices || count>indices-first || first%3 || count%3 || material>=materials || material>255 ||
           minVertex>maxVertex || maxVertex>=vertices)return false;
        for(uint32_t j=0;j<count;++j) {
            const auto v=Read16(bytes.data()+g.indices.offset+(first+j)*2);
            if(v<minVertex || v>maxVertex)return false;
        }
    }
    if(g.bspNodes.present!=g.bspFaces.present)return false;
    if(g.bspFaces.present)for(uint32_t i=0;i<g.bspFaces.size/2;++i)
        if(Read16(bytes.data()+g.bspFaces.offset+i*2)>=faces)return false;
    return true;
}

}
