// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::quest_marker {
bool Install(); // Atomic 51f/520/536/537 ownership before the legacy facade.
void UiReady(); // Root FrameXML has loaded; installs presentation/pump.
void RenderObjectives(); // WorldFrame world pass, before projected effects flush.
void Destroy() noexcept; // Before protocol destruction; clears all session state.
}
