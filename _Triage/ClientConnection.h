#ifndef _TRIAGE_CLIENTCONNECTION_H
#define _TRIAGE_CLIENTCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/WowServices/RealmConnection.h"
#include "_Triage/Types.h"

class ClientConnection {
    public:
    /* 0000 */ RealmConnection unk_0000;
    /* 2F30 */ int32_t m_connected;
    /* 2F34 */ int32_t m_statusComplete;
    /* 2F38 */ int32_t m_statusResult;
    /* 2F3C */ WOWCS_OPS m_statusCop;
    /* 2F40 */ int32_t m_errorCode;
    /* 2F44 */ uint8_t unk_2F44;
    /* 2F45 */ uint8_t unk_2F45[3];
    /* 2F48 */ void (__stdcall* m_cleanup )();
};

#endif
