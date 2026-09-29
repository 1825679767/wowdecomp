#ifndef WOW_SOURCE_UI_CHATBUBBLEFRAME_H
#define WOW_SOURCE_UI_CHATBUBBLEFRAME_H

#include <cstddef>
#include <cstdint>

class CGChatBubbleFrame {
    public:
    static void Remove(CGChatBubbleFrame* bubble);

    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[664];
    /* 029C */ uint32_t unk_029C[2];
    /* 02A4 */ uint32_t unk_02A4;
    /* 02A8 */ uint32_t unk_02A8;
    /* 02AC */ uint32_t unk_02AC;
    /* 02B0 */ uint32_t unk_02B0;
    /* 02B4 */ uint32_t unk_02B4;
    /* 02B8 */ uint32_t unk_02B8;
    /* 02BC */ uint32_t unk_02BC;
    /* 02C0 */ uint32_t unk_02C0;
    /* 02C4 */ uint32_t unk_02C4;
    /* 02C8 */ uint32_t unk_02C8;
    /* 02CC */ uint32_t unk_02CC;
    /* 02D0 */ uint32_t unk_02D0;
    /* 02D4 */ uint32_t unk_02D4;
    /* 02D8 */ uint32_t unk_02D8;
    /* 02DC */ uint8_t unk_02DC;
    /* 02DD */ uint8_t unk_02DD[603];
};

#endif
