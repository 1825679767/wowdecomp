#ifndef WOW_SOURCE_WORLDCLIENT_MAPCHUNKLIQUID_H
#define WOW_SOURCE_WORLDCLIENT_MAPCHUNKLIQUID_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

class CGxBuf;
class CGxPool;
class CMapChunkBuf;
class CMapChunkLiquid;

class CMapChunkBuf {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ float m_flushTimer;
    /* 0008 */ uint32_t m_vertexDataSize;
    /* 000C */ uint32_t m_indexDataSize;
    /* 0010 */ CGxBuf* m_vertexBuf;
    /* 0014 */ CGxBuf* m_indexBuf;
    /* 0018 */ CGxPool* m_vertexPool;
    /* 001C */ CGxPool* m_indexPool;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
};

class CMapChunkLiquid {
    public:
    /* 0000 */ uint32_t m_vertexDataSize;
    /* 0004 */ uint32_t m_indexDataSize;
    /* 0008 */ TSExplicitList<CMapChunkBuf> m_liquids;
    /* 0014 */ TSLink<CMapChunkLiquid> m_link;
};

#endif
