#ifndef WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H
#define WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H

#include <cstdint>
#include "Engine/Source/ShaderEffect/ShadowCache.h"

struct CVar;

extern CVar* g_shadowCull;
extern CVar* g_shadowScissor;
extern CVar* g_shadowInstancing;

class CShadowQuery {
    public:
    static void FreeBuffers();
    static void FrustumCallback(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band);
    static void Initialize();
    static void MatrixCallback(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band);
    static int32_t QueryCallback(CShadowCacheQueryData& data, uint32_t frame);
    static int32_t RenderCallback(CShadowCacheQueryData& data, int32_t band, CGxTex* target, CGxTex* colorTarget, C3Vector& direction);
};

#endif
