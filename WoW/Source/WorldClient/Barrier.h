#ifndef WOW_SOURCE_WORLDCLIENT_BARRIER_H
#define WOW_SOURCE_WORLDCLIENT_BARRIER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CImVector.h"

class CBarrier_Contact;
class CGxVertexPCT0T1;
class CTexture;
class CMapChunk;
class CMapObjDefGroup;

class CBarrier {
    public:
    static CTexture* s_mask;
    static CTexture* s_pattern;

    static void Initialize();

    /* 0000 */ int32_t m_mode;
    /* 0004 */ uint32_t m_vertAlloc;
    /* 0008 */ uint32_t m_vertCount;
    /* 000C */ CGxVertexPCT0T1* m_verts;
    /* 0010 */ uint32_t m_vertChunk;
    /* 0014 */ uint32_t m_contactAlloc;
    /* 0018 */ uint32_t m_contactCount;
    /* 001C */ CBarrier_Contact* m_contacts;
    /* 0020 */ uint32_t m_contactChunk;
    /* 0024 */ uint32_t m_chunkAlloc;
    /* 0028 */ uint32_t m_chunkCount;
    /* 002C */ CMapChunk** m_chunks;
    /* 0030 */ uint32_t m_chunkChunk;
    /* 0034 */ uint32_t m_groupAlloc;
    /* 0038 */ uint32_t m_groupCount;
    /* 003C */ CMapObjDefGroup** m_groups;
    /* 0040 */ uint32_t m_groupChunk;
    /* 0044 */ C3Vector m_viewerPos;
};

class CBarrier_Contact {
    public:
    /* 0000 */ C3Vector m_point;
    /* 000C */ float m_strength;
    /* 0010 */ C4Plane m_plane;
};

class CGxVertexPCT0T1 {
    public:
    /* 0000 */ C3Vector m_pos;
    /* 000C */ CImVector m_color;
    /* 0010 */ float m_t0x;
    /* 0014 */ float m_t0y;
    /* 0018 */ float m_t1x;
    /* 001C */ float m_t1y;
};

#endif
