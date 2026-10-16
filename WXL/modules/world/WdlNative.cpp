// Modern distant-terrain byte adapter. Native WDL geometry/render lifetime stays owner.
// Retains true MARE heights and MAHO holes; modern distant placement semantics remain a donor TODO.
#include "WorldCompat.hpp"
#include "WorldBytes.hpp"
#include "NativeFeatures.hpp"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "Engine/Source/Storm/H/storm.h"
#include "common/Log.hpp"
#include <map>
namespace wxl::native::world {
bool PrepareWdl(CMapLowDetail& object,uint32_t& size) noexcept {try{
 if(!SplitAdtEnabled())return true;if(!object.m_data||size<20||size>64u*1024u*1024u)return false;
 using namespace bytes;const auto* input=reinterpret_cast<const uint8_t*>(object.m_data);if(Read(input)!=Tag("MVER")||Read(input+4)!=4)return false;
 if((Read(input+12)>>16)!=((uint32_t('M')<<8)|uint32_t('L')))return true;
 Blob source(input,input+size);std::vector<Part> parts;if(!Walk(source,0,size,parts))return false;
 bool modern=false;for(const auto& part:parts)if((part.tag>>16)==((uint32_t('M')<<8)|uint32_t('L'))||part.tag==Tag("MAOE"))modern=true;if(!modern)return true;
 std::map<size_t,size_t> positions;const Part* offsets=nullptr;for(size_t i=0;i<parts.size();++i){const auto& p=parts[i];positions.emplace(p.at-8,i);if(p.tag==Tag("MAOF")){if(offsets||p.size!=4096*4)return false;offsets=&p;}}
 if(!offsets)return false;Blob result;Append(result,Tag("MVER"),source.data()+8,4);Blob table(4096*4);const auto tableAt=Append(result,"MAOF",table);unsigned tiles=0,invalid=0;
 for(unsigned slot=0;slot<4096;++slot){const auto offset=Read(source.data()+offsets->at+slot*4);if(!offset)continue;auto it=positions.find(offset);if(it==positions.end()){++invalid;continue;}const auto& height=parts[it->second];if(height.tag!=Tag("MARE")||height.size<545*2){++invalid;continue;}
  Blob holes(32);for(size_t next=it->second+1,hops=0;next<parts.size()&&hops<3;++next,++hops){const auto& child=parts[next];if(child.tag==Tag("MARE"))break;if(child.tag==Tag("MAHO")){if(child.size<32){++invalid;}else std::memcpy(holes.data(),source.data()+child.at,32);break;}}
  const auto shapedAt=Append(result,Tag("MARE"),source.data()+height.at,height.size);Append(result,"MAHO",holes);Write(result.data()+tableAt+8+slot*4,uint32_t(shapedAt));++tiles;if(result.size()>64u*1024u*1024u)return false;
 }
 // Preserve auxiliary serialized metadata without treating new placement structures as stock pointers.
 for(const auto& p:parts)if((p.tag>>16)==((uint32_t('M')<<8)|uint32_t('L'))||p.tag==Tag("MAOE"))Append(result,p.tag,source.data()+p.at,p.size);
 auto* data=static_cast<char*>(SMemAlloc(result.size(),__FILE__,__LINE__,0));if(!data)return false;std::memcpy(data,result.data(),result.size());SMemFree(object.m_data,__FILE__,__LINE__,0);object.m_data=data;size=uint32_t(result.size());WLOG_INFO("native-wdl: modern distant terrain tiles=%u invalid=%u bytes=%u",tiles,invalid,size);return true;
 }catch(...){return false;}}
}
