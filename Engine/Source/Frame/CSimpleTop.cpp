#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

// STATIC: 0x00B499A8
CSimpleTop* CSimpleTop::s_instance;

// VERIFY: 3a1b6d83 auto
// OFFSET: 0x004883B0
CSimpleTop* CSimpleTop::GetInstance() {
    return CSimpleTop::s_instance;
}

// VERIFY: df78660e auto
// OFFSET: 0x00494F90
void CSimpleTop::RegisterFrame(CSimpleFrame* frame) {
    this->m_frames.LinkToTail(frame);
}

// VERIFY: b22dcea3 auto
// OFFSET: 0x00495050
void CSimpleTop::RegisterForCleanup(CSimpleFrame* frame) {
    this->m_destroyed.LinkToTail(frame);
}
