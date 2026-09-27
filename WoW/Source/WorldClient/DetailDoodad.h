#ifndef WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H
#define WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxPool;
class CGxShader;
class CDetailDoodadData;

class CDetailDoodad {
    public:
    static CGxTex* s_alphaRampTexture;
    static int32_t s_bPoolsDirty;
    static int32_t s_bUseShaders;
    static TSGrowableArray<CDetailDoodadData*> s_doodadList;
    static CGxPool* s_gxIndexPool;
    static CGxPool* s_gxVertexPool;
    static uint32_t s_idxBufItems;
    static uint32_t* s_instHeap;
    static CGxShader* s_pixelShader[8];
    static CGxShader* s_vertexShader[6];
    static uint32_t s_vtxBufItems;

    static void CreateAlphaRampTexture(const void*& texels);
    static void Initialize();
    static void UpdateAlphaRampTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
};

class CDetailDoodadInst {
    public:
    /* 0000 */ uint8_t unk_0000[4];
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
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
};

#endif
