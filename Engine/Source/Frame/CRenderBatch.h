#ifndef ENGINE_SOURCE_FRAME_CRENDERBATCH_H
#define ENGINE_SOURCE_FRAME_CRENDERBATCH_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

class CGxStringBatch;
class CRenderBatch;
struct CSimpleBatchedMesh;
struct RENDERCALLBACKNODE;

class CGxStringBatch {
    public:
    /* 0000 */ uint32_t m_vftable;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
};

struct RENDERCALLBACKNODE {
    /* 0000 */ void* m_callback;
    /* 0004 */ void* m_param;
};

class CRenderBatch {
    public:
    /* 0004 */ uint32_t m_count;
    /* 0008 */ TSGrowableArray<CSimpleBatchedMesh> m_texturelist;
    /* 0018 */ CGxStringBatch* m_stringbatch;
    /* 001C */ TSList<RENDERCALLBACKNODE> m_callbacks;
    /* 0028 */ TSLink<CRenderBatch> m_renderLink;

    virtual ~CRenderBatch();
};

#endif
