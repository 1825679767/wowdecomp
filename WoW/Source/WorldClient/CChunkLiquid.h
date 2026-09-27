#ifndef WOW_SOURCE_WORLDCLIENT_CCHUNKLIQUID_H
#define WOW_SOURCE_WORLDCLIENT_CCHUNKLIQUID_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C2iVector.h"

class CChunkLiquid;
class CMapChunk;
class CMapChunkBuf;

class CChunkGeomFactory {
    public:
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ CChunkLiquid* m_liquidChunk;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ CMapChunkBuf* m_mapChunkBuf;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint16_t m_unkFlags2c;
    /* 002E */ uint16_t m_unkFlags2e;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ C44Matrix m_matrix;

    virtual ~CChunkGeomFactory();
};

class CChunkLiquid {
    public:
    /* 0000 */ int32_t m_objectIndex;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ C3Vector m_topLeftCoords;
    /* 0018 */ C3Vector m_center;
    /* 0024 */ float m_radius;
    /* 0028 */ C2Vector m_height;
    /* 0030 */ float m_resPurgeTimer;
    /* 0034 */ C2iVector m_tileBegin;
    /* 003C */ C2iVector m_tileEnd;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ uint8_t* m_tiles;
    /* 0058 */ uint8_t* m_liquidInstPtr;
    /* 005C */ CMapChunk* m_owner;
    /* 0060 */ CChunkLiquid* m_prev;
    /* 0064 */ CChunkLiquid* m_next;
    /* 0068 */ int32_t unk_0068;
    /* 006C */ int32_t unk_006C;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ int32_t unk_0074;
    /* 0078 */ C3Vector m_verts [81];
};

#endif
