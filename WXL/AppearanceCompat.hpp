// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
class CCharacterComponent;
namespace wxl::native {
struct AppearanceAssets;
// True after the exact actor's body/material recipe binds; independently
// loading customization children must not restore the legacy body atlas.
bool ApplyAppearance(CCharacterComponent&) noexcept;
bool AppearanceReady(const CCharacterComponent&) noexcept;
bool AppearanceChildrenPending(const CCharacterComponent&) noexcept;
void ReleaseAppearance(CCharacterComponent&) noexcept;
}
