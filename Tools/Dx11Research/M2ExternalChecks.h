#pragma once
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/OsMemory.h"
#include "Engine/Source/Services/Types.h"
#include <iomanip>
#include <sstream>

// Exercise the real native relocation code against extracted archive bytes.
// No archive, game/account state, or GPU is needed for this mode.
static std::vector<uint8_t> M2Bytes(const std::filesystem::path& path){
    auto n=std::filesystem::file_size(path);Check(n&&n<64*1024*1024,"bounded model/animation input");
    std::vector<uint8_t> out(size_t(n),uint8_t{});std::ifstream f(path,std::ios::binary);
    Check(bool(f.read(reinterpret_cast<char*>(out.data()),out.size())),"model/animation input read");return out;
}
template<class T> static void M2Leaf(const M2Array<T>& a,const uint8_t* base,size_t size){
    if(!a.m_count)return;auto p=reinterpret_cast<uintptr_t>(a.Data()),b=reinterpret_cast<uintptr_t>(base);
    Check(p>=b&&p-b<=size&&uint64_t(a.m_count)*sizeof(T)<=size-(p-b),"relocated animation leaf remains in owning buffer");
    volatile uint8_t first=*reinterpret_cast<const uint8_t*>(p);(void)first;
}
template<class T> static void M2TrackCheck(const M2Track<T>& t,uint32_t index,const uint8_t* model,size_t modelSize,const uint8_t* anim,size_t animSize){
    if(t.m_loopIndex!=0xffff){
        for(uint32_t i=0;i<t.m_sequenceTimes.m_count;++i)M2Leaf(t.m_sequenceTimes[i],model,modelSize);
        for(uint32_t i=0;i<t.m_sequenceKeys.m_count;++i)M2Leaf(t.m_sequenceKeys[i],model,modelSize);
    }else{
        if(index<t.m_sequenceTimes.m_count)M2Leaf(t.m_sequenceTimes[index],anim,animSize);
        if(index<t.m_sequenceKeys.m_count)M2Leaf(t.m_sequenceKeys[index],anim,animSize);
    }
}
static int M2ExternalChecks(const std::filesystem::path& directory){
    uint32_t loaded=0;
    for(const char* name:{"ogremale","ogrefemale"}){
        const auto original=M2Bytes(directory/(std::string(name)+".m2"));
        const auto& raw=*reinterpret_cast<const M2Data*>(original.data());
        Check(raw.m_sequences.m_offset+uint64_t(raw.m_sequences.m_count)*sizeof(M2Sequence)<=original.size(),"raw sequence table bounds");
        const auto* sequences=reinterpret_cast<const M2Sequence*>(original.data()+raw.m_sequences.m_offset);
        for(uint32_t i=0;i<raw.m_sequences.m_count;++i){
            if(sequences[i].m_flags&(0x20|0x40))continue;
            std::ostringstream file;file<<name<<std::setfill('0')<<std::setw(4)<<sequences[i].m_id<<'-'<<std::setw(2)<<sequences[i].m_variationIndex<<".anim";
            const auto bytes=M2Bytes(directory/"animations"/file.str());
            auto* model=static_cast<uint8_t*>(OsAllocNear(nullptr,original.size()));Check(model!=nullptr,"model allocation");
            auto* animation=static_cast<uint8_t*>(OsAllocNear(model,bytes.size()));Check(animation!=nullptr,"near animation allocation");
            std::memcpy(model,original.data(),original.size());std::memcpy(animation,bytes.data(),bytes.size());
            auto* data=reinterpret_cast<M2Data*>(model);
            std::cerr<<"external sequence "<<file.str()<<" index="<<i<<'\n'<<std::flush;
            Check(M2Init(model,uint32_t(original.size()),*data)!=0,"native model relocation");
            CM2Shared shared(nullptr);shared.m_data=data;shared.m_fileSize=uint32_t(original.size());
            std::snprintf(shared.m_fileName,sizeof(shared.m_fileName),"%s.m2",name);
            struct Detach{CM2Shared& shared;~Detach(){shared.m_data=nullptr;}} detach{shared};
            std::vector<uint32_t> aliases{i};auto next=data->m_sequences[i].m_aliasNext;
            while(next!=i){Check(next<data->m_sequences.m_count&&aliases.size()<data->m_sequences.m_count,"bounded native alias ring");aliases.push_back(next);next=data->m_sequences[next].m_aliasNext;}
            CAsyncObject object{};object.m_buffer=animation;object.m_size=uint32_t(bytes.size());
            Check(shared.FinishLoadingLowPrioritySequence(i,&object)!=0,"native external animation and alias relocation");
            for(auto index:aliases){
                auto track=[&](const auto& t){M2TrackCheck(t,index,model,original.size(),animation,bytes.size());};
                for(uint32_t b=0;b<data->m_bones.m_count;++b){const auto& bone=data->m_bones[b];track(bone.m_translationTrack);track(bone.m_rotationTrack);track(bone.m_scaleTrack);}
                for(uint32_t b=0;b<data->m_colors.m_count;++b){track(data->m_colors[b].m_color);track(data->m_colors[b].m_alpha);}
                for(uint32_t b=0;b<data->m_textureWeights.m_count;++b)track(data->m_textureWeights[b].m_weight);
                for(uint32_t b=0;b<data->m_textureTransforms.m_count;++b){const auto& t=data->m_textureTransforms[b];track(t.m_translation);track(t.m_rotation);track(t.m_scaling);}
                for(uint32_t b=0;b<data->m_attachments.m_count;++b)track(data->m_attachments[b].m_animateAttached);
                for(uint32_t b=0;b<data->m_lights.m_count;++b){const auto& t=data->m_lights[b];track(t.m_ambientColor);track(t.m_ambientIntensity);track(t.m_diffuseColor);track(t.m_diffuseIntensity);track(t.m_attenuationStart);track(t.m_attenuationEnd);track(t.m_visibility);}
                for(uint32_t b=0;b<data->m_cameras.m_count;++b){const auto& t=data->m_cameras[b];track(t.m_positions);track(t.m_targetPositions);track(t.m_roll);}
            }
            shared.m_data=nullptr;OsFreeNear(animation);OsFreeNear(model);++loaded;
        }
    }
    Check(loaded==72,"all exact Ogre external sequences exercised");
    std::cout<<"native Ogre external sequences="<<loaded<<" checks="<<checks<<'\n';return 0;
}
