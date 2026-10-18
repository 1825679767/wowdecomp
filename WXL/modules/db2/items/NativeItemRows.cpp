// Native typed, DBC-first retail item bridge. No packed x86 row aliases.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "ItemGraph.hpp"
#include "NativeFeatures.hpp"
#include "RetailItemCatalog.hpp"
#include "ItemDisplayIndex.hpp"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include <algorithm>
#include <iterator>
#include <atomic>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>
namespace wxl::native {
namespace {
namespace display=wxl::runtime::db2::itemdisplay;
namespace items=wxl::runtime::db2::retailitems;
std::atomic<uint64_t> generation{0};
// Native row callers can retain their char pointers after a graph snapshot retires.
// Retain only requested rows, bounded at 65536; node addresses never move.
struct StableRows {
 std::mutex mutex;
 std::map<int32_t,ItemRec> items;
 std::map<int32_t,ItemDisplayInfoRec> displays;
 std::set<std::string> strings;
 const char* Intern(const char* value) {return value&&*value?strings.emplace(value).first->c_str():"";}
};
StableRows& Rows(){static auto* rows=new StableRows;return *rows;}
struct Replay {uint64_t observed=0;size_t cursor=0;std::vector<WOWGUID> guids;};
Replay& Replays(){static auto* replay=new Replay;return *replay;}
int32_t Collect(WOWGUID guid,void* param) {
 auto& replay=*static_cast<Replay*>(param);
 if(replay.guids.size()>=4096)return 0;
 if(ClntObjMgrObjectPtr(guid,TYPEMASK_PLAYER,__FILE__,__LINE__)) replay.guids.push_back(guid);
 return 1;
}
const char* Text(const char* value){return value?value:"";}
}
void ItemGraphPublished() noexcept {generation.fetch_add(1,std::memory_order_release);}
uint64_t ItemGraphGeneration() noexcept {return generation.load(std::memory_order_acquire);}
bool ItemGraphOwnsDisplay(uint32_t id) noexcept {
 if(!Db2Enabled()||!id||id>INT32_MAX)return false;
 const auto& db=g_itemDisplayInfoDB;
 return !db.m_recordsById || int32_t(id)<db.m_minID || int32_t(id)>db.m_maxID || !db.m_recordsById[int32_t(id)-db.m_minID];
}
ItemRec* MissingItemRecord(int32_t id) noexcept {
 if(id<=0||!Db2Enabled())return nullptr;
 try {
  auto catalog=items::Current();if(!catalog)return nullptr;
  auto found=catalog->items.find(uint32_t(id));if(found==catalog->items.end())return nullptr;
  auto& store=Rows();std::lock_guard lock(store.mutex);
  auto old=store.items.find(id);if(old!=store.items.end())return &old->second;
  if(store.items.size()>=65536)return nullptr;
  const auto& item=found->second;auto& row=store.items[id];
  row.m_id=id;row.m_classID=int32_t(item.classId);row.m_subclassID=int32_t(item.subclassId);
  row.m_soundOverrideSubclassID=int32_t(item.soundOverride);row.m_material=int32_t(item.material);
  row.m_displayInfoID=item.displayId<=INT32_MAX?int32_t(item.displayId):0;
  row.m_inventoryType=int32_t(item.inventoryType);row.m_sheatheType=int32_t(item.sheatheType);
  return &row;
 }catch(...){return nullptr;}
}
bool MissingItemDisplayRecord(int32_t id,ItemDisplayInfoRec* output) noexcept {
 if(id<=0||!output||!ItemGraphOwnsDisplay(uint32_t(id)))return false;
 try {
  auto index=display::Current();
  if(!index||!index->resolvedDisplays.contains(uint32_t(id))) {display::Request(uint32_t(id));return false;}
  auto found=index->displayRecords.find(uint32_t(id));if(found==index->displayRecords.end())return false;
  auto& store=Rows();std::lock_guard lock(store.mutex);
  auto old=store.displays.find(id);if(old!=store.displays.end()){*output=old->second;return true;}
  if(store.displays.size()>=65536)return false;
  ItemDisplayInfoRec row;const auto& source=found->second;row.m_id=id;
  for(unsigned i=0;i<2;++i){row.m_modelName[i]=store.Intern(source.nativeModelNames[i]);row.m_modelTexture[i]=store.Intern(source.nativeModelTextures[i]);row.m_inventoryIcon[i]="";row.m_helmetGeosetVisID[i]=int32_t(source.helmetVis[i]);}
  for(unsigned i=0;i<3;++i)row.m_geosetGroup[i]=int32_t(source.geosets[i]);
  row.m_flags=int32_t(source.flags);row.m_spellVisualID=0;row.m_groupSoundIndex=0;
  if(auto catalog=items::Current())if(auto sound=catalog->soundByDisplay.find(uint32_t(id));sound!=catalog->soundByDisplay.end())row.m_groupSoundIndex=int32_t(sound->second);
  // Modern visual/particle-color IDs require their own table conversion; never
  // interpret them as legacy native IDs merely because the integer fits.
  row.m_itemVisual=0;row.m_particleColorID=0;
  for(unsigned layer=0;layer<8;++layer){
   const char* texture=source.componentTextures[layer];bool ambiguous=false;
   if(auto materials=index->materials.find(uint32_t(id));materials!=index->materials.end())
    for(const auto& material:materials->second)if(material.layer==layer && (material.raceId || material.genderId!=UINT32_MAX))ambiguous=true;
   row.m_texture[layer]=ambiguous?"":store.Intern(texture);
  }
  auto inserted=store.displays.emplace(id,row);*output=inserted.first->second;return true;
 }catch(...){return false;}
}
bool ItemGraphHelmetData(uint32_t id,uint32_t sex,uint32_t modelRace,ItemGraphHelmet& output) noexcept {
 output={};if(!Db2Enabled()||!id||id>INT32_MAX||sex>1)return false;
 try {auto index=display::Current();if(!index||!index->helmetData)return false;
  ItemGraphHelmet result;
  if(!ItemGraphOwnsDisplay(id)){
   // Donor applies retail hide-group rules to a native DBC helmet as well,
   // while its own DBC visibility ID (even zero) remains authoritative.
   ItemDisplayInfoRec native;
   if(!g_itemDisplayInfoDB.GetRecord(int32_t(id),&native))return false;
   const auto visibility=native.m_helmetGeosetVisID[sex];
   if(visibility<0)return false;
   result.visibilityId=uint32_t(visibility);
  }else{
   if(!index->resolvedDisplays.contains(id)){display::Request(id);return false;}
   const auto row=index->displayRecords.find(id);if(row==index->displayRecords.end())return false;
   result.visibilityId=row->second.helmetVis[sex];
  }
  if(result.visibilityId){
   if(const auto rules=index->helmetData->geosetsByVis.find(result.visibilityId);rules!=index->helmetData->geosetsByVis.end()){
    if(rules->second.size()>256)return false;result.rules.reserve(rules->second.size());
    for(const auto& rule:rules->second)result.rules.push_back({rule.raceId,rule.hideGroup,rule.raceBitSelection,rule.flags});
   }
   const uint64_t key=(uint64_t(result.visibilityId)<<32)|modelRace;
   if(const auto scale=index->helmetData->animScaleByVisRace.find(key);scale!=index->helmetData->animScaleByVisRace.end()){result.hasAnimScale=true;result.animScale=scale->second;}
  }
  output=std::move(result);return true;
 }catch(...){return false;}
}
bool ItemGraphWeaponSubclass(uint32_t id,uint32_t& subclass) noexcept {
 subclass=UINT32_MAX;if(!ItemGraphOwnsDisplay(id))return false;
 try {auto catalog=items::Current();if(!catalog)return false;
 const auto found=catalog->weaponSubclassByDisplay.find(id);
 if(found==catalog->weaponSubclassByDisplay.end()||found->second==UINT32_MAX)return false;
 subclass=found->second;return true;
 }catch(...){return false;}
}
bool ItemGraphDisplayVisual(uint32_t id,ItemGraphVisual& output) noexcept {
 output={};if(!ItemGraphOwnsDisplay(id))return false;
 try {auto index=display::Current();if(!index||!index->resolvedDisplays.contains(id)){display::Request(id);return false;}
 const auto found=index->displayRecords.find(id);if(found==index->displayRecords.end())return false;
 if(!found->second.itemVisual)return true;
 return ItemGraphVisualData(found->second.itemVisual,output);
 }catch(...){output={};return false;}
}
bool ItemGraphInventoryType(uint32_t id,uint32_t& inventoryType) noexcept {
 inventoryType=0;if(!ItemGraphOwnsDisplay(id))return false;
 try {auto index=display::Current();if(!index||!index->resolvedDisplays.contains(id)){display::Request(id);return false;}
 auto found=index->displayRecords.find(id);if(found==index->displayRecords.end())return false;
 inventoryType=found->second.inventoryType;return true;
 }catch(...){return false;}
}
bool ItemGraphModels(uint32_t id,std::vector<ItemGraphModel>& output) noexcept {
 output.clear();if(!ItemGraphOwnsDisplay(id))return false;
 try {auto index=display::Current();if(!index||!index->resolvedDisplays.contains(id)){display::Request(id);return false;}
  auto found=index->models.find(id);if(found==index->models.end())return true;if(found->second.size()>256)return false;
  std::vector<ItemGraphModel> result;result.reserve(found->second.size());
  for(const auto& value:found->second){ItemGraphModel row;row.modelSlot=value.modelSlot;row.attachId=value.attachId;row.modelIndex=value.modelIndex;row.raceId=value.raceId;row.genderId=value.genderId;row.modelFlags=value.modelFlags;row.textureFlags=value.textureFlags;row.folder=Text(value.folder);row.model=Text(value.model);row.texture=Text(value.texture);row.geoFilter.count=std::min(value.geoFilter.count,16u);std::copy_n(value.geoFilter.ids,row.geoFilter.count,row.geoFilter.ids);result.push_back(std::move(row));}
  output.swap(result);return true;
 }catch(...){return false;}
}
bool ItemGraphMaterials(uint32_t id,std::vector<ItemGraphMaterial>& output) noexcept {
 output.clear();if(!ItemGraphOwnsDisplay(id))return false;
 try {auto index=display::Current();if(!index||!index->resolvedDisplays.contains(id)){display::Request(id);return false;}
  auto found=index->materials.find(id);if(found==index->materials.end())return true;if(found->second.size()>256)return false;
  std::vector<ItemGraphMaterial> result;result.reserve(found->second.size());
  for(const auto& value:found->second){ItemGraphMaterial row;row.modelIndex=value.modelIndex;row.modelColumn=value.modelColumn;row.layer=value.layer;row.textureType=value.textureType;row.raceId=value.raceId;row.genderId=value.genderId;row.folder=Text(value.folder);row.model=Text(value.model);row.texture=Text(value.texture);row.texturePath=Text(value.texturePath);row.skinSectionIds=Text(value.skinSectionIds);row.batchIndexes=Text(value.batchIndexes);row.targetSkinSectionIds=Text(value.targetSkinSectionIds);row.targetBatchIndexes=Text(value.targetBatchIndexes);row.targetMode=Text(value.targetMode);result.push_back(std::move(row));}
  output.swap(result);return true;
 }catch(...){return false;}
}
void RetryItemGraph(CCharacterComponent& component) noexcept {
 const auto current=ItemGraphGeneration();if(component.m_wxlItemGeneration==current)return;
 component.m_wxlItemGeneration=current;
 for(unsigned slot=0;slot<12;++slot){auto id=component.m_wxlPendingItemDisplay[slot];if(!id)continue;
  ItemDisplayInfoRec row;if(g_itemDisplayInfoDB.GetRecord(int32_t(id),&row))component.AddItem(static_cast<ITEM_SLOT>(slot),&row,component.m_wxlPendingItemVisual[slot]);
  else {
   // A completed negative lookup is terminal, not a reason to keep a preview
   // queued forever. Preserve any already displayed item; retire only demand.
   const auto index=display::Current();
   if(index&&index->resolvedDisplays.contains(id)&&!index->displayRecords.contains(id)){
    component.m_wxlPendingItemDisplay[slot]=0;component.m_wxlPendingItemVisual[slot]=0;
   }
  }
 }
}
bool ItemGraphPending(const CCharacterComponent& component) noexcept {
 return Db2Enabled()&&std::any_of(std::begin(component.m_wxlPendingItemDisplay),std::end(component.m_wxlPendingItemDisplay),[](uint32_t id){return id!=0;});
}
void PumpItemGraph() noexcept {
 try {auto& replay=Replays();if(!ClntObjMgrGetCurrent()){replay.guids.clear();replay.cursor=0;replay.observed=0;return;}
  const auto current=ItemGraphGeneration();
  if(replay.cursor>=replay.guids.size() && replay.observed!=current){replay.guids.clear();replay.cursor=0;replay.observed=current;ClntObjMgrEnumVisibleObjects(&Collect,&replay);}
  for(unsigned budget=0;budget<8&&replay.cursor<replay.guids.size();++budget){auto guid=replay.guids[replay.cursor++];
   auto* player=static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid,TYPEMASK_PLAYER,__FILE__,__LINE__));if(!player)continue;
   for(int slot=0;slot<19;++slot){auto* visible=player->GetVisibleItem(slot);if(!visible||!VisibleItemKey(visible->m_itemId))continue;if(slot<15&&player->m_characterComponent){
    const auto componentSlot=CCharacterComponent::GetItemSlotForInvSlot(static_cast<INVENTORY_SLOTS>(slot));
    const auto id=const_cast<VisibleItem_C*>(visible)->GetDisplayID();
    if(componentSlot!=NUM_ITEM_SLOTS&&id>0&&player->m_characterComponent->m_itemDisplayID[componentSlot]==uint32_t(id))continue;
   }
   player->EquipVisibleItem(visible,slot);if(slot==15||slot==16)player->UpdateObjComponentVisualsSlot(visible,slot);}
  }
 }catch(...){/* Rendering keeps native DBC/stat-cache presentation on allocation failure. */}
}
}
