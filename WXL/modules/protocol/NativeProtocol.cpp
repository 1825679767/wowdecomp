// Native source transport adapted from WXL platform/waystone/Network.cpp.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeProtocol.hpp"
#include "ClientExtensions.hpp"
#include "NativeFeatures.hpp"
#include "ProtocolConsumers.hpp"
#include "AppearanceSession.hpp"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

#include <deque>
#include <span>
#include <cstring>
#include <mutex>
#include <map>
#include <set>
#include <thread>
#include <utility>

namespace wxl::native::protocol
{
    namespace
    {
        constexpr size_t MaxPackets = 128, MaxBytes = 4 * 1024 * 1024;
        struct Message
        {
            uint16_t opcode;
            NetClient* client;
            WowConnection* transport;
            uint64_t generation;
            std::vector<uint8_t> payload;
        };
        std::mutex mutex;
        // Existing portable feature wire IDs. Appearance save needs a separate
        // feature owner for its hello/prepare/query state machine.
        std::set<std::pair<uint16_t, bool>> declarations{
            {0x525, true}, {0x526, false}, {0x550, true}, {0x551, false},
            {0x552, true}, {0x553, false}};
        std::deque<Message> pending;
        std::map<uint16_t, NativeDeclaration> nativeHandlers;
        std::set<uint16_t> nativeOutbound;
        void ResetNativeOwners() noexcept
        {
            for (const auto& [opcode, entry] : nativeHandlers)
                if (entry.reset) entry.reset(entry.user);
        }
        size_t pendingBytes = 0;
        uint64_t generation = 1;
        std::thread::id owner;
        lua_State* context = nullptr;
        ClientConnection* observedClient = nullptr;
        WowConnection* observedTransport = nullptr;

        bool IsOwner()
        {
            return context && owner == std::this_thread::get_id();
        }

        ClientConnection* Current()
        {
            // Do not call Connection(), whose constructor can create a session.
            auto* client = ClientServices::s_currentConnection;
            return client && client->m_netState == NS_CONNECTED &&
                client->m_serverConnection && !client->m_suspended ? client : nullptr;
        }

        // Owner thread, with mutex held. A redirect can replace the transport
        // without a disconnect event; invalidate consumer state at that seam too.
        ClientConnection* ObserveSession()
        {
            auto* client = ProtocolEnabled() ? Current() : nullptr;
            auto* transport = client ? client->m_serverConnection : nullptr;
            if (client != observedClient || transport != observedTransport)
            {
                observedClient = client;
                observedTransport = transport;
                ++generation;
                pending.clear();
                pendingBytes = 0;
                consumers::Reset();
                ResetNativeOwners();
            }
            return client;
        }

        int LuaSend(lua_State* L)
        {
            const lua_Number number = lua_tonumber(L, 1);
            size_t length = 0;
            const char* data = lua_tolstring(L, 2, &length);
            bool sent = false;
            bool privateOpcode = false;
            if (number >= NUM_MSG_TYPES && number <= 65535) {
                std::lock_guard lock(mutex);
                privateOpcode = nativeOutbound.contains(static_cast<uint16_t>(number));
            }
            if (L == context && lua_type(L, 1) == LUA_TNUMBER &&
                !privateOpcode && lua_type(L, 2) == LUA_TSTRING && number >= NUM_MSG_TYPES && number <= 65535 &&
                number == static_cast<uint16_t>(number) && length <= MaxPayload)
                sent = Send(static_cast<uint16_t>(number), data, static_cast<uint32_t>(length));
            lua_pushboolean(L, sent);
            return 1;
        }

        bool CopyPop(uint16_t& opcode, char* bytes, size_t& length)
        {
            std::vector<uint8_t> payload;
            if (!Pop(opcode, payload)) return false;
            length = payload.size();
            if (length) std::memcpy(bytes, payload.data(), length);
            return true;
        }

        int LuaPop(lua_State* L)
        {
            uint16_t opcode = 0;
            size_t length = 0;
            static thread_local char bytes[MaxPayload];
            // CopyPop destroys all C++ owning storage before Lua allocation can
            // longjmp. No locks or nontrivial local objects cross a Lua call.
            if (L != context || !CopyPop(opcode, bytes, length)) return 0;
            lua_pushinteger(L, opcode);
            lua_pushlstring(L, bytes, length);
            return 2;
        }
    }

    bool RegisterOpcode(uint16_t opcode, bool outbound) noexcept
    {
        if (opcode < NUM_MSG_TYPES) return false;
        try
        {
            std::lock_guard lock(mutex);
            return declarations.size() < 64 && declarations.emplace(opcode, outbound).second;
        }
        catch (...) { return false; }
    }

    bool Send(uint16_t opcode, const void* payload, uint32_t length) noexcept
    {
        if (!ProtocolEnabled() || length > MaxPayload || (length && !payload) ||
            !consumers::ValidateSend(opcode, payload, length)) return false;
        try
        {
            {
                std::lock_guard lock(mutex);
                if (!IsOwner() || !declarations.contains({opcode, true})) return false;
                if (!ObserveSession()) return false;
            }
            auto* client = Current();
            if (!client) return false;
            CDataStore packet;
            packet.Put(static_cast<uint32_t>(opcode));
            if (length) packet.PutData(payload, length);
            packet.Finalize();
            // This is the existing sender and current session. Its suspension
            // queue is deliberately avoided by Current()'s suspended guard.
            client->Send(&packet);
            return true; // handed to native sender; no server acknowledgement implied
        }
        catch (...) { return false; }
    }

    bool Capture(NetClient* client, uint16_t opcode, CDataStore* packet) noexcept
    {
        if (!ProtocolEnabled()) return false;
        bool claimed = false;
        try
        {
            std::lock_guard lock(mutex);
            if (!declarations.contains({opcode, false})) return false;
            claimed = true;
            if (!packet) return true;
            if (!IsOwner() || client != ObserveSession() || !packet->m_data ||
                packet->m_read < packet->m_base || packet->m_read > packet->m_size) return true;
            const uint32_t length = packet->m_size - packet->m_read;
            const uint32_t offset = packet->m_read - packet->m_base;
            if (length > MaxPayload ||
                (packet->m_alloc != UINT32_MAX &&
                 (offset > packet->m_alloc || length > packet->m_alloc - offset))) return true;
            // Built-in feature consumers publish validated owner-side state.
            // They perform no Lua calls while native queue/capture locks exist.
            if (consumers::Consume(opcode, packet->m_data + offset, length, generation)) return true;
            if (auto handler = nativeHandlers.find(opcode); handler != nativeHandlers.end()) {
                const auto& entry = handler->second;
                entry.handler(opcode, packet->m_data + offset, length, generation, entry.user);
                return true;
            }
            if (pending.size() >= MaxPackets || length > MaxBytes - pendingBytes) return true;
            Message message{opcode, client, client->m_serverConnection, generation, {}};
            if (length) message.payload.assign(packet->m_data + offset, packet->m_data + offset + length);
            pending.push_back(std::move(message));
            pendingBytes += length;
            return true;
        }
        catch (...) { return claimed; }
    }

    bool DropOversizedIncoming(uint16_t opcode, uint32_t wireSize) noexcept
    {
        if (!ProtocolEnabled() || wireSize <= MaxPayload + 2) return false;
        try { std::lock_guard lock(mutex); return declarations.contains({opcode, false}); }
        catch (...) { return false; }
    }

    bool Pop(uint16_t& opcode, std::vector<uint8_t>& payload) noexcept
    {
        try
        {
            std::lock_guard lock(mutex);
            if (!IsOwner()) return false;
            auto* current = ObserveSession();
            while (!pending.empty())
            {
                Message message = std::move(pending.front());
                pending.pop_front();
                pendingBytes -= message.payload.size();
                // Compare saved pointers; never dereference the saved owners.
                if (!current || message.client != current || message.generation != generation ||
                    message.transport != current->m_serverConnection) continue;
                opcode = message.opcode;
                payload = std::move(message.payload);
                return true;
            }
            return false;
        }
        catch (...) { return false; }
    }

    void ResetSession(NetClient*) noexcept
    {
        try
        {
            std::lock_guard lock(mutex);
            ++generation;
            pending.clear();
            pendingBytes = 0;
            consumers::Reset();
            ResetNativeOwners();
            observedClient = nullptr;
            observedTransport = nullptr;
        }
        catch (...) {}
    }

    void InitializeLua()
    {
        {
            std::lock_guard lock(mutex);
            context = FrameScript_GetContext();
            owner = std::this_thread::get_id();
            ++generation;
            pending.clear();
            pendingBytes = 0;
            consumers::Reset();
            ResetNativeOwners();
            observedClient = nullptr;
            observedTransport = nullptr;
        }
        FrameScript_RegisterFunction("WXL_NativeSendCustom", &LuaSend);
        FrameScript_RegisterFunction("WXL_NativePopCustom", &LuaPop);
        consumers::InitializeLua();
        appearance_session::Install();
        client_extensions::Install();
    }

    void DestroyLua() noexcept
    {
        client_extensions::Destroy();
        try
        {
            std::lock_guard lock(mutex);
            context = nullptr;
            owner = {};
            ++generation;
            pending.clear();
            pendingBytes = 0;
            consumers::Reset();
            ResetNativeOwners();
            observedClient = nullptr;
            observedTransport = nullptr;
        }
        catch (...) {}
    }

    uint64_t SessionGeneration(lua_State* state) noexcept
    {
        try
        {
            std::lock_guard lock(mutex);
            if (!IsOwner() || context != state) return 0;
            return ObserveSession() ? generation : 0;
        }
        catch (...) { return 0; }
    }

    void UiReady()
    {
        {
            std::lock_guard lock(mutex);
            if (!IsOwner()) return;
        }
        consumers::UiReady();
        client_extensions::UiReady();
    }

    bool RegisterNativeDispatch(const NativeDeclaration* data,uint32_t count) noexcept
    {
        if (!data || !count || count>64) return false;
        const std::span<const NativeDeclaration> entries(data,count);
        try {
            std::lock_guard lock(mutex);
            if (!IsOwner()) return false;
            auto nextDeclarations = declarations;
            auto nextHandlers = nativeHandlers;
            auto nextOutbound = nativeOutbound;
            for (const auto& entry : entries) {
                if (entry.opcode < NUM_MSG_TYPES ||
                    (entry.outbound ? (entry.handler || entry.reset) : !entry.handler) ||
                    !nextDeclarations.emplace(entry.opcode, entry.outbound).second) return false;
                if (!entry.outbound && !nextHandlers.emplace(entry.opcode, entry).second) return false;
                if (entry.outbound) nextOutbound.insert(entry.opcode);
            }
            if (nextDeclarations.size() > 64) return false;
            declarations.swap(nextDeclarations);
            nativeHandlers.swap(nextHandlers);
            nativeOutbound.swap(nextOutbound);
            return true;
        } catch (...) { return false; }
    }
    bool CanLegacySend(uint16_t opcode) noexcept {
        try {std::lock_guard lock(mutex);return IsOwner() && ProtocolEnabled() &&
            declarations.contains({opcode,true}) && !nativeOutbound.contains(opcode);}
        catch(...){return false;}
    }
    bool CanLegacyObserve(uint16_t opcode) noexcept {
        try {std::lock_guard lock(mutex);return IsOwner() && ProtocolEnabled() &&
            declarations.contains({opcode,false}) && !nativeHandlers.contains(opcode) &&
            !consumers::OwnsIncoming(opcode);}
        catch(...){return false;}
    }

}
