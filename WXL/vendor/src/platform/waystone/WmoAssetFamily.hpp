// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/WmoRootBytes.hpp"
#include <map>
namespace wxl::waystone::wmo {
struct AssetFamily {RootBytes root;std::map<std::string,std::string> groups;};
// Converts GFID/MODI to owned native names, preserves all doodad records, refuses unsupported LOD/metadata.
// A caller must validate/prepare every group before committing this family and its aliases.
bool PrepareAssetFamily(const char* path,std::span<const uint8_t> source,TextureResolver texture,TextureResolver model,AssetFamily& output,std::string& reason,bool layeredRenderer=false);
}
