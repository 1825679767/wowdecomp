#ifndef ENGINE_SOURCE_BASE_RCSTRING_H
#define ENGINE_SOURCE_BASE_RCSTRING_H

#include <cstddef>
#include <cstdint>

class TRefCnt {
    public:
    /* 0004 */ uint32_t m_refcnt;

    virtual ~TRefCnt();
};

class RCString : public TRefCnt {
    public:
    /* 0008 */ uint8_t* m_str;
};

class CStringMemory {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
};

#endif
