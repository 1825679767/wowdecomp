// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "PositionerGraph.hpp"
#include "RetailSchemas.hpp"
#include <bit>
#include <cmath>
#include <unordered_set>
namespace wxl::native {
namespace {
namespace db=wxl::runtime::db2;namespace schema=db::retail;
float F(const db::Table& t,const db::wdc5::Row& r,std::string_view n,uint32_t i=0){return std::bit_cast<float>(t.Value(r,n,i));}
}
std::unordered_map<uint32_t,std::shared_ptr<const ItemGraphPositioner>> LoadItemPositioners(const std::vector<uint32_t>& ids){
 std::unordered_map<uint32_t,std::shared_ptr<const ItemGraphPositioner>> result;if(ids.empty()||ids.size()>4096)return result;
 db::wdc5::SnapshotFilter filter;filter.source=db::wdc5::SnapshotFilterSource::RowId;filter.values=ids;
 db::Table roots,states,entries,matrices;
 if(!roots.LoadFiltered(schema::Positioner,filter)||!states.Load(schema::PositionerState)||!entries.Load(schema::PositionerStateEntry)||!matrices.Load(schema::TransformMatrix))return result;
 for(const auto* t:{&roots,&states,&entries,&matrices})if(t->Rows().size()>65536)return result;
 for(const auto& row:roots.Rows()){
  auto graph=std::make_shared<ItemGraphPositioner>();graph->id=row.id;graph->flags=roots.Value(row,"Flags");graph->startLife=F(roots,row,"StartLife");graph->startPercent=roots.Value(row,"StartLifePercent");
  bool valid=std::isfinite(graph->startLife);std::unordered_set<uint32_t> visited;auto id=roots.Value(row,"FirstStateID");
  while(id&&valid){
   if(graph->states.size()>=32||!visited.insert(id).second){valid=false;break;}
   const auto* source=states.Find(id);if(!source){valid=false;break;}ItemGraphPositionerState state;state.id=id;
   state.nextState=states.Value(*source,"NextStateID");state.flags=states.Value(*source,"Flags");state.endLife=F(states,*source,"EndLife");state.endPercent=states.Value(*source,"EndLifePercent");state.transformId=states.Value(*source,"TransformMatrixID");valid&=std::isfinite(state.endLife);
   if(state.transformId){const auto* transform=matrices.Find(state.transformId);if(!transform){valid=false;break;}
    for(uint32_t i=0;i<3;++i){state.position[i]=F(matrices,*transform,"Pos",i);valid&=std::isfinite(state.position[i]);}
    state.yaw=F(matrices,*transform,"Yaw");state.pitch=F(matrices,*transform,"Pitch");state.roll=F(matrices,*transform,"Roll");state.scale=F(matrices,*transform,"Scale");
    valid&=std::isfinite(state.yaw)&&std::isfinite(state.pitch)&&std::isfinite(state.roll)&&std::isfinite(state.scale)&&state.scale>0;
   }
   uint32_t axis=0;for(auto name:{"PosEntryID","RotEntryID","ScaleEntryID"}){
    const auto entryId=states.Value(*source,name);state.entryIds[axis++]=entryId;if(!entryId)continue;const auto* e=entries.Find(entryId);if(!e){valid=false;break;}
    ItemGraphPositionerEntry entry;entry.id=entryId;entry.paramA=F(entries,*e,"ParamA");entry.paramB=F(entries,*e,"ParamB");entry.curveId=entries.Value(*e,"CurveID");
    auto signed16=[](uint32_t value){return int32_t(int16_t(value));};
    entry.sourceValueType=signed16(entries.Value(*e,"SrcValType"));entry.sourceValue=signed16(entries.Value(*e,"SrcVal"));entry.destinationValueType=signed16(entries.Value(*e,"DstValType"));entry.destinationValue=signed16(entries.Value(*e,"DstVal"));
    entry.entryType=entries.Value(*e,"EntryType");entry.style=entries.Value(*e,"Style");entry.sourceType=entries.Value(*e,"SrcType");entry.destinationType=entries.Value(*e,"DstType");valid&=std::isfinite(entry.paramA)&&std::isfinite(entry.paramB);state.entries.push_back(entry);
   }
   id=state.nextState;graph->states.push_back(std::move(state));
  }
  if(!valid)continue;
  // No inferred entry enums or time units: only an untimed terminal transform
  // can use the existing native attachment-matrix compatibility policy.
  graph->staticCompatible=graph->flags==0&&graph->startLife==0&&graph->startPercent==0&&graph->states.size()<=1;
  for(const auto& s:graph->states)graph->staticCompatible&=!s.nextState&&!s.flags&&s.endLife==0&&!s.endPercent&&s.entries.empty();
  result.emplace(graph->id,std::move(graph));
 }
 return result;
}
}
