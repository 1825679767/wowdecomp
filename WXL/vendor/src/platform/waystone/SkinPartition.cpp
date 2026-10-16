// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/SkinPartition.hpp"
#include <cstring>
#include <set>
namespace wxl::waystone::skin {
namespace {
uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
uint16_t U16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
bool Fits(size_t size,uint32_t at,uint32_t n,uint32_t stride){return at<=size && uint64_t(n)*stride<=size-at;}
}
bool Partition(std::span<const uint8_t> body,std::span<const uint8_t> bytes,uint32_t skeletonBones,bool extended,Plan& output,std::string& error,uint32_t paletteLimit){
    error.clear();auto fail=[&](const char* why){error=why;return false;};
    if(body.size()<304 || bytes.size()<48 || body.size()>64u*1024u*1024u || bytes.size()>64u*1024u*1024u || U32(body.data())!=0x3032444d || U32(bytes.data())!=0x4e494b53 || !skeletonBones || skeletonBones>65535)return fail("body/skin contract");
    if(!paletteLimit || paletteLimit>75)return fail("device palette limit");
    const uint32_t nv=U32(body.data()+0x3c),vo=U32(body.data()+0x40),nc=U32(body.data()+0x78),co=U32(body.data()+0x7c);
    if(!Fits(body.size(),vo,nv,48) || !Fits(body.size(),co,nc,2))return fail("body array bounds");
    const uint32_t lookupN=U32(bytes.data()+4),lookupO=U32(bytes.data()+8),indexN=U32(bytes.data()+12),indexO=U32(bytes.data()+16),boneN=U32(bytes.data()+20),boneO=U32(bytes.data()+24),sectionN=U32(bytes.data()+28),sectionO=U32(bytes.data()+32);
    if(lookupN>65535 || boneN!=lookupN || sectionN>16384 ||
       !Fits(bytes.size(),lookupO,lookupN,2) || !Fits(bytes.size(),indexO,indexN,2) ||
       !Fits(bytes.size(),boneO,boneN,4) || !Fits(bytes.size(),sectionO,sectionN,48))return fail("skin array bounds");
    Plan result;result.sourceSections=sectionN;
    for(uint32_t section=0;section<sectionN;++section){
        const auto* s=bytes.data()+sectionO+section*48;
        const uint32_t first=U16(s+8)|(extended?uint32_t(U16(s+2))<<16:0),count=U16(s+10),vfirst=U16(s+4),vcount=U16(s+6),palette=U16(s+12),combo=U16(s+14);
        if(!extended && U16(s+2))continue; // Legacy LOD sections are not current geometry.
        if(first>indexN || count>indexN-first || count%3 || vfirst>lookupN || vcount>lookupN-vfirst || combo>nc || palette>nc-combo || U16(s+16)>4)return fail("section bounds");
        if(!count)continue;
        if(result.sourceTriangles+count/3>64u*1024u*1024u/6)return fail("triangle reference limit");
        result.sourceTriangles+=count/3;std::set<uint16_t> active;uint32_t start=first,draws=0;
        auto emit=[&](uint32_t end){if(end==start)return;result.draws.push_back({section,start,end-start,{active.begin(),active.end()}});++draws;start=end;active.clear();};
        for(uint32_t at=first;at<first+count;at+=3){
            std::set<uint16_t> triangle;
            for(unsigned corner=0;corner<3;++corner){
                const auto vertex=U16(bytes.data()+indexO+(at+corner)*2);
                if(vertex<vfirst || uint32_t(vertex)>=vfirst+vcount)return fail("triangle outside section vertex window");
                const auto modelVertex=U16(bytes.data()+lookupO+vertex*2);
                if(modelVertex>=nv)return fail("invalid vertex lookup");
                const auto* weights=body.data()+vo+modelVertex*48+12;
                for(unsigned influence=0;influence<4;++influence){
                    if(!weights[influence])continue;
                    const auto slot=bytes[boneO+vertex*4+influence];
                    if(slot>=palette)return fail("invalid palette slot");
                    const auto bone=U16(body.data()+co+(combo+slot)*2);
                    if(bone>=skeletonBones)return fail("invalid skeleton bone");
                    triangle.insert(bone);
                }
            }
            if(triangle.size()>paletteLimit)return fail("triangle exceeds device palette");
            std::set<uint16_t> joined=active;joined.insert(triangle.begin(),triangle.end());
            if(joined.size()>paletteLimit){emit(at);joined=triangle;}
            active=std::move(joined);
        }
        emit(first+count);if(draws>1)++result.splitSections;
    }
    output=std::move(result);return true;
}
}
