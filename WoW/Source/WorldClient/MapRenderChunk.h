#ifndef WOW_SOURCE_WORLDCLIENT_MAPRENDERCHUNK_H
#define WOW_SOURCE_WORLDCLIENT_MAPRENDERCHUNK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxBuf;
struct CGxPool;
class CMapChunk;
class CMapChunkBufBlock;
class CMapRenderChunk;
class CMapChunkBuf;
class CTexture;

class CMapChunkBuf {
    public:
    /* 0000 */ CGxBuf* m_vertexBuf;
    /* 0004 */ CGxBuf* m_indexBuf;
    /* 0008 */ CMapRenderChunk* m_renderChunk;
    /* 000C */ CMapChunkBufBlock* m_block;
    /* 0010 */ uint32_t m_unkFlags;
    /* 0014 */ TSLink<CMapChunkBuf> unk_0014;

    CMapChunkBuf();
};

class CMapChunkBufBlock {
    public:
    /* 0000 */ uint32_t m_vertexCount;
    /* 0004 */ CMapChunkBuf m_bufs [2];
    /* 003C */ TSLink<CMapChunkBufBlock> m_blockLink;
    /* 0044 */ TSLink<CMapChunkBufBlock> m_bufLink;

    CMapChunkBufBlock();
};

class CMapRenderChunkLayer {
    public:
    /* 0000 */ int16_t m_flags;
    /* 0002 */ int16_t m_layerIndex;
    /* 0004 */ CTexture* m_texture;
    /* 0008 */ int32_t m_textureId;
    /* 000C */ int32_t m_layerTexture;
    /* 0010 */ CMapRenderChunk* m_owner;
};

class CMapRenderChunk {
    public:
    static int32_t s_bPoolsDirty;
    static EGxVertexBufferFormat s_gxBufVertexFormat;
    static uint32_t s_pnEstimateVertex;
    static uint32_t s_pnEstimateIndex;
    static CGxPool* s_gxVertexPool;
    static CGxPool* s_gxIndexPool;
    static TSGrowableArray<CMapChunkBufBlock> s_chunkBlockArray;
    static TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, unk_0014)> s_renderChunkBufFreeList;
    static TSExplicitList<CMapChunkBufBlock, offsetof(CMapChunkBufBlock, m_blockLink)> s_chunkBufBlockFreeList;
    static CGxBatch unk_AEEC60;

    static void Destroy();
    static void DestroyBufs();
    static void Initialize();
    static void SetDirty();

    ~CMapRenderChunk();

    /* 0000 */ TSLink<CMapRenderChunk> m_renderChunkLink;
    /* 0008 */ uint8_t m_unkFlags;
    /* 0009 */ uint8_t m_layersCount;
    /* 000A */ int16_t unk_000A;
    /* 000C */ float m_lastUpdateTime;
    /* 0010 */ CMapChunk* m_mapChunkPtr1;
    /* 0014 */ CMapChunk* m_mapChunkPtr2;
    /* 0018 */ C3Vector m_vec1;
    /* 0024 */ CAaSphere m_sphere;
    /* 0034 */ CMapRenderChunkLayer m_layers [4];
    /* 0084 */ CTexture* m_terrainBlendTexture;
    /* 0088 */ CTexture* m_shadowTexture;
    /* 008C */ CMapChunkBuf* m_chunkBuf;
    /* 0090 */ CGxBatch m_batch;
};

struct SMMapHeader {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_something;
    /* 0008 */ uint32_t m_unused [6];
};

struct TextureLayerInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint8_t* m_alphaData;
};

#endif
