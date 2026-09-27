#include "Barrier.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"

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
