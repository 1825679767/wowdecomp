// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <memory>
#include <vector>
class CM2Shared;
class CParticleEmitter2;
struct CGxBuf;
#include "Engine/Source/Gx/Types.h"
namespace wxl::native {
// Immutable, owned source-lifetime UNORM16 samples. No native M2 file layout changes.
struct ParticleCutoffTrack {
 std::vector<uint16_t> times,values;
 float Sample(float normalizedAge) const noexcept;
};
using ParticleCutoffSet=std::vector<std::shared_ptr<const ParticleCutoffTrack>>;
std::shared_ptr<const ParticleCutoffTrack> ParticleCutoffFor(const CM2Shared&,uint32_t index) noexcept;
bool DrawParticleCutoffVertices(CGxBuf*,EGxVertexBufferFormat,uint32_t vertexCount,
 uint32_t verticesPerParticle,uint32_t indicesPerParticle,const std::vector<float>& cutoffs);
}
