// Private owner API, C++20 module implementation only.
#pragma once
#include "WXL/ItemGraph.hpp"
class CM2Model;
namespace wxl::native::equipment_attachments {
void RegisterTimedItemVisuals();
bool SameTimedItemVisual(const CM2Model&,int32_t visual,uint32_t display,uint32_t subclass) noexcept;
void RetireTimedItemVisuals(CM2Model&) noexcept;
bool OwnsTimedItemVisual(const CM2Model&,const CM2Model*) noexcept;
void InstallTimedItemVisuals(CM2Model&,int32_t visual,uint32_t display,uint32_t subclass,const std::vector<ItemGraphKitModel>&);
}
