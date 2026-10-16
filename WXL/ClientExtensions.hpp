// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::client_extensions {
void Install(); // after protocol::InitializeLua; native UI/owner thread
void UiReady(); // after root FrameXML; starts compatibility OnUpdate delivery
void Destroy() noexcept; // before protocol::DestroyLua
void InstallHelpers();
void InstallPackets();
void StartPacketPump();
void DestroyPackets() noexcept;
bool InstallParagon();
void DestroyParagon() noexcept;
}
