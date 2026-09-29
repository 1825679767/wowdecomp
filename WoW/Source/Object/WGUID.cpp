#include "WGUID.h"
#include "Engine/Source/Base/CDataStore.h"

// VERIFY: df1d6b6b auto
// OFFSET: 0x0076DC20
CDataStore& operator>>(CDataStore& msg, SmartGUID& guid) {
    guid.m_guid = 0;

    uint8_t mask;
    msg.Get(mask);

    for (uint32_t i = 0, bit = 0; i < 64; i += 8, bit++) {
        if (mask & (1 << bit)) {
            uint8_t byte;
            msg.Get(byte);
            guid.m_guid |= static_cast<uint64_t>(byte) << i;
        }
    }

    return msg;
}
