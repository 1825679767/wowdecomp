// Copyright (C) 2026 WarcraftXL. GPLv3.
// Serialized owned output only; native finalize/palette ownership is a separate gate.
#include "platform/waystone/SkinPartition.hpp"
#include <algorithm>
#include <cstring>
#include <map>
namespace wxl::waystone::skin {
namespace {
uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
uint16_t U16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
void W32(std::vector<uint8_t>& b,size_t at,uint32_t v){std::memcpy(b.data()+at,&v,4);}
void W16(std::vector<uint8_t>& b,size_t at,uint16_t v){std::memcpy(b.data()+at,&v,2);}
void Add16(std::vector<uint8_t>& b,uint16_t v){size_t at=b.size();b.resize(at+2);W16(b,at,v);}
}
bool Rebuild(std::span<const uint8_t> body,std::span<const uint8_t> source,uint32_t skeletonBones,bool extended,Owned& output,std::string& error,uint32_t paletteLimit){
    Plan plan;if(!Partition(body,source,skeletonBones,extended,plan,error,paletteLimit))return false;
    auto fail=[&](const char* why){error=why;return false;};
    const uint32_t batchN=U32(source.data()+36),batchO=U32(source.data()+40);
    if(batchN>65535 || batchO>source.size() || uint64_t(batchN)*24>source.size()-batchO)return fail("batch array bounds");
    const auto sectionO=U32(source.data()+32),lookupO=U32(source.data()+8),indexO=U32(source.data()+16),propsO=U32(source.data()+24);
    const auto vertexO=U32(body.data()+0x40),comboO=U32(body.data()+0x7c);
    Owned result;result.plan=plan;
    std::vector<uint8_t> vertices,lookup,indices,props,sections,combos,batches;
    std::vector<std::vector<uint16_t>> runs(plan.sourceSections);
    uint32_t maxPalette=0;
    for(const auto& draw:plan.draws){
        if(indices.size()+uint64_t(draw.indexCount)*2>64u*1024u*1024u)return fail("rebuilt index limit");
        if(sections.size()/48>=65535 || combos.size()/2+std::max<size_t>(1,draw.bones.size())>65535)return fail("rebuilt section/palette limit");
        const uint16_t sectionIndex=uint16_t(sections.size()/48);
        runs[draw.sourceSection].push_back(sectionIndex);
        const auto* original=source.data()+sectionO+draw.sourceSection*48;
        const uint32_t vertexStart=uint32_t(lookup.size()/2),indexStart=uint32_t(indices.size()/2),comboStart=uint32_t(combos.size()/2);
        for(auto bone:draw.bones)Add16(combos,bone);
        if(draw.bones.empty())Add16(combos,0); // no weighted influences; valid inert palette
        maxPalette=std::max(maxPalette,uint32_t(std::max<size_t>(1,draw.bones.size())));
        std::map<uint16_t,uint16_t> remap;
        for(uint32_t i=0;i<draw.indexCount;++i){
            const auto old=U16(source.data()+indexO+2*(draw.firstIndex+i));
            auto found=remap.find(old);
            if(found==remap.end()){
                if(lookup.size()/2>=65535)return fail("rebuilt vertex limit");
                const auto target=uint16_t(lookup.size()/2),origin=U16(source.data()+lookupO+2*old);
                const size_t at=vertices.size();vertices.insert(vertices.end(),body.begin()+vertexO+origin*48,body.begin()+vertexO+(origin+1)*48);
                const size_t propsAt=props.size();props.resize(propsAt+4,0);
                for(unsigned k=0;k<4;++k){
                    uint8_t slot=0;
                    if(vertices[at+12+k]){
                        const auto sourceSlot=source[propsO+old*4+k];
                        const auto bone=U16(body.data()+comboO+2*(U16(original+14)+sourceSlot));
                        auto pos=std::lower_bound(draw.bones.begin(),draw.bones.end(),bone);
                        if(pos==draw.bones.end() || *pos!=bone)return fail("missing planned palette influence");
                        slot=uint8_t(pos-draw.bones.begin());
                    }
                    props[propsAt+k]=slot;vertices[at+16+k]=slot;
                }
                Add16(lookup,target);result.vertexOrigins.push_back(origin);
                found=remap.emplace(old,target).first;
            }
            Add16(indices,found->second);
        }
        const auto at=sections.size();sections.insert(sections.end(),original,original+48);
        W16(sections,at+2,uint16_t(indexStart>>16));W16(sections,at+4,uint16_t(vertexStart));
        W16(sections,at+6,uint16_t(lookup.size()/2-vertexStart));W16(sections,at+8,uint16_t(indexStart));
        W16(sections,at+10,uint16_t(draw.indexCount));W16(sections,at+12,uint16_t(std::max<size_t>(1,draw.bones.size())));
        W16(sections,at+14,uint16_t(comboStart));
    }
    for(uint32_t i=0;i<batchN;++i){
        const auto* batch=source.data()+batchO+i*24;const auto original=U16(batch+4);
        if(original>=runs.size())return fail("batch section reference");
        for(auto section:runs[original]){
            if(batches.size()/24>=65535)return fail("rebuilt batch limit");
            const auto at=batches.size();batches.insert(batches.end(),batch,batch+24);W16(batches,at+4,section);
        }
    }
    if(body.size()+vertices.size()+combos.size()>64u*1024u*1024u)return fail("rebuilt body limit");
    result.body.assign(body.begin(),body.end());result.skin.assign(source.begin(),source.end());
    auto append=[](std::vector<uint8_t>& target,const std::vector<uint8_t>& bytes,uint32_t head,uint32_t stride){
        while(target.size()%4)target.push_back(0);
        const auto at=uint32_t(target.size());target.insert(target.end(),bytes.begin(),bytes.end());
        W32(target,head,uint32_t(bytes.size()/stride));W32(target,head+4,bytes.empty()?0:at);
    };
    append(result.body,vertices,0x3c,48);append(result.body,combos,0x78,2);
    append(result.skin,lookup,4,2);append(result.skin,indices,12,2);append(result.skin,props,20,4);
    append(result.skin,sections,28,48);append(result.skin,batches,36,24);W32(result.skin,44,maxPalette);
    if(result.skin.size()>64u*1024u*1024u)return fail("rebuilt skin limit");
    output=std::move(result);return true;
}
}
