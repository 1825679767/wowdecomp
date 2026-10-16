// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CCharacterComponent;
namespace wxl::native::appearance_session {
bool Install() noexcept;
bool CanPrepareCurrent() noexcept;
// Invoke after native name validation, immediately before the native create/
// customize/race-change/faction-change call. Prepared is a one-use permit.
bool BeforeCreate(const char* validatedName) noexcept;
void Tick() noexcept;
void Cancel() noexcept;
bool QueryCharacter(uint64_t guid) noexcept;
bool QuerySelectedCharacter() noexcept;
// Snapshot cache and source-native restoration for the caller-supplied actor.
bool HasSnapshot(uint64_t guid, uint32_t* revision = nullptr) noexcept;
void RefreshCharacter(uint64_t guid, CCharacterComponent* component) noexcept;
void RefreshCreationForms() noexcept;
// Call on successful existing-character save result to permit a fresh query.
void InvalidateCharacter(uint64_t guid) noexcept;
void InvalidateContext() noexcept;
}
