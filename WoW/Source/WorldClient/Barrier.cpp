#include "Barrier.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"

// STATIC: 0x00CD85F8
CM2Model* CBarrier::s_sparkles[4];

// STATIC: 0x00CD8608
CTexture* CBarrier::s_pattern;

// STATIC: 0x00CD860C
CTexture* CBarrier::s_mask;

// VERIFY: 6badcb93 manual
// OFFSET: 0x00799730
void CBarrier::Initialize() {
    CGxTexFlags flags;
    CStatus status;

    flags.m_filter = GxTex_Linear;
    flags.m_wrapU = 0;
    flags.m_wrapV = 0;
    CBarrier::s_mask = TextureCreate("XTextures\\FX\\LBTBarrierMask", flags, &status, 0);

    flags.m_wrapU = 1;
    flags.m_wrapV = 1;
    CBarrier::s_pattern = TextureCreate("XTextures\\FX\\LBTBarrier", flags, &status, 0);
}

// VERIFY: f774ccf6 manual
// OFFSET: 0x0078F510
void CBarrier::Destroy() {
    for (uint32_t i = 0; i < sizeof(CBarrier::s_sparkles) / sizeof(CBarrier::s_sparkles[0]); i++) {
        if (CBarrier::s_sparkles[i]) {
            CBarrier::s_sparkles[i]->Release();
            CBarrier::s_sparkles[i] = nullptr;
        }
    }

    if (CBarrier::s_mask) {
        HandleClose(reinterpret_cast<HOBJECT>(CBarrier::s_mask));
        CBarrier::s_mask = nullptr;
    }

    if (CBarrier::s_pattern) {
        HandleClose(reinterpret_cast<HOBJECT>(CBarrier::s_pattern));
        CBarrier::s_pattern = nullptr;
    }
}
