#ifndef _TRIAGE_CLIENTDB_H
#define _TRIAGE_CLIENTDB_H

#include <cstddef>
#include <cstdint>

class ClientDB {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[8];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[4];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
};

#endif
