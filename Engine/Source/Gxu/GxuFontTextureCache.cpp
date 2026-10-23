#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Gxu/GxuFontMiscClasses.h"
#include "Engine/Source/Gxu/GxuFontUtil.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C7D328
static uint16_t s_textureData[GXU_FONT_LEGACY_ATLAS_SIZE * GXU_FONT_LEGACY_ATLAS_SIZE];

// The high-resolution page is 512x512 ARGB8888.  Glyphs are rasterized at their display
// size, so a cell can be several hundred rows tall; a 256-row page could not hold one.
static uint32_t s_highResolutionTextureData[GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE * GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE];

// The legacy paste routines below address the 256x256 page with hard-coded row strides and
// fixed stack scratch buffers, so their cell height stays bounded by the original limit.
static constexpr uint32_t kMaxFontCellHeight = GXU_FONT_LEGACY_CELL_HEIGHT_LIMIT;

// INLINED
uint32_t TEXTURECACHE::GetAtlasSize() const {
    return this->m_theFace && this->m_theFace->IsHighResolution() ? GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE : GXU_FONT_LEGACY_ATLAS_SIZE;
}

// Scratch buffer for the outlined high-resolution paste.  Sized for the largest cell a
// 512-row page can hold; reused by every glyph because pasting is single-threaded.
static uint8_t* AcquireHighResolutionOutlineState(uint32_t bytes) {
    static uint8_t* s_state = nullptr;
    static uint32_t s_stateSize = 0;

    if (bytes > s_stateSize) {
        if (s_state) {
            SMemFree(s_state, __FILE__, __LINE__, 0);
        }

        s_state = static_cast<uint8_t*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
        s_stateSize = s_state ? bytes : 0;
    }

    return s_state;
}

// VERIFY: 7a9fdc75 manual
// OFFSET: 0x006C8E70
void TEXTURECACHE::PasteGlyphOutlinedMonochrome(const GLYPHBITMAPDATA& data, uint16_t* dest) {
    const uint8_t* source = static_cast<const uint8_t*>(data.m_data);
    uint32_t thick = this->m_theFace->m_fontFlags & 8;

    uint16_t buffer[256 * kMaxFontCellHeight];
    memset(buffer, 0, sizeof(buffer));

    uint32_t yStart = data.m_yStart;
    uint16_t* line = &buffer[256 * yStart + 257];

    if (thick) {
        line++;
    }

    for (uint32_t y = data.m_glyphHeight; y; y--) {
        for (uint32_t x = 0; x < data.m_glyphWidth; x++) {
            line[x] = (source[x >> 3] >> (7 - (x & 7))) & 1;
        }

        source += data.m_glyphPitch;
        line += 256;
    }

    uint32_t cellHeight = this->m_theFace->m_cellHeight;
    uint32_t cellWidth = data.m_glyphCellWidth;
    uint32_t passes = (thick != 0) + 1;

    for (uint32_t pass = 0; pass < passes; pass++) {
        uint32_t y = yStart;

        if (pass == 1 && yStart) {
            y = yStart - 1;
        }

        for (; y < cellHeight; y++) {
            uint32_t row = y << 8;

            for (uint32_t x = 0; x < cellWidth; x++) {
                uint16_t mask = 0;
                uint16_t value = 0;

                if (pass == 1) {
                    if (buffer[row + x]) {
                        continue;
                    }

                    mask = 4;
                    value = 2;
                } else if (pass == 0) {
                    mask = 1;
                    value = 4;
                }

                uint16_t* cell = &buffer[row + x];
                int32_t found;

                if (y == 0) {
                    if (x == 0) {
                        found = (cell[1] & mask) || (cell[257] & mask) || (cell[256] & mask);
                    } else if (x == cellWidth - 1) {
                        found = (cell[-1] & mask) || (cell[255] & mask) || (cell[256] & mask);
                    } else {
                        found = (cell[-1] & mask) || (cell[1] & mask) || (cell[255] & mask) || (cell[256] & mask) || (cell[257] & mask);
                    }
                } else if (y == cellHeight - 1) {
                    if (x == 0) {
                        found = (cell[-256] & mask) || (cell[-255] & mask) || (cell[1] & mask);
                    } else if (x == cellWidth - 1) {
                        found = (cell[-1] & mask) || (cell[-257] & mask) || (cell[-256] & mask);
                    } else {
                        found = (cell[-257] & mask) || (cell[-256] & mask) || (cell[-255] & mask) || (cell[-1] & mask) || (cell[1] & mask);
                    }
                } else if (x == 0) {
                    found = (cell[-256] & mask) || (cell[-255] & mask) || (cell[1] & mask) || (cell[257] & mask) || (cell[256] & mask);
                } else if (x == cellWidth - 1) {
                    found = (cell[-256] & mask) || (cell[-257] & mask) || (cell[-1] & mask) || (cell[255] & mask) || (cell[256] & mask);
                } else {
                    found = (cell[-257] & mask) || (cell[-256] & mask) || (cell[-255] & mask) || (cell[-1] & mask) || (cell[1] & mask) || (cell[255] & mask) || (cell[256] & mask) || (cell[257] & mask);
                }

                if (found && *cell != 1) {
                    *cell = value;
                }
            }
        }
    }

    uint16_t* bufferRow = buffer;

    for (uint32_t y = 0; y < this->m_theFace->m_cellHeight; y++) {
        for (uint32_t x = 0; x < data.m_glyphCellWidth; x++) {
            uint16_t value = bufferRow[x];

            if (value == 1) {
                dest[x] = 0xFFFF;
            } else if (value == 2) {
                dest[x] = 0x7000;
            } else {
                dest[x] = value != 4 ? 0 : 0xF000;
            }
        }

        bufferRow += 256;
        dest += 256;
    }
}

// VERIFY: 891087d8 manual
// OFFSET: 0x006C9330
void TEXTURECACHE::PasteGlyphNonOutlinedMonochrome(const GLYPHBITMAPDATA& data, uint16_t* dest) {
    const uint8_t* source = static_cast<const uint8_t*>(data.m_data);
    uint32_t pitch = data.m_glyphPitch;
    size_t size = 2 * data.m_glyphCellWidth;

    for (uint32_t y = 0; y < data.m_yStart; y++) {
        memset(dest, 0, size);
        dest += 256;
    }

    for (uint32_t y = 0; y < data.m_glyphHeight; y++) {
        for (uint32_t x = 0; x < data.m_glyphWidth; x++) {
            dest[x] = -(((source[x >> 3] >> (7 - (x & 7))) & 1) != 0);
        }

        source += pitch;
        dest += 256;
    }

    int32_t remaining = this->m_theFace->m_cellHeight - data.m_yStart - data.m_glyphHeight;

    if (remaining > 0) {
        for (uint32_t y = 0; y < static_cast<uint32_t>(remaining); y++) {
            memset(dest, 0, size);
            dest += 256;
        }
    }
}

// VERIFY: c5efdcbb manual
// OFFSET: 0x006C9420
void TEXTURECACHE::PasteGlyphOutlinedAA(const GLYPHBITMAPDATA& data, uint16_t* dest) {
    static const uint8_t s_outlineAlpha[] = { 0, 1, 1, 3, 5, 7, 9, 11, 13, 15 };

    const uint8_t* source = static_cast<const uint8_t*>(data.m_data);
    uint32_t thick = this->m_theFace->m_fontFlags & 8;

    uint16_t alpha[256 * kMaxFontCellHeight];
    uint16_t coverage[256 * kMaxFontCellHeight];
    uint16_t state[256 * kMaxFontCellHeight];

    memset(state, 0, sizeof(state));
    memset(coverage, 0, sizeof(coverage));

    uint16_t* stateLine = &state[256 * data.m_yStart + 257];

    if (thick) {
        stateLine++;
    }

    uint16_t* coverageLine = &coverage[stateLine - state];

    for (uint32_t y = data.m_glyphHeight; y; y--) {
        for (uint32_t x = 0; x < data.m_glyphWidth; x++) {
            uint8_t value = source[x];

            if (value) {
                stateLine[x] = 1;
                coverageLine[x] = value;
            }
        }

        coverageLine += 256;
        source += data.m_glyphPitch;
        stateLine += 256;
    }

    uint32_t cellHeight = this->m_theFace->m_cellHeight;
    uint32_t passes = (thick != 0) + 1;

    for (uint32_t pass = 0; pass < passes; pass++) {
        uint16_t value = 2 * (pass != 0) + 2;
        uint16_t mask = 2 * (pass != 0) + 1;
        int32_t firstRow = static_cast<int32_t>(data.m_yStart) - 1;

        for (uint32_t y = firstRow < 0 ? 0 : firstRow; y < cellHeight; y++) {
            uint32_t cellWidth = data.m_glyphCellWidth;
            uint32_t row = y << 8;

            for (uint32_t x = 0; x < cellWidth; x++) {
                uint16_t* cell = &state[row + x];

                if (*cell & mask) {
                    continue;
                }

                int32_t found;

                if (y == 0) {
                    if (x == 0) {
                        found = (cell[1] & mask) || (cell[257] & mask) || (cell[256] & mask);
                    } else if (x == cellWidth - 1) {
                        found = (cell[-1] & mask) || (cell[255] & mask) || (cell[256] & mask);
                    } else {
                        found = (cell[-1] & mask) || (cell[1] & mask) || (cell[255] & mask) || (cell[256] & mask) || (cell[257] & mask);
                    }
                } else if (y == cellHeight - 1) {
                    if (x == 0) {
                        found = (cell[-256] & mask) || (cell[-255] & mask) || (cell[1] & mask);
                    } else if (x == cellWidth - 1) {
                        found = (cell[-1] & mask) || (cell[-257] & mask) || (cell[-256] & mask);
                    } else {
                        found = (cell[-257] & mask) || (cell[-256] & mask) || (cell[-255] & mask) || (cell[-1] & mask) || (cell[1] & mask);
                    }
                } else if (x == 0) {
                    found = (cell[-256] & mask) || (cell[-255] & mask) || (cell[1] & mask) || (cell[257] & mask) || (cell[256] & mask);
                } else if (x == cellWidth - 1) {
                    found = (cell[-256] & mask) || (cell[-257] & mask) || (cell[-1] & mask) || (cell[255] & mask) || (cell[256] & mask);
                } else {
                    found = (cell[-257] & mask) || (cell[-256] & mask) || (cell[-255] & mask) || (cell[-1] & mask) || (cell[1] & mask) || (cell[255] & mask) || (cell[256] & mask) || (cell[257] & mask);
                }

                if (found) {
                    *cell = value;
                }
            }
        }
    }

    memset(alpha, 0, sizeof(alpha));

    for (uint32_t y = 0; y < cellHeight; y++) {
        uint32_t row = y << 8;

        for (uint32_t x = 0; x < data.m_glyphCellWidth; x++) {
            uint16_t* cell = &state[row + x];
            uint32_t cellWidth = data.m_glyphCellWidth;
            int32_t count;

            if (y == 0) {
                if (x == 0) {
                    count = (cell[0] != 0) + (cell[1] != 0) + (cell[257] != 0) + (cell[256] != 0);
                } else if (x == cellWidth - 1) {
                    count = (cell[0] != 0) + (cell[256] != 0) + (cell[255] != 0) + (cell[-1] != 0);
                } else {
                    count = (cell[0] != 0) + (cell[1] != 0) + (cell[257] != 0) + (cell[256] != 0) + (cell[255] != 0) + (cell[-1] != 0);
                }
            } else if (y == cellHeight - 1) {
                if (x == 0) {
                    count = (cell[0] != 0) + (cell[1] != 0) + (cell[-255] != 0) + (cell[-256] != 0);
                } else if (x == cellWidth - 1) {
                    count = (cell[-256] != 0) + (cell[-257] != 0) + (cell[0] != 0) + (cell[-1] != 0);
                } else {
                    count = (cell[-256] != 0) + (cell[-257] != 0) + (cell[-255] != 0) + (cell[1] != 0) + (cell[0] != 0) + (cell[-1] != 0);
                }
            } else if (x == 0) {
                count = (cell[0] != 0) + (cell[256] != 0) + (cell[1] != 0) + (cell[257] != 0) + (cell[-255] != 0) + (cell[-256] != 0);
            } else if (x == cellWidth - 1) {
                count = (cell[256] != 0) + (cell[255] != 0) + (cell[-256] != 0) + (cell[-257] != 0) + (cell[0] != 0) + (cell[-1] != 0);
            } else {
                count = (cell[256] != 0) + (cell[255] != 0) + (cell[-255] != 0) + (cell[-256] != 0) + (cell[-257] != 0) + (cell[257] != 0) + (cell[1] != 0) + (cell[0] != 0) + (cell[-1] != 0);
            }

            alpha[row + x] = s_outlineAlpha[count];
        }
    }

    uint32_t offset = 0;

    for (uint32_t y = 0; y < this->m_theFace->m_cellHeight; y++) {
        uint32_t index = offset;

        for (uint32_t x = 0; x < data.m_glyphCellWidth; x++) {
            if (!state[index]) {
                dest[index] = 0;
            } else if (!coverage[index]) {
                dest[index] = alpha[index] << 12;
            } else {
                uint8_t gray = (255 * static_cast<uint8_t>(coverage[index])) >> 12;
                dest[index] = ((((alpha[index] << 4) | gray) << 4 | gray) << 4) | gray;
            }

            index++;
        }

        offset += 256;
    }
}

// VERIFY: 45fd06c3 manual
// OFFSET: 0x006C9C70
void TEXTURECACHE::PasteGlyphNonOutlinedAA(const GLYPHBITMAPDATA& data, uint16_t* dest) {
    const uint8_t* source = static_cast<const uint8_t*>(data.m_data);
    uint32_t pitch = data.m_glyphPitch;
    size_t size = 2 * data.m_glyphCellWidth;

    for (uint32_t y = 0; y < data.m_yStart; y++) {
        memset(dest, 0, size);
        dest += 256;
    }

    for (uint32_t y = 0; y < data.m_glyphHeight; y++) {
        for (uint32_t x = 0; x < data.m_glyphWidth; x++) {
            dest[x] = (source[x] << 8) | 0xFFF;
        }

        source += pitch;
        dest += 256;
    }

    int32_t remaining = this->m_theFace->m_cellHeight - data.m_yStart - data.m_glyphHeight;

    if (remaining > 0) {
        for (uint32_t y = 0; y < static_cast<uint32_t>(remaining); y++) {
            memset(dest, 0, size);
            dest += 256;
        }
    }
}

// Writes one glyph cell into the 8-bit page.  The glyph is rasterized at its display size, so
// its coverage values are copied straight into the alpha channel with no 4-bit quantisation.
// Outline and monochrome fonts keep the shape the legacy path produces (a dilated edge whose
// alpha comes from the number of filled neighbours) but at 8-bit precision.
void TEXTURECACHE::PasteGlyphHighResolution(const GLYPHBITMAPDATA& data, uint32_t* dest, uint32_t fontFlags) {
    static const uint8_t s_outlineAlpha[] = { 0, 1, 1, 3, 5, 7, 9, 11, 13, 15 };

    const uint8_t* source = static_cast<const uint8_t*>(data.m_data);
    const uint32_t atlasSize = GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE;
    const uint32_t cellHeight = this->m_theFace->m_cellHeight;
    // A cell wider than the page would bleed into the next row; the allocator never hands one
    // out, but the paste itself stays inside the page either way.
    const uint32_t cellWidth = data.m_glyphCellWidth > atlasSize ? atlasSize : data.m_glyphCellWidth;
    const uint32_t firstGlyphRow = data.m_yStart;
    const uint32_t lastGlyphRow = firstGlyphRow + data.m_glyphHeight;
    const bool monochrome = (fontFlags & 2) != 0;
    const bool outlined = (fontFlags & (1 | 8)) != 0;
    const uint32_t outlinePasses = (fontFlags & 8) ? 2 : (outlined ? 1 : 0);

    // dest already points at the first texel of the cell inside the page.  Clearing the full
    // cell also clears the transparent column that separates it from the next glyph.
    for (uint32_t y = 0; y < cellHeight; y++) {
        memset(dest + y * atlasSize, 0, cellWidth * sizeof(uint32_t));
    }

    if (!outlinePasses) {
        for (uint32_t y = 0; y < data.m_glyphHeight && firstGlyphRow + y < cellHeight; y++) {
            uint32_t* row = dest + (firstGlyphRow + y) * atlasSize;

            for (uint32_t x = 0; x < data.m_glyphWidth && x < cellWidth; x++) {
                uint8_t value = monochrome ? static_cast<uint8_t>(((source[x >> 3] >> (7 - (x & 7))) & 1) * 0xFF) : source[x];

                if (value) {
                    row[x] = (static_cast<uint32_t>(value) << 24) | 0x00FFFFFF;
                }
            }

            source += data.m_glyphPitch;
        }

        return;
    }

    // Outlined path: mark the glyph in a per-cell state buffer, grow it outwards, then shade
    // the grown ring from the number of filled neighbours so the edge keeps its anti-aliasing.
    uint8_t* state = AcquireHighResolutionOutlineState(atlasSize * cellHeight);

    if (!state) {
        return;
    }

    memset(state, 0, atlasSize * cellHeight);

    for (uint32_t y = 0; y < data.m_glyphHeight && firstGlyphRow + y < cellHeight; y++) {
        uint8_t* stateRow = state + (firstGlyphRow + y) * atlasSize;

        for (uint32_t x = 0; x < data.m_glyphWidth && x < cellWidth; x++) {
            uint8_t value = monochrome ? static_cast<uint8_t>(((source[x >> 3] >> (7 - (x & 7))) & 1) * 0xFF) : source[x];

            if (value) {
                stateRow[x] = 1;
                dest[(firstGlyphRow + y) * atlasSize + x] = (static_cast<uint32_t>(value) << 24) | 0x00FFFFFF;
            }
        }

        source += data.m_glyphPitch;
    }

    for (uint32_t pass = 0; pass < outlinePasses; pass++) {
        uint8_t value = static_cast<uint8_t>(pass + 2);
        uint8_t mask = static_cast<uint8_t>((1 << (pass + 1)) | 1);

        for (uint32_t y = 0; y < cellHeight; y++) {
            uint8_t* row = state + y * atlasSize;

            for (uint32_t x = 0; x < cellWidth; x++) {
                if (row[x] & mask) {
                    continue;
                }

                bool found = false;

                for (int32_t dy = -1; dy <= 1 && !found; dy++) {
                    int32_t ny = static_cast<int32_t>(y) + dy;

                    if (ny < 0 || ny >= static_cast<int32_t>(cellHeight)) {
                        continue;
                    }

                    const uint8_t* neighbour = state + ny * atlasSize;

                    for (int32_t dx = -1; dx <= 1; dx++) {
                        int32_t nx = static_cast<int32_t>(x) + dx;

                        if (nx < 0 || nx >= static_cast<int32_t>(cellWidth)) {
                            continue;
                        }

                        if (neighbour[nx] & mask) {
                            found = true;
                            break;
                        }
                    }
                }

                if (found) {
                    row[x] = value;
                }
            }
        }
    }

    for (uint32_t y = 0; y < cellHeight; y++) {
        const uint8_t* stateRow = state + y * atlasSize;
        uint32_t* destRow = dest + y * atlasSize;

        for (uint32_t x = 0; x < cellWidth; x++) {
            if (stateRow[x] < 2) {
                continue;
            }

            uint32_t filled = 0;

            for (int32_t dy = -1; dy <= 1; dy++) {
                int32_t ny = static_cast<int32_t>(y) + dy;

                if (ny < 0 || ny >= static_cast<int32_t>(cellHeight)) {
                    continue;
                }

                const uint8_t* neighbour = state + ny * atlasSize;

                for (int32_t dx = -1; dx <= 1; dx++) {
                    int32_t nx = static_cast<int32_t>(x) + dx;

                    if (nx < 0 || nx >= static_cast<int32_t>(cellWidth)) {
                        continue;
                    }

                    if (neighbour[nx]) {
                        filled++;
                    }
                }
            }

            uint32_t alpha = static_cast<uint32_t>(s_outlineAlpha[filled]) * 17;
            destRow[x] = (alpha << 24) | 0x00FFFFFF;
        }
    }
}

// VERIFY: d9c18e4c auto
// OFFSET: 0x006C9D50
void TEXTURECACHE::UpdateDirty() {
    if (this->m_anyDirtyGlyphs && this->m_texture) {
        CGxTex* gxTex = TextureGetGxTex(this->m_texture, 1, nullptr);
        uint32_t atlasSize = this->GetAtlasSize();
        GxTexUpdate(gxTex, 0, 0, atlasSize, atlasSize, 1);
        this->m_anyDirtyGlyphs = 0;
    }
}

// VERIFY: 0d8fc085 manual
// OFFSET: 0x006C9D90
CHARCODEDESC* TEXTURECACHE::AllocateNewGlyph(GLYPHBITMAPDATA* data) {
    uint32_t count = this->m_textureRows.m_count;

    for (uint32_t i = 0; i < count; i++) {
        CHARCODEDESC* desc = this->m_textureRows.m_data[i].CreateNewDesc(data, i, this->m_theFace->m_cellHeight, this->GetAtlasSize());

        if (desc) {
            return desc;
        }
    }

    return nullptr;
}

// VERIFY: 248ca280 manual
// OFFSET: 0x006C9F50
void TEXTURECACHE::TextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command != GxTex_Latch) {
        return;
    }

    TEXTURECACHE* cache = static_cast<TEXTURECACHE*>(userArg);
    CGxFont* font = cache->m_theFace;
    const bool highResolution = font && font->IsHighResolution();
    const uint32_t atlasSize = highResolution ? GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE : GXU_FONT_LEGACY_ATLAS_SIZE;

    if (highResolution) {
        memset(s_highResolutionTextureData, 0, sizeof(s_highResolutionTextureData));
    }

    for (uint32_t row = cache->m_textureRows.m_count; row > 0; row--) {
        TEXTURECACHEROW& cacheRow = cache->m_textureRows.m_data[row - 1];

        for (CHARCODEDESC* glyph = cacheRow.m_glyphlist.Head(); glyph; glyph = cacheRow.m_glyphlist.Next(glyph)) {
            if (!cache->m_texture || !font || !font->m_cellHeight) {
                continue;
            }

            uint32_t offset = glyph->m_glyphStartPixel + glyph->m_rowNumber * font->m_cellHeight * atlasSize;

            if (highResolution) {
                cache->PasteGlyphHighResolution(glyph->m_bitmapData, &s_highResolutionTextureData[offset], font->m_fontFlags);
                continue;
            }

            uint16_t* dest = &s_textureData[offset];

            if (font->m_fontFlags & 1) {
                if (font->m_fontFlags & 2) {
                    cache->PasteGlyphOutlinedMonochrome(glyph->m_bitmapData, dest);
                } else {
                    cache->PasteGlyphOutlinedAA(glyph->m_bitmapData, dest);
                }
            } else if (font->m_fontFlags & 2) {
                cache->PasteGlyphNonOutlinedMonochrome(glyph->m_bitmapData, dest);
            } else {
                cache->PasteGlyphNonOutlinedAA(glyph->m_bitmapData, dest);
            }
        }
    }

    if (highResolution) {
        texelStrideInBytes = atlasSize * sizeof(uint32_t);
        texels = s_highResolutionTextureData;
    } else {
        texelStrideInBytes = atlasSize * sizeof(uint16_t);
        texels = s_textureData;
    }
}

// VERIFY: 0f656a97 auto
// OFFSET: 0x006CA050
void TEXTURECACHE::CreateTexture(int32_t filter, bool highResolution) {
    CGxTexFlags flags(static_cast<EGxTexFilter>(filter != 0), 0, 0, 0, 0, 0, 1, 0, 0, 0);
    EGxTexFormat format = highResolution ? GxTex_Argb8888 : GxTex_Argb4444;
    uint32_t atlasSize = highResolution ? GXU_FONT_HIGH_RESOLUTION_ATLAS_SIZE : GXU_FONT_LEGACY_ATLAS_SIZE;
    this->m_texture = TextureCreate(atlasSize, atlasSize, format, format, flags, this, TEXTURECACHE::TextureCallback, "GxuFont", 0);
}

// VERIFY: ba005b39 manual
// OFFSET: 0x006CA130
void TEXTURECACHE::Initialize(CGxFont* face, uint32_t cellHeight) {
    this->m_theFace = face;
    uint32_t atlasSize = this->GetAtlasSize();
    uint32_t rows = atlasSize / cellHeight;
    this->m_textureRows.SetCount(rows);

    for (uint32_t i = 0; i < rows; i++) {
        this->m_textureRows.m_data[i].m_widestFreeSlot = atlasSize;
    }
}

// INLINED
TEXTURECACHE::TEXTURECACHE() {
    this->m_texture = nullptr;
    this->m_theFace = nullptr;
    this->m_anyDirtyGlyphs = 0;
}

// INLINED
TEXTURECACHE::~TEXTURECACHE() {
    this->Destroy();
}

// INLINED
void TEXTURECACHE::Destroy() {
    if (this->m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_texture));
    }

    this->m_texture = nullptr;
    this->m_textureRows.Clear();
}
