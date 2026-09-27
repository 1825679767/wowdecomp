#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/Texture.h"

// VERIFY: d7a68913 auto
// OFFSET: 0x00482770
static int32_t SortByTexture(const void* elem1, const void* elem2) {
    const CSimpleBatchedMesh* mesh1 = static_cast<const CSimpleBatchedMesh*>(elem1);
    const CSimpleBatchedMesh* mesh2 = static_cast<const CSimpleBatchedMesh*>(elem2);

    if (mesh1->m_textureID != mesh2->m_textureID) {
        return static_cast<int32_t>(reinterpret_cast<uintptr_t>(mesh1->m_textureID) - reinterpret_cast<uintptr_t>(mesh2->m_textureID));
    }

    int32_t alphaMode1 = (mesh1->m_color && mesh1->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh1->m_alphaMode;
    int32_t alphaMode2 = (mesh2->m_color && mesh2->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh2->m_alphaMode;

    if (alphaMode1 != alphaMode2) {
        return alphaMode1 - alphaMode2;
    }

    return static_cast<int32_t>(reinterpret_cast<uintptr_t>(mesh1->m_shader) - reinterpret_cast<uintptr_t>(mesh2->m_shader));
}

// VERIFY: 2cc3a1bc auto
// OFFSET: 0x00484450
void CRenderBatch::Finish() {
    if (this->m_texturelist.Count() > 1) {
        qsort(this->m_texturelist.Ptr(), this->m_texturelist.Count(), sizeof(CSimpleBatchedMesh), &SortByTexture);
    }
}

// VERIFY: f0079ae6 auto
// OFFSET: 0x004846F0
void CRenderBatch::Queue(CTexture* texture, EGxBlend alphaMode, int32_t posCount, const C3Vector* position, const C2Vector* texCoord, int32_t colorCount, const CImVector* color, int32_t idxCount, const uint16_t* indices, CGxShader* shader) {
    CGxTex* gxTex = TextureGetGxTex(texture, 1, nullptr);

    if (!gxTex) {
        return;
    }

    uint32_t index = this->m_texturelist.Count();
    this->m_texturelist.SetCount(index + 1);

    CSimpleBatchedMesh* mesh = &this->m_texturelist[index];
    mesh->m_texture = texture;
    mesh->m_textureID = gxTex;
    mesh->m_alphaMode = alphaMode;
    mesh->m_shader = shader;
    mesh->m_posCount = posCount;
    mesh->m_position = position;
    mesh->m_texCoord = texCoord;
    mesh->m_color = color;
    mesh->m_colorCount = colorCount;
    mesh->m_indices = indices;
    mesh->m_idxCount = idxCount;
    mesh->m_onAtlas = TextureOnAtlas(texture);

    if (mesh->m_onAtlas) {
        TextureGetAtlasOffsetAndScale(mesh->m_texture, mesh->m_atlasOffset, mesh->m_atlasScale);
    }

    this->m_count++;
}

// VERIFY: 90c75e14 auto
// OFFSET: 0x00485530
CRenderBatch::CRenderBatch() {
    this->m_count = 0;
    this->m_stringbatch = nullptr;
}

// VERIFY: 6542125c manual
// OFFSET: 0x004858E0
void CRenderBatch::QueueCallback(void (*callback)(void*), void* param) {
    RENDERCALLBACKNODE* node = this->m_callbacks.NewNode(2, 0, 0);
    node->m_callback = callback;
    node->m_param = param;
    this->m_count++;
}

// VERIFY: 0c4ec2c0 auto
// OFFSET: 0x00485F00
void CRenderBatch::Clear() {
    this->m_texturelist.SetCount(0);

    if (this->m_stringbatch) {
        GxuFontDestroyBatch(this->m_stringbatch);
        this->m_stringbatch = nullptr;
    }

    this->m_callbacks.Clear();
    this->m_count = 0;
}

// VERIFY: 731464f9 manual
// OFFSET: [0x00486FC0, 0x00487160]
CRenderBatch::~CRenderBatch() {
    this->Clear();
}
