// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include <string>
namespace wxl::native {
void PreserveMaterials(std::span<const uint8_t>,std::vector<uint8_t>&);
}
