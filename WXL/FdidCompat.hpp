// Copyright (C) 2026 WarcraftXL. GPLv3 or later.
#pragma once
#include <cstdint>

namespace wxl::native
{
    // Resolve against TextureFilePath/ModelFilePath from mounted client data.
    // Returned pointers remain valid for the process lifetime; nullptr is a miss.
    // First use must occur after storage mounts are available.
    const char* ResolveTexture(uint32_t fileDataId) noexcept;
    const char* ResolveModel(uint32_t fileDataId) noexcept;
}
