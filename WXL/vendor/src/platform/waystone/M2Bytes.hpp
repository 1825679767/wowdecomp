// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <span>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <memory>
namespace wxl::waystone::m2bytes {
using Resolve=const char*(__cdecl*)(uint32_t);
using Read=bool(*)(const char*,std::vector<uint8_t>&);
struct Stats {uint32_t version{},textures{},particles{},cameras{},skins{},omittedEmitters{},twoStageBatches{},animationFiles{},skeletonFiles{},aliases{},sharedWeightStage{},sharedTransformStage{},collapsedNamedBatches{},envSplitBatches{},stage13Batches{},privateTextureSlots{};std::array<uint64_t,4> skinFingerprints{};std::string reason;};
// Immutable, unpublished CPU source/prepared pair. This grants no native owner,
// registry registration, provider admission or palette/material capability.
struct PartitionedPair {
 std::vector<uint8_t> sourceModel,sourceSkin,model,skin;
 std::vector<uint16_t> bones,starts,sizes;std::vector<uint32_t> vertexOrigins;
 std::string marker,skinPath;uint64_t sourceModelFingerprint{},sourceSkinFingerprint{},sourceTriangles{};
 uint32_t vertices{},skeletonBones{},sourceSections{},splitSections{},materialBlendFallbacks{},materialFlagMasks{},omittedEmitters{},wxlFirstTextureApproximationBatches{};
};
bool PreparePartitioned(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,
 std::shared_ptr<const PartitionedPair>& output,Stats& stats,bool screenBlend=false,uint32_t paletteLimit=75);
// Explicit WXL x32 first-texture approximation for exact known source shader28
// selectors only. Original source metadata stays raw; default/service stays strict.
bool PreparePartitionedWxlFirstTextureApproximation(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,
 std::shared_ptr<const PartitionedPair>& output,Stats& stats,bool screenBlend=false,uint32_t paletteLimit=75);
// Shared source shader admission used before character partition fallback.
bool SupportsSourceMaterial(uint32_t shader,uint32_t textures);
uint64_t Fingerprint(std::span<const uint8_t> bytes);
bool ValidateClassicHandSource(std::span<const uint8_t> body,std::span<const uint8_t> skin,uint32_t paletteLimit);
// Prepared byte contract only. No provider or native material capability grant.
bool ValidateClassicNamedHandSource(std::span<const uint8_t> body,std::span<const uint8_t> skin,uint32_t paletteLimit);
// Exact current main45479 effect1/type0+type2 family; not generic hand admission.
bool ValidateClassicCurrentGlaiveSource(std::span<const uint8_t> body,std::span<const uint8_t> skin,uint32_t paletteLimit);
bool ConvertSkin(std::span<const uint8_t> input,std::vector<uint8_t>& output,bool extendedIndices=false);
bool Convert(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend=false,bool extendedIndices=false,uint32_t paletteLimit=75);
// Source-native consumer policy. Retains modern uint16 animation IDs after
// bounded owned assembly, curated classic-role mapping and native hash rebuild.
// No injected-host capability or single-508 fixture contract is used. All other
// source/track/topology/texture/material limits remain identical to Convert.
// Optional source-native SKIN output owns rebuilt full topology for every LOD
// when a palette exceeds the actual device budget. Empty means no rebuild.
// All profiles share one bounded native vertex/combo array; no archive alias.
bool ConvertForNativeRuntime(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend=false,bool extendedIndices=false,uint32_t paletteLimit=75,std::vector<std::vector<uint8_t>>* rebuiltSkins=nullptr);
// Explicit unpublished CPU byte policy. Caller must separately obtain the exact
// verified native capability before publication. No owner/renderer grant here.
// Requires fully embedded/noalias source and validated supplied native hash;
// permits only singleton508/duration1000/flags20-or-A1/no variations; preserves
// noncurated IDs, timing and lookup bytes. Strict APIs remain unchanged.
bool ConvertWithNativeSequenceHash(const char* path,std::span<const uint8_t> input,Resolve resolve,Read read,std::vector<uint8_t>& output,Stats& stats,bool screenBlend=false,bool extendedIndices=false,uint32_t paletteLimit=75);
}
