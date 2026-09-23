#ifndef ENGINE_SOURCE_IMAGES_TGA_H
#define ENGINE_SOURCE_IMAGES_TGA_H

#include <cstddef>
#include <cstdint>

class SFile;

struct TGAFooter {
    /* 0000 */ uint32_t m_dwExtensionOffset;
    /* 0004 */ uint32_t m_dwDeveloperOffset;
    /* 0008 */ uint8_t m_szSigniture [18];
    /* 001A */ uint8_t unk_001A[2];
};

struct TGAHeader {
    /* 0000 */ uint8_t m_bIDLength;
    /* 0001 */ uint8_t m_bColorMapType;
    /* 0002 */ uint8_t m_bImageType;
    /* 0003 */ uint8_t unk_0003[1];
    /* 0004 */ uint16_t m_wColorMapStartIndex;
    /* 0006 */ uint16_t m_wColorMapEntries;
    /* 0008 */ uint8_t m_bColorMapEntrySize;
    /* 0009 */ uint8_t unk_0009[1];
    /* 000A */ uint16_t m_wXOrigin;
    /* 000C */ uint16_t m_wYOrigin;
    /* 000E */ uint16_t m_wWidth;
    /* 0010 */ uint16_t m_wHeight;
    /* 0012 */ uint8_t m_bPixelDepth;
    /* 0013 */ uint8_t unk_0013[1];
};

class CTgaFile {
    public:
    /* 0000 */ SFile* m_file;
    /* 0004 */ uint8_t* m_image;
    /* 0008 */ TGAHeader m_header;
    /* 001C */ uint8_t* m_addlHeaderData;
    /* 0020 */ TGAFooter m_footer;
    /* 003C */ uint32_t m_imageBytes;
    /* 0040 */ uint8_t* m_colorMap;
};

struct TGAImageDesc {
    /* 0000 */ uint8_t m_bAlphaChannelBits : 4;
    /* 0000 */ uint8_t m_bLeftRightOrder : 1;
    /* 0000 */ uint8_t m_bTopBottomOrder : 1;
    /* 0000 */ uint8_t m_bReserved : 2;
};

#endif
