// Modern ItemVisuals/ItemVisualsXEffect are a distinct graph, never native DBC rows.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ItemVisualGraph.hpp"
#include "PositionerGraph.hpp"
#include "WXL/ProceduralPayload.hpp"
#include "RetailSchemas.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/FdidCompat.hpp"
#include "WXL/NativeFeatures.hpp"
#include <algorithm>
#include <atomic>
#include <unordered_set>
#include <bit>
#include <cmath>
#include <memory>
#include <unordered_map>
namespace wxl::native {
namespace {
std::atomic<uint64_t> visualGeneration{0};
using Catalog=std::unordered_map<uint32_t,ItemGraphVisual>;
std::shared_ptr<const Catalog>& Slot(){static auto* slot=new std::shared_ptr<const Catalog>;return *slot;}
std::string ModelPath(uint32_t id){const auto* path=id?ResolveModel(id):nullptr;return path?path:"";}
namespace db=wxl::runtime::db2;namespace schema=db::retail;
float Float(const db::Table& table,const db::wdc5::Row& row,std::string_view name,size_t index=0){return std::bit_cast<float>(table.Value(row,name,index));}
bool Filter(const db::Definition& definition,db::wdc5::SnapshotFilterSource source,const std::vector<uint32_t>& ids,db::Table& output){
 if(ids.empty()||ids.size()>4096)return false;db::wdc5::SnapshotFilter filter;filter.source=source;filter.values=ids;
 return output.LoadFiltered(definition,filter)&&output.Rows().size()<=65536;
}
void Unique(std::vector<uint32_t>& ids){std::sort(ids.begin(),ids.end());ids.erase(std::unique(ids.begin(),ids.end()),ids.end());}
// Same source relationships as RetailSpellCatalog: direct parent-kit attaches,
// plus KitEffect type2 -> ModelAttach row ID -> EffectName model FDID.
// Keep kit/attach pair identity: a shared attach row may belong to several kits.
void ResolveKits(Catalog& catalog){
 std::vector<uint32_t> kits;
 for(const auto& [id,visual]:catalog){if(visual.spellVisualKitId1)kits.push_back(visual.spellVisualKitId1);if(visual.spellVisualKitId2)kits.push_back(visual.spellVisualKitId2);for(const auto& effect:visual.effects)if(effect.spellVisualKitId)kits.push_back(effect.spellVisualKitId);}
 Unique(kits);if(kits.empty())return;
 if(kits.size()>4096){for(auto& [id,visual]:catalog)++visual.unsupportedKitRows;return;}
 struct Attach {uint32_t kit;db::wdc5::Row row;};
 std::vector<Attach> attachments;std::unordered_set<uint64_t> seen;
 db::Table direct,links,indirect,names;
 auto add=[&](uint32_t kit,const db::wdc5::Row& row){if(attachments.size()<65536&&seen.emplace((uint64_t(kit)<<32)|row.id).second)attachments.push_back({kit,row});};
 if(Filter(schema::SpellVisualKitModelAttach,db::wdc5::SnapshotFilterSource::ParentId,kits,direct))for(const auto& row:direct.Rows())add(row.parentId,row);
 std::unordered_map<uint32_t,std::vector<uint32_t>> indirectKits;
 std::unordered_map<uint32_t,uint32_t> unsupported;
 struct ProcedureLink{uint32_t kit,link;};
 std::unordered_map<uint32_t,std::vector<ProcedureLink>> procedureOwners;
 std::unordered_map<uint32_t,std::vector<ItemGraphProcedural>> proceduresByKit;
 if(Filter(schema::SpellVisualKitEffect,db::wdc5::SnapshotFilterSource::ParentId,kits,links)){
  for(const auto& row:links.Rows()){
   if(links.Value(row,"EffectType")==2&&links.Value(row,"Effect"))indirectKits[links.Value(row,"Effect")].push_back(row.parentId);
   else if(links.Value(row,"EffectType")==1&&links.Value(row,"Effect"))procedureOwners[links.Value(row,"Effect")].push_back({row.parentId,row.id});
   else ++unsupported[row.parentId];
  }
 }
 std::vector<uint32_t> procedureIds;for(const auto& [id,owners]:procedureOwners)procedureIds.push_back(id);Unique(procedureIds);
 db::Table procedures;
 const bool proceduresLoaded=!procedureIds.empty()&&Filter(schema::SpellProceduralEffect,db::wdc5::SnapshotFilterSource::RowId,procedureIds,procedures);
 for(const auto& [id,owners]:procedureOwners)for(const auto& owner:owners){
  const auto* row=proceduresLoaded?procedures.Find(id):nullptr;if(!row){++unsupported[owner.kit];continue;}
  ItemGraphProcedural effect;effect.kitId=owner.kit;effect.kitEffectId=owner.link;effect.procedureId=id;effect.type=procedures.Value(*row,"Type");
  for(uint32_t value=0;value<4;++value)effect.rawValues[value]=procedures.Value(*row,"Value",value);
  std::array<float,4> payload;effect.compatible=DecodeProceduralPayload(effect,payload);
  if(!effect.compatible)++unsupported[owner.kit];
  if(proceduresByKit[owner.kit].size()<256)proceduresByKit[owner.kit].push_back(effect);else ++unsupported[owner.kit];
 }
 // Stable KitEffect row order for multiple writes to the same field.
 for(auto& [kit,rows]:proceduresByKit)std::stable_sort(rows.begin(),rows.end(),[](const auto& a,const auto& b){return a.kitEffectId<b.kitEffectId;});
 std::vector<uint32_t> indirectIds;for(const auto& [id,owners]:indirectKits)indirectIds.push_back(id);Unique(indirectIds);
 if(!indirectIds.empty()&&Filter(schema::SpellVisualKitModelAttach,db::wdc5::SnapshotFilterSource::RowId,indirectIds,indirect))for(const auto& row:indirect.Rows())for(auto kit:indirectKits[row.id])add(kit,row);
 std::vector<uint32_t> effectIds;
 // All ModelAttach instances use the same declared physical schema. A table
 // with that schema must have loaded before its field-name bindings are used.
 const auto& attachTable=direct.Rows().empty()?indirect:direct;
 for(const auto& attach:attachments){auto id=attachTable.Value(attach.row,"SpellVisualEffectNameID");if(id)effectIds.push_back(id);}
 Unique(effectIds);
 if(!effectIds.empty()&&!Filter(schema::SpellVisualEffectName,db::wdc5::SnapshotFilterSource::RowId,effectIds,names))return;
 std::vector<uint32_t> positionerIds;
 for(const auto& attach:attachments){auto id=attachTable.Value(attach.row,"PositionerID");if(id)positionerIds.push_back(id);}Unique(positionerIds);
 auto positioners=LoadItemPositioners(positionerIds);
 std::unordered_map<uint32_t,std::vector<ItemGraphKitModel>> byKit;
 for(const auto& attach:attachments){
  const auto& row=attach.row;const auto effectId=attachTable.Value(row,"SpellVisualEffectNameID");const auto* effect=names.Find(effectId);
  auto refuse=[&]{++unsupported[attach.kit];};
  if(!effect){refuse();continue;}
  // Recovered native Euler/sequence ownership admits static transforms and
  // start/loop/end timing. Positioners/random variation/unknown flags stay refused.
  bool supported=!attachTable.Value(row,"Flags")&&!attachTable.Value(row,"AnimKitID");
  const auto positionerId=attachTable.Value(row,"PositionerID");const auto positioned=positioners.find(positionerId);
  supported&=!positionerId||(positioned!=positioners.end()&&positioned->second->staticCompatible);
  for(auto field:{"YawVariation","PitchVariation","RollVariation","ScaleVariation"})supported&=Float(attachTable,row,field)==0;
  for(unsigned axis=0;axis<3;++axis)supported&=Float(attachTable,row,"OffsetVariation",axis)==0;
  // Effect-name substitutions, procedural types, positioners and dissolve
  // pipelines are outside a model-only equipment owner.
  supported&=!names.Value(*effect,"TextureFileDataID")&&!names.Value(*effect,"DissolveEffectID")&&!names.Value(*effect,"Flags")&&!names.Value(*effect,"ModelPosition");
  ItemGraphKitModel model;if(positionerId&&positioned!=positioners.end())model.positioner=positioned->second;model.kitId=attach.kit;model.modelAttachId=row.id;model.fileDataId=names.Value(*effect,"ModelFileDataID");model.attachmentId=attachTable.Value(row,"AttachmentID");model.animationId=attachTable.Value(row,"AnimID");
  auto animation=[](uint32_t value)->int32_t{return value==0xffffu||value==0xffffffffu?-1:int32_t(value);};
  model.startAnimationId=animation(attachTable.Value(row,"StartAnimID"));model.endAnimationId=animation(attachTable.Value(row,"EndAnimID"));
  supported&=model.startAnimationId>=-1&&model.endAnimationId>=-1;
  model.startDelay=Float(attachTable,row,"StartDelay");model.sourceFlags=attachTable.Value(row,"Flags");
  model.yaw=Float(attachTable,row,"Yaw");model.pitch=Float(attachTable,row,"Pitch");model.roll=Float(attachTable,row,"Roll");
  supported&=std::isfinite(model.startDelay)&&model.startDelay>=0&&model.startDelay<=float(INT32_MAX)/1000.0f;
  supported&=std::isfinite(model.yaw)&&std::isfinite(model.pitch)&&std::isfinite(model.roll);
  float first=Float(attachTable,row,"Scale"),second=Float(names,*effect,"Scale");
  // Preserve donor graph's default scale treatment before multiplying rows.
  if(!std::isfinite(first)||first<=0)first=1;if(!std::isfinite(second)||second<=0)second=1;
  model.scale=first*second;model.alpha=Float(names,*effect,"Alpha");
  supported&=model.attachmentId<=255&&model.fileDataId&&std::isfinite(model.scale)&&model.scale>0&&std::isfinite(model.alpha)&&model.alpha>=0&&model.alpha<=1;
  for(unsigned axis=0;axis<3;++axis){model.offset[axis]=Float(attachTable,row,"Offset",axis);supported&=std::isfinite(model.offset[axis]);}
  if(!supported){refuse();continue;}model.path=ModelPath(model.fileDataId);if(model.path.empty()){refuse();continue;}
  if(model.animationId==0xffffu||model.animationId==0xffffffffu)model.animationId=0;
  if(byKit[attach.kit].size()<256)byKit[attach.kit].push_back(std::move(model));else refuse();
 }
 for(auto& [id,visual]:catalog){
  auto append=[&](uint32_t kit,uint32_t selector,uint32_t source){if(!kit)return;visual.unsupportedKitRows+=unsupported[kit];
   if(!byKit.contains(kit)&&!proceduresByKit.contains(kit)&&!unsupported[kit])++visual.unsupportedKitRows;
   if(const auto rows=proceduresByKit.find(kit);rows!=proceduresByKit.end())for(auto effect:rows->second){if(visual.procedurals.size()>=256){++visual.unsupportedKitRows;break;}effect.displayWeaponSubclassId=selector;effect.sourceEffectId=source;visual.procedurals.push_back(effect);}
   if(const auto rows=byKit.find(kit);rows!=byKit.end())for(auto model:rows->second){if(visual.kitModels.size()>=256){++visual.unsupportedKitRows;break;}model.displayWeaponSubclassId=selector;model.sourceEffectId=source;visual.kitModels.push_back(std::move(model));}
  };
  append(visual.spellVisualKitId1,UINT32_MAX,0);if(visual.spellVisualKitId2!=visual.spellVisualKitId1)append(visual.spellVisualKitId2,UINT32_MAX,0);
  for(const auto& effect:visual.effects)append(effect.spellVisualKitId,effect.displayWeaponSubclassId,effect.id);
 }
}
}
void LoadItemVisualGraph() noexcept {try{
 namespace db=wxl::runtime::db2;namespace schema=db::retail;
 db::Table visuals,effects;if(!visuals.Load(schema::ItemVisuals)||visuals.Rows().size()>65536)return;
 auto next=std::make_shared<Catalog>();next->reserve(visuals.Rows().size());
 for(const auto& row:visuals.Rows()){
  ItemGraphVisual visual;visual.id=row.id;
  visual.spellVisualKitId1=visuals.Value(row,"SpellVisualKitId1");
  visual.spellVisualKitId2=visuals.Value(row,"SpellVisualKitId2");
  for(uint32_t point=0;point<5;++point){const auto id=visuals.Value(row,"ModelFileID",point);if(!id)continue;
   ItemGraphVisualModel model;model.attachmentId=point;model.fileDataId=id;model.path=ModelPath(id);
   if(!model.path.empty())visual.models.push_back(std::move(model));
  }
  next->emplace(row.id,std::move(visual));
 }
 if(effects.Load(schema::ItemVisualsXEffect)&&effects.Rows().size()<=65536){
  for(const auto& row:effects.Rows()){
   const auto parent=next->find(effects.RelationKey(row,"visual"));if(parent==next->end()||parent->second.effects.size()>=256)continue;
   ItemGraphVisualEffect effect;effect.id=row.id;effect.attachmentId=effects.Value(row,"AttachmentID");
   effect.displayWeaponSubclassId=effects.Value(row,"DisplayWeaponSubclassID");effect.spellVisualKitId=effects.Value(row,"SpellVisualKitID");
   effect.fileDataId=effects.Value(row,"AttachmentModelFileID");effect.scale=std::bit_cast<float>(effects.Value(row,"Scale"));effect.path=ModelPath(effect.fileDataId);
   parent->second.effects.push_back(std::move(effect));
  }
 }
 try{ResolveKits(*next);}catch(...){
  // Optional kit failure must not discard the already resolved direct models.
  for(auto& [id,visual]:*next){visual.kitModels.clear();visual.procedurals.clear();++visual.unsupportedKitRows;}
 }
 std::atomic_store_explicit(&Slot(),std::shared_ptr<const Catalog>(std::move(next)),std::memory_order_release);
 visualGeneration.fetch_add(1,std::memory_order_release);
 ItemGraphPublished();
 }catch(...){}}
uint64_t ItemGraphVisualGeneration() noexcept {return visualGeneration.load(std::memory_order_acquire);}
void ClearItemVisualGraph() noexcept {visualGeneration.fetch_add(1,std::memory_order_release);std::atomic_store_explicit(&Slot(),std::shared_ptr<const Catalog>{},std::memory_order_release);}
bool ItemGraphVisualData(uint32_t id,ItemGraphVisual& output) noexcept {
 output={};if(!id||!Db2Enabled())return false;
 try{auto current=std::atomic_load_explicit(&Slot(),std::memory_order_acquire);if(!current)return false;
 const auto found=current->find(id);if(found==current->end())return false;output=found->second;return true;
 }catch(...){output={};return false;}
}
}
