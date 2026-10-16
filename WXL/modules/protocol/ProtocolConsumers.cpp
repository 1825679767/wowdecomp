// Native source consumers adapted from WXL area-loot, garrison and spell-charges.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "ProtocolConsumers.hpp"
#include "NativeProtocol.hpp"
#include "SpellChargesBootstrap.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <deque>
#include <mutex>
#include <span>
#include <string>
#include <vector>

namespace wxl::native::protocol::consumers
{
    namespace
    {
        struct Charge
        {
            uint32_t spell = 0, category = 0;
            uint8_t maximum = 0, current = 0;
            uint32_t duration = 0, remaining = 0, received = 0;
        };
        std::mutex mutex;
        std::deque<std::string> area, garrison;
        std::vector<Charge> charges;
        uint64_t stateSession = 0;
        uint32_t revision = 0, lastRequest = 0;
        bool requested = false;

        void AdvanceRevision()
        {
            if (!++revision) ++revision;
        }

        void ClearState()
        {
            area.clear();
            garrison.clear();
            charges.clear();
            lastRequest = 0;
            requested = false;
            AdvanceRevision();
        }

        void SelectSession(uint64_t session)
        {
            if (session == stateSession) return;
            ClearState();
            stateSession = session;
        }

        bool ValidArea(const uint8_t* data, size_t length)
        {
            if (!data || !length || length > 8192) return false;
            size_t begin = 0;
            for (size_t i = 0; i <= length; ++i)
            {
                if (i != length)
                {
                    const auto c = data[i];
                    if ((c < 0x20 && c != '\n') || c > 0x7e) return false;
                    if (c != '\n') continue;
                }
                if (i == begin || i - begin > 240) return false;
                begin = i + 1;
            }
            return true;
        }

        bool ValidGarrison(const uint8_t* data, size_t length, size_t maximum)
        {
            if (!data || length < 2 || length > maximum) return false;
            size_t tab = 0;
            while (tab < length && data[tab] != '\t')
            {
                const auto c = data[tab];
                if (!((c >= 'A' && c <= 'Z') || c == '_') || tab >= 48) return false;
                ++tab;
            }
            if (!tab || tab == length) return false;
            for (size_t i = tab + 1; i < length; ++i)
                if ((data[i] < 0x20 && data[i] != '\t') || data[i] == 0x7f) return false;
            return true;
        }

        // Fixed little-endian wire reader, without native object-layout assumptions.
        struct Reader
        {
            std::span<const uint8_t> bytes;
            size_t position = 0;
            template<class T> bool Read(T& value)
            {
                if (position > bytes.size() || sizeof(T) > bytes.size() - position) return false;
                uint32_t next = 0;
                for (size_t n = 0; n < sizeof(T); ++n)
                    next |= uint32_t(bytes[position++]) << (8 * n);
                value = static_cast<T>(next);
                return true;
            }
        };

        bool ParseCharges(const uint8_t* data, size_t length, std::vector<Charge>& next)
        {
            if (!data || length < 3 || length > MaxPayload) return false;
            Reader reader{{data, length}};
            uint8_t version = 0;
            uint16_t count = 0;
            if (!reader.Read(version) || !reader.Read(count) || version != 1 ||
                length != 3 + size_t(count) * 18) return false;
            next.reserve(count);
            const uint32_t now = OsGetAsyncTimeMs();
            for (uint16_t n = 0; n < count; ++n)
            {
                Charge row;
                if (!reader.Read(row.spell) || !reader.Read(row.category) ||
                    !reader.Read(row.maximum) || !reader.Read(row.current) ||
                    !reader.Read(row.duration) || !reader.Read(row.remaining)) return false;
                if (!row.spell || !row.maximum) continue;
                row.current = std::min(row.current, row.maximum);
                row.received = now;
                next.push_back(row);
            }
            return reader.position == length;
        }

        bool CopyRecord(lua_State* L, bool isArea, char* output, size_t& length)
        {
            const auto session = SessionGeneration(L);
            std::lock_guard lock(mutex);
            SelectSession(session);
            auto& queue = isArea ? area : garrison;
            if (!session || queue.empty()) return false;
            length = queue.front().size();
            std::memcpy(output, queue.front().data(), length);
            queue.pop_front();
            return true;
        }

        int PopArea(lua_State* L)
        {
            // Owning objects and locks are destroyed before Lua may longjmp.
            char bytes[240];
            size_t length = 0;
            bool present = false;
            try { present = CopyRecord(L, true, bytes, length); } catch (...) {}
            if (present) lua_pushlstring(L, bytes, length); else lua_pushnil(L);
            return 1;
        }

        int PopGarrison(lua_State* L)
        {
            char bytes[4096];
            size_t length = 0;
            bool present = false;
            try { present = CopyRecord(L, false, bytes, length); } catch (...) {}
            if (present) lua_pushlstring(L, bytes, length); else lua_pushnil(L);
            return 1;
        }

        int Clear(lua_State* L, bool isArea)
        {
            const auto session = SessionGeneration(L);
            try
            {
                std::lock_guard lock(mutex);
                SelectSession(session);
                (isArea ? area : garrison).clear();
            }
            catch (...) {}
            return 0;
        }
        int ClearArea(lua_State* L) { return Clear(L, true); }
        int ClearGarrison(lua_State* L) { return Clear(L, false); }

        int SendText(lua_State* L, bool isArea)
        {
            size_t length = 0;
            const char* text = lua_type(L, 1) == LUA_TSTRING ? lua_tolstring(L, 1, &length) : nullptr;
            bool valid = false;
            if (text && SessionGeneration(L))
            {
                if (isArea)
                    valid = length && length <= 64 &&
                        std::all_of(text, text + length, [](unsigned char c) { return c >= 0x20 && c <= 0x7e; });
                else valid = ValidGarrison(reinterpret_cast<const uint8_t*>(text), length, 254);
            }
            lua_pushboolean(L, valid && Send(isArea ? 0x550 : 0x552, text, static_cast<uint32_t>(length)));
            return 1;
        }
        int SendArea(lua_State* L) { return SendText(L, true); }
        int SendGarrison(lua_State* L) { return SendText(L, false); }

        bool Request(lua_State* L)
        {
            const auto session = SessionGeneration(L);
            if (!session) return false;
            const uint32_t now = OsGetAsyncTimeMs();
            {
                std::lock_guard lock(mutex);
                SelectSession(session);
                if (requested && now - lastRequest < 250) return true;
            }
            if (!Send(0x525, nullptr, 0)) return false;
            {
                std::lock_guard lock(mutex);
                if (stateSession != session) return false;
                requested = true;
                lastRequest = now;
            }
            return true;
        }

        int RequestCharges(lua_State* L)
        {
            bool sent = false;
            try { sent = Request(L); } catch (...) {}
            lua_pushboolean(L, sent);
            return 1;
        }

        int Snapshot(lua_State* L)
        {
            const auto session = SessionGeneration(L);
            size_t count = 0;
            uint32_t value = 0;
            try
            {
                std::lock_guard lock(mutex);
                SelectSession(session);
                if (session) count = charges.size();
                value = revision;
            }
            catch (...) {}
            lua_pushnumber(L, static_cast<lua_Number>(count));
            lua_pushnumber(L, value);
            return 2;
        }

        bool CopyCharge(lua_State* L, double index, Charge& row)
        {
            const auto session = SessionGeneration(L);
            std::lock_guard lock(mutex);
            SelectSession(session);
            if (!session || !std::isfinite(index) || index < 1 || index > 65535 || std::floor(index) != index ||
                size_t(index) > charges.size()) return false;
            row = charges[size_t(index) - 1];
            return true;
        }

        int Row(lua_State* L)
        {
            const double index = lua_type(L, 1) == LUA_TNUMBER ? lua_tonumber(L, 1) : 0;
            Charge row;
            bool present = false;
            try { present = CopyCharge(L, index, row); } catch (...) {}
            if (!present) { lua_pushnil(L); return 1; }
            const uint32_t elapsed = OsGetAsyncTimeMs() - row.received;
            const uint32_t remaining = elapsed >= row.remaining ? 0 : row.remaining - elapsed;
            lua_pushnumber(L, row.spell);
            lua_pushnumber(L, row.category);
            lua_pushnumber(L, row.maximum);
            lua_pushnumber(L, row.current);
            lua_pushnumber(L, row.duration);
            lua_pushnumber(L, remaining);
            return 6;
        }
    }

    bool OwnsIncoming(uint16_t opcode) noexcept {return opcode==0x526 || opcode==0x551 || opcode==0x553;}

    bool Consume(uint16_t opcode, const uint8_t* data, size_t length, uint64_t session) noexcept
    {
        if (!OwnsIncoming(opcode)) return false;
        try
        {
            if (!session) return true;
            // Validate the entire payload before publishing any feature state.
            std::vector<Charge> replacement;
            if (opcode == 0x551 && !ValidArea(data, length)) return true;
            if (opcode == 0x553 && !ValidGarrison(data, length, 4096)) return true;
            if (opcode == 0x526 && !ParseCharges(data, length, replacement)) return true;
            std::lock_guard lock(mutex);
            SelectSession(session);
            if (opcode == 0x526)
            {
                charges.swap(replacement);
                AdvanceRevision();
            }
            else if (opcode == 0x553)
            {
                // Preserve donor resync behavior when a complete snapshot may
                // have been lost rather than applying an old queue prefix.
                if (garrison.size() == 1024) garrison.clear();
                garrison.emplace_back(reinterpret_cast<const char*>(data), length);
            }
            else
            {
                size_t begin = 0;
                for (size_t end = 0; end <= length; ++end)
                {
                    if (end != length && data[end] != '\n') continue;
                    if (area.size() == 256) area.pop_front();
                    area.emplace_back(reinterpret_cast<const char*>(data + begin), end - begin);
                    begin = end + 1;
                }
            }
        }
        catch (...) { Reset(); }
        return true;
    }

    void Reset() noexcept
    {
        try { std::lock_guard lock(mutex); ClearState(); stateSession = 0; } catch (...) {}
    }

    bool ValidateSend(uint16_t opcode, const void* data, uint32_t length) noexcept
    {
        const auto* bytes = static_cast<const uint8_t*>(data);
        if (opcode == 0x525) return length == 0;
        if (opcode == 0x552) return ValidGarrison(bytes, length, 254);
        if (opcode == 0x550)
            return bytes && length && length <= 64 &&
                std::all_of(bytes, bytes + length, [](uint8_t c) { return c >= 0x20 && c <= 0x7e; });
        return true;
    }

    void InitializeLua()
    {
        FrameScript_RegisterFunction("_WXLWOW_AREA_LOOT_SEND", SendArea);
        FrameScript_RegisterFunction("_WXLWOW_AREA_LOOT_POP", PopArea);
        FrameScript_RegisterFunction("_WXLWOW_AREA_LOOT_CLEAR", ClearArea);
        FrameScript_RegisterFunction("_WXL_GARRISON_SEND", SendGarrison);
        FrameScript_RegisterFunction("_WXL_GARRISON_POP", PopGarrison);
        FrameScript_RegisterFunction("_WXL_GARRISON_CLEAR", ClearGarrison);
        FrameScript_RegisterFunction("_WXLWOW_REQUEST_SPELL_CHARGES", RequestCharges);
        FrameScript_RegisterFunction("_WXLWOW_SPELL_CHARGE_SNAPSHOT", Snapshot);
        FrameScript_RegisterFunction("_WXLWOW_SPELL_CHARGE_ROW", Row);
        FrameScript_RegisterFunction("WXL64_AreaLootSend", SendArea);
        FrameScript_RegisterFunction("WXL64_AreaLootPop", PopArea);
        FrameScript_RegisterFunction("WXL64_AreaLootClear", ClearArea);
        FrameScript_RegisterFunction("WXL64_GarrisonSend", SendGarrison);
        FrameScript_RegisterFunction("WXL64_GarrisonPop", PopGarrison);
        FrameScript_RegisterFunction("WXL64_GarrisonClear", ClearGarrison);
        FrameScript_RegisterFunction("WXL64_RequestSpellCharges", RequestCharges);
        FrameScript_RegisterFunction("WXL64_SpellChargeSnapshot", Snapshot);
        FrameScript_RegisterFunction("WXL64_SpellChargeRow", Row);
        // Publish data APIs in both Glue and GameUI; action presentation waits
        // for UiReady after the root FrameXML has installed its globals.
        FrameScript_Execute(
            "wxlwow = wxlwow or {}; wxlwow.request_spell_charges = _WXLWOW_REQUEST_SPELL_CHARGES; "
            "wxlwow.spell_charge_snapshot = _WXLWOW_SPELL_CHARGE_SNAPSHOT; "
            "wxlwow.spell_charge_row = _WXLWOW_SPELL_CHARGE_ROW", "native-protocol-data-api", nullptr);
    }

    void UiReady()
    {
        FrameScript_Execute(SpellChargesBootstrap, "native-spell-charges", nullptr);
    }
}
