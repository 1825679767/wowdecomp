#include "CGTabardCreationFrame.h"

// STATIC: 0x00C0E518
WOWGUID CGTabardCreationFrame::m_vendor;

// VERIFY: 4176cf42 auto
// OFFSET: 0x00598DC0
void CGTabardCreationFrame::EnterWorld() {
    CGTabardCreationFrame::m_vendor = WOWGUID();
}

// VERIFY: b533bac0 auto
// OFFSET: 0x007EAE60
uint32_t GuildGetTabardCost() {
    return 100000;
}
