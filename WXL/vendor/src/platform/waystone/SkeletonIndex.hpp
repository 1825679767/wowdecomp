// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <span>
#include <cstdint>
#include <string>
#include <vector>
namespace wxl::waystone::skeleton {
// Serialized offsets only. The caller owns/retains source bytes; no native pointers.
struct Array {uint32_t count{},offset{};};
struct View {uint32_t offset{},size{};bool present{};};
struct AnimationFile {uint16_t animation{},variation{};uint32_t fileId{};};
struct Index {
    View bonesPayload,sequencesPayload,attachmentsPayload;
    std::vector<AnimationFile> animationFiles;
    Array bones,boneLookup,loops,sequences,sequenceLookup,attachments,attachmentLookup;
    uint32_t parentId{},externalSequences{},aliasSequences{},unknownChunks{},animationIds{};
};
bool Read(std::span<const uint8_t> source,Index& output,std::string& error);
}
