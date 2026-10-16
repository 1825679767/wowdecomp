// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
namespace wxl::assets::wmo {
// Existing WXL fallback families, not full retail shader equivalence.
constexpr uint32_t NativeShaderFor(uint32_t source) {
    if(source<=6)return source;
    switch(source) {
    case 7:case 9:case 13:case 15:case 18:case 19:case 20:return 6;
    case 11:case 12:case 17:return 5;
    default:return 0;
    }
}
}
