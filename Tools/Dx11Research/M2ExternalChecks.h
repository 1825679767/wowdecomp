#pragma once
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Model.h"
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
template<class T,class V> static void M2SampleCheck(CM2Model& model,M2ModelBone& bone,const M2Track<T>& track,M2ModelTrack<V>& state,const V& fallback){
    const auto index=uint32_t(bone.m_sequence.m_animIndex);
    if(track.m_sequenceKeys.m_count){const auto& keys=track.m_sequenceKeys[index<track.m_sequenceKeys.m_count?index:0];
        if(keys.m_count){uint32_t first=0,last=0;float ratio=0;model.FindKey(&bone.m_sequence,track,first,last,ratio);
            Check(first<keys.m_count&&last<keys.m_count&&std::isfinite(ratio)&&ratio>=0&&ratio<=1,"native animation key sampling remains bounded");}}
    M2AnimateTrack<T,V>(&model,&bone,track,state,fallback);
    float components[4]{};static_assert(sizeof(V)<=sizeof(components));std::memcpy(components,&state.m_currentValue,sizeof(V));
    for(unsigned i=0;i<sizeof(V)/sizeof(float);++i)Check(std::isfinite(components[i]),"native animated bone component is finite");
}
static void M2GlobalExternalCheck(){
    constexpr uint32_t size=2048;auto* bytes=static_cast<uint8_t*>(OsAllocNear(nullptr,size));Check(bytes!=nullptr,"global-track model allocation");std::memset(bytes,0,size);
    auto* external=static_cast<uint8_t*>(OsAllocNear(bytes,8));Check(external!=nullptr,"global-track external allocation");std::memset(external,0,8);
    auto& data=*reinterpret_cast<M2Data*>(bytes);data.m_magic=0x3032444d;data.m_version=264;uint32_t offset=sizeof(M2Data);
    data.m_loops={1,offset};reinterpret_cast<M2Loop*>(bytes+offset)->m_length=1000;offset+=sizeof(M2Loop);
    data.m_sequences={1,offset};auto* sequence=reinterpret_cast<M2Sequence*>(bytes+offset);sequence->m_id=74;sequence->m_duration=1000;offset+=sizeof(M2Sequence);
    data.m_bones={1,offset};auto* bone=reinterpret_cast<M2CompBone*>(bytes+offset);offset+=sizeof(M2CompBone);bone->m_parentIndex=0xffff;bone->m_translationTrack.m_loopIndex=0;bone->m_translationTrack.m_trackType=1;bone->m_rotationTrack.m_loopIndex=bone->m_scaleTrack.m_loopIndex=0xffff;
    bone->m_translationTrack.m_sequenceTimes={1,offset};auto* times=reinterpret_cast<M2Array<uint32_t>*>(bytes+offset);offset+=sizeof(*times);
    bone->m_translationTrack.m_sequenceKeys={1,offset};auto* keys=reinterpret_cast<M2Array<C3Vector>*>(bytes+offset);offset+=sizeof(*keys);
    *times={1,offset};*reinterpret_cast<uint32_t*>(bytes+offset)=0;offset+=4;*keys={1,offset};*reinterpret_cast<C3Vector*>(bytes+offset)=C3Vector(1,2,3);offset+=sizeof(C3Vector);Check(offset<size,"bounded synthetic global-track fixture");
    Check(M2Init(bytes,size,data)!=0,"native global-track model relocation");auto* originalTimes=bone->m_translationTrack.m_sequenceTimes[0].Data();auto* originalKeys=bone->m_translationTrack.m_sequenceKeys[0].Data();
    CM2Shared shared(nullptr);shared.m_data=&data;shared.m_fileSize=size;std::snprintf(shared.m_fileName,sizeof(shared.m_fileName),"global-track-fixture.m2");
    struct Cleanup{CM2Shared& s;uint8_t* model;uint8_t* stream;~Cleanup(){s.m_data=nullptr;OsFreeNear(stream);OsFreeNear(model);}} cleanup{shared,bytes,external};
    CAsyncObject object{};object.m_buffer=external;object.m_size=8;Check(shared.FinishLoadingLowPrioritySequence(0,&object)!=0,"external index zero can coexist with an embedded global track");
    Check(bone->m_translationTrack.m_sequenceTimes[0].Data()==originalTimes&&bone->m_translationTrack.m_sequenceKeys[0].Data()==originalKeys,"external loading preserves global-track model ownership");
    Check(*originalTimes==0&&originalKeys->x==1&&originalKeys->y==2&&originalKeys->z==3,"embedded global-track values remain readable and unchanged");
}
static int M2ExternalChecks(const std::filesystem::path& directory){
    uint32_t loaded=0;
    for(const char* name:{"ogremale","ogrefemale"}){
        const auto original=M2Bytes(directory/(std::string(name)+".m2"));
        const auto& raw=*reinterpret_cast<const M2Data*>(original.data());
        Check(raw.m_sequences.m_offset+uint64_t(raw.m_sequences.m_count)*sizeof(M2Sequence)<=original.size(),"raw sequence table bounds");
        const auto* sequences=reinterpret_cast<const M2Sequence*>(original.data()+raw.m_sequences.m_offset);bool metadataCases=false;
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
            const std::vector<uint8_t> before(model,model+original.size());
            object.m_size=1;Check(!shared.FinishLoadingLowPrioritySequence(i,&object),"truncated external animation rejected before relocation");
            Check(!std::memcmp(model,before.data(),before.size()),"short-read rejection leaves all model bytes and aliases unchanged");
            object.m_size=0;Check(!shared.FinishLoadingLowPrioritySequence(i,&object),"empty external animation rejected");
            object.m_size=uint32_t(bytes.size());
            Check(!shared.FinishLoadingLowPrioritySequence(UINT32_MAX,&object)&&!shared.FinishLoadingLowPrioritySequence(i,static_cast<CAsyncObject*>(nullptr)),"invalid external sequence index and async object rejected");
            const auto savedAlias=data->m_sequences[i].m_aliasNext;data->m_sequences[i].m_aliasNext=uint16_t(data->m_sequences.m_count);
            const std::vector<uint8_t> badAlias(model,model+original.size());
            Check(!shared.FinishLoadingLowPrioritySequence(i,&object),"out-of-range native alias rejected before any relocation");
            Check(!std::memcmp(model,badAlias.data(),badAlias.size()),"bad alias rejection is transactional");data->m_sequences[i].m_aliasNext=savedAlias;
            if(!metadataCases){
                const uint32_t other=i?0:1;Check(other<data->m_sequences.m_count,"alias-cycle fixture index");const auto savedOther=data->m_sequences[other].m_aliasNext;
                data->m_sequences[i].m_aliasNext=uint16_t(other);data->m_sequences[other].m_aliasNext=uint16_t(other);const std::vector<uint8_t> cyclic(model,model+original.size());
                Check(!shared.FinishLoadingLowPrioritySequence(i,&object),"nonreturning native alias cycle rejects within a bounded walk");Check(!std::memcmp(model,cyclic.data(),cyclic.size()),"cyclic alias rejection does not change model state");data->m_sequences[i].m_aliasNext=savedAlias;data->m_sequences[other].m_aliasNext=savedOther;
                bool checkedCount=false;auto rejectCount=[&](auto& track){if(checkedCount||track.m_loopIndex!=0xffff||i>=track.m_sequenceKeys.m_count||!track.m_sequenceKeys[i].m_count)return;auto& leaf=track.m_sequenceKeys[i];const auto saved=leaf.m_count;leaf.m_count=UINT32_MAX;const std::vector<uint8_t> overflow(model,model+original.size());Check(!shared.FinishLoadingLowPrioritySequence(i,&object),"oversized external key count rejects without integer overflow");Check(!std::memcmp(model,overflow.data(),overflow.size()),"bad key-count rejection is transactional");leaf.m_count=saved;checkedCount=true;};
                for(uint32_t b=0;b<data->m_bones.m_count;++b){auto& bone=data->m_bones[b];rejectCount(bone.m_translationTrack);rejectCount(bone.m_rotationTrack);rejectCount(bone.m_scaleTrack);}Check(checkedCount,"real Ogre stream contains a bounded key-count corruption target");metadataCases=true;
            }
            Check(shared.FinishLoadingLowPrioritySequence(i,&object)!=0,"native external animation and alias relocation");
            const std::vector<uint8_t> loadedBytes(model,model+original.size());
            Check(shared.FinishLoadingLowPrioritySequence(i,&object)!=0&&!std::memcmp(model,loadedBytes.data(),loadedBytes.size()),"already loaded external aliases are idempotent");
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
            {
                std::vector<int32_t> loops(data->m_loops.m_count);CM2Model evaluator;shared.AddRef();evaluator.m_shared=&shared;evaluator.m_loops=loops.data();
                struct UnbindLoops{CM2Model& model;~UnbindLoops(){model.m_loops=nullptr;}} unbind{evaluator};
                for(auto index:aliases){const auto duration=data->m_sequences[index].m_duration;
                    for(uint32_t time:{0u,250u,500u,duration/2,duration,duration+1}){
                        for(uint32_t k=0;k<loops.size();++k)loops[k]=time%std::max(1u,data->m_loops[k].m_length);
                        for(uint32_t b=0;b<data->m_bones.m_count;++b){const auto& source=data->m_bones[b];M2ModelBone bone;bone.m_blendFactor=0;bone.m_sequence.m_animIndex=uint16_t(index);bone.m_sequence.m_currentTime=time;
                            M2SampleCheck(evaluator,bone,source.m_translationTrack,bone.m_translation,C3Vector(0,0,0));
                            C4Quaternion identity;identity.x=identity.y=identity.z=0;identity.w=1;M2SampleCheck(evaluator,bone,source.m_rotationTrack,bone.m_rotation,identity);
                            M2SampleCheck(evaluator,bone,source.m_scaleTrack,bone.m_scale,C3Vector(1,1,1));
                        }
                    }
                }
            }
            shared.m_data=nullptr;OsFreeNear(animation);OsFreeNear(model);++loaded;
        }
    }
    Check(loaded==72,"all exact Ogre external sequences exercised");
    M2GlobalExternalCheck();
    std::cout<<"native Ogre external sequences="<<loaded<<" checks="<<checks<<'\n';return 0;
}
