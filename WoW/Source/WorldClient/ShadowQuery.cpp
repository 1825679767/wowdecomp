#include "ShadowQuery.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D25320
ShadowQueryBuffers CShadowQuery::s_unk_D25320[3];

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

// OFFSET: 0x007BA7D0
void CShadowQuery::FreeBuffers() {
    for (ShadowQueryBuffers* buffers = CShadowQuery::s_unk_D25320; buffers < CShadowQuery::s_unk_D25320 + sizeof(CShadowQuery::s_unk_D25320) / sizeof(CShadowQuery::s_unk_D25320[0]); buffers++) {
        if (buffers->unk_0004) {
            SMemFree(buffers->unk_0004, nullptr, 0, 0);
        }

        if (buffers->unk_0008) {
            SMemFree(buffers->unk_0008, nullptr, 0, 0);
        }

        delete[] buffers->unk_000C;
        delete[] buffers->unk_0018;
    }
}
