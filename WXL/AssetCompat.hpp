// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include "NativeStorage.hpp"
#include "BlpCompat.hpp"
class CM2Shared;
namespace wxl::native::mesh_materials {struct Source;}
namespace wxl::native {
struct ModelAssets;
// Native shared-model owns all converted companion bytes until its destructor.
bool PrepareModel(CM2Shared&);
void SelectSkin(CM2Shared&,uint32_t);
bool PrepareSkin(CM2Shared&,uint32_t&);
void ReleaseModel(CM2Shared&) noexcept;
bool IsExactRiftSource(const CM2Shared&) noexcept;
const mesh_materials::Source* MeshSourceFor(const CM2Shared&) noexcept;
}
