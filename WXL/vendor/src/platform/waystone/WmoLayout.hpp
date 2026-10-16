// Disk-only subset of WXL WmoLayout.hpp; native object offsets/RVAs removed.
// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
#include <cstddef>
namespace wxl::waystone::wmo {
struct Slot { const char* tag; uint16_t stride; };
inline constexpr Slot RootSlots[]{
 {"MOHD",0},{"MOTX",1},{"MOMT",64},{"MOGN",1},{"MOGI",32},
 {"MOSB",0},{"MOPV",12},{"MOPT",20},{"MOPR",8},{"MOVV",12},
 {"MOVB",4},{"MOLT",48},{"MODS",32},{"MODN",1},{"MODD",40},
 {"MFOG",48},{"MCVP",16}};
}
