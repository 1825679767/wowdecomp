// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <array>
#include <string>
class CCharacterComponent;
class CM2Model;
class CM2Scene;
namespace wxl::native {
struct EquipmentAttachmentState;
namespace equipment_attachments {
bool CloneItemVisuals(const CM2Model& source,CM2Model& destination) noexcept;
bool OwnsTimedItemVisual(const CM2Model& parent,const CM2Model* child) noexcept;
void TickItemVisuals(CM2Scene&) noexcept; // scene owner before animation workers
void ReleaseItemVisuals(CM2Model&) noexcept; // parent teardown/detachment
void ReleaseSceneItemVisuals(CM2Scene&) noexcept;
void Register();
bool Enabled() noexcept;
void Apply(CCharacterComponent&) noexcept;
std::string RetailHelmetPath(const CCharacterComponent&,const char* requested) noexcept;
bool Pending(const CCharacterComponent&) noexcept;
// Restore only this owner's hidden sections before native/recipe geometry writes.
void BeginGeometry(CCharacterComponent&) noexcept;
void Release(CCharacterComponent&) noexcept;
void InvalidateSlot(CCharacterComponent&,uint32_t slot) noexcept;
void BindNativeMaterials(CM2Model*,uint32_t display) noexcept;
// Actor-selected component sections 0..7; owned exact mounted paths. Native
// priority/robe/glove/boot rules decide where these sections are composed.
bool SelectBodyLayers(const CCharacterComponent&,uint32_t display,std::array<std::string,8>& paths) noexcept;
bool SuppressNativeArmor(const CCharacterComponent&,uint32_t slot) noexcept;
// The native enchant owner retains its own item/visual identity. This only
// creates validated native ItemVisualEffects children at real attachment IDs.
void ApplyItemVisual(CM2Model*,int32_t visual,uint32_t display=0,uint32_t weaponSubclass=UINT32_MAX) noexcept;
}
}
