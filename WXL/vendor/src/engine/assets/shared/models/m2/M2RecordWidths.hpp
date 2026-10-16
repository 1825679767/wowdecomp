// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
// On-disk record widths and encodings, independent of runtime pointer size.
namespace wxl::structure::m2records {
constexpr uint32_t kParticleStrideClient=0x1dc;
constexpr uint32_t kParticleStrideModern=0x1ec;
constexpr uint32_t kParticleModernMinVer=272;
constexpr uint32_t kCameraStrideClient=0x64;
constexpr uint32_t kCameraStrideModern=0x74;
constexpr uint32_t kCameraModernMinVer=272;
constexpr uint32_t kParticleFlagMultiTex=0x10000000;
}
