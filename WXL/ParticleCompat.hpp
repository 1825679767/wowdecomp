// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include <string>
#include "ParticleCutoff.hpp"
namespace wxl::native {
// Bake constant EXPT/EXP2 RGB and alpha multipliers into private native keys.
// Requires neutral zSource/TXAC and native-representable key values. EXP2
// takes precedence over EXPT. A supplied source-owned cutoff set admits nonzero
// linear UNORM16 lifetime ramps for single-texture sprites with strictly ordered
// timestamps. Zero ramps retain existing native rendering without a sidecar.
// Emitter order/count and mesh/particle blend identities are preserved.
// On refusal the caller must discard its candidate buffer; no runtime model
// owns the candidate until this conversion and all other model gates succeed.
bool ApplyParticleExtensions(std::span<const uint8_t>,std::vector<uint8_t>&,std::string&,ParticleCutoffSet* cutoffs=nullptr);
}
