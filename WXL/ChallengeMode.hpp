// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CGGameObject_C;
namespace wxl::native::challenge_mode {
bool Install(); // Atomic exclusive challenge/loot/toast transport ownership.
void UiReady(); // Root FrameXML boundary; preserves donor C_ChallengeMode policy.
void Destroy() noexcept;
const char* SelectModel(const CGGameObject_C& object,const char* original) noexcept;
int PortalHidden(const CGGameObject_C& object,int nativeHidden) noexcept;
void RefreshPortal(CGGameObject_C& object);
void DestroyPortals() noexcept;
void InstallMythicUi();
}
