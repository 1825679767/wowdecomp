// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
class CM2Shared;
class CM2SceneRender;
class CGxDevice;
class M2Batch;
struct CGxBatch;
namespace wxl::native::mesh_materials {
struct DndSource;
// Immutable source contract owned by one converted companion/model family.
struct BatchSource {
 // Stock siblings retain their native equation; only custom rows own draw overrides.
 bool custom=true;
 std::array<uint8_t,24> batch{},nativeBatch{};
 std::array<uint32_t,2> textureIds{};
 std::array<uint16_t,2> textureIndices{},weightIndices{},transformIndices{};
 uint16_t flags=0,blend=0;
};
struct SectionSource {
 uint16_t firstVertex=0,vertexCount=0,paletteOffset=0,influences=0;
 std::vector<uint16_t> palette;
 std::vector<std::array<uint8_t,48>> cpuVertices;
 std::vector<uint16_t> cpuIndices;
};
struct Source {
 bool weighted=false,cpuSkinning=false;
 uint32_t boneCount=0;
 std::vector<SectionSource> sections;
 uint64_t modelFingerprint=0,skinFingerprint=0;
 std::string modelPath;
 uint32_t sectionCount=0,textureCount=0;
 std::vector<BatchSource> batches;
 std::shared_ptr<const DndSource> dnd;
 // Original recipe/audit remains immutable when full topology is partitioned.
 std::shared_ptr<const Source> topologyParent;
 std::vector<uint32_t> nativeBatchOrigins;
};
std::shared_ptr<const Source> Prepare(const char* modelPath,const uint8_t* model,size_t modelBytes,
 const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared,bool nativeGpuSkinning=false);
// Exact named donor correction uses native stock Mod_Mod2x; no custom draw grant.
bool PrepareNativeCompatibilitySkin(const char* modelPath,const uint8_t* model,size_t modelBytes,
 const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared);
bool ConvertedMatches(const Source&,const std::vector<uint8_t>& converted) noexcept;
std::shared_ptr<const Source> BindTopology(std::shared_ptr<const Source>,const std::vector<uint8_t>& prepared,
 const std::vector<uint8_t>& model,const std::vector<uint8_t>& rebuilt,const std::vector<uint32_t>& origins,
 uint32_t paletteLimit,bool nativeGpuSkinning);
bool Draw(CM2SceneRender&,CGxBatch&); // true only when the native draw was issued.
bool OwnsBatch(const CM2SceneRender&) noexcept;
bool OwnsBatch(const CM2Shared&,const M2Batch*) noexcept;
bool ApplyPreparedMaterials(const Source&,std::vector<uint8_t>&);
void RegisterDndMaterials();
void DestroyPrograms(CGxDevice&) noexcept; // GxDevDestroy, before backend destruction.
}
