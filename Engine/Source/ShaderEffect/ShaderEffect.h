#ifndef ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H
#define ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"

class CGxShader;
class CShaderEffect;
struct HASHKEY_STRI;

class CShaderEffect {
    public:
    /* 0000 */ TSHashObject<CShaderEffect, HASHKEY_STRI> m_hashObj;
    /* 0018 */ uint32_t m_fixedFuncOpCount;
    /* 001C */ uint32_t m_colorOps;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t m_alphaOps;
    /* 0028 */ uint32_t m_vertexShaders;
    /* 002C */ CGxShader* m_vertexShaders2 [90];
    /* 0194 */ CGxShader* m_pixelShaders [16];
};

#endif
