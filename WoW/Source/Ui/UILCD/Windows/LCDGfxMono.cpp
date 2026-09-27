#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

// VERIFY: e375fb59 auto
// OFFSET: 0x0095BEA0
CLCDGfxMono::CLCDGfxMono() {
    this->m_pLCDScreen = &this->m_lcdBitmap.hdr;
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
}

// VERIFY: 0fa5b226 manual
// OFFSET: 0x0095BF60
CLCDGfxMono::~CLCDGfxMono() {
}

// VERIFY: 5c18fa55 auto
// OFFSET: 0x0095BEC0
HRESULT CLCDGfxMono::Initialize() {
    this->Shutdown();

    this->m_nWidth = LGLCD_BMP_WIDTH;
    this->m_nHeight = LGLCD_BMP_HEIGHT;

    HRESULT hr = CLCDGfxBase::Initialize();

    if (FAILED(hr)) {
        return hr;
    }

    hr = this->CreateBitmap(8);

    return FAILED(hr) ? hr : S_OK;
}
