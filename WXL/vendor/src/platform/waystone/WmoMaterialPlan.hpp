// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <array>
#include <span>
#include <string>
#include <vector>
#include <cstdint>
namespace wxl::waystone::wmo {
using TextureResolver=const char*(*)(uint32_t);
struct MaterialRecord {
    std::array<uint8_t,64> native{};
    // For shader 23: diffuse A-D, environment, height A-D. Preserve before
    // portable compatibility shaping clears the final sixteen serialized bytes.
    // Build4 native runtime texture refs live separately at root+0x1218.
    std::array<uint32_t,9> fileIds{};
    std::array<std::string,9> paths{};
    uint32_t sourceShader{},nativeShader{};bool layered{};
};
struct MaterialPlan {std::vector<MaterialRecord> records;uint32_t remapped{},layered{};};
// Owns bytes and names; never loads/releases textures or publishes root fields.
// Strict unresolved-ID refusal. Layered records still require a renderer backend.
bool PlanMaterials(std::span<const uint8_t> root,TextureResolver resolve,bool effects,
                   MaterialPlan& output,std::string& reason);
}
