// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>
namespace wxl::native {
struct ComponentBlpInfo { uint32_t width=0,height=0,mipCount=0,alphaSize=0; };
// Validated classic palette input stays byte-exact (empty replacement).
// Modern DXT/BGRA components produce a bounded native palette image <=256px.
bool PrepareComponentBlp(const void*,uint32_t,std::vector<uint8_t>& replacement,ComponentBlpInfo&) noexcept;
bool ConvertBlp(const void*,uint32_t,std::vector<uint8_t>&);
}
