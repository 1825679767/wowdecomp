// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/SkeletonIndex.hpp"
namespace wxl::waystone::skeleton {
using AnimationReader=bool(*)(uint16_t animation,uint16_t variation,uint32_t fileId,std::vector<uint8_t>& bytes);
struct Embedded {std::vector<uint8_t> body;uint32_t animationFiles{},aliases{};};
// Owned serialized assembly only. No native object allocation or draw publication.
bool InheritBindPose(std::span<const uint8_t> child,std::span<const uint8_t> parent,std::vector<uint8_t>& output,std::string& error);
bool EmbedInline(std::span<const uint8_t> modelBody,std::span<const uint8_t> afid,AnimationReader read,Embedded& output,std::string& error);
// Optional modelSkeleton describes derived model tracks; read still owns parent bone tracks.
bool Embed(std::span<const uint8_t> modelBody,std::span<const uint8_t> skeleton,AnimationReader read,Embedded& output,std::string& error,std::span<const uint8_t> modelSkeleton={},AnimationReader modelRead=nullptr);
}
