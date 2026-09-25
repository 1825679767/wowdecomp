#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/TextureBlob.h"
#include "Engine/Source/Storm/H/storm.h"
#include "_Triage/MippedImg.h"

// STATIC: 0x009F103C
static const uint16_t s_texFormatBitDepth[GxTexFormats_Last] = { 0, 32, 32, 16, 16, 16, 4, 8, 8, 16, 32, 32, 32 };

// STATIC: 0x009F1058
static const uint16_t s_texFormatBytesPerBlock[GxTexFormats_Last] = { 0, 0, 0, 0, 0, 0, 8, 16, 16, 0, 0, 0, 0 };

// STATIC: 0x009F1120
static const uint32_t s_texFormatBytesPerTexel[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 8, 16, 16, 2, 4, 4, 4 };

// STATIC: 0x009F1154
static const uint32_t s_texFormatTexelsPerBlock[GxTexFormats_Last] = { 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1 };

// STATIC: 0x00AC3234
static EGxTexFormat s_pixelFormatToGxTexFormat[NUM_PIXEL_FORMATS] = { GxTex_Dxt1, GxTex_Dxt3, GxTex_Argb8888, GxTex_Argb1555, GxTex_Argb4444, GxTex_Rgb565, GxTex_Unknown, GxTex_Dxt5, GxTex_Unknown, GxTex_Unknown };

// STATIC: 0x00AC3298
static EGxTexFilter s_filterMode = GxTex_LinearMipNearest;

// STATIC: 0x00AC329C
static uint32_t s_maxAnisotropy = 1;

// STATIC: 0x00AC32A0
static int32_t s_createBlpAsync = 1;

// STATIC: 0x00AC3348
static TSExplicitList<CTexture, offsetof(CTexture, m_link)> s_textureList;

// STATIC: 0x00AC3354
static uint32_t s_defaultColor = 0xFF00FF00;

// STATIC: 0x00AC3358
static TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)> s_gxTexCacheFree;

// STATIC: 0x00AC3370
static TSExplicitList<CTextureAtlas, offsetof(CTextureAtlas, m_link)> s_atlasList;

// STATIC: 0x00AC337C
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncLoadList;

// STATIC: 0x00AC3388
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncTextureList;

// STATIC: 0x00B49C78
static uint32_t s_currentTime;

// STATIC: 0x00B49C7C
static char* s_substituteFrom;

// STATIC: 0x00B49C80
static char* s_substituteTo;

// STATIC: 0x00B49C84
static int32_t s_atlasEnable;

// STATIC: 0x00B49C90
static MipBits* s_mipBits;

// STATIC: 0x00B49C94
static int32_t s_pumping;

// STATIC: 0x00B49C98
static uint32_t s_gxTexCacheSize;

// STATIC: 0x00B49C9C
static uint32_t s_gxTexCacheMaxSize;

// STATIC: 0x00B49CA0
static uint32_t s_asyncLoadBufferUsed;

// STATIC: 0x00B49CA4
static TSHashTableReuse<CTexture, HASHKEY_TEXTUREFILE> s_textureCache;

// STATIC: 0x00B49CD8
static TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)> s_gxTexCache[6][6];

// INLINED
HASHKEY_TEXTUREFILE::HASHKEY_TEXTUREFILE() {
    this->m_filename = nullptr;
    this->m_anyTexFlags = 0;
}

// INLINED
bool HASHKEY_TEXTUREFILE::operator==(const HASHKEY_TEXTUREFILE& key) const {
    return !SStrCmpI(this->m_filename, key.m_filename, 260) && (this->m_anyTexFlags || key.m_anyTexFlags || this->m_texFlags == key.m_texFlags);
}

// VERIFY: c74642f9 manual
// OFFSET: 0x005EEB70
void GxTexNullUserFunc(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
}

// VERIFY: 10032e81 auto
// OFFSET: 0x004B50D0
void CTextureAtlas::Update(CTexture* texture) {
    this->m_updateSlot = texture->m_atlasBlockIndex;
    GxTexUpdate(this->m_gxTex, (texture->m_atlasBlockIndex & 7) * this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height, (texture->m_atlasBlockIndex & 7) * this->m_width + this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height + this->m_height, 1);
    this->m_updateSlot = -1;
}

// VERIFY: 89c9895e manual
// OFFSET: 0x004B5130
void AsyncTextureDestroyedCallback(CAsyncObject* object) {
    s_asyncLoadBufferUsed -= object->m_size;
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// VERIFY: 0a24e97f auto
// OFFSET: 0x004B5170
void TextureReportLoadError(const char* type, const char* fileName, CStatus* status) {
    char buffer[256];
    SErrGetErrorStr(SErrGetLastError(), buffer, sizeof(buffer));
    status->Add(STATUS_FATAL, "Error loading %s file \"%s\": %s\n", type, fileName, buffer);
    SErrSetLastError(0);
}

// VERIFY: 23747110 auto
// OFFSET: 0x004B51C0
char* OsPathFindExtensionWithDot(const char* path) {
    const char* slash = strrchr(path, '\\');

    if (!slash) {
        slash = strrchr(path, '/');
    }

    const char* dot = strrchr(path, '.');

    if (dot && (!slash || slash < dot)) {
        return const_cast<char*>(dot);
    }

    return const_cast<char*>(path + strlen(path));
}

// VERIFY: 463fb781 auto
// OFFSET: 0x004B5210
int32_t TextureBuildAliasPath(char* buffer, const char* fileName) {
    if (!s_substituteFrom) {
        return 0;
    }

    const char* name = SStrChrR(const_cast<char*>(fileName), '\\');
    name = name ? name + 1 : fileName;

    if (SStrCmpI(s_substituteFrom, name, STORM_MAX_STR)) {
        return 0;
    }

    SStrCopy(buffer, fileName, 260);
    SStrCopy(&buffer[name - fileName], s_substituteTo, 260 - (name - fileName));

    return 1;
}

// VERIFY: a6ae9dde manual
// OFFSET: 0x004B5280
MipBits* GetDefaultTexture(uint32_t width, uint32_t height) {
    MipBits* images = s_mipBits;
    MipBits* level = images;

    MippedImgSet(PIXEL_ARGB8888, width, height, images);

    while (width > 1 || height > 1) {
        memset(level->m_mip[0], 0xFF, 4 * height * width);
        level++;
        width = width >> 1 ? width >> 1 : 1;
        height >>= 1;

        if (!height) {
            height = 1;
        }
    }

    return images;
}

// VERIFY: 7a812510 auto
// OFFSET: 0x004B5BB0
void RequestImageDimensions(uint32_t* width, uint32_t* height, uint32_t* bestMip) {
    CGxCaps caps = g_theGxDevicePtr->Caps();
    uint32_t maxSize = caps.m_texMaxSize[0];

    if (!maxSize) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    while (*width > maxSize || *height > maxSize) {
        *width >>= 1;
        *height >>= 1;
        (*bestMip)++;

        if (!*width) {
            *width = 1;
        }

        if (!*height) {
            *height = 1;
        }
    }
}

// VERIFY: b37c3e13 manual
// OFFSET: 0x004B5C30
int32_t TextureLoadBlpMips(char* fileName, char* extension, int32_t flags, MipBits** images, uint32_t* width, uint32_t* height, EGxTexFormat* gxTexFormat, int32_t* opaque, uint32_t* alphaBits, PIXEL_FORMAT* pixelFormat) {
    if (extension) {
        SStrCopy(extension, ".blp", STORM_MAX_STR);
    }

    CBLPFile image;

    if (!image.Open(fileName, flags)) {
        return 0;
    }

    EGxTexFormat format = GxTex_Argb8888;
    PIXEL_FORMAT pixels = PIXEL_ARGB8888;

    if (image.m_header.m_colorEncoding == 2) {
        pixels = static_cast<PIXEL_FORMAT>(image.m_header.m_preferredFormat);

        if (pixels == PIXEL_DXT1) {
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt1]) {
                format = GxTex_Dxt1;
            } else if (image.m_header.m_alphaSize) {
                format = GxTex_Argb1555;
                pixels = PIXEL_ARGB1555;
            } else {
                format = GxTex_Rgb565;
                pixels = PIXEL_RGB565;
            }
        } else if (pixels == PIXEL_DXT3) {
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt3]) {
                format = GxTex_Dxt3;
            } else {
                format = GxTex_Argb4444;
                pixels = PIXEL_ARGB4444;
            }
        } else if (pixels == PIXEL_DXT5) {
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt5]) {
                format = GxTex_Dxt5;
            } else {
                format = GxTex_Argb4444;
                pixels = PIXEL_ARGB4444;
            }
        }
    }

    uint32_t imageWidth = image.m_header.m_width;
    uint32_t bestMip = 0;
    uint32_t imageHeight = image.m_header.m_height;
    RequestImageDimensions(&imageWidth, &imageHeight, &bestMip);

    if (width) {
        *width = imageWidth;
    }

    if (height) {
        *height = imageHeight;
    }

    if (gxTexFormat) {
        *gxTexFormat = format;
    }

    uint8_t alphaSize = image.m_header.m_alphaSize;

    if (opaque) {
        *opaque = image.m_header.m_alphaSize == 0;
    }

    if (alphaBits) {
        *alphaBits = alphaSize;
    }

    if (pixelFormat) {
        *pixelFormat = pixels;
    }

    if (!image.LockChain(pixels, *images, bestMip)) {
        return 0;
    }

    return 1;
}

// VERIFY: 363481eb manual
// OFFSET: 0x004B5E10
int32_t LoadBlpWithAlias(char* fileName, int32_t flags, MipBits** images) {
    char buffer[260];
    char* extension = &fileName[SStrLen(fileName)];
    extension[0] = '.';
    extension[1] = 'b';
    extension[2] = 'l';
    extension[3] = 'p';
    extension[4] = '\0';

    if (TextureBuildAliasPath(buffer, fileName)) {
        fileName = buffer;
    }

    int32_t result = TextureLoadBlpMips(fileName, nullptr, flags, images, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    *extension = '\0';

    return result;
}

// VERIFY: aac6b028 manual
// OFFSET: 0x004B5E80
void UpdateBlpTextureAsync(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CTexture* texture = static_cast<CTexture*>(userArg);

    switch (command) {
        case GxTex_Lock:
            if (!s_pumping && !LoadBlpWithAlias(texture->m_filename, texture->m_flags & 2, &s_mipBits)) {
                GetDefaultTexture(width, height);
                GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded -- replaced with default.\n", texture->m_filename);
            }

            break;

        case GxTex_Latch: {
            if (texture->m_gxTexFormat < GxTex_Dxt1 || texture->m_gxTexFormat > GxTex_Dxt5) {
                texelStrideInBytes = (width * s_texFormatBitDepth[texture->m_dataFormat]) >> 3;
            } else {
                texelStrideInBytes = (width >> 2) * s_texFormatBytesPerBlock[texture->m_dataFormat];

                if (texelStrideInBytes < s_texFormatBytesPerBlock[texture->m_dataFormat]) {
                    texelStrideInBytes = s_texFormatBytesPerBlock[texture->m_dataFormat];
                }
            }

            const uint8_t* bits = reinterpret_cast<const uint8_t*>(s_mipBits[level].m_mip[0]);
            texels = bits;

            if (texture->m_gxTexTarget == GxTex_CubeMap) {
                uint32_t order = face;

                switch (face) {
                    case 0:
                        order = 0;
                        break;

                    case 1:
                        order = 2;
                        break;

                    case 2:
                        order = 4;
                        break;

                    case 3:
                        order = 5;
                        break;

                    case 4:
                        order = 3;
                        break;

                    case 5:
                        order = 1;
                        break;
                }

                texels = bits + order * texelStrideInBytes;
                texelStrideInBytes *= 6;
            }

            break;
        }

        default:
            break;
    }
}

// VERIFY: 85789b70 manual
// OFFSET: 0x004B5FE0
void TextureGetTextureFormats(PIXEL_FORMAT& pixelFormat, EGxTexFormat& gxTexFormat, PIXEL_FORMAT preferredFormat, uint32_t alphaBits) {
    switch (preferredFormat) {
        case PIXEL_DXT1:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt1]) {
                gxTexFormat = GxTex_Dxt1;
                pixelFormat = PIXEL_DXT1;
            } else if (alphaBits) {
                gxTexFormat = GxTex_Argb1555;
                pixelFormat = PIXEL_ARGB1555;
            } else {
                gxTexFormat = GxTex_Rgb565;
                pixelFormat = PIXEL_RGB565;
            }

            break;

        case PIXEL_DXT3:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt3]) {
                gxTexFormat = GxTex_Dxt3;
                pixelFormat = PIXEL_DXT3;
            } else {
                gxTexFormat = GxTex_Argb4444;
                pixelFormat = PIXEL_ARGB4444;
            }

            break;

        case PIXEL_ARGB8888:
            gxTexFormat = GxTex_Argb8888;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_ARGB1555:
            gxTexFormat = GxTex_Argb1555;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_ARGB4444:
            gxTexFormat = GxTex_Argb4444;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_RGB565:
            gxTexFormat = GxTex_Rgb565;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_DXT5:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt5]) {
                gxTexFormat = GxTex_Dxt5;
                pixelFormat = PIXEL_DXT5;
            } else {
                gxTexFormat = GxTex_Argb4444;
                pixelFormat = PIXEL_ARGB4444;
            }

            break;

        case PIXEL_UNSPECIFIED:
            if (!alphaBits) {
                gxTexFormat = GxTex_Rgb565;
                pixelFormat = PIXEL_ARGB8888;
            } else if (alphaBits == 1) {
                gxTexFormat = GxTex_Argb1555;
                pixelFormat = PIXEL_ARGB8888;
            } else {
                gxTexFormat = alphaBits == 4 ? GxTex_Argb4444 : GxTex_Argb8888;
                pixelFormat = PIXEL_ARGB8888;
            }

            break;

        default:
            break;
    }
}

// VERIFY: 835e236e auto
// OFFSET: 0x004B6300
uint32_t TextureCalcGxTexSize(EGxTexFormat format, uint32_t width, uint32_t height, EGxTexFilter filter) {
    uint32_t baseMip = g_theGxDevicePtr->DeviceBaseMipLevel();
    width >>= baseMip;
    height >>= baseMip;

    if (!width) {
        width = 1;
    }

    if (!height) {
        height = 1;
    }

    uint32_t texelsPerBlock = s_texFormatTexelsPerBlock[format];
    uint32_t bytesPerTexel = s_texFormatBytesPerTexel[format];
    uint32_t size = (texelsPerBlock + width - 1) / texelsPerBlock * ((texelsPerBlock + height - 1) / texelsPerBlock) * bytesPerTexel;

    if (filter > GxTex_Linear) {
        while (width != 1 || height != 1) {
            if (width > 1) {
                width >>= 1;
            }

            if (height > 1) {
                height >>= 1;
            }

            size += (texelsPerBlock + width - 1) / texelsPerBlock * ((texelsPerBlock + height - 1) / texelsPerBlock) * bytesPerTexel;
        }
    }

    return size;
}

// VERIFY: b532779f manual
// OFFSET: 0x004B63B0
void CTextureAtlas::Reload() {
    for (uint32_t i = 0; i < 64; i++) {
        CTexture* texture = this->m_textures[i];

        if (!texture) {
            continue;
        }

        s_pumping = 1;

        if (!LoadBlpWithAlias(texture->m_filename, texture->m_flags & 2, &s_mipBits)) {
            GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded.\n", texture->m_filename);
        }

        this->Update(texture);
        s_pumping = 0;
    }

    this->m_dirty &= ~1;
}

// VERIFY: 7f5ee904 manual
// OFFSET: 0x004B64E0
void AsyncTextureProcess(CAsyncObject* object, int32_t immediate) {
    object->m_link.Unlink();
    object->m_buffer = SMemAlloc(object->m_size, __FILE__, __LINE__, 0);
    s_asyncLoadBufferUsed += object->m_size;
    AsyncFileReadObject(object, immediate);
}

// VERIFY: 22d68fde auto
// OFFSET: 0x004B6550
void AsyncTextureWait(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (object) {
        if (!object->m_buffer) {
            AsyncTextureProcess(object, 1);
        }

        AsyncFileReadWait(texture->m_asyncObject);
    }
}

// VERIFY: b2a5ecf0 manual
// OFFSET: 0x004B6700
CTextureAtlas::CTextureAtlas() {
    this->m_slots = 64;
    this->m_count = 0;
    this->m_dirty = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_capacity = 0;
    this->m_used = 0;
    this->m_freeSlot = -1;
    this->m_gxTex = nullptr;
    this->m_updateSlot = -1;
    memset(this->m_textures, 0, sizeof(this->m_textures));
}

// VERIFY: 00142d3e manual
// OFFSET: 0x004B6760
CGxTex* TextureAllocGxTex(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, EGxTexFormat dataFormat, const char* name) {
    CGxTexParms parms;
    parms.m_target = target;
    parms.m_width = width;
    parms.m_height = height;
    parms.m_depth = depth;
    parms.m_format = format;
    parms.m_dataFormat = dataFormat;
    parms.m_flags = flags;
    parms.m_flags.m_generateMipMaps = 0;
    parms.m_userArg = userArg;
    parms.m_userFunc = userFunc;

    if (GxTexReusable(parms) && width - 32 <= 480 && height - 32 <= 480) {
        uint32_t w = width >> 5;
        uint32_t h = height >> 5;
        uint32_t x = 0;
        uint32_t y = 0;

        if (!(width & 0x20)) {
            do {
                w >>= 1;
                x++;
            } while (!(w & 1));
        }

        if (!(height & 0x20)) {
            do {
                h >>= 1;
                y++;
            } while (!(h & 1));
        }

        CGxTexParms cached;
        TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)>& list = s_gxTexCache[x][y];

        for (CGxTexCache* entry = list.Head(); entry; entry = list.Next(entry)) {
            GxTexParameters(entry->m_gxTex, cached);

            if (parms.m_flags.m_filter == cached.m_flags.m_filter && cached.m_format == parms.m_format && cached.m_target == parms.m_target && cached.m_depth == parms.m_depth) {
                s_gxTexCacheSize -= entry->m_size;
                entry->m_link.Unlink();
                CGxTex* gxTex = entry->m_gxTex;
                entry->m_gxTex = nullptr;
                s_gxTexCacheFree.LinkToHead(entry);
                GxTexSetDataFormat(gxTex, parms.m_dataFormat);
                GxTexSetUserData(gxTex, parms.m_userFunc, parms.m_userArg);
                GxTexSetFlags(gxTex, parms.m_flags);

                return gxTex;
            }
        }
    }

    CGxTex* gxTex = nullptr;
    GxTexCreate(parms, gxTex);

    return gxTex;
}

// VERIFY: ead404bb manual
// OFFSET: 0x004B6920
void FillInSolidTexture(const CImVector& color, CTexture* texture) {
    texture->m_gxTex = TextureAllocGxTex(GxTex_2d, 8, 8, 0, GxTex_Argb8888, CGxTexFlags(), reinterpret_cast<void*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(&color))), GxuUpdateSingleColorTexture, GxTex_Argb8888, texture->m_filename);

    if (color.a < 0xFE) {
        texture->m_flags &= ~1;
    } else {
        texture->m_flags |= 1;
    }

    texture->m_dataFormat = GxTex_Argb8888;
    texture->m_gxTexFormat = GxTex_Argb8888;
    texture->m_gxTexTarget = GxTex_2d;
    texture->m_gxWidth = 8;
    texture->m_gxHeight = 8;
    texture->m_gxTexFlags = CGxTexFlags();
    SStrCopy(texture->m_filename, "SolidTexture", 260);
}

// VERIFY: 04d8356e auto
// OFFSET: 0x004B6C50
void TextureIncreasePriority(CTexture* texture) {
    if (!SFile::IsStreamingMode()) {
        return;
    }

    if (texture->m_asyncObject->m_buffer) {
        AsyncFile::EnterQueueLock();
        CAsyncObject* object = texture->m_asyncObject;

        if (!object->m_isCurrent && !object->m_isRead && !object->m_isProcessed) {
            AsyncFileReadIncreasePriority(texture->m_asyncObject);
        }

        AsyncFile::LeaveQueueLock();
    } else {
        s_asyncLoadList.LinkToHead(texture->m_asyncObject);
    }
}

// VERIFY: 3c936638 auto
// OFFSET: 0x004B6CB0
CGxTex* TextureGetGxTex(CTexture* texture, int32_t wait, CStatus* status) {
    if (!texture) {
        DebugValidate("textureptr");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    if (texture->m_flags & 4) {
        if (texture->m_asyncObject) {
            if (wait != 1 && (wait != 2 || texture->m_asyncObject->m_char24)) {
                TextureIncreasePriority(texture);
                return nullptr;
            }

            AsyncTextureWait(texture);

            if (status) {
                status->Add(texture->m_status);
            }
        }

        if (texture->m_atlas) {
            if (texture->m_atlas->m_dirty & 1) {
                texture->m_atlas->Reload();
            }

            return texture->m_atlas->m_gxTex;
        }
    }

    if (texture->m_asyncObject) {
        TextureIncreasePriority(texture);
    }

    if (!texture->m_gxTex) {
        if (wait != 1 && (wait != 2 || texture->m_asyncObject->m_char24)) {
            return nullptr;
        }

        AsyncTextureWait(texture);

        if (status) {
            status->Add(texture->m_status);
        }
    }

    return texture->m_gxTex;
}

// VERIFY: 9761fb06 manual
// OFFSET: 0x004B6E90
CTextureAtlas* CTextureAtlas::Alloc(uint32_t width, uint32_t height, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags) {
    CTextureAtlas* atlas = new (__FILE__, __LINE__) CTextureAtlas;
    atlas->m_height = height;
    atlas->m_width = width;
    atlas->m_used = 0;
    atlas->m_freeSlot = -1;
    atlas->m_capacity = 64;
    atlas->m_gxTex = TextureAllocGxTex(GxTex_2d, 8 * width, 8 * height, 0, format, flags, atlas, ResolveMipLevel, dataFormat, "Atlas");
    atlas->m_dataFormat = dataFormat;
    atlas->m_format = format;
    atlas->m_texFlags = flags;

    return atlas;
}

// VERIFY: 841c7f74 auto
// OFFSET: 0x004B6FA0
CTexture* TextureCacheGetTexture(char* fileName, char* extension, CGxTexFlags texFlags) {
    if (extension) {
        *extension = '\0';
    }

    uint32_t hashval = SStrHashHT(fileName);
    HASHKEY_TEXTUREFILE key;
    key.m_filename = fileName;
    key.m_texFlags = texFlags;
    CTexture* texture = s_textureCache.Ptr(hashval, key);

    if (extension) {
        *extension = '.';
    }

    if (!texture) {
        return nullptr;
    }

    return reinterpret_cast<CTexture*>(HandleCreate(texture, "HTEXTURE"));
}

// VERIFY: 028fd060 auto
// OFFSET: 0x004B7020
CTexture* TextureCacheGetTexture(const CImVector& color) {
    CGxTexFlags texFlags;
    HASHKEY_TEXTUREFILE key;
    key.m_filename = const_cast<char*>("");
    key.m_texFlags = texFlags;
    CTexture* texture = s_textureCache.Ptr(*reinterpret_cast<const uint32_t*>(&color), key);

    if (!texture) {
        return nullptr;
    }

    return reinterpret_cast<CTexture*>(HandleCreate(texture, "HTEXTURE"));
}

// VERIFY: fb7f7312 manual
// OFFSET: 0x004B70A0
void TextureFreeGxTex(CGxTex* gxTex, const char* name) {
    CGxTexParms parms;
    GxTexParameters(gxTex, parms);

    if (GxTexReusable(gxTex) && parms.m_width - 32 <= 480 && parms.m_height - 32 <= 480) {
        uint32_t w = parms.m_width >> 5;
        uint32_t h = parms.m_height >> 5;
        uint32_t x = 0;
        uint32_t y = 0;

        if (!(parms.m_width & 0x20)) {
            do {
                w >>= 1;
                x++;
            } while (!(w & 1));
        }

        if (!(parms.m_height & 0x20)) {
            do {
                h >>= 1;
                y++;
            } while (!(h & 1));
        }

        CGxTexCache* entry = s_gxTexCacheFree.Head();

        if (!entry) {
            entry = s_gxTexCacheFree.NewNode(1, 0, 0);
        }

        entry->m_link.Unlink();
        entry->m_gxTex = gxTex;
        entry->m_time = s_currentTime;
        entry->m_size = TextureCalcGxTexSize(parms.m_format, parms.m_width, parms.m_height, static_cast<EGxTexFilter>(parms.m_flags.m_filter));
        s_gxTexCache[x][y].LinkToTail(entry);
        s_gxTexCacheSize += entry->m_size;
        GxTexSetUserData(entry->m_gxTex, GxTexNullUserFunc, nullptr);
    } else {
        TextureCalcGxTexSize(parms.m_format, parms.m_width, parms.m_height, static_cast<EGxTexFilter>(parms.m_flags.m_filter));
        GxTexDestroy(gxTex);
    }
}

// VERIFY: a81fc586 manual
// OFFSET: 0x004B76E0
CTextureAtlas::~CTextureAtlas() {
    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, "ATLAS");
    }

    this->m_link.Unlink();
}

// VERIFY: f80ba6e6 manual
// OFFSET: 0x004B7770
CTextureAtlas* CTextureAtlas::Get(CTexture* texture) {
    uint16_t width = texture->m_gxWidth;
    uint16_t height = texture->m_gxHeight;

    if (width != height || width > 64 || height > 64) {
        return nullptr;
    }

    CGxTexFlags texFlags = texture->m_gxTexFlags;
    texFlags.m_bit15 = 1;
    CTextureAtlas* best = nullptr;

    for (CTextureAtlas* atlas = s_atlasList.Head(); atlas; atlas = s_atlasList.Next(atlas)) {
        if (atlas->m_format == texture->m_gxTexFormat && atlas->m_texFlags == texFlags && atlas->m_dataFormat == texture->m_dataFormat && atlas->m_width == width && atlas->m_height == height) {
            if (static_cast<int32_t>(atlas->m_used) < static_cast<int32_t>(atlas->m_capacity) && (!best || static_cast<int32_t>(atlas->m_used) > static_cast<int32_t>(best->m_used))) {
                best = atlas;
            }
        }
    }

    CTextureAtlas* atlas = best;

    if (!atlas) {
        atlas = CTextureAtlas::Alloc(width, height, texture->m_gxTexFormat, texture->m_dataFormat, texFlags);
        s_atlasList.LinkToTail(atlas);
    }

    texture->m_atlasBlockIndex = -1;

    if (atlas->m_freeSlot == -1) {
        for (uint32_t i = 0; i < 64; i++) {
            if (!atlas->m_textures[i]) {
                texture->m_atlasBlockIndex = i;
                break;
            }
        }
    } else {
        texture->m_atlasBlockIndex = atlas->m_freeSlot;
        atlas->m_freeSlot = -1;
    }

    atlas->m_used++;
    atlas->m_count++;
    atlas->m_textures[texture->m_atlasBlockIndex] = texture;

    return atlas;
}

// VERIFY: c0a503e5 manual
// OFFSET: 0x004B7BD0
int32_t PumpBlpTextureAsync(CTexture* texture, void* data) {
    CBLPFile image;

    if (!image.Source(data)) {
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" invalid file version\n", texture->m_filename);
        return 0;
    }

    if ((texture->m_flags & 4) && !(image.m_header.m_hasMips & 0x10)) {
        texture->m_flags &= ~4;
    }

    texture->m_alphaBits = image.m_header.m_alphaSize;

    if (!image.m_header.m_alphaSize) {
        texture->m_flags |= 1;
    }

    uint32_t width = image.m_header.m_width;
    uint32_t bestMip = 0;
    uint32_t height = image.m_header.m_height;
    RequestImageDimensions(&width, &height, &bestMip);
    texture->m_bestMip = bestMip;

    PIXEL_FORMAT pixelFormat;
    EGxTexFormat gxTexFormat;
    TextureGetTextureFormats(pixelFormat, gxTexFormat, static_cast<PIXEL_FORMAT>(image.m_header.m_preferredFormat), image.m_header.m_alphaSize);

    s_pumping = 1;

    if (!image.LockChain2(texture->m_filename, pixelFormat, s_mipBits, texture->m_bestMip, 1)) {
        s_pumping = 0;
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" decompression failed.\n", texture->m_filename);
        return 0;
    }

    uint32_t faceWidth = width;
    EGxTexTarget target = GxTex_2d;

    if (width == 6 * height) {
        target = GxTex_CubeMap;
        faceWidth = width / 6;
    }

    texture->m_gxHeight = height;
    texture->m_dataFormat = s_pixelFormatToGxTexFormat[pixelFormat];
    texture->m_gxTexTarget = target;
    texture->m_gxWidth = faceWidth;
    texture->m_gxTexFormat = gxTexFormat;

    if (faceWidth < 256 && image.m_numLevels == 1 && !texture->m_gxTexFlags.m_generateMipMaps) {
        texture->m_gxTexFlags.m_filter = GxTex_Nearest;
    }

    if (texture->m_flags & 4) {
        texture->m_atlas = CTextureAtlas::Get(texture);

        if (texture->m_atlas) {
            texture->m_atlas->Update(texture);
        } else {
            texture->m_flags &= ~4;
        }
    }

    if (!texture->m_atlas) {
        if (texture->m_gxTex) {
            TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
            texture->m_gxTex = nullptr;
        }

        texture->m_gxTex = TextureAllocGxTex(texture->m_gxTexTarget, texture->m_gxWidth, texture->m_gxHeight, 0, texture->m_gxTexFormat, texture->m_gxTexFlags, texture, UpdateBlpTextureAsync, texture->m_dataFormat, texture->m_filename);

        if (!texture->m_gxTex) {
            s_pumping = 0;
            texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" allocating %dx%d texture failed.\n", texture->m_filename, faceWidth, height);
            return 0;
        }

        GxTexUpdate(texture->m_gxTex, 0, 0, faceWidth, height, 1);
    }

    s_pumping = 0;

    return 1;
}

// VERIFY: f4a9a3e6 manual
// OFFSET: 0x004B7F80
void TextureInitialize() {
    s_mipBits = static_cast<MipBits*>(SMemAlloc(MippedImgCalcSize(PIXEL_ARGB8888, 1024, 1024), __FILE__, __LINE__, 0));
    AsyncFileReadAddPollHandler(AsyncTextureHandler);
    AsyncFileReadAddStatusHandler(AsyncTextureStatus);
    s_currentTime = OsGetAsyncTimeMs();
    s_gxTexCacheSize = 0;
    s_gxTexCacheMaxSize = 0;
    EventRegisterEx(EVENT_ID_POLL, TextureUpdate, nullptr, 0.0f);
}

// VERIFY: 3e889496 manual
// OFFSET: 0x004B8720
void CTextureAtlas::Free(CTextureAtlas* atlas, CTexture* texture) {
    atlas->m_textures[texture->m_atlasBlockIndex] = nullptr;
    atlas->m_count--;
    atlas->m_used--;
    atlas->m_freeSlot = texture->m_atlasBlockIndex;

    if (static_cast<int16_t>(atlas->m_count) < 1) {
        delete atlas;
    }

    texture->m_atlasBlockIndex = -1;
}

// VERIFY: 0882c3ed manual
// OFFSET: 0x004B8770
CTexture::CTexture() {
    this->m_flags = 0;
    this->m_bestMip = 0;
    this->m_alphaBits = 0;
    this->m_asyncObject = nullptr;
    this->m_gxTex = nullptr;
    this->m_gxTexTarget = GxTex_2d;
    this->m_gxWidth = 0;
    this->m_gxHeight = 0;
    this->m_gxTexFormat = GxTex_Unknown;
    this->m_dataFormat = GxTex_Unknown;
    this->m_atlas = nullptr;
    this->m_atlasBlockIndex = 0;
    this->m_filename[0] = '\0';
    s_textureList.LinkToTail(this);
}

// VERIFY: 2820aa71 auto
// OFFSET: 0x004B8820
const char* CTexture::GetObjectName() {
    return this->m_filename;
}

// VERIFY: e23c2bfb manual
// OFFSET: 0x004B8830
CTexture::~CTexture() {
    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, this->m_filename);
    }

    if (this->m_atlas) {
        CTextureAtlas::Free(this->m_atlas, this);
    }

    if (this->m_asyncObject) {
        if (this->m_flags & 0x20) {
            this->m_asyncObject->m_userArg = this->m_asyncObject;
            this->m_asyncObject->m_userPostUnloadCallback = reinterpret_cast<void*>(AsyncTextureDestroyedCallback);
            this->m_asyncObject->m_userPostloadCallback = this->m_asyncObject->m_userPostUnloadCallback;
        } else {
            void* buffer = this->m_asyncObject->m_buffer;

            if (AsyncFile::Cancel(this->m_asyncObject, AsyncTextureDestroyedCallback)) {
                s_asyncLoadBufferUsed -= this->m_asyncObject->m_size;
                SMemFree(buffer, __FILE__, __LINE__, 0);
            }
        }
    }
}

// VERIFY: 996bc4c4 manual
// OFFSET: 0x004B8910
CTexture* CreateBlpSync(const char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags) {
    SFileImpl* file = nullptr;
    SErrSetLastError(0);

    if (!SFile::OpenEx(nullptr, fileName, (flags >> 1) & 1, &file)) {
        if (!SErrGetLastError()) {
            SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
        }

        return nullptr;
    }

    if (!file) {
        return nullptr;
    }

    CTexture* texture = new ("HTEXTURE", SLOG_OBJECT) CTexture;
    texture->m_gxTexFlags = texFlags;

    if (flags & 2) {
        texture->m_flags |= 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    if (extension) {
        *extension = '\0';
    }

    SStrCopy(texture->m_filename, fileName, STORM_MAX_STR);
    uint32_t size = SFileGetFileSize(file, nullptr);
    void* data = SMemAlloc(size, __FILE__, __LINE__, 0);

    if (!SFileReadFile(file, data, size, nullptr, nullptr, 0)) {
        DebugLog("CreateBlpTexture() failed read: %s.\n", fileName);
    }

    if (!PumpBlpTextureAsync(texture, data)) {
        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    SFileCloseFile(file);
    SMemFree(data, __FILE__, __LINE__, 0);

    return texture;
}

// VERIFY: 3cc759a9 manual
// OFFSET: 0x004B8A50
CTexture* CreateBlpAsync(char* extension, const char* fileName, uint32_t flags, CGxTexFlags texFlags) {
    SFileImpl* file = nullptr;
    SErrSetLastError(0);

    if (!SFile::OpenEx(nullptr, fileName, (flags >> 1) & 1, &file)) {
        if (!SErrGetLastError()) {
            SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
        }

        return nullptr;
    }

    if (!file) {
        return nullptr;
    }

    if (extension) {
        *extension = '\0';
    }

    CTextureBlobTexture* blobTexture = nullptr;

    if (!(flags & 4)) {
        blobTexture = CTextureBlob::GetTexture(fileName);
    }

    CTexture* texture = new ("HTEXTURE", SLOG_OBJECT) CTexture;
    texture->m_gxTexFlags = texFlags;

    if (flags & 2) {
        texture->m_flags |= 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    if ((flags & 0x20) && SFile::IsStreamingMode()) {
        texture->m_flags |= 0x20;
    }

    SStrCopy(texture->m_filename, fileName, STORM_MAX_STR);

    if (blobTexture) {
        CTextureBlob::CreateTexture(texture, blobTexture);
    }

    texture->m_asyncObject = AsyncFileReadAllocObject();
    texture->m_asyncObject->m_userArg = texture;
    texture->m_asyncObject->m_userPostloadCallback = reinterpret_cast<void*>(AsyncTextureSuccessCallback);
    texture->m_asyncObject->m_userPostUnloadCallback = reinterpret_cast<void*>(AsyncTextureFailureCallback);
    texture->m_asyncObject->m_file = file;
    texture->m_asyncObject->m_size = SFileGetFileSize(file, nullptr);

    if (blobTexture) {
        texture->m_asyncObject->m_priority = static_cast<int8_t>(131);
    } else if (flags & 0x10) {
        texture->m_asyncObject->m_priority = static_cast<int8_t>(129);
    } else {
        texture->m_asyncObject->m_priority = static_cast<int8_t>(130);
    }

    CAsyncObject* object = texture->m_asyncObject;

    if (0x400000 - s_asyncLoadBufferUsed < object->m_size) {
        s_asyncTextureList.LinkNode(object, 2, nullptr);
    } else {
        AsyncTextureProcess(object, 0);
    }

    return texture;
}

// VERIFY: 77916a1a auto
// OFFSET: 0x004B8BE0
CTexture* CreateBlpTexture(const char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags) {
    char buffer[260];

    if (extension) {
        SStrCopy(extension, ".blp", STORM_MAX_STR);
    }

    const char* name = fileName;

    if (TextureBuildAliasPath(buffer, fileName)) {
        name = buffer;
        extension = OsPathFindExtensionWithDot(buffer);
    }

    CTexture* texture = s_createBlpAsync ? CreateBlpAsync(extension, name, flags, texFlags) : CreateBlpSync(name, extension, flags, texFlags);

    if (!texture) {
        return nullptr;
    }

    return reinterpret_cast<CTexture*>(HandleCreate(texture, "HTEXTURE"));
}

// VERIFY: 07bcfe26 manual
// OFFSET: 0x004B9480
void TextureCacheNewTexture(CTexture* texture, CGxTexFlags texFlags) {
    uint32_t hashval = SStrHashHT(texture->m_filename);
    HASHKEY_TEXTUREFILE key;
    key.m_filename = texture->m_filename;
    key.m_texFlags = texFlags;
    s_textureCache.Insert(texture, hashval, key);
}

// VERIFY: 27dbedb5 manual
// OFFSET: 0x004B94E0
void TextureCacheNewTexture(CTexture* texture, const CImVector& color) {
    CGxTexFlags texFlags;
    HASHKEY_TEXTUREFILE key;
    key.m_filename = const_cast<char*>("");
    key.m_texFlags = texFlags;
    s_textureCache.Insert(texture, *reinterpret_cast<const uint32_t*>(&color), key);
}

// VERIFY: 875e1f5e auto
// OFFSET: 0x004B9550
CTexture* TextureCreateSolid(const CImVector& color) {
    CTexture* texture = TextureCacheGetTexture(color);

    if (texture) {
        return texture;
    }

    texture = new ("HTEXTURE", SLOG_OBJECT) CTexture;

    if (!texture) {
        return nullptr;
    }

    FillInSolidTexture(color, texture);
    CTexture* handle = reinterpret_cast<CTexture*>(HandleCreate(texture, "HTEXTURE"));
    TextureCacheNewTexture(handle, color);

    return handle;
}

// VERIFY: 8821b223 manual
// OFFSET: 0x004B9760
CTexture* TextureCreate(const char* fileName, CGxTexFlags texFlags, CStatus* status, uint32_t flags) {
    if (!fileName) {
        DebugValidate("fileName");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    if (!fileName[0]) {
        DebugValidate("fileName[0]");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    if (!status) {
        DebugValidate("status");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    if (!(flags & 1)) {
        texFlags.m_filter = s_filterMode;
    }

    texFlags.m_maxAnisotropy = texFlags.m_filter == GxTex_Anisotropic ? s_maxAnisotropy : 1;
    CGxTexFlags requested = texFlags;

    int32_t tries = 2;
    int32_t type = 1;
    char name[260];
    SStrCopy(name, fileName, 260);
    char* extension = OsPathFindExtensionWithDot(name);

    if ((flags & 8) && extension) {
        tries = 1;
        type = SStrCmpI(extension, ".blp", STORM_MAX_STR) == 0;
        extension = nullptr;
    }

    CTexture* texture = TextureCacheGetTexture(name, extension, texFlags);

    if (texture) {
        return texture;
    }

    for (int32_t i = 0; i < tries; i++) {
        if (type == 0) {
            texture = CreateTgaTexture(name, extension, flags & 2, texFlags, status);
        } else if (type == 1) {
            texture = CreateBlpTexture(name, extension, flags, texFlags);
        } else {
            type = (type + 1) % 2;
            continue;
        }

        if (texture) {
            TextureCacheNewTexture(texture, requested);
            return texture;
        }

        texFlags = requested;
        type = (type + 1) % 2;
    }

    TextureReportLoadError("texture", fileName, status);

    return TextureCreateSolid(*reinterpret_cast<const CImVector*>(&s_defaultColor));
}
