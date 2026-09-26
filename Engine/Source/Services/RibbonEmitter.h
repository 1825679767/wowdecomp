#ifndef ENGINE_SOURCE_SERVICES_RIBBONEMITTER_H
#define ENGINE_SOURCE_SERVICES_RIBBONEMITTER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CAaBox.h"

class C3Vector;
class CAaRect;
class CImVector;
class C44Matrix;
class CRibbonEmitter;
class CTexture;

class CRibbonMat {
    public:
    /* 0000 */ uint32_t m_lit : 1;
    uint32_t m_fogged : 1;
    uint32_t m_depthTest : 1;
    uint32_t m_depthWrite : 1;
    uint32_t m_cull : 1;
    /* 0004 */ uint32_t m_blendMode;

    CRibbonMat();
};

class CRibbonEmitter {
    public:
    /* 0000 */ uint8_t unk_0000 [0xF4];
    /* 00F4 */ CAaBox m_worldBounds;
    /* 010C */ uint8_t unk_010C [0x74];

    CRibbonEmitter();
    ~CRibbonEmitter();
    void Initialize(float edgesPerSecond, float edgeLifetime, const CImVector& color, const TSGrowableArray<CTexture*>& textures, const TSGrowableArray<CRibbonMat>& materials, const TSGrowableArray<uint32_t>& textureTypes, const CAaRect& texCoords, uint32_t rows, uint32_t cols);
    void SetGravity(float gravity);
    void ChangeFrameOfReference(const C44Matrix* matrix);
    uint32_t GetNumBatches();
    uint32_t GetNumPrimitives();
    int32_t IsDead();
    void Render(void* frameOfReference);
    void ReplaceTexture(uint32_t type, CTexture* texture);
    void SetAbove(float above);
    void SetAlpha(float alpha);
    void SetBelow(float below);
    void SetColor(float r, float g, float b);
    void SetDataEnabled(int32_t enabled);
    void SetPos(const C44Matrix& transform, const C3Vector& position, const C44Matrix* frameOfReference);
    void SetTexSlot(uint32_t slot);
    void SetUserEnabled(uint32_t enabled);
    void Update(float deltaTime, int32_t suppressNewEdges);
};

#endif
