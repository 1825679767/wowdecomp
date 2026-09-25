#ifndef ENGINE_SOURCE_SERVICES_TEXTUREBLOB_H
#define ENGINE_SOURCE_SERVICES_TEXTUREBLOB_H

#include <cstddef>
#include <cstdint>

class CTexture;
class CTextureBlobTexture;

class CTextureBlob {
    public:
    static void CreateTexture(CTexture* texture, CTextureBlobTexture* blobTexture);
    static CTextureBlobTexture* GetTexture(const char* fileName);
};

#endif
