#ifndef WOW_SOURCE_UI_CACCOUNTMSGHEADERS_H
#define WOW_SOURCE_UI_CACCOUNTMSGHEADERS_H

#include <cstddef>
#include <cstdint>

class CAccountMsgHeaders {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[52];
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
};

void AccountMsg_RegisterScriptFunctions();

#endif
