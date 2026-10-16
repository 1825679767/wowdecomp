// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>
class CM2Shared;
namespace wxl::native {
struct ModelAssets;
bool ReadAsset(const char*,std::vector<uint8_t>&);
bool ConvertBlp(const void*,uint32_t,std::vector<uint8_t>&);
// Native shared-model owns all converted companion bytes until its destructor.
bool PrepareModel(CM2Shared&);
void SelectSkin(CM2Shared&,uint32_t);
bool PrepareSkin(CM2Shared&,uint32_t&);
void ReleaseModel(CM2Shared&) noexcept;
}
