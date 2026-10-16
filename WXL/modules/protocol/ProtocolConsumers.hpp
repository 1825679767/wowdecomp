// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <cstddef>
namespace wxl::native::protocol::consumers {
bool OwnsIncoming(uint16_t opcode) noexcept;
bool Consume(uint16_t opcode, const uint8_t* data, size_t length, uint64_t session) noexcept;
void Reset() noexcept;
void InitializeLua();
void UiReady();
bool ValidateSend(uint16_t opcode, const void* data, uint32_t length) noexcept;
}
