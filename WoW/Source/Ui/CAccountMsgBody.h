#ifndef WOW_SOURCE_UI_CACCOUNTMSGBODY_H
#define WOW_SOURCE_UI_CACCOUNTMSGBODY_H

#include <cstddef>
#include <cstdint>

class CAccountMsgBody {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[52];
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
};

#endif
