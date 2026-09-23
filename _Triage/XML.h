#ifndef _TRIAGE_XML_H
#define _TRIAGE_XML_H

#include <cstddef>
#include <cstdint>

class XML {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[20];
    /* 0020 */ uint32_t unk_0020;
};

#endif
