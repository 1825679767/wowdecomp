// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
class CM2Shared;
class CM2SceneRender;
class CGxDevice;
struct CGxBatch;
namespace wxl::native::mesh_materials {
// Immutable source contract owned by one converted companion/model family.
struct Source {
 uint64_t modelFingerprint=0,skinFingerprint=0;
 std::array<uint8_t,24> batch{};
 std::array<uint32_t,2> textureIds{};
 std::array<uint16_t,2> textureIndices{},weightIndices{},transformIndices{};
 uint16_t flags=0,blend=0;
};
std::shared_ptr<const Source> Prepare(const uint8_t* model,size_t modelBytes,
 const std::vector<uint8_t>& original,std::vector<uint8_t>& prepared);
bool ConvertedMatches(const Source&,const std::vector<uint8_t>& converted) noexcept;
bool Draw(CM2SceneRender&,CGxBatch&); // true only when the native draw was issued.
void DestroyPrograms(CGxDevice&) noexcept; // GxDevDestroy, before backend destruction.
}
