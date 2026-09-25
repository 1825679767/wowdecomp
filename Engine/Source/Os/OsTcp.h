#ifndef ENGINE_SOURCE_OS_OSTCP_H
#define ENGINE_SOURCE_OS_OSTCP_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/Types.h"

union NETADDR {
    /* 0000 */ uint8_t data[16];
};

namespace OsNet {
extern const char* OSNETERR_INTERNAL;

struct TCPNET {
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ TSGrowableArray<void*> m_pumpThreads;
    /* 0018 */ uint8_t unk_0018[1528];
    /* 0610 */ void* m_port;

    static TCPNET* s_pnet;
    static HSLOG s_log;

    static void LogWrite(const char* format, ...);

    void IoPump(uint32_t timeout);
    void Pump(uint32_t timeout);
};
}

uint32_t OsNetAddrGetAddress(const NETADDR* addr, uint16_t* port);
void OsNetAddrMake(uint32_t address, uint16_t port, NETADDR* addr);
const char* OsNetAddrToStr(const NETADDR* addr, char* buffer, uint32_t size);
void OsNetPump(uint32_t timeout);

#endif
