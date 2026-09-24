#ifndef ENGINE_SOURCE_GX_CGXDEVICE_CGXSHADER_H
#define ENGINE_SOURCE_GX_CGXDEVICE_CGXSHADER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

class CGxShader {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ char m_hashObject [24];
    /* 001C */ uint32_t m_refCount;
    /* 0020 */ void* m_apiSpecific;
    /* 0024 */ int32_t m_target;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t m_valid;
    /* 0030 */ int32_t m_loaded;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t m_patched;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int16_t unk_0044;
    /* 0046 */ int16_t unk_0046;
    /* 0048 */ TSGrowableArray<uint8_t> m_code;
};

#endif
