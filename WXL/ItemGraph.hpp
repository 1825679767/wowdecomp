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
 std::string folder,model,texture,skinSectionIds,batchIndexes,targetSkinSectionIds,targetBatchIndexes,targetMode;
};
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
