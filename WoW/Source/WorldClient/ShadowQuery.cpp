#include "ShadowQuery.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"

// VERIFY: a0192ad6 auto
// OFFSET: 0x007BD3A0
void CShadowQuery::Initialize() {
    CShadowCache::SetMatrixCallback(CShadowQuery::MatrixCallback);
    CShadowCache::SetFrustumCallback(CShadowQuery::FrustumCallback);
    CShadowCache::SetQueryCallback(CShadowQuery::QueryCallback);
    CShadowCache::SetRenderCallback(CShadowQuery::RenderCallback);

    g_shadowCull = CVar::Register("shadowCull", "enable shadow frustum culling", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    g_shadowScissor = CVar::Register("shadowScissor", "enable scissoring when rendering shadowmaps", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    g_shadowInstancing = CVar::Register("shadowInstancing", "enable instancing when rendering shadowmaps", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);

    CShadowCache::Init();
}
