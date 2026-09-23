#ifndef _TRIAGE_CCLIENTOBJCREATE_H
#define _TRIAGE_CCLIENTOBJCREATE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "_Triage/CClientMoveUpdate.h"

class CClientObjCreate {
    public:
    /* 0000 */ CClientMoveUpdate m_move;
    /* 02A8 */ uint32_t unk_02A8;
    /* 02AC */ uint32_t m_flags;
    /* 02B0 */ uint32_t unk_02B0;
    /* 02B4 */ uint32_t m_lowGuid;
    /* 02B8 */ uint32_t unk_02B8;
    /* 02BC */ WGUID m_targetGuid;
    /* 02C4 */ uint32_t m_someTransportTime;
    /* 02C8 */ uint32_t m_vehicleId;
    /* 02CC */ uint32_t m_vehicleOrientation;
    /* 02D0 */ uint64_t m_packedRotation;
};

#endif
