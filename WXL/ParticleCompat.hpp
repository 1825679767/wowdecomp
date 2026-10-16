// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include <string>
#include "ParticleCutoff.hpp"
namespace wxl::native {
// Bake constant EXPT/EXP2 RGB and alpha multipliers into private native keys.
// Requires neutral TXAC and native-representable key values. Nonnegative
// zSource constants >= the native 0.001 directional epsilon are owned as native
// constant float tracks when the original track is empty/neutral; conflicting
// native animation and negative/sub-epsilon source points remain refused. EXP2
// takes precedence over EXPT. A supplied source-owned cutoff set admits nonzero
// linear UNORM16 lifetime ramps for single-texture sprites with strictly ordered
// timestamps. Zero ramps retain existing native rendering without a sidecar.
// Emitter order/count and mesh/particle blend identities are preserved.
// On refusal the caller must discard its candidate buffer; no runtime model
// owns the candidate until this conversion and all other model gates succeed.
bool ApplyParticleExtensions(std::span<const uint8_t>,std::vector<uint8_t>&,std::string&,ParticleCutoffSet* cutoffs=nullptr);
}
