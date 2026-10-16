// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include "MeshMaterials.hpp"
namespace wxl::native::mesh_materials {
struct DndBatch {
 std::array<uint8_t,24> original{};
 std::array<uint32_t,3> ids{},textureFlags{};
 std::array<uint16_t,3> textures{},weights{},transforms{};
 std::array<uint8_t,2> txac{};
};
struct DndSource {
 uint32_t mode=4,materialCount=0;
 std::array<DndBatch,2> batches;
 std::array<uint8_t,24> nativeBatch{};
};
std::shared_ptr<const Source> PrepareDnd(const char*,const uint8_t*,size_t,const std::vector<uint8_t>&,std::vector<uint8_t>&);
bool DndConvertedMatches(const Source&,const std::vector<uint8_t>&) noexcept;
bool DrawDnd(CM2SceneRender&,CGxBatch&,const Source&,uint32_t);
void DestroyDndPrograms(CGxDevice&) noexcept;
}
