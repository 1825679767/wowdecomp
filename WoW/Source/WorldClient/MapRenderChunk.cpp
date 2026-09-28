#include "MapRenderChunk.h"
#include <cstring>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"

// STATIC: 0x00AEEBF8
static uint32_t s_maxVertexCount = 145;

// STATIC: 0x00AEEC18
static uint16_t unk_AEEC18 = 768;

// STATIC: 0x00D1CFD8
static uint8_t s_defaultShadowRow[64];

// STATIC: 0x00D1D018
static uint8_t s_defaultAlphaRow[64];

// STATIC: 0x00AEEC70
TSExplicitList<CMapChunkBufBlock, offsetof(CMapChunkBufBlock, m_blockLink)> CMapRenderChunk::s_chunkBufBlockFreeList;

// STATIC: 0x00AEEC88
TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, unk_0014)> CMapRenderChunk::s_renderChunkBufFreeList;

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

// VERIFY: 73f06b5c manual
// OFFSET: 0x007BA5A0
void CMapRenderChunk::Destroy() {
    CMapRenderChunk::DestroyBufs();
    CMapRenderChunk::s_pnEstimateVertex = 0;
    CMapRenderChunk::s_pnEstimateIndex = 0;
    CMapRenderChunk::s_bPoolsDirty = 1;

    if (CMapRenderChunk::s_gxVertexPool) {
        g_theGxDevicePtr->PoolDestroy(CMapRenderChunk::s_gxVertexPool);
        CMapRenderChunk::s_gxVertexPool = nullptr;
    }

    if (CMapRenderChunk::s_gxIndexPool) {
        g_theGxDevicePtr->PoolDestroy(CMapRenderChunk::s_gxIndexPool);
        CMapRenderChunk::s_gxIndexPool = nullptr;
    }
}

// VERIFY: 61c7e992 manual
// OFFSET: 0x007BA3D0
void CMapRenderChunk::DestroyBufs() {
    CMapChunkBuf* buf = CMapRenderChunk::s_renderChunkBufFreeList.Head();

    while (buf) {
        CMapChunkBuf* next = CMapRenderChunk::s_renderChunkBufFreeList.Next(buf);
        buf->unk_0014.Unlink();
        buf = next;
    }

    CMapChunkBufBlock* block = CMapRenderChunk::s_chunkBufBlockFreeList.Head();

    while (block) {
        CMapChunkBufBlock* next = CMapRenderChunk::s_chunkBufBlockFreeList.Next(block);
        block->m_blockLink.Unlink();
        block->m_bufLink.Unlink();
        block = next;
    }

    for (uint32_t i = 0; i < CMapRenderChunk::s_chunkBlockArray.Count(); i++) {
        for (uint32_t j = 0; j < 2; j++) {
            CMapChunkBuf& chunkBuf = CMapRenderChunk::s_chunkBlockArray[i].m_bufs[j];

            if (chunkBuf.m_vertexBuf) {
                CGxBuf* vertexBuf = chunkBuf.m_vertexBuf;

                if (vertexBuf != g_theGxDevicePtr->BufStream(vertexBuf->m_pool->m_target, 0, 0)) {
                    g_theGxDevicePtr->BufDestroy(vertexBuf);
                }

                chunkBuf.m_vertexBuf = nullptr;
            }

            if (chunkBuf.m_indexBuf) {
                CGxBuf* indexBuf = chunkBuf.m_indexBuf;

                if (indexBuf != g_theGxDevicePtr->BufStream(indexBuf->m_pool->m_target, 0, 0)) {
                    g_theGxDevicePtr->BufDestroy(indexBuf);
                }

                chunkBuf.m_indexBuf = nullptr;
            }
        }
    }

    CMapRenderChunk::s_chunkBlockArray.SetCount(0);
}

// INLINED
CMapChunkBuf::CMapChunkBuf() {
    this->m_vertexBuf = nullptr;
    this->m_indexBuf = nullptr;
    this->m_renderChunk = nullptr;
    this->m_block = nullptr;
    this->m_unkFlags = 0;
}

// INLINED
CMapChunkBufBlock::CMapChunkBufBlock() {
    this->m_vertexCount = 0;
}
