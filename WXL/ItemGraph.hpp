// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
class ItemRec;
class ItemDisplayInfoRec;
class CCharacterComponent;
namespace wxl::native {
struct ItemGraphGeoFilter { uint16_t ids[16]{}; uint32_t count=0; };
struct ItemGraphModel {
 uint32_t modelSlot=UINT32_MAX,attachId=UINT32_MAX,modelIndex=UINT32_MAX,raceId=0,genderId=UINT32_MAX;
 uint32_t modelFlags=6,textureFlags=0;
 std::string folder,model,texture;
 ItemGraphGeoFilter geoFilter;
};
struct ItemGraphMaterial {
 uint32_t modelIndex=UINT32_MAX,modelColumn=UINT32_MAX,layer=UINT32_MAX,textureType=UINT32_MAX,raceId=0,genderId=UINT32_MAX;
 std::string folder,model,texture,texturePath,skinSectionIds,batchIndexes,targetSkinSectionIds,targetBatchIndexes,targetMode;
};
struct ItemGraphHelmetRule {uint32_t raceId=0,hideGroup=0,raceBitSelection=0,flags=0;};
struct ItemGraphHelmet {
 uint32_t visibilityId=0;std::vector<ItemGraphHelmetRule> rules;
 // Retained metadata only. The donor has no HelmetAnimScaling consumer;
 // Amount is not permission to scale an actor or an attached helmet model.
 bool hasAnimScale=false;float animScale=0;
};
// Modern ItemVisuals owns FDID slots directly; never legacy effect-row IDs.
struct ItemGraphVisualModel {uint32_t attachmentId=0,fileDataId=0;std::string path;};
struct ItemGraphVisualEffect {
 uint32_t id=0,attachmentId=0,displayWeaponSubclassId=0,spellVisualKitId=0,fileDataId=0;
 float scale=1;std::string path;
};
struct ItemGraphKitModel {
 uint32_t kitId=0,modelAttachId=0,fileDataId=0,attachmentId=0;
 uint32_t displayWeaponSubclassId=UINT32_MAX,sourceEffectId=0,animationId=0;
 float scale=1,alpha=1,offset[3]{};
 std::string path;
};
struct ItemGraphVisual {
 uint32_t id=0,spellVisualKitId1=0,spellVisualKitId2=0;
 std::vector<ItemGraphVisualModel> models;
 // Explicit selectors/kit ownership are retained, not guessed as legacy IDs.
 std::vector<ItemGraphVisualEffect> effects;
 std::vector<ItemGraphKitModel> kitModels;
 uint32_t unsupportedKitRows=0;
};
bool ItemGraphVisualData(uint32_t visual,ItemGraphVisual&) noexcept;
bool ItemGraphWeaponSubclass(uint32_t display,uint32_t& subclass) noexcept;
bool ItemGraphDisplayVisual(uint32_t display,ItemGraphVisual&) noexcept;
bool ItemGraphHelmetData(uint32_t display,uint32_t sex,uint32_t modelRace,ItemGraphHelmet&) noexcept;
// Existing native DBC presentation always wins. Queries copy bounded owned data.
bool ItemGraphOwnsDisplay(uint32_t displayId) noexcept;
bool ItemGraphInventoryType(uint32_t displayId,uint32_t& inventoryType) noexcept;
bool ItemGraphModels(uint32_t displayId,std::vector<ItemGraphModel>& output) noexcept;
bool ItemGraphMaterials(uint32_t displayId,std::vector<ItemGraphMaterial>& output) noexcept;
void StartItemGraph() noexcept;
void StopItemGraph() noexcept; // joins before any archive/DB shutdown
void PumpItemGraph() noexcept; // main-thread bounded GUID-resolved replay
uint64_t ItemGraphGeneration() noexcept;
void ItemGraphPublished() noexcept;
ItemRec* MissingItemRecord(int32_t id) noexcept;
bool MissingItemDisplayRecord(int32_t id,ItemDisplayInfoRec* output) noexcept;
void RetryItemGraph(CCharacterComponent&) noexcept;
inline uint32_t VisibleItemKey(int32_t id) noexcept {
 const int64_t value=id<0?-int64_t(id):int64_t(id);
 return value>0 && value<=INT32_MAX?uint32_t(value):0;
}
}
