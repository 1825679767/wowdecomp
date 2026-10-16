// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::radial_ping {
bool Install(); // exclusive native521/522 owner; call before legacy facade
void UiReady(); // after root FrameXML, installs deferred owner notification pump
void Destroy() noexcept;
}
