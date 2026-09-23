#ifndef _TRIAGE_CCLIENTMOVEUPDATE_H
#define _TRIAGE_CCLIENTMOVEUPDATE_H

#include <cstddef>
#include <cstdint>
#include "_Triage/CMoveSpline.h"
#include "_Triage/CMovementStatus.h"

class CClientMoveUpdate {
    public:
    /* 0000 */ CMovementStatus m_status;
    /* 0060 */ float m_walkSpeed;
    /* 0064 */ float m_runSpeed;
    /* 0068 */ float m_runBackSpeed;
    /* 006C */ float m_swimSpeed;
    /* 0070 */ float m_swimBackSpeed;
    /* 0074 */ float m_flightSpeed;
    /* 0078 */ float m_flightBackSpeed;
    /* 007C */ float m_turnRate;
    /* 0080 */ float m_pitchRate;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ CMoveSpline m_moveSpline;
};

#endif
