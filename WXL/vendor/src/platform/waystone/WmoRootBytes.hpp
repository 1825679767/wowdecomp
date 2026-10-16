// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/WmoMaterialPlan.hpp"
namespace wxl::waystone::wmo {
struct RootBytes {
    std::vector<uint8_t> bytes;
    MaterialPlan materials;
};
// Conservative first native root adapter. Named group/doodad files only.
// GFID/MODI require an owned provider seam and are refused, never discarded.
// Layered materials require a renderer backend and are refused, never flattened.
// Success owns all strings/bytes. Failure leaves output and input unchanged.
bool PrepareNamedRoot(std::span<const uint8_t> source,TextureResolver resolve,
                      bool effects,RootBytes& output,std::string& reason,bool layeredRenderer=false);
}
