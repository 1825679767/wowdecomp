// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
class CM2Scene;
namespace wxl::native::spell_visuals {
void Register();
bool Enabled() noexcept;
void Install();
void UiReady();
void WorldLeave() noexcept;
void ReleaseScene(CM2Scene*) noexcept;
void Shutdown() noexcept;
}
