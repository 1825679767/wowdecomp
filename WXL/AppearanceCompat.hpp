// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
class CCharacterComponent;
namespace wxl::native {
struct AppearanceAssets;
// True only after a complete bounded recipe for the actual modern model binds.
bool ApplyAppearance(CCharacterComponent&) noexcept;
bool AppearanceReady(const CCharacterComponent&) noexcept;
void ReleaseAppearance(CCharacterComponent&) noexcept;
}
