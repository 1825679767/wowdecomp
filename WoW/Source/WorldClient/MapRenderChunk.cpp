#include "MapRenderChunk.h"
#include <cstring>

// STATIC: 0x00AEEBF8
static uint32_t s_maxVertexCount = 145;

// STATIC: 0x00AEEC18
static uint16_t unk_AEEC18 = 768;

// STATIC: 0x00D1CFD8
static uint8_t s_defaultShadowRow[64];

// STATIC: 0x00D1D018
static uint8_t s_defaultAlphaRow[64];

// STATIC: 0x00AEEC60
CGxBatch CMapRenderChunk::unk_AEEC60;

// STATIC: 0x00D1D058
int32_t CMapRenderChunk::s_bPoolsDirty;

// STATIC: 0x00D1D05C
uint32_t CMapRenderChunk::s_pnEstimateIndex;

// STATIC: 0x00D1D060
uint32_t CMapRenderChunk::s_pnEstimateVertex;

// STATIC: 0x00D1D064
CGxPool* CMapRenderChunk::s_gxIndexPool;

// STATIC: 0x00D1D068
CGxPool* CMapRenderChunk::s_gxVertexPool;

// STATIC: 0x00D1D06C
EGxVertexBufferFormat CMapRenderChunk::s_gxBufVertexFormat;

// STATIC: 0x00D252F0
TSGrowableArray<CMapChunkBufBlock> CMapRenderChunk::s_chunkBlockArray;

// VERIFY: b7c2a1ad auto
// OFFSET: 0x007B7320
void CMapRenderChunk::SetDirty() {
    CMapRenderChunk::s_bPoolsDirty = 1;
}

// VERIFY: a2b225c8 auto
// OFFSET: 0x007BA340
void CMapRenderChunk::Initialize() {
    CMapRenderChunk::s_bPoolsDirty = 1;
    CMapRenderChunk::s_gxBufVertexFormat = GxVBF_PN;
    CMapRenderChunk::unk_AEEC60.m_maxIndex = s_maxVertexCount - 1;
    CMapRenderChunk::s_pnEstimateVertex = 0;
    CMapRenderChunk::s_pnEstimateIndex = 0;
    CMapRenderChunk::unk_AEEC60.m_primType = GxPrim_Triangles;
    CMapRenderChunk::unk_AEEC60.m_count = unk_AEEC18;
    CMapRenderChunk::unk_AEEC60.m_start = 0;
    CMapRenderChunk::unk_AEEC60.m_minIndex = 0;
    CMapRenderChunk::s_gxVertexPool = nullptr;
    CMapRenderChunk::s_gxIndexPool = nullptr;
    CMapRenderChunk::s_chunkBlockArray.SetCount(0);
    memset(s_defaultAlphaRow, 0, sizeof(s_defaultAlphaRow));
    memset(s_defaultShadowRow, 0, sizeof(s_defaultShadowRow));
}
