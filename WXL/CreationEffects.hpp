// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
class CM2Model;
namespace wxl::native::creation_effects {
// Register once with the other native CVars. All calls run on the Glue thread.
void Register();
bool Enabled() noexcept;
void InitializeLua();
void SetCustomizationOpen(bool) noexcept;
void UpdateCreation();
// Required BEFORE actor/background retirement, including OnHide/UI rebuild.
void StopCreation() noexcept;
// Scoped native Dress() refresh only; never applies to world/foreign models.
bool EquipmentSheathed(const CM2Model*) noexcept;
}
