#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H

#include <cstddef>
#include <cstdint>

class CSimpleMessageFrame;

class CSimpleMessageFrameLineNode {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
};

#endif
