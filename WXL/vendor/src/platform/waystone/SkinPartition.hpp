// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <span>
#include <cstdint>
#include <vector>
#include <string>
namespace wxl::waystone::skin {
// Geometry is borrowed. Each run retains the original triangle ordering.
// This plan alone does not rewrite vertices or install native draw hooks.
// Pass the verified device transport limit (1..75); callers without a device
// retain the legacy 75-bone preparation ceiling.
struct Draw {uint32_t sourceSection{},firstIndex{},indexCount{};std::vector<uint16_t> bones;};
struct Plan {std::vector<Draw> draws;uint64_t sourceTriangles{};uint32_t sourceSections{},splitSections{};};
struct Owned {std::vector<uint8_t> body,skin;std::vector<uint32_t> vertexOrigins;Plan plan;};
bool Rebuild(std::span<const uint8_t> body,std::span<const uint8_t> skin,uint32_t skeletonBones,bool extendedIndexStarts,Owned& output,std::string& error,uint32_t paletteLimit=75);
bool Partition(std::span<const uint8_t> body,std::span<const uint8_t> skin,uint32_t skeletonBones,bool extendedIndexStarts,Plan& output,std::string& error,uint32_t paletteLimit=75);
}
