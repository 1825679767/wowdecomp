// Explicit native donor presentation approximation. Existing material recipes win.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace wxl::native {
void RegisterDonorMaterialFallback();
bool DonorMaterialFallbackEnabled() noexcept;
enum class DonorMaterialRule : uint8_t { NamedRebase, DiffuseCollapse, DefaultDiffuse, EnvironmentSplit, SecondaryUVSplit };
struct DonorMaterialBatch {
 uint32_t sourceBatch=0,firstNativeBatch=0;
 uint16_t sourceShader=0,sourceTextureCount=0;
 DonorMaterialRule rule=DonorMaterialRule::DefaultDiffuse;
 std::array<uint8_t,24> source{};
};
struct DonorMaterialUVFix {uint32_t nativeBatch=0;uint16_t material=0,textureCombo=0;};
struct DonorMaterialRecipe {
 uint64_t modelFingerprint=0,sourceSkinFingerprint=0,preparedSkinFingerprint=0;
 uint32_t sourceBatches=0,preparedBatches=0,nativeBatches=0;
 std::vector<DonorMaterialBatch> batches;
 std::vector<DonorMaterialUVFix> secondaryUV;
};
// Produces an owned prepared clone. No changes means an empty clone/recipe.
// Source21/33/35 require their existing exact prepared renderer recipe.
bool PrepareDonorMaterialSkin(const uint8_t* model,size_t modelSize,const std::vector<uint8_t>& source,
 std::vector<uint8_t>& prepared,std::shared_ptr<const DonorMaterialRecipe>& recipe,std::string& reason);
bool FinishDonorMaterialSkin(const DonorMaterialRecipe& recipe,const std::vector<uint8_t>& model,
 std::vector<uint8_t>& converted,std::string& reason);
}
