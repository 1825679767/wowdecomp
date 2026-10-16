// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native {
void RegisterFeatures();
bool NativeEnabled() noexcept;
bool Db2Enabled() noexcept;
bool AppearancesEnabled() noexcept;
bool ProtocolEnabled() noexcept;
bool AssetsEnabled() noexcept;
bool BlpEnabled() noexcept;
bool MaterialsEnabled() noexcept;
bool ParticlesEnabled() noexcept;
}
