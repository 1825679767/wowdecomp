// Read-only equipped/applied state bridge for the Retail transmog editor.
#include "ExtensionApi.hpp"
#include "TransmogEquipmentWire.hpp"


#include <mutex>

namespace
{
namespace Wire = WXL::TransmogEquipmentWire;

std::mutex mutex;
Wire::State current;
uint32_t requested = 0, generation = 0;
bool valid = false;

void __cdecl OnState(const uint8_t* data, uint32_t size, void*)
{
    Wire::State incoming;
    if (!Wire::Decode({data, size}, incoming)) return;
    std::lock_guard lock(mutex);
    if (!requested || incoming.RequestId != requested || valid) return;
    current = incoming; valid = true; ++generation;
}
int __cdecl Request(lua_State* state)
{
    std::vector<uint8_t> bytes;
    {
        std::lock_guard lock(mutex);
        if (!++requested) ++requested;
        valid = false;
        Wire::Put(bytes, Wire::Version, 1); Wire::Put(bytes, requested, 4);
    }
    bool sent = wxl_retail_ui::Network()->Send(Wire::RequestOpcode, bytes.data(), uint32_t(bytes.size())) != 0;
    lua_pushboolean(state, sent);
    return 1;
}
int __cdecl Reset(lua_State*)
{
    std::lock_guard lock(mutex);
    // Advance rather than reset the sequence: old replies cannot become fresh after relog.
    if (!++requested) ++requested;
    current = {}; valid = false; ++generation;
    return 0;
}
int __cdecl Status(lua_State* state)
{
    std::lock_guard lock(mutex);
    lua_pushboolean(state, valid);
    lua_pushnumber(state, generation);
    lua_pushboolean(state, valid && current.Enabled);
    return 3;
}
int __cdecl ReadSlot(lua_State* state)
{
    double index = lua_tonumber(state, 1);
    std::lock_guard lock(mutex);
    if (!valid || !(index >= 1 && index <= Wire::SlotCount) || index != int(index))
    {
        lua_pushnil(state);
        return 1;
    }
    auto const& slot = current.Slots[size_t(index - 1)];
    auto push = lua_pushnumber;
    push(state, slot.ItemGuid); push(state, slot.ItemId); push(state, slot.SourceKind);
    push(state, slot.SourceId); push(state, slot.DisplayId);
    return 5;
}
const char* const bootstrap =
#include "RetailTransmogEquipmentLua.inc"
;
}
bool wxl_retail_ui::InstallRetailTransmogEquipment()
{
    auto network = Network(); auto script = FrameScript();
    bool ok = network->RegisterClientOpcode(Wire::RequestOpcode, "CMSG_WXL_TRANSMOG_EQUIPMENT_REQUEST") != 0;
    ok &= network->RegisterServerOpcode(Wire::StateOpcode, "SMSG_WXL_TRANSMOG_EQUIPMENT_STATE", &OnState, nullptr) != 0;
    ok &= script->RegisterFunction("_WXL_TRANSMOG_EQUIPMENT_REQUEST", &Request) != 0;
    ok &= script->RegisterFunction("_WXL_TRANSMOG_EQUIPMENT_RESET", &Reset) != 0;
    ok &= script->RegisterFunction("_WXL_TRANSMOG_EQUIPMENT_STATUS", &Status) != 0;
    ok &= script->RegisterFunction("_WXL_TRANSMOG_EQUIPMENT_SLOT", &ReadSlot) != 0;
    ok &= script->RegisterScript("retail-transmog-equipment", bootstrap) != 0;
    return ok;
}

void wxl_retail_ui::ResetEquipment(){Reset(nullptr);}
