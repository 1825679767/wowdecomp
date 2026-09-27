#include "WoW/Source/Ui/UILCD/Windows/LCDBase.h"

// VERIFY: 25c2f114 auto
// OFFSET: 0x0095BC70
CLCDBase::CLCDBase() {
    this->m_Size.cx = 0;
    this->m_Size.cy = 0;
    this->m_Origin.x = 0;
    this->m_Origin.y = 0;
    this->m_bVisible = 1;
    this->m_bInverted = 0;
    this->unk_001C = 0;
    this->unk_0020 = 0;
    this->unk_0024 = 0;
    this->unk_0028 = 0;
    this->m_nBkMode = TRANSPARENT;
    this->m_nObjectType = 4;
    this->m_crBackgroundColor = RGB(0, 0, 0);
    this->m_crForegroundColor = RGB(255, 255, 255);
}

// VERIFY: de2ec0bb auto
// OFFSET: 0x0095BCC0
CLCDBase::~CLCDBase() {
}
