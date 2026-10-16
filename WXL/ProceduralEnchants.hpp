// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CM2Model;
class CM2Scene;
namespace wxl::native {
struct ProceduralEnchantState;
struct ItemGraphVisual;
namespace procedural_enchants {
void Clone(const CM2Model& source,CM2Model& destination) noexcept;
void Register();
bool Enabled() noexcept;
void Apply(CM2Model*,const ItemGraphVisual&,uint32_t weaponSubclass) noexcept;
void Release(CM2Model*) noexcept;
void TickScene(CM2Scene*) noexcept;
void ReleaseScene(CM2Scene*) noexcept;
}
}
