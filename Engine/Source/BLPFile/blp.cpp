#include <cstdint>
#include <cstring>
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "_Triage/MippedImg.h"

// STATIC: 0x00AD90B0
uint8_t CBLPFile::s_eightBitAlphaLookup[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

// STATIC: 0x00AD90C0
uint8_t CBLPFile::s_oneBitAlphaLookup[2] = { 0x00, 0xFF };

// STATIC: 0x00C79610
static TSGrowableArray<uint8_t> s_blpFileLoadBuffer;

// INLINED
static BlitFormat GetBlitFormat(PIXEL_FORMAT format) {
    // STATIC: 0x00AD91BC
    static BlitFormat blitFmt[NUM_PIXEL_FORMATS] = {
        BlitFormat_Dxt1,
        BlitFormat_Dxt3,
        BlitFormat_Argb8888,
        BlitFormat_Argb1555,
        BlitFormat_Argb4444,
        BlitFormat_Rgb565,
        BlitFormat_Unknown,
        BlitFormat_Dxt5,
        BlitFormat_Unknown,
        BlitFormat_Unknown,
    };

    return blitFmt[format];
}

// INLINED
uint32_t CBLPFile::HasMips() const {
    return this->m_header.m_hasMips & 0xF;
}

// INLINED
int32_t CBLPFile::IsValidMip(uint32_t mipLevel) const {
    return !mipLevel || (this->HasMips() && mipLevel < this->m_numLevels);
}

// VERIFY: e564938b manual
// OFFSET: 0x006AE8B0
void CBLPFile::Close() {
    this->m_inMemoryImage = nullptr;

    if (this->m_images) {
        SMemFree(this->m_images, __FILE__, __LINE__, 0);
    }

    this->m_images = nullptr;
}

// VERIFY: 507be47e auto
// OFFSET: 0x006AE8E0
int32_t CBLPFile::ValidateHeader(const BLPHeader& header) {
    return header.m_magic == 0x32504C42 && header.m_formatVersion == 1;
}

// VERIFY: ec3a562c manual
// OFFSET: 0x006AE900
int32_t CBLPFile::Source(void* data) {
    this->Close();

    this->m_inMemoryNeedsFree = 0;
    this->m_inMemoryImage = data;
    memcpy(&this->m_header, data, sizeof(this->m_header));

    if (!CBLPFile::ValidateHeader(this->m_header)) {
        return 0;
    }

    if (this->HasMips()) {
        this->m_numLevels = CalcLevelCount(this->m_header.m_width, this->m_header.m_height);
    } else {
        this->m_numLevels = 1;
    }

    return 1;
}

// VERIFY: 3d010c6b manual
// OFFSET: 0x006AE990
void CBLPFile::DecompPalFastPath(uint8_t* out, const void* in, uint32_t count) {
    BlpPalPixel* pixels = reinterpret_cast<BlpPalPixel*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    for (uint32_t i = count; i; i--) {
        *pixels = this->m_header.m_extended.m_palette[*indices];
        pixels->m_pad = indices[count];
        pixels++;
        indices++;
    }
}

// VERIFY: 9923d899 manual
// OFFSET: 0x006AE9E0
void CBLPFile::DecompPalARGB8888(uint8_t* out, const void* in, uint32_t count) {
    BlpPalPixel* pixels = reinterpret_cast<BlpPalPixel*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    for (uint32_t i = 0; i < count; i++) {
        pixels[i] = this->m_header.m_extended.m_palette[*indices];
        pixels[i].m_pad = 0xFF;
        indices++;
    }

    const uint8_t* alpha = indices;

    switch (this->m_header.m_alphaSize) {
        case 1: {
            uint32_t bytes = count >> 3;

            for (uint32_t i = 0; i < bytes; i++) {
                uint8_t bits = alpha[i];

                for (uint32_t bit = 0; bit < 8; bit++) {
                    pixels->m_pad = CBLPFile::s_oneBitAlphaLookup[bits & 1];
                    bits >>= 1;
                    pixels++;
                }
            }

            if (count & 7) {
                uint8_t bits = alpha[bytes];

                for (uint32_t bit = count & 7; bit; bit--) {
                    pixels->m_pad = CBLPFile::s_oneBitAlphaLookup[bits & 1];
                    bits >>= 1;
                    pixels++;
                }
            }

            break;
        }

        case 4: {
            for (uint32_t i = count >> 1; i; i--) {
                pixels[0].m_pad = CBLPFile::s_eightBitAlphaLookup[*alpha & 0xF];
                pixels[1].m_pad = CBLPFile::s_eightBitAlphaLookup[*alpha >> 4];
                pixels += 2;
                alpha++;
            }

            if (count & 1) {
                pixels->m_pad = CBLPFile::s_eightBitAlphaLookup[*alpha & 0xF];
            }

            break;
        }

        case 8: {
            for (uint32_t i = 0; i < count; i++) {
                pixels[i].m_pad = *alpha;
                alpha++;
            }

            break;
        }
    }
}

// VERIFY: 083826a8 auto
// OFFSET: 0x006AF660
uint32_t CBLPFile::Width(uint32_t mipLevel) const {
    uint32_t width = this->m_header.m_width >> mipLevel;
    return width > 1 ? width : 1;
}

// VERIFY: 2983056f auto
// OFFSET: 0x006AF680
uint32_t CBLPFile::Height(uint32_t mipLevel) const {
    uint32_t height = this->m_header.m_height >> mipLevel;
    return height > 1 ? height : 1;
}

// VERIFY: 6efde90e auto
// OFFSET: 0x006AF6A0
uint32_t CBLPFile::Pixels(uint32_t mipLevel) const {
    return this->Height(mipLevel) * this->Width(mipLevel);
}

// VERIFY: c1020237 manual
// OFFSET: 0x006AF6E0
int32_t CBLPFile::Unlock(uint32_t mipLevel) {
    if (this->m_lockDecompMem) {
        SMemFree(this->m_lockDecompMem, __FILE__, __LINE__, 0);
    }

    return this->IsValidMip(mipLevel);
}

// VERIFY: ebad915e auto
// OFFSET: 0x006AF730
int32_t CBLPFile::GetFormatSize(PIXEL_FORMAT format, uint32_t mipLevel, uint32_t* size, uint32_t* stride) const {
    uint32_t width = this->Width(mipLevel);
    uint32_t height = this->Height(mipLevel);
    uint32_t pixels = width * height;
    uint32_t alphaBytes = pixels >> 2 ? pixels >> 2 : 1;

    switch (format) {
        case PIXEL_ARGB8888:
            *size = 4 * pixels;
            *stride = 4 * width;
            return 1;

        case PIXEL_ARGB1555:
        case PIXEL_ARGB4444:
        case PIXEL_RGB565:
            *size = 2 * pixels;
            *stride = 2 * width;
            return 1;

        case PIXEL_ARGB2565:
            *size = alphaBytes + 2 * pixels;
            *stride = 2 * width;
            return 1;

        default:
            *size = 0;
            *stride = 0;
            return 0;
    }
}

// VERIFY: a7900f2e manual
// OFFSET: 0x006AF810
int32_t CBLPFile::DecompPalDither(PIXEL_FORMAT format, uint32_t mipLevel, uint8_t* out, const void* in) {
    switch (format) {
        case PIXEL_ARGB8888: {
            uint32_t pixels = this->Pixels(mipLevel);

            if (this->m_header.m_alphaSize == 8) {
                this->DecompPalFastPath(out, in, pixels);
            } else {
                this->DecompPalARGB8888(out, in, pixels);
            }

            return 1;
        }

        case PIXEL_ARGB1555:
            this->DecompPalARGB1555DitherFloydSteinberg(out, in, this->Width(mipLevel), this->Height(mipLevel));
            return 1;

        case PIXEL_ARGB4444:
            this->DecompPalARGB4444DitherFloydSteinberg(out, in, this->Width(mipLevel), this->Height(mipLevel));
            return 1;

        case PIXEL_RGB565:
            this->DecompPalARGB565DitherFloydSteinberg(out, in, this->Width(mipLevel), this->Height(mipLevel));
            return 1;

        case PIXEL_ARGB2565:
            this->DecompPalARGB2565DitherFloydSteinberg(out, in, this->Width(mipLevel), this->Height(mipLevel));
            return 1;

        default:
            return 0;
    }
}

// VERIFY: 53c2d8d4 manual
// OFFSET: 0x006AF990
int32_t CBLPFile::Lock(PIXEL_FORMAT format, uint32_t mipLevel, uint8_t*& data, uint32_t& stride) {
    if (!this->m_inMemoryImage) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    this->m_lockDecompMem = nullptr;

    if (!this->IsValidMip(mipLevel)) {
        return 0;
    }

    uint8_t* image = static_cast<uint8_t*>(this->m_inMemoryImage) + this->m_header.m_mipOffsets[mipLevel];

    if (!this->m_header.m_mipSizes[mipLevel]) {
        return 0;
    }

    switch (this->m_header.m_colorEncoding) {
        case 1: {
            uint32_t size;
            if (!this->GetFormatSize(format, mipLevel, &size, &stride)) {
                return 0;
            }

            data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
            int32_t result = this->DecompPalDither(format, mipLevel, data, image);
            this->m_lockDecompMem = data;

            return result;
        }

        case 2: {
            switch (format) {
                case PIXEL_DXT1:
                case PIXEL_DXT3:
                case PIXEL_DXT5:
                    data = image;
                    return 1;

                case PIXEL_ARGB8888:
                case PIXEL_ARGB1555:
                case PIXEL_ARGB4444:
                case PIXEL_RGB565: {
                    uint32_t size;
                    if (!this->GetFormatSize(format, mipLevel, &size, &stride)) {
                        return 0;
                    }

                    BlitFormat inFormat = GetBlitFormat(static_cast<PIXEL_FORMAT>(this->m_header.m_preferredFormat));
                    BlitFormat outFormat = GetBlitFormat(format);

                    data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
                    this->m_lockDecompMem = data;

                    uint32_t width = this->m_header.m_width >> mipLevel;
                    uint32_t height = this->m_header.m_height >> mipLevel;
                    C2iVector blitSize = { static_cast<int32_t>(width), static_cast<int32_t>(height) };

                    if (Blit(blitSize, BlitAlpha_0, image, CalcRowStride(inFormat, width, height), inFormat, data, stride, outFormat)) {
                        return 1;
                    }

                    SMemFree(data, __FILE__, __LINE__, 0);
                    return 0;
                }

                default:
                    return 1;
            }
        }

        case 3:
            data = image;
            return 1;

        default:
            return 0;
    }
}

// VERIFY: 8029449e manual
// OFFSET: 0x006AFB70
int32_t CBLPFile::LockChain(PIXEL_FORMAT format, MipBits*& images, uint32_t mipLevel) {
    if (!this->IsValidMip(mipLevel)) {
        return 0;
    }

    MipBits* allocated = nullptr;

    if (images) {
        MippedImgSet(format, this->Width(mipLevel), this->Height(mipLevel), images);
    } else {
        allocated = MippedImgAllocA(format, this->Width(mipLevel), this->Height(mipLevel), __FILE__, __LINE__);
        images = allocated;

        if (!allocated) {
            return 0;
        }
    }

    for (uint32_t i = 0; mipLevel < this->m_numLevels; i++, mipLevel++) {
        uint8_t* data;
        uint32_t stride;

        if (!this->Lock(format, mipLevel, data, stride)) {
            if (allocated) {
                SMemFree(allocated, __FILE__, __LINE__, 0);
                images = nullptr;
            }

            return 0;
        }

        memcpy(images[i].m_mip[0], data, CalcLevelSize(mipLevel, this->m_header.m_width, this->m_header.m_height, format));
        this->Unlock(mipLevel);
    }

    return 1;
}

// VERIFY: 282eeba2 manual
// OFFSET: 0x006AFCE0
int32_t CBLPFile::Lock2(const char* fileName, PIXEL_FORMAT format, uint32_t mipLevel, uint8_t* data, uint32_t& stride) {
    if (!this->m_inMemoryImage) {
        DebugValidate("m_inMemoryImage");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!this->IsValidMip(mipLevel)) {
        return 0;
    }

    uint8_t* image = static_cast<uint8_t*>(this->m_inMemoryImage) + this->m_header.m_mipOffsets[mipLevel];
    uint32_t imageSize = this->m_header.m_mipSizes[mipLevel];

    if (!imageSize) {
        return 0;
    }

    switch (this->m_header.m_colorEncoding) {
        case 0:
            DebugLog("%s: JPEG decompresion not enabled", fileName);
            return 0;

        case 1: {
            uint32_t size;
            int32_t result = this->GetFormatSize(format, mipLevel, &size, &stride);

            if (result) {
                result = this->DecompPalDither(format, mipLevel, data, image);
                this->m_lockDecompMem = data;
            }

            return result;
        }

        case 2: {
            switch (format) {
                case PIXEL_DXT1:
                case PIXEL_DXT3:
                case PIXEL_DXT5:
                    memcpy(data, image, imageSize);
                    return 1;

                case PIXEL_ARGB8888:
                case PIXEL_ARGB1555:
                case PIXEL_ARGB4444:
                case PIXEL_RGB565: {
                    uint32_t width = this->Width(mipLevel);
                    uint32_t height = this->Height(mipLevel);
                    BlitFormat inFormat = GetBlitFormat(static_cast<PIXEL_FORMAT>(this->m_header.m_preferredFormat));
                    BlitFormat outFormat = GetBlitFormat(format);
                    C2iVector blitSize = { static_cast<int32_t>(width), static_cast<int32_t>(this->Height(mipLevel)) };

                    return Blit(blitSize, BlitAlpha_0, image, CalcRowStride(inFormat, width, height), inFormat, data, CalcRowStride(outFormat, width, height), outFormat);
                }

                case PIXEL_ARGB2565:
                    DebugLog("%s: component texture saved as DXT compressed", fileName);
                    return 0;

                default:
                    DebugLog("%s: tried to decompress a DXT texture into pixel format %d", fileName, format);
                    return 0;
            }
        }

        case 3:
            memcpy(data, image, imageSize);
            return 1;

        default:
            DebugLog("%s: unsupported image format", fileName);
            return 0;
    }
}

// VERIFY: 8d2086fa manual
// OFFSET: 0x006AFF10
int32_t CBLPFile::Open(const char* fileName, int32_t flags) {
    if (!fileName) {
        DebugValidate("filename");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    this->Close();

    SFileImpl* file;
    if (!SFile::OpenEx(nullptr, fileName, flags != 0, &file)) {
        return 0;
    }

    s_blpFileLoadBuffer.SetCount(SFileGetFileSize(file, nullptr));

    uint32_t bytesRead;
    SFileReadFile(file, s_blpFileLoadBuffer.Ptr(), s_blpFileLoadBuffer.Count(), &bytesRead, nullptr, 0);
    SFileCloseFile(file);

    return this->Source(s_blpFileLoadBuffer.Ptr());
}

// VERIFY: 3d8e87b0 manual
// OFFSET: 0x006AFFD0
int32_t CBLPFile::LockChain2(const char* fileName, PIXEL_FORMAT format, MipBits*& images, uint32_t mipLevel, int32_t inPlace) {
    if (!this->IsValidMip(mipLevel)) {
        return 0;
    }

    if (!images) {
        images = MippedImgAllocA(format, this->Width(mipLevel), this->Height(mipLevel), __FILE__, __LINE__);

        if (!images) {
            return 0;
        }
    } else {
        bool setImages = true;

        if (this->m_header.m_colorEncoding == 2) {
            setImages = format == PIXEL_ARGB4444 || format == PIXEL_RGB565 || format == PIXEL_ARGB1555 || format == PIXEL_ARGB8888 || !inPlace;
        } else if (this->m_header.m_colorEncoding == 3) {
            setImages = !inPlace;
        }

        if (!setImages) {
            for (uint32_t i = 0; this->m_header.m_mipSizes[i]; i++) {
                images[i].m_mip[0] = reinterpret_cast<C4Pixel*>(static_cast<uint8_t*>(this->m_inMemoryImage) + this->m_header.m_mipOffsets[i]);
            }

            this->m_inMemoryImage = nullptr;
            return 1;
        }

        MippedImgSet(format, this->Width(mipLevel), this->Height(mipLevel), images);
    }

    for (uint32_t i = 0; mipLevel < this->m_numLevels; i++, mipLevel++) {
        uint32_t stride;

        if (!this->Lock2(fileName, format, mipLevel, reinterpret_cast<uint8_t*>(images[i].m_mip[0]), stride)) {
            return 0;
        }
    }

    this->m_inMemoryImage = nullptr;

    return 1;
}
