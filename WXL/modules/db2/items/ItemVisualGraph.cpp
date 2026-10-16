// Modern ItemVisuals/ItemVisualsXEffect are a distinct graph, never native DBC rows.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ItemVisualGraph.hpp"
#include "RetailSchemas.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/FdidCompat.hpp"
#include "WXL/NativeFeatures.hpp"
#include <atomic>
#include <bit>
#include <cmath>
#include <memory>
#include <unordered_map>
namespace wxl::native {
namespace {
using Catalog=std::unordered_map<uint32_t,ItemGraphVisual>;
std::shared_ptr<const Catalog>& Slot(){static auto* slot=new std::shared_ptr<const Catalog>;return *slot;}
std::string ModelPath(uint32_t id){const auto* path=id?ResolveModel(id):nullptr;return path?path:"";}
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
 std::atomic_store_explicit(&Slot(),std::shared_ptr<const Catalog>(std::move(next)),std::memory_order_release);
 ItemGraphPublished();
 }catch(...){}}
void ClearItemVisualGraph() noexcept {std::atomic_store_explicit(&Slot(),std::shared_ptr<const Catalog>{},std::memory_order_release);}
bool ItemGraphVisualData(uint32_t id,ItemGraphVisual& output) noexcept {
 output={};if(!id||!Db2Enabled())return false;
 try{auto current=std::atomic_load_explicit(&Slot(),std::memory_order_acquire);if(!current)return false;
 const auto found=current->find(id);if(found==current->end())return false;output=found->second;return true;
 }catch(...){output={};return false;}
}
}
