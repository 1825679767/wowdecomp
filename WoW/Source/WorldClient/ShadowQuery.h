#ifndef WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H
#define WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H

#include <cstdint>
#include "Engine/Source/ShaderEffect/ShadowCache.h"

struct CVar;

extern CVar* g_shadowCull;
extern CVar* g_shadowScissor;
extern CVar* g_shadowInstancing;

struct ShadowQueryBuffers {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ void* unk_0004;
    /* 0008 */ void* unk_0008;
    /* 000C */ uint8_t* unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t* unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
};

class CShadowQuery {
    public:
    static ShadowQueryBuffers s_unk_D25320[3];

    static void FreeBuffers();
    static void FrustumCallback(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band);
    static void Initialize();
    static void MatrixCallback(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band);
    static int32_t QueryCallback(CShadowCacheQueryData& data, uint32_t frame);
    static int32_t RenderCallback(CShadowCacheQueryData& data, int32_t band, CGxTex* target, CGxTex* colorTarget, C3Vector& direction);
};

#endif
