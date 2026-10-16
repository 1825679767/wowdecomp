// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/SkeletonIndex.hpp"
#include <cstring>
#include <map>
#include <vector>
namespace wxl::waystone::skeleton {
namespace {
uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
uint16_t U16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
uint32_t Tag(const char* p){return uint32_t(uint8_t(p[0]))|(uint32_t(uint8_t(p[1]))<<8)|(uint32_t(uint8_t(p[2]))<<16)|(uint32_t(uint8_t(p[3]))<<24);}
bool Fits(size_t size,uint32_t off,uint32_t count,uint32_t stride){return off<=size && uint64_t(count)*stride<=size-off;}
}
bool Read(std::span<const uint8_t> source,Index& output,std::string& error){
    error.clear();auto fail=[&](const char* why){error=why;return false;};
    if(source.size()<8 || source.size()>64u*1024u*1024u)return fail("skeleton size");
    Index result;std::map<uint32_t,View> chunks;
    for(uint32_t at=0;at<source.size();){
        if(source.size()-at<8)return fail("truncated chunk header");
        const auto tag=U32(source.data()+at),length=U32(source.data()+at+4);
        if(length>source.size()-at-8)return fail("truncated chunk payload");
        const bool known=tag==Tag("SKB1") || tag==Tag("SKS1") || tag==Tag("SKA1") || tag==Tag("SKPD") || tag==Tag("AFID") || tag==Tag("SKL1");
        if(known && chunks.contains(tag))return fail("duplicate skeleton chunk");
        if(known)chunks.emplace(tag,View{at+8,length,true});else ++result.unknownChunks;
        at+=8+length;
    }
    result.bonesPayload=chunks[Tag("SKB1")];result.sequencesPayload=chunks[Tag("SKS1")];result.attachmentsPayload=chunks[Tag("SKA1")];
    if(!result.bonesPayload.present || !result.sequencesPayload.present)return fail("missing bones/sequences payload");
    auto array=[&](View payload,uint32_t head,uint32_t stride,uint32_t headerSize,Array& out){
        if(!payload.present || payload.size<head+8)return false;
        const auto* p=source.data()+payload.offset+head;
        const auto n=U32(p),off=U32(p+4);
        if(n>65535 || (n && (off<headerSize || !Fits(payload.size,off,n,stride))))return false;
        out={n,n?payload.offset+off:0};return true;
    };
    if(!array(result.bonesPayload,0,88,16,result.bones) || !array(result.bonesPayload,8,2,16,result.boneLookup) ||
       !array(result.sequencesPayload,0,4,24,result.loops) || !array(result.sequencesPayload,8,64,24,result.sequences) ||
       !array(result.sequencesPayload,16,2,24,result.sequenceLookup))return fail("invalid skeleton array");
    if(result.bones.count>4096 || result.sequences.count>4096)return fail("skeleton count limit");
    if(result.attachmentsPayload.present && (!array(result.attachmentsPayload,0,40,16,result.attachments) ||
       !array(result.attachmentsPayload,8,2,16,result.attachmentLookup)))return fail("invalid attachment array");
    const auto parent=chunks[Tag("SKPD")];if(parent.present){if(parent.size<12)return fail("short parent chunk");result.parentId=U32(source.data()+parent.offset+8);}
    const auto afid=chunks[Tag("AFID")];if(afid.present){if(afid.size%8 || afid.size/8>65535)return fail("invalid animation ID table");result.animationIds=afid.size/8;
        std::map<uint32_t,uint32_t> seen;for(uint32_t i=0;i<result.animationIds;++i){
            const auto* record=source.data()+afid.offset+i*8;const auto key=U32(record),id=U32(record+4);
            if(seen.contains(key))return fail("duplicate animation file key");seen.emplace(key,id);
            result.animationFiles.push_back({U16(record),U16(record+2),id});
        }}
    auto lookup=[&](Array a,uint32_t bound){for(uint32_t i=0;i<a.count;++i){const auto v=U16(source.data()+a.offset+i*2);if(v!=65535 && v>=bound)return false;}return true;};
    if(!lookup(result.boneLookup,result.bones.count) || !lookup(result.sequenceLookup,result.sequences.count) ||
       !lookup(result.attachmentLookup,result.attachments.count))return fail("invalid lookup reference");
    for(uint32_t i=0;i<result.sequences.count;++i){
        const auto* seq=source.data()+result.sequences.offset+i*64;const auto flags=U32(seq+12);
        const auto next=static_cast<int16_t>(U16(seq+60));
        if(next<-1 || (next>=0 && uint32_t(next)>=result.sequences.count) || ((flags&0x40) && U16(seq+62)>=result.sequences.count))return fail("invalid sequence link");
        if(flags&0x40)++result.aliasSequences;else if(!(flags&0x20))++result.externalSequences;
    }

    // Alias traversal must terminate before native animation evaluation can use it.
    for(uint32_t start=0;start<result.sequences.count;++start){
        uint32_t at=start;
        for(uint32_t steps=0;;++steps){
            const auto* seq=source.data()+result.sequences.offset+at*64;
            if(!(U32(seq+12)&0x40))break;
            if(steps>=result.sequences.count)return fail("cyclic sequence alias");
            at=U16(seq+62);
        }
    }
    std::vector<uint8_t> colors(result.bones.count);
    for(uint32_t i=0;i<result.bones.count;++i){
        std::vector<uint32_t> path;int32_t bone=int32_t(i);
        while(bone>=0){
            if(uint32_t(bone)>=result.bones.count)return fail("invalid bone parent");
            if(colors[bone]==1)return fail("cyclic bone parent");
            if(colors[bone]==2)break;colors[bone]=1;path.push_back(uint32_t(bone));
            bone=static_cast<int16_t>(U16(source.data()+result.bones.offset+uint32_t(bone)*88+8));
            if(bone<-1)return fail("invalid bone parent");
        }
        for(auto v:path)colors[v]=2;
    }
    auto track=[&](View payload,uint32_t at,uint32_t width){
        if(!Fits(payload.size,at,1,width?20:12))return false;
        const auto* p=source.data()+payload.offset+at;const auto loop=static_cast<int16_t>(U16(p+2));
        if(U16(p)>3 || loop<-1 || (loop>=0 && uint32_t(loop)>=result.loops.count))return false;
        for(unsigned pair=4;pair<=(width?12u:4u);pair+=8){
            const auto n=U32(p+pair),off=U32(p+pair+4);
            if(n>4096 || (n && !Fits(payload.size,off,n,8)))return false;
            for(uint32_t j=0;j<n;++j){
                const auto* inner=source.data()+payload.offset+off+j*8;
                bool embedded=loop>=0;
                if(!embedded){if(j>=result.sequences.count)return false;embedded=(U32(source.data()+result.sequences.offset+j*64+12)&0x60)==0x20;}
                // External leaf offsets address another file; defer them to its decoder.
                if(embedded && !Fits(payload.size,U32(inner+4),U32(inner),pair==4?4:width))return false;
            }
        }return true;
    };
    for(uint32_t i=0;i<result.bones.count;++i){
        const uint32_t at=result.bones.offset-result.bonesPayload.offset+i*88;
        if(!track(result.bonesPayload,at+16,12) || !track(result.bonesPayload,at+36,8) || !track(result.bonesPayload,at+56,12))return fail("invalid bone track");
    }
    for(uint32_t i=0;i<result.attachments.count;++i){
        const auto at=result.attachments.offset-result.attachmentsPayload.offset+i*40;
        if(U16(source.data()+result.attachments.offset+i*40+4)>=result.bones.count || !track(result.attachmentsPayload,at+20,1))return fail("invalid attachment track");
    }
    output=result;return true;
}
}
