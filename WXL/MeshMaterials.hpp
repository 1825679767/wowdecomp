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
struct CGxBatch;
namespace wxl::native::mesh_materials {
struct DndSource;
// Immutable source contract owned by one converted companion/model family.
struct BatchSource {
 std::array<uint8_t,24> batch{};
 std::array<uint32_t,2> textureIds{};
 std::array<uint16_t,2> textureIndices{},weightIndices{},transformIndices{};
 uint16_t flags=0,blend=0;
};
struct SectionSource {
 uint16_t firstVertex=0,vertexCount=0,paletteOffset=0,influences=0;
 std::vector<uint16_t> palette;
};
struct Source {
 bool weighted=false;
 uint32_t boneCount=0;
 std::vector<SectionSource> sections;
 uint64_t modelFingerprint=0,skinFingerprint=0;
 std::string modelPath;
 uint32_t sectionCount=0,textureCount=0;
 std::vector<BatchSource> batches;
 std::shared_ptr<const DndSource> dnd;
};
std::shared_ptr<const Source> Prepare(const char* modelPath,const uint8_t* model,size_t modelBytes,
 const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared,bool nativeGpuSkinning=false);
// Exact named donor correction uses native stock Mod_Mod2x; no custom draw grant.
bool PrepareNativeCompatibilitySkin(const char* modelPath,const uint8_t* model,size_t modelBytes,
 const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared);
bool ConvertedMatches(const Source&,const std::vector<uint8_t>& converted) noexcept;
bool Draw(CM2SceneRender&,CGxBatch&); // true only when the native draw was issued.
bool OwnsBatch(const CM2SceneRender&) noexcept;
bool ApplyPreparedMaterials(const Source&,std::vector<uint8_t>&);
void RegisterDndMaterials();
void DestroyPrograms(CGxDevice&) noexcept; // GxDevDestroy, before backend destruction.
}
