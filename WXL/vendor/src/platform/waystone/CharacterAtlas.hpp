// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
#include <string_view>
#include <span>
#include <vector>
namespace wxl::waystone::appearance {
// Stock native ordinal inputs; these are not server retail choice IDs.
struct Inputs {
 uint32_t skin{},face{},hairStyle{},hairColor{},facialStyle{};
 bool operator==(const Inputs&) const = default;
};
uint32_t BlindfoldMaterial(uint32_t retailRace,uint32_t sex,std::span<const uint32_t> choices);
bool FacialOption(std::string_view option);
uint32_t ChoiceOrdinal(std::string_view option,uint32_t count,const Inputs& inputs);
struct Rect {uint32_t x{},y{},width{},height{};};
struct Image {uint32_t width{},height{};std::vector<uint8_t> rgba;};
bool DecodeLayerImage(std::span<const uint8_t> source,Image& image,bool blended);
bool Paint(std::span<uint32_t> dest,uint32_t width,uint32_t height,const Image& source,Rect span,Rect region,uint32_t blend);
constexpr uint32_t FeatherBottom=0x1000;
bool PaintBaseDetails(std::span<uint32_t> dest,uint32_t width,uint32_t height,const Image& source,Rect span,std::span<const Rect> sections,uint32_t layout);
uint32_t PrivateOverlayRegion(uint32_t type,uint32_t texture);
bool OwnRetailFaceRegion(uint32_t layout,uint32_t region);
bool OwnFirstBodyPass(uint32_t race,uint32_t layout,uint32_t region,uint32_t& regions);
bool HidePrivatePreviewCape(uint32_t race,bool preview,std::span<const uint16_t> ids,std::span<uint32_t> shown);
void Mips(uint32_t width,uint32_t height,std::vector<std::vector<uint32_t>>& levels);
bool SelectGeosetGroup(std::span<const uint16_t> ids,std::span<uint32_t> shown,std::span<const uint16_t> chosen,uint16_t first,uint16_t last);
bool SelectGeosets(std::span<const uint16_t> ids,std::span<uint32_t> shown,std::span<const uint16_t> owned,std::span<const uint16_t> chosen);
}
