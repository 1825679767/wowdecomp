// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CCharacterComponent;
class CM2Model;
namespace wxl::native {
struct EquipmentAttachmentState;
namespace equipment_attachments {
void Register();
bool Enabled() noexcept;
void Apply(CCharacterComponent&) noexcept;
void Release(CCharacterComponent&) noexcept;
void InvalidateSlot(CCharacterComponent&,uint32_t slot) noexcept;
void BindNativeMaterials(CM2Model*,uint32_t display) noexcept;
bool SuppressNativeArmor(const CCharacterComponent&,uint32_t slot) noexcept;
// The native enchant owner retains its own item/visual identity. This only
// creates validated native ItemVisualEffects children at real attachment IDs.
void ApplyItemVisual(CM2Model*,int32_t visual) noexcept;
}
}
