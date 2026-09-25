#if defined(WOWDECOMP_ARCH_32)

#include <cstdarg>
#include <cstdint>
#include <winsock2.h>
#include <windows.h>
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B1D3F4
const char* OsNet::OSNETERR_INTERNAL = "Internal error";

// STATIC: 0x00D41830
OsNet::TCPNET* OsNet::TCPNET::s_pnet;

// STATIC: 0x00D41674
HSLOG OsNet::TCPNET::s_log;

// VERIFY: ae872b98 auto
// OFFSET: 0x0086D770
void OsNet::TCPNET::LogWrite(const char* format, ...) {
    char line[1024];

    DWORD lastError = GetLastError();
    int wsaError = WSAGetLastError();
    uint32_t length = static_cast<uint32_t>(SStrPrintf(line, sizeof(line), "t[0x%08x] se[%u] we[%u] ", GetCurrentThreadId(), wsaError, lastError));

    va_list arglist;
    va_start(arglist, format);
    SStrVPrintf(&line[length], sizeof(line) - length, format, arglist);

    DebugLog("OsNet: (INFO): %s\n", line);

    if (OsNet::TCPNET::s_log) {
        SLogWrite(OsNet::TCPNET::s_log, line);
        SLogFlush(OsNet::TCPNET::s_log);
    }
}

// VERIFY: 68e53455 auto
// OFFSET: 0x0086DBF0
void OsNetAddrMake(uint32_t address, uint16_t port, NETADDR* addr) {
    sockaddr_in* in = reinterpret_cast<sockaddr_in*>(addr);
    in->sin_family = AF_INET;
    in->sin_port = htons(port);
    in->sin_addr.s_addr = address;
    *reinterpret_cast<uint32_t*>(&in->sin_zero[0]) = 0;
    *reinterpret_cast<uint32_t*>(&in->sin_zero[4]) = 0;
}

// VERIFY: 2bad892b auto
// OFFSET: 0x0086DC20
uint32_t OsNetAddrGetAddress(const NETADDR* addr, uint16_t* port) {
    const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(addr);

    if (port) {
        *port = ntohs(in->sin_port);
    }

    return in->sin_addr.s_addr;
}

// VERIFY: 731d2940 auto
// OFFSET: 0x0086DC50
const char* OsNetAddrToStr(const NETADDR* addr, char* buffer, uint32_t size) {
    const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(addr);
    SStrPrintf(buffer, size, "%s:%u", inet_ntoa(in->sin_addr), ntohs(in->sin_port));
    return buffer;
}

// INLINED
void OsNet::TCPNET::Pump(uint32_t timeout) {
    if (this->m_pumpThreads.Count()) {
        Sleep(timeout);
    } else if (this->m_port) {
        this->IoPump(timeout);
    } else {
        OsNet::TCPNET::LogWrite("%s 7", OsNet::OSNETERR_INTERNAL);
        Sleep(timeout);
    }
}

// VERIFY: 8b85bf60 manual
// OFFSET: 0x0086F1C0
void OsNetPump(uint32_t timeout) {
    if (OsNet::TCPNET::s_pnet) {
        OsNet::TCPNET::s_pnet->Pump(timeout);
    } else {
        Sleep(timeout);
    }
}

#endif
