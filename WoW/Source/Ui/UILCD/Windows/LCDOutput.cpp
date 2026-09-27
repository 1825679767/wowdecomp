#include "WoW/Source/Ui/UILCD/Windows/LCDOutput.h"

// VERIFY: 9a7faf5f auto
// OFFSET: 0x00959C80
CLCDOutput::CLCDOutput() {
    this->unk_004C = 0;
    this->unk_0050 = 0;
    this->unk_005C = 0;
    this->m_pGfx = nullptr;
    this->m_hDevice = LGLCD_INVALID_DEVICE;
    this->unk_0058 = 1;
    this->m_nPriority = LGLCD_PRIORITY_NORMAL;
    this->unk_0068 = 0;
    this->unk_006C = 0;
    this->unk_0070 = 0;
    this->unk_0074 = 0;
    this->unk_0078 = 0;
}

// VERIFY: 414b5d59 auto
// OFFSET: 0x0095A290
CLCDOutput::~CLCDOutput() {
    this->Close();
}

// VERIFY: 89d33d8c auto
// OFFSET: 0x00959D20
void CLCDOutput::Close() {
    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->OnClosingDevice(this->m_hDevice);
        lgLcdClose(this->m_hDevice);
        this->m_hDevice = LGLCD_INVALID_DEVICE;
    }
}

// VERIFY: 7579de46 auto
// OFFSET: 0x00959D10
void CLCDOutput::SetGfx(CLCDGfxBase* gfx) {
    this->m_pGfx = gfx;
}
