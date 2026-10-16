// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>

class NetClient;
class CDataStore;
struct lua_State;

namespace wxl::native::protocol
{
    constexpr uint32_t MaxPayload = 10235;
    // Compiled feature owners may declare custom opcodes; standard IDs and
    // duplicate declarations are rejected. Lua cannot declare arbitrary IDs.
    bool RegisterOpcode(uint16_t opcode, bool outbound) noexcept;
    bool Send(uint16_t opcode, const void* payload, uint32_t length) noexcept;
    bool Pop(uint16_t& opcode, std::vector<uint8_t>& payload) noexcept;
    // NetClient owner/event-queue boundary, after reading the uint16 opcode.
    // True means the custom packet was claimed, including a bounded drop.
    bool Capture(NetClient* client, uint16_t opcode, CDataStore* packet) noexcept;
    bool DropOversizedIncoming(uint16_t opcode, uint32_t wireSize) noexcept;
    void ResetSession(NetClient* client) noexcept;
    void InitializeLua();
    void DestroyLua() noexcept;
    // Call after the root UI has loaded; installs spell-charge presentation.
    void UiReady();
    // Owner-only validity token for compiled feature Lua entry points.
    uint64_t SessionGeneration(lua_State* state) noexcept;
    // Compiled owner-side packet dispatch. Callbacks run under the transport
    // mutex: no Lua calls, sends or protocol API reentry are permitted.
    using NativePacketHandler = void (*)(uint16_t, const uint8_t*, uint32_t, uint64_t, void*) noexcept;
    using NativeSessionReset = void (*)(void*) noexcept;
    struct NativeDeclaration {
        uint16_t opcode;
        bool outbound;
        NativePacketHandler handler = nullptr;
        NativeSessionReset reset = nullptr;
        void* user = nullptr;
    };
    // Entire group commits atomically; no partial feature registration.
    bool RegisterNativeDispatch(const NativeDeclaration* entries,uint32_t count) noexcept;
}
