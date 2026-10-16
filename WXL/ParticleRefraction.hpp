// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "Engine/Source/Gx/Types.h"
class CGxDevice;
class CM2Shared;
class CParticleEmitter2;
struct CGxBatch;
namespace wxl::native {
// Immutable original emitter record, owned with the shared model before conversion.
struct ParticleRefractionRecipe {
 uint32_t sourceFlags{},heightFileDataId{};
 uint8_t sourceBlend{};
 uint16_t textureIndex{};
 std::vector<uint8_t> sourceRecord;
};
struct ParticleRefractionCapture;
enum class ParticleRefractionDraw {Native,Drawn,Suppressed};
void RegisterParticleRefraction();
bool ParticleRefractionEnabled() noexcept;
std::shared_ptr<const ParticleRefractionRecipe> CaptureParticleRefraction(const char*,const uint8_t*,size_t);
std::shared_ptr<const ParticleRefractionRecipe> ParticleRefractionFor(const CM2Shared&,uint32_t) noexcept;
ParticleRefractionDraw DrawParticleRefraction(CParticleEmitter2&,CGxBatch&,EGxVertexBufferFormat,uint32_t);
// Both D3D backends call release before Reset; GxDevDestroy also destroys programs.
void ReleaseParticleRefractionCapture(CGxDevice&) noexcept;
void DestroyParticleRefraction(CGxDevice&) noexcept;
}
