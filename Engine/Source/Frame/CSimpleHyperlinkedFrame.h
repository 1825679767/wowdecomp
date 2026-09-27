#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleHyperlinkedFrame : public CSimpleFrame {
    public:
    /* 029C */ uint32_t m_simpleHyperlinkedFrameUkn1;
    /* 02A0 */ uint32_t m_simpleHyperlinkedFrameUkn2;
    /* 02A4 */ uint32_t m_simpleHyperlinkedFrameUkn3;
    /* 02A8 */ uint32_t m_simpleHyperlinkedFrameUkn4;
    /* 02AC */ uint32_t m_simpleHyperlinkedFrameUkn5;
    /* 02B0 */ uint32_t m_simpleHyperlinkedFrameUkn6;
    /* 02B4 */ uint32_t m_simpleHyperlinkedFrameUkn7;
};

class CSimpleHyperlinkButton {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[28];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t unk_0024[88];
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint8_t unk_0080[540];
    /* 029C */ uint32_t unk_029C[23];
    /* 02F8 */ uint32_t unk_02F8;
    /* 02FC */ uint32_t unk_02FC;
    /* 0300 */ uint8_t unk_0300[752];
};

#endif
