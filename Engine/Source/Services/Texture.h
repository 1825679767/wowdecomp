#ifndef ENGINE_SOURCE_SERVICES_TEXTURE_H
#define ENGINE_SOURCE_SERVICES_TEXTURE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CImVector.h"

class CTexture;
class CTextureAtlas;
class CTextureBlobTexture;

class CGxTexCache {
    public:
    /* 0000 */ CGxTex* m_gxTex;
    /* 0004 */ uint32_t m_time;
    /* 0008 */ uint32_t m_size;
    /* 000C */ TSLink<CGxTexCache> m_link;
};

struct HASHKEY_TEXTUREFILE {
    /* 0000 */ char* m_filename;
    /* 0004 */ CGxTexFlags m_texFlags;
    /* 0008 */ uint32_t m_anyTexFlags : 1;

    HASHKEY_TEXTUREFILE();
    bool operator==(const HASHKEY_TEXTUREFILE& key) const;
};

class CTexture : public CHandleObject, public TSHashObject<CTexture, HASHKEY_TEXTUREFILE> {
    public:
    /* 0028 */ uint16_t m_flags;
    /* 002A */ uint8_t m_bestMip;
    /* 002B */ uint8_t m_alphaBits;
    /* 002C */ CStatus m_status;
    /* 0040 */ CAsyncObject* m_asyncObject;
    /* 0044 */ CGxTex* m_gxTex;
    /* 0048 */ EGxTexTarget m_gxTexTarget;
    /* 004C */ uint16_t m_gxWidth;
    /* 004E */ uint16_t m_gxHeight;
    /* 0050 */ EGxTexFormat m_gxTexFormat;
    /* 0054 */ EGxTexFormat m_dataFormat;
    /* 0058 */ CGxTexFlags m_gxTexFlags;
    /* 005C */ CTextureAtlas* m_atlas;
    /* 0060 */ uint32_t m_atlasBlockIndex;
    /* 0064 */ TSLink<CTexture> m_link;
    /* 006C */ char m_filename[260];

    virtual ~CTexture();
    virtual const char* GetObjectName();

    CTexture();
};

class CTextureAtlas {
    public:
    static CTextureAtlas* Alloc(uint32_t width, uint32_t height, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags);
    static void Free(CTextureAtlas* atlas, CTexture* texture);
    static CTextureAtlas* Get(CTexture* texture);

    /* 0000 */ uint16_t m_count;
    /* 0002 */ uint16_t m_dirty;
    /* 0004 */ uint32_t m_width;
    /* 0008 */ uint32_t m_height;
    /* 000C */ uint32_t m_capacity;
    /* 0010 */ uint32_t m_used;
    /* 0014 */ uint32_t m_freeSlot;
    /* 0018 */ CGxTex* m_gxTex;
    /* 001C */ EGxTexFormat m_format;
    /* 0020 */ CGxTexFlags m_texFlags;
    /* 0024 */ EGxTexFormat m_dataFormat;
    /* 0028 */ uint32_t m_slots;
    /* 002C */ CTexture* m_textures[64];
    /* 012C */ int32_t m_updateSlot;
    /* 0130 */ TSLink<CTextureAtlas> m_link;

    CTextureAtlas();
    ~CTextureAtlas();
    void Reload();
    void Update(CTexture* texture);
};

void AsyncTextureDestroyedCallback(CAsyncObject* object);
void AsyncTextureFailureCallback(CTexture* texture);
void AsyncTextureHandler();
void AsyncTextureProcess(CAsyncObject* object, int32_t immediate);
int32_t AsyncTextureStatus();
void AsyncTextureSuccessCallback(CTexture* texture);
void AsyncTextureWait(CTexture* texture);
CTexture* CreateBlpAsync(char* extension, const char* fileName, uint32_t flags, CGxTexFlags texFlags);
CTexture* CreateBlpSync(const char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags);
CTexture* CreateBlpTexture(const char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags);
CTexture* CreateTgaTexture(const char* fileName, char* extension, int32_t flags, CGxTexFlags texFlags, CStatus* status);
void FillInSolidTexture(const CImVector& color, CTexture* texture);
MipBits* GetDefaultTexture(uint32_t width, uint32_t height);
int32_t LoadBlpWithAlias(char* fileName, int32_t flags, MipBits** images);
char* OsPathFindExtensionWithDot(const char* path);
int32_t PumpBlpTextureAsync(CTexture* texture, void* data);
void GxTexNullUserFunc(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
void RequestImageDimensions(uint32_t* width, uint32_t* height, uint32_t* bestMip);
void ResolveMipLevel(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
CGxTex* TextureAllocGxTex(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, EGxTexFormat dataFormat, const char* name);
int32_t TextureBuildAliasPath(char* buffer, const char* fileName);
CTexture* TextureCacheGetTexture(char* fileName, char* extension, CGxTexFlags texFlags);
CTexture* TextureCacheGetTexture(const CImVector& color);
void TextureCacheNewTexture(CTexture* texture, CGxTexFlags texFlags);
void TextureCacheNewTexture(CTexture* texture, const CImVector& color);
uint32_t TextureCalcGxTexSize(EGxTexFormat format, uint32_t width, uint32_t height, EGxTexFilter filter);
CTexture* TextureCreate(const char* fileName, CGxTexFlags texFlags, CStatus* status, uint32_t flags);
CTexture* TextureCreateSolid(const CImVector& color);
void TextureFreeGxTex(CGxTex* gxTex, const char* name);
CGxTex* TextureGetGxTex(CTexture* texture, int32_t wait, CStatus* status);
void TextureGetTextureFormats(PIXEL_FORMAT& pixelFormat, EGxTexFormat& gxTexFormat, PIXEL_FORMAT preferredFormat, uint32_t alphaBits);
void TextureIncreasePriority(CTexture* texture);
void TextureInitialize();
int32_t TextureLoadBlpMips(char* fileName, char* extension, int32_t flags, MipBits** images, uint32_t* width, uint32_t* height, EGxTexFormat* gxTexFormat, int32_t* opaque, uint32_t* alphaBits, PIXEL_FORMAT* pixelFormat);
void TextureReportLoadError(const char* type, const char* fileName, CStatus* status);
int32_t TextureUpdate(const void* data, void* param);
void UpdateBlpTextureAsync(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
void UpdateTgaTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

#endif
