// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/WmoLayout.hpp"
#include <array>
#include <span>
namespace wxl::waystone::wmo {
// Borrowed offsets only: caller retains ownership of the original file bytes.
// This index does not load a WMO, resolve GPU materials or publish native fields.
struct Range {uint32_t offset{},size{};bool present{};};
struct RootIndex {
    std::array<Range,std::size(RootSlots)> slots{};
    uint32_t unknownChunks{};bool modern{},doodadIds{};
    Range groupFileIds{},doodadFileIds{};
};
bool IndexRoot(std::span<const uint8_t> bytes,RootIndex& output);
// All ranges borrow the original complete file, including parked modern data.
// No native group fields, BSP links or GPU resources are published by this API.
struct GroupIndex {
    Range header{},triangles{},indices{},vertices{},normals{},batches{};
    Range lights{},doodads{},bspNodes{},bspFaces{},replacementBatches{};
    Range liquid{},indices32{},layerWeights{};
    std::array<Range,4> uv{};
    std::array<Range,2> colors{};
    uint32_t flags{},groupId{},unknownChunks{},uvSets{},colorSets{};
};
bool IndexGroup(std::span<const uint8_t> bytes,GroupIndex& output);
// Separate from indexing: validates the legacy geometry references a native
// consumer would use. MOVX needs a separate 32-bit index backend and is refused.
bool ValidateGroupGeometry(std::span<const uint8_t> bytes,const GroupIndex& index,
                           uint32_t materialCount);

}
