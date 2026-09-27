#if defined(WOWDECOMP_ARCH_32)

#include <winsock2.h>
#include <windows.h>
#include "WoW/Source/WowServices/WowConnectionNet.h"

// VERIFY: 825fe3ba auto
// OFFSET: 0x004697E0
void WowConnectionNet::PlatformChangeState(WowConnection* connection, WOW_CONN_STATE state) {
    long networkEvents = 0;

    switch (connection->m_connState) {
        case WOWC_CONNECTING:
            networkEvents = FD_CONNECT | FD_CLOSE | FD_WRITE;
            break;

        case WOWC_LISTENING:
            networkEvents = FD_ACCEPT;
            break;

        case WOWC_CONNECTED:
            networkEvents = connection->m_sendList.Head() || connection->unk_0140 ? FD_READ | FD_WRITE | FD_CLOSE : FD_READ | FD_CLOSE;
            break;
    }

    if (connection->m_event && connection->m_sock >= 0) {
        WSAEventSelect(connection->m_sock, connection->m_event, networkEvents);
    }

    SetEvent(this->m_event);
}

// VERIFY: db5d8368 manual
// OFFSET: 0x00469860
void WowConnectionNet::PlatformRun() {
    this->m_networkStarted = true;

    DWORD timeout = 500;

    if (this->m_stop) {
        return;
    }

    do {
        WowConnection* connections[MAXIMUM_WAIT_OBJECTS];
        HANDLE handles[MAXIMUM_WAIT_OBJECTS];

        this->m_connectionsLock.Enter();

        connections[0] = nullptr;
        handles[0] = this->m_event;
        DWORD count = 1;

        for (WowConnection* connection = this->m_connections.Head(); connection; connection = this->m_connections.Next(connection)) {
            if (connection->unk_0130) {
                continue;
            }

            HANDLE event = connection->m_event;

            switch (connection->m_connState) {
                case WOWC_CONNECTING:
                    WSAEventSelect(connection->m_sock, event, FD_CONNECT | FD_CLOSE);
                    break;

                case WOWC_LISTENING:
                    WSAEventSelect(connection->m_sock, event, FD_ACCEPT);
                    break;

                case WOWC_CONNECTED:
                    WSAEventSelect(connection->m_sock, event, (connection->m_sendList.Head() || connection->unk_0140 ? FD_WRITE : 0) | FD_READ | FD_CLOSE);
                    break;

                case WOWC_DISCONNECTING:
                    timeout = 0;
                    break;

                default:
                    continue;
            }

            if (count < MAXIMUM_WAIT_OBJECTS) {
                connection->m_refCount.Increment();
                handles[count] = event;
                connections[count] = connection;
                count++;

                if (connection->unk_00FC.Head()) {
                    timeout = 0;
                }
            }
        }

        this->m_connectionsLock.Leave();

        DWORD result = WaitForMultipleObjects(count, handles, FALSE, timeout);

        if (result < count) {
            if (result == WAIT_OBJECT_0) {
                ResetEvent(this->m_event);
            }

            for (DWORD i = 1; i < count; i++) {
                WowConnection* connection = connections[i];

                WSANETWORKEVENTS networkEvents;
                WSAEnumNetworkEvents(connection->m_sock, handles[i], &networkEvents);

                uint32_t flags = 0;

                if (networkEvents.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE)) {
                    flags = 2;
                }

                if (networkEvents.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE)) {
                    flags |= 1;
                }

                if (connection->m_connState == WOWC_DISCONNECTING) {
                    flags |= 8;
                }

                if (WowConnection::s_unkB38D20 || WowConnection::s_unkB38D24) {
                    flags |= 3;
                    timeout = 10;
                } else {
                    timeout = 500;
                }

                if (flags) {
                    this->SignalWorker(connection, flags);
                }
            }
        }

        for (DWORD i = 1; i < count; i++) {
            connections[i]->Release();
        }
    } while (!this->m_stop);
}

// VERIFY: 10cfb4a7 auto
// OFFSET: 0x004696E0
void WowConnectionNet::PlatformInit(bool useEngine) {
    WSADATA data;

    if (!WSAStartup(MAKEWORD(2, 2), &data) && data.wVersion == MAKEWORD(2, 2)) {
        this->m_event = CreateEventA(nullptr, TRUE, FALSE, nullptr);
    }
}

#endif
