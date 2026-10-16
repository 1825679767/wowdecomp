// Immutable modern positioner graphs, owned by the item catalog.
#pragma once
#include "WXL/ItemGraph.hpp"
#include <unordered_map>
namespace wxl::native {
std::unordered_map<uint32_t,std::shared_ptr<const ItemGraphPositioner>> LoadItemPositioners(const std::vector<uint32_t>& ids);
}
