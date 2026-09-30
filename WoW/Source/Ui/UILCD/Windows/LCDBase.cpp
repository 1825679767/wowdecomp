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

// VERIFY: 21134cdc auto
// OFFSET: 0x00959CD0
int32_t CLCDBase::GetWidth() {
    return this->GetSize().cx;
}

// VERIFY: ce3beab0 auto
// OFFSET: 0x00959CE0
int32_t CLCDBase::GetHeight() {
    return this->GetSize().cy;
}

// VERIFY: 638d71cb auto
// OFFSET: 0x0095BD70
SIZE& CLCDBase::GetSize() {
    return this->m_Size;
}

// VERIFY: c4c3b568 auto
// OFFSET: 0x0095BE30
void CLCDBase::SetBackgroundMode(int32_t mode) {
    this->m_nBkMode = mode;
}

// VERIFY: 31752403 auto
// OFFSET: 0x009C1FD0
POINT& CLCDBase::GetOrigin() {
    return this->m_Origin;
}

// VERIFY: 3a4f2369 auto
// OFFSET: 0x0095BCD0
void CLCDBase::SetOriginXY(int32_t x, int32_t y) {
    this->m_Origin.x = x;
    this->m_Origin.y = y;
}

// VERIFY: 4424a140 auto
// OFFSET: 0x0095BCF0
void CLCDBase::SetOrigin(POINT pt) {
    this->SetOriginXY(pt.x, pt.y);
}

// VERIFY: f615e99a auto
// OFFSET: 0x0095BD10
void CLCDBase::SetSize(SIZE& size) {
    this->m_Size = size;
    this->SetLogicalSize(this->m_Size);
}

// VERIFY: 22bef7d7 auto
// OFFSET: 0x0095BD40
void CLCDBase::SetSizeWH(int32_t width, int32_t height) {
    SIZE size;
    size.cx = width;
    size.cy = height;
    this->SetSize(size);
}
