// Native source transport adapted from WXL platform/waystone/Network.cpp.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeProtocol.hpp"
#include "NativeFeatures.hpp"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

#include <deque>
#include <cstring>
#include <mutex>
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
            {0x526, false}, {0x550, true}, {0x551, false},
            {0x552, true}, {0x553, false}};
        std::deque<Message> pending;
        size_t pendingBytes = 0;
        uint64_t generation = 1;
        std::thread::id owner;
        lua_State* context = nullptr;

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

        int LuaSend(lua_State* L)
        {
            const lua_Number number = lua_tonumber(L, 1);
            size_t length = 0;
            const char* data = lua_tolstring(L, 2, &length);
            bool sent = false;
            if (L == context && lua_type(L, 1) == LUA_TNUMBER &&
                lua_type(L, 2) == LUA_TSTRING && number >= NUM_MSG_TYPES && number <= 65535 &&
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
        if (!ProtocolEnabled() || length > MaxPayload || (length && !payload)) return false;
        try
        {
            {
                std::lock_guard lock(mutex);
                if (!IsOwner() || !declarations.contains({opcode, true})) return false;
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
            if (!IsOwner() || client != Current() || !packet->m_data ||
                packet->m_read < packet->m_base || packet->m_read > packet->m_size) return true;
            const uint32_t length = packet->m_size - packet->m_read;
            const uint32_t offset = packet->m_read - packet->m_base;
            if (length > MaxPayload || pending.size() >= MaxPackets || length > MaxBytes - pendingBytes ||
                (packet->m_alloc != UINT32_MAX &&
                 (offset > packet->m_alloc || length > packet->m_alloc - offset))) return true;
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
            auto* current = ProtocolEnabled() ? Current() : nullptr;
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
        }
        FrameScript_RegisterFunction("WXL_NativeSendCustom", &LuaSend);
        FrameScript_RegisterFunction("WXL_NativePopCustom", &LuaPop);
    }

    void DestroyLua() noexcept
    {
        try
        {
            std::lock_guard lock(mutex);
            context = nullptr;
            owner = {};
            ++generation;
            pending.clear();
            pendingBytes = 0;
        }
        catch (...) {}
    }
}
