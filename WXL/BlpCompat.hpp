// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>
namespace wxl::native {
bool ConvertBlp(const void*,uint32_t,std::vector<uint8_t>&);
}
