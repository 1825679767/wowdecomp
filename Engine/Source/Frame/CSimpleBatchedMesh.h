#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H
#define ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C2Vector.h"

struct C3Vector;
struct CTexture;

class CSimpleBatchedMesh {
    public:
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ void* m_textureID;
    /* 0008 */ uint32_t m_alphaMode;
    /* 000C */ void* m_shader;
    /* 0010 */ int32_t m_posCount;
    /* 0014 */ C3Vector* m_position;
    /* 0018 */ C2Vector* m_texCoord;
    /* 001C */ void* m_color;
    /* 0020 */ int32_t m_colorCount;
    /* 0024 */ uint16_t* m_indices;
    /* 0028 */ int32_t m_idxCount;
    /* 002C */ int32_t m_onAtlas;
    /* 0030 */ float m_atlasScale;
    /* 0034 */ C2Vector m_atlasOffset;
};

#endif
