// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CCharacterComponent;
namespace wxl::native::barber_appearance {
void Register();
bool Enabled() noexcept;
void InitializeLua();
void Begin() noexcept;
void Tick() noexcept;
void End() noexcept;
void ResetPreview() noexcept;
bool Active() noexcept;
bool Editing(uint64_t guid,const CCharacterComponent*) noexcept;
CCharacterComponent* Editor() noexcept;
bool BeforeApply() noexcept;
}
