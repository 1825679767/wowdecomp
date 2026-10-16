// Private C++20 spell graph DTOs; donor fields remain distinct from native slots.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace wxl::native::spell_visuals {
struct GraphModel {
 uint32_t fileDataId{};float scale=1;int32_t attachmentId=-1;
 uint32_t positionerId{},spellVisualKitId{},startEvent{},endEvent{},targetType{},role{};
};
struct Catalog {uint32_t spellId{},visualId{};std::vector<GraphModel> models;std::string error;};
std::shared_ptr<const Catalog> BuildCatalog();
}
