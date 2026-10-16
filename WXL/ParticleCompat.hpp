// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include <string>
namespace wxl::native {
// Bake constant EXPT/EXP2 RGB and alpha multipliers into private native keys.
// Requires neutral zSource/TXAC, native-representable key values and zero EXP2
// cutoff. EXP2 takes precedence when EXPT also exists. Identically-zero cutoff
// tracks accept the entire ordered source uint16 lifetime domain.
// Emitter order/count and mesh/particle blend identities are preserved.
// On refusal the caller must discard its candidate buffer; no runtime model
// owns the candidate until this conversion and all other model gates succeed.
bool ApplyParticleExtensions(std::span<const uint8_t>,std::vector<uint8_t>&,std::string&);
}
