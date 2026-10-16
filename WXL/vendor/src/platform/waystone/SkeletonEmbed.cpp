// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/SkeletonEmbed.hpp"
#include <cstring>
#include <map>
#include <cstdio>
#include "../../../extensions/wxl-modern-m2/src/load/M2WalkLayout.hpp"
namespace wxl::waystone::skeleton {
namespace {
constexpr uint32_t Limit=64u*1024u*1024u;
uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
uint16_t U16(const uint8_t* p){uint16_t v;std::memcpy(&v,p,2);return v;}
void W32(std::vector<uint8_t>& b,uint32_t at,uint32_t v){std::memcpy(b.data()+at,&v,4);}
bool Fits(size_t size,uint32_t at,uint32_t n,uint32_t stride){return at<=size && uint64_t(n)*stride<=size-at;}
struct Streams {View model,bone,attachment;uint32_t base{};};
bool StreamsOf(std::span<const uint8_t> bytes,Streams& out){
    if(bytes.size()<8 || bytes.size()>Limit)return false;
    View flat,bone,attachment;
    for(uint32_t at=0;at<bytes.size();){
        if(bytes.size()-at<8)return false;
        uint32_t size=U32(bytes.data()+at+4);if(size>bytes.size()-at-8)return false;
        View* target=nullptr;
        if(!std::memcmp(bytes.data()+at,"AFM2",4))target=&flat;
        else if(!std::memcmp(bytes.data()+at,"AFSB",4))target=&bone;
        else if(!std::memcmp(bytes.data()+at,"AFSA",4))target=&attachment;
        if(target){if(target->present)return false;*target={at+8,size,true};}
        at+=8+size;
    }
    if(!bone.present && !attachment.present){bone=flat;attachment=flat;}
    out.model=flat;out.bone=bone;out.attachment=attachment;return flat.present || bone.present || attachment.present;
}
}
bool Embed(std::span<const uint8_t> body,std::span<const uint8_t> source,AnimationReader read,Embedded& output,std::string& error,std::span<const uint8_t> modelSkeleton,AnimationReader modelRead){
    Index index;if(!Read(source,index,error))return false;
    auto fail=[&](const char* why){error=why;return false;};
    if(body.size()<304 || body.size()>Limit || std::memcmp(body.data(),"MD20",4))return fail("model body contract");
    if(index.parentId)return fail("parent skeleton assembly deferred");
    for(uint32_t head:{0x14u,0x1cu,0x24u,0x2cu,0x34u,0xf0u,0xf8u})if(U32(body.data()+head))return fail("model already owns skeleton arrays");
    Embedded result;result.body.assign(body.begin(),body.end());result.aliases=index.aliasSequences;
    auto append=[&](std::span<const uint8_t> bytes,uint32_t& base){
        const size_t padding=(4-result.body.size()%4)%4;
        if(result.body.size()+padding+bytes.size()>Limit)return false;
        result.body.insert(result.body.end(),padding,0);base=uint32_t(result.body.size());
        result.body.insert(result.body.end(),bytes.begin(),bytes.end());return true;
    };
    uint32_t bonesBase{},sequencesBase{},attachmentsBase{};
    if(!append(source.subspan(index.bonesPayload.offset,index.bonesPayload.size),bonesBase) ||
       !append(source.subspan(index.sequencesPayload.offset,index.sequencesPayload.size),sequencesBase))return fail("assembled body limit");
    if(index.attachmentsPayload.present && !append(source.subspan(index.attachmentsPayload.offset,index.attachmentsPayload.size),attachmentsBase))return fail("assembled attachment limit");
    auto head=[&](uint32_t at,Array a,View view,uint32_t base){W32(result.body,at,a.count);W32(result.body,at+4,a.count?base+a.offset-view.offset:0);};
    head(0x14,index.loops,index.sequencesPayload,sequencesBase);head(0x1c,index.sequences,index.sequencesPayload,sequencesBase);head(0x24,index.sequenceLookup,index.sequencesPayload,sequencesBase);
    head(0x2c,index.bones,index.bonesPayload,bonesBase);head(0x34,index.boneLookup,index.bonesPayload,bonesBase);
    head(0xf0,index.attachments,index.attachmentsPayload,attachmentsBase);head(0xf8,index.attachmentLookup,index.attachmentsPayload,attachmentsBase);
    std::map<uint32_t,AnimationFile> files;
    for(const auto& f:index.animationFiles)files.emplace(uint32_t(f.animation)|(uint32_t(f.variation)<<16),f);
    std::map<uint32_t,Streams> streams;
    for(uint32_t i=0;i<index.sequences.count;++i){
        const auto* seq=source.data()+index.sequences.offset+i*64;
        if(U32(seq+12)&0x60)continue;
        auto f=files.find(U32(seq));
        if(!read || f==files.end())return fail("missing animation file mapping");
        std::vector<uint8_t> bytes;Streams s;
        if(!read(f->second.animation,f->second.variation,f->second.fileId,bytes))return fail("missing animation companion");
        if(!StreamsOf(bytes,s))return fail("unsupported animation container");
        if(!append(bytes,s.base))return fail("assembled animation limit");
        streams.emplace(i,s);++result.animationFiles;
    }
    auto terminal=[&](uint32_t i){while(U32(source.data()+index.sequences.offset+i*64+12)&0x40)i=U16(source.data()+index.sequences.offset+i*64+62);return i;};
    auto track=[&](View view,uint32_t base,uint32_t at,uint32_t stride,bool attachment){
        const auto* raw=source.data()+view.offset+at;const bool global=static_cast<int16_t>(U16(raw+2))>=0;
        for(unsigned pair:{4u,12u}){
            const uint32_t count=U32(raw+pair),offset=U32(raw+pair+4);
            W32(result.body,base+at+pair+4,count?base+offset:0);
            for(uint32_t i=0;i<count;++i){
                const uint32_t target=global?i:terminal(i);
                const uint32_t destination=base+offset+i*8;
                if(target>=count){W32(result.body,destination,0);W32(result.body,destination+4,0);continue;}
                const auto* leaf=source.data()+view.offset+offset+target*8;const uint32_t n=U32(leaf),off=U32(leaf+4);
                uint32_t targetBase=base;View range{0,view.size,true};
                if(!global && !(U32(source.data()+index.sequences.offset+target*64+12)&0x20)){
                    const auto& s=streams.at(target);range=attachment?s.attachment:s.bone;targetBase=s.base+range.offset;
                }
                if(n && (!range.present || !Fits(range.size,off,n,pair==4?4:stride)))return false;
                W32(result.body,destination,n);W32(result.body,destination+4,n?targetBase+off:0);
            }
        }
        return true;
    };
    for(uint32_t i=0;i<index.bones.count;++i){
        const uint32_t at=index.bones.offset-index.bonesPayload.offset+i*88;
        if(!track(index.bonesPayload,bonesBase,at+16,12,false) || !track(index.bonesPayload,bonesBase,at+36,8,false) || !track(index.bonesPayload,bonesBase,at+56,12,false))return fail("animation bone leaf bounds");
    }
    for(uint32_t i=0;i<index.attachments.count;++i){
        const uint32_t at=index.attachments.offset-index.attachmentsPayload.offset+i*40;
        if(!track(index.attachmentsPayload,attachmentsBase,at+20,1,true))return fail("animation attachment leaf bounds");
    }

    // Model-owned tracks use AFM2, while skeleton bones/attachments use AFSB/AFSA.
    // Rebase those leaves before making all sequences embedded.
    namespace d=wxl::runtime::m2native::detail;
    namespace f=wxl::structure::m2;
    // A derived model indexes its own SKS1, not the inherited parent's SKS1.
    // Preserve parent bone animation, map model-owned channels by action/variation,
    // and obtain external model leaves from the derived actor's AFM2 stream.
    Index modelIndex=index;const auto modelSource=modelSkeleton.empty()?source:modelSkeleton;
    const bool derived=!modelSkeleton.empty();
    if(derived && !Read(modelSource,modelIndex,error))return false;
    auto modelTerminal=[&](uint32_t i){while(U32(modelSource.data()+modelIndex.sequences.offset+i*64+12)&0x40)i=U16(modelSource.data()+modelIndex.sequences.offset+i*64+62);return i;};
    std::map<uint32_t,uint32_t> modelActions;
    if(derived)for(uint32_t i=0;i<modelIndex.sequences.count;++i)
        if(!modelActions.emplace(U32(modelSource.data()+modelIndex.sequences.offset+i*64),i).second)return fail("duplicate model sequence identity");
    std::vector<uint32_t> loopMap(modelIndex.loops.count);
    if(derived){
        std::vector<uint32_t> loops;
        for(uint32_t i=0;i<index.loops.count;++i)loops.push_back(U32(source.data()+index.loops.offset+i*4));
        for(uint32_t i=0;i<modelIndex.loops.count;++i){const auto duration=U32(modelSource.data()+modelIndex.loops.offset+i*4);auto found=std::find(loops.begin(),loops.end(),duration);
            if(found==loops.end()){loopMap[i]=uint32_t(loops.size());loops.push_back(duration);}else loopMap[i]=uint32_t(found-loops.begin());
        }
        if(loops.size()>32767)return fail("inherited global loop limit");
        uint32_t base{};if(!append({reinterpret_cast<const uint8_t*>(loops.data()),loops.size()*4},base))return fail("inherited global loop size");
        W32(result.body,0x14,uint32_t(loops.size()));W32(result.body,0x18,loops.empty()?0:base);
    }
    std::map<uint32_t,Streams> modelStreams;
    std::map<uint32_t,AnimationFile> modelFiles;
    for(const auto& file:modelIndex.animationFiles)modelFiles.emplace(uint32_t(file.animation)|(uint32_t(file.variation)<<16),file);
    auto modelTrack=[&](uint32_t at,uint32_t stride){
        if(!Fits(body.size(),at,1,stride?20:12))return false;
        const auto loop=static_cast<int16_t>(U16(body.data()+at+2));const bool global=loop>=0;
        if(U16(body.data()+at)>3 || loop<-1 || (global && uint32_t(loop)>=modelIndex.loops.count))return false;
        if(derived && global){const auto value=uint16_t(loopMap[loop]);std::memcpy(result.body.data()+at+2,&value,2);}
        for(uint32_t pair=4;pair<=(stride?12u:4u);pair+=8){
            const uint32_t count=U32(body.data()+at+pair),offset=U32(body.data()+at+pair+4);
            if(count>4096 || !Fits(body.size(),offset,count,8) || (!global && count>modelIndex.sequences.count))return false;
            const uint32_t outCount=derived && !global && count?index.sequences.count:count;
            uint32_t destinationBase=offset;
            if(derived && !global && count){std::vector<uint8_t> descriptors(size_t(outCount)*8);if(!append(descriptors,destinationBase))return false;
                W32(result.body,at+pair,outCount);W32(result.body,at+pair+4,destinationBase);}
            for(uint32_t i=0;i<outCount;++i){
                uint32_t target=global?i:terminal(i),parentTarget=target;
                const uint32_t destination=destinationBase+i*8;
                if(derived && !global){auto found=modelActions.find(U32(source.data()+index.sequences.offset+target*64));
                    if(found==modelActions.end()){W32(result.body,destination,0);W32(result.body,destination+4,0);continue;}target=modelTerminal(found->second);}
                if(target>=count){W32(result.body,destination,0);W32(result.body,destination+4,0);continue;}
                const auto* leaf=body.data()+offset+target*8;const uint32_t n=U32(leaf),off=U32(leaf+4);
                uint32_t targetBase=0;View range{0,uint32_t(body.size()),true};
                if(n && !global && !(U32(modelSource.data()+modelIndex.sequences.offset+target*64+12)&0x20)){
                    if(derived){auto found=modelStreams.find(target);
                        if(found==modelStreams.end()){auto file=modelFiles.find(U32(modelSource.data()+modelIndex.sequences.offset+target*64));std::vector<uint8_t> bytes;Streams stream;
                            if(!modelRead || file==modelFiles.end() || !modelRead(file->second.animation,file->second.variation,file->second.fileId,bytes) || !StreamsOf(bytes,stream) || !append(bytes,stream.base))return false;
                            found=modelStreams.emplace(target,stream).first;++result.animationFiles;}
                        range=found->second.model;targetBase=found->second.base+range.offset;
                    }else{const auto found=streams.find(target);if(found==streams.end())return false;range=found->second.model;targetBase=found->second.base+range.offset;}
                }
                if(n && (!range.present || !Fits(range.size,off,n,pair==4?4:stride)))return false;
                uint32_t finalOffset=n?targetBase+off:0;
                if(n && derived && !global && pair==4){
                    const auto from=U32(modelSource.data()+modelIndex.sequences.offset+target*64+4),to=U32(source.data()+index.sequences.offset+parentTarget*64+4);
                    std::vector<uint8_t> times(size_t(n)*4);uint32_t previous{};
                    for(uint32_t k=0;k<n;++k){const auto time=U32(result.body.data()+finalOffset+k*4);if((k && time<previous) || time>from || (!from && time))return false;previous=time;
                        const auto scaled=from?uint32_t(uint64_t(time)*to/from):0;std::memcpy(times.data()+k*4,&scaled,4);}
                    if(!append(times,finalOffset))return false;
                }
                W32(result.body,destination,n);W32(result.body,destination+4,finalOffset);
            }
        }
        return true;
    };
    for(const auto& a:d::kHeaderArrays){
        if(a.at==offsetof(f::M2Header,bones) || a.at==offsetof(f::M2Header,attachments) || !a.stepCount)continue;
        uint32_t width=a.stride;
        if(a.at==offsetof(f::M2Header,cameras))width=0x74;
        if(a.at==offsetof(f::M2Header,particleEmitters))width=0x1ec;
        const auto count=U32(body.data()+a.at),offset=U32(body.data()+a.at+4);
        if(!Fits(body.size(),offset,count,width))return fail("model record bounds");
        for(uint32_t i=0;i<count;++i){
            for(uint32_t j=0;j<a.stepCount;++j){
                const auto& step=a.steps[j];if(step.kind!=d::kStepTrack)continue;
                uint32_t at=offset+i*width+step.at;
                if(a.at==offsetof(f::M2Header,cameras))at-=4;
                if(!modelTrack(at,step.stride)){char diagnostic[200]{};std::snprintf(diagnostic,sizeof(diagnostic),"animation model leaf bounds header=0x%x record=%u track=0x%x slots=%u sequences=%u",a.at,i,at,U32(body.data()+at+4),index.sequences.count);return fail(diagnostic);}
            }
            if(a.at==offsetof(f::M2Header,cameras) && !modelTrack(offset+i*width+0x60,12))return fail("animation model FOV bounds");
        }
    }

    const auto seqBase=U32(result.body.data()+0x20);
    for(uint32_t i=0;i<index.sequences.count;++i){
        const auto target=terminal(i);const auto* original=source.data()+index.sequences.offset+target*64;
        // Alias timing must agree with the target whose key arrays were installed.
        if(target!=i){W32(result.body,seqBase+i*64+4,U32(original+4));W32(result.body,seqBase+i*64+8,U32(original+8));}
        W32(result.body,seqBase+i*64+12,(U32(original+12)|0x20u)&~0x40u);
    }
    output=std::move(result);error.clear();return true;
}
bool InheritBindPose(std::span<const uint8_t> child,std::span<const uint8_t> parent,std::vector<uint8_t>& output,std::string& error){
 Index derived,base;if(!Read(child,derived,error) || !Read(parent,base,error))return false;
 if(!derived.parentId || base.parentId || !base.bones.count || derived.bones.count!=base.bones.count){error="parent skeleton identity/count";return false;}
 std::vector<uint8_t> merged(parent.begin(),parent.end());
 for(uint32_t i=0;i<base.bones.count;++i){const auto* from=child.data()+derived.bones.offset+i*88;auto* to=merged.data()+base.bones.offset+i*88;std::memcpy(to,from,16);std::memcpy(to+76,from+76,12);}
 Index checked;if(!Read(merged,checked,error))return false;output=std::move(merged);return true;
}
bool EmbedInline(std::span<const uint8_t> body,std::span<const uint8_t> afid,AnimationReader read,Embedded& output,std::string& error){
 auto fail=[&](const char* why){error=why;return false;};error.clear();
 if(body.size()<312 || body.size()>Limit || std::memcmp(body.data(),"MD20",4) || afid.size()%8 || afid.size()/8>65535)return fail("inline skeleton header");
 const auto seqN=U32(body.data()+0x1c),seqAt=U32(body.data()+0x20),boneN=U32(body.data()+0x2c),boneAt=U32(body.data()+0x30),loops=U32(body.data()+0x14);
 if(!seqN || seqN>4096 || !boneN || boneN>4096 || loops>4096 || !Fits(body.size(),seqAt,seqN,64) || !Fits(body.size(),boneAt,boneN,88) || !Fits(body.size(),U32(body.data()+0x18),loops,4))return fail("inline skeleton arrays");
 std::vector<uint32_t> targets(seqN);
 for(uint32_t i=0;i<seqN;++i){uint32_t at=i;std::vector<bool> seen(seqN);while(U32(body.data()+seqAt+at*64+12)&0x40){if(seen[at])return fail("inline cyclic alias");seen[at]=true;at=U16(body.data()+seqAt+at*64+62);if(at>=seqN)return fail("inline alias target");}targets[i]=at;}
 std::vector<uint8_t> colors(boneN);
 for(uint32_t i=0;i<boneN;++i){int32_t bone=i;std::vector<uint32_t> chain;while(bone>=0){if(uint32_t(bone)>=boneN || colors[bone]==1)return fail("inline bone parent");if(colors[bone]==2)break;colors[bone]=1;chain.push_back(bone);bone=static_cast<int16_t>(U16(body.data()+boneAt+bone*88+8));if(bone<-1)return fail("inline bone parent");}for(auto b:chain)colors[b]=2;}
 std::map<uint32_t,uint32_t> files;for(size_t i=0;i<afid.size();i+=8){if(!files.emplace(U32(afid.data()+i),U32(afid.data()+i+4)).second)return fail("inline duplicate animation key");}
 Embedded result;result.body.assign(body.begin(),body.end());std::map<uint32_t,Streams> streams;
 for(uint32_t i=0;i<seqN;++i){const auto* seq=body.data()+seqAt+i*64;if(U32(seq+12)&0x60){if(U32(seq+12)&0x40)++result.aliases;continue;}auto f=files.find(U32(seq));if(f==files.end() || !read)return fail("inline animation mapping");std::vector<uint8_t> bytes;
  if(!read(U16(seq),U16(seq+2),f->second,bytes) || bytes.empty() || bytes.size()>Limit)return fail("inline animation companion");Streams s;
  const bool container=bytes.size()>=4 && (!std::memcmp(bytes.data(),"AFM2",4) || !std::memcmp(bytes.data(),"AFSB",4) || !std::memcmp(bytes.data(),"AFSA",4));
  if(container){if(!StreamsOf(bytes,s))return fail("inline animation container");}else s.model=s.bone=s.attachment={0,uint32_t(bytes.size()),true};
  const size_t padding=(4-result.body.size()%4)%4;if(result.body.size()+padding+bytes.size()>Limit)return fail("inline assembled limit");result.body.insert(result.body.end(),padding,0);s.base=uint32_t(result.body.size());result.body.insert(result.body.end(),bytes.begin(),bytes.end());streams.emplace(i,s);++result.animationFiles;
 }
 namespace d=wxl::runtime::m2native::detail;namespace f=wxl::structure::m2;
 auto track=[&](uint32_t at,uint32_t stride,uint32_t kind){if(!Fits(body.size(),at,1,stride?20:12))return false;const auto* raw=body.data()+at;const int16_t loop=static_cast<int16_t>(U16(raw+2));if(U16(raw)>3 || loop<-1 || (loop>=0 && uint32_t(loop)>=loops))return false;
  for(uint32_t pair=4;pair<=(stride?12u:4u);pair+=8){const auto count=U32(raw+pair),offset=U32(raw+pair+4);if(count>4096 || (! (loop>=0) && count>seqN) || !Fits(body.size(),offset,count,8))return false;
   for(uint32_t i=0;i<count;++i){const auto target=loop>=0?i:targets[i],dest=offset+i*8;if(target>=count){W32(result.body,dest,0);W32(result.body,dest+4,0);continue;}const auto* leaf=body.data()+offset+target*8;const auto n=U32(leaf),off=U32(leaf+4);uint32_t base=0;View range{0,uint32_t(body.size()),true};
    if(loop<0 && !(U32(body.data()+seqAt+target*64+12)&0x20)){auto found=streams.find(target);if(found==streams.end())return false;const auto& s=found->second;range=kind==1?s.bone:kind==2?s.attachment:s.model;base=s.base+range.offset;}
    if(n && (!range.present || !Fits(range.size,off,n,pair==4?4:stride)))return false;W32(result.body,dest,n);W32(result.body,dest+4,n?base+off:0);
   }
  }return true;
 };
 for(const auto& a:d::kHeaderArrays){if(!a.stepCount)continue;uint32_t width=a.stride;if(a.at==offsetof(f::M2Header,cameras))width=0x74;if(a.at==offsetof(f::M2Header,particleEmitters))width=0x1ec;const auto n=U32(body.data()+a.at),off=U32(body.data()+a.at+4);if(n>65535 || !Fits(body.size(),off,n,width))return fail("inline record bounds");
  const uint32_t kind=a.at==offsetof(f::M2Header,bones)?1:a.at==offsetof(f::M2Header,attachments)?2:0;
  for(uint32_t i=0;i<n;++i){for(uint32_t j=0;j<a.stepCount;++j){const auto& step=a.steps[j];if(step.kind!=d::kStepTrack)continue;auto at=off+i*width+step.at;if(a.at==offsetof(f::M2Header,cameras))at-=4;if(!track(at,step.stride,kind))return fail("inline track leaf bounds");}if(a.at==offsetof(f::M2Header,cameras) && !track(off+i*width+0x60,12,0))return fail("inline FOV bounds");}
 }
 for(uint32_t i=0;i<seqN;++i){const auto* original=body.data()+seqAt+targets[i]*64;if(targets[i]!=i){W32(result.body,seqAt+i*64+4,U32(original+4));W32(result.body,seqAt+i*64+8,U32(original+8));}W32(result.body,seqAt+i*64+12,(U32(original+12)|0x20)&~0x40);}
 output=std::move(result);return true;
}

}
