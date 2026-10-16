// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
class CGxDevice;
class CM2Shared;
class CM2Model;
class CParticleEmitter2;
class CParticle2;
class CTexture;
namespace wxl::native {
// Complete source record retained before normalization; no borrowed source bytes.
struct ParticleLayerRecipe {
 uint32_t sourceFlags{};
 uint8_t sourceBlend{};
 uint16_t packedIndices{};
 std::array<uint8_t,2> txac{};
 bool hasTxac{},threeColor{};
 std::array<uint16_t,3> textures{};
 std::array<float,2> scale{};
 std::array<std::array<float,2>,2> mid{},range{};
 std::vector<uint8_t> sourceRecord;
};
using ParticleLayerSet=std::vector<std::shared_ptr<const ParticleLayerRecipe>>;
struct ParticleLayerBinding {
 std::shared_ptr<const ParticleLayerRecipe> recipe;
 std::array<CTexture*,3> textures{};
 ~ParticleLayerBinding();
};
bool CaptureParticleLayers(const uint8_t*,size_t,ParticleLayerSet&,std::string&);
std::shared_ptr<const ParticleLayerRecipe> ParticleLayersFor(const CM2Shared&,uint32_t) noexcept;
std::shared_ptr<const ParticleLayerBinding> BindParticleLayers(const CM2Model&,uint32_t) noexcept;
// Called around native BuildVertex; result order follows native sort/twinkle.
bool AppendParticleLayerQuad(const ParticleLayerRecipe&,const CParticle2&,const uint8_t*,uint32_t,std::vector<uint8_t>&);
bool DrawParticleLayers(CParticleEmitter2&);
void DestroyParticleLayerPrograms(CGxDevice&) noexcept;
}
