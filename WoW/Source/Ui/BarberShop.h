#ifndef WOW_SOURCE_UI_BARBERSHOP_H
#define WOW_SOURCE_UI_BARBERSHOP_H

#include <cstddef>
#include <cstdint>

class CGBarberShop;

class M2SequenceInfo {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t m_duration;
    /* 0008 */ float m_moveSpeed;
    /* 000C */ float unk_000C [10];
    /* 0034 */ uint32_t m_resolvedAnimId;
    /* 0038 */ uint32_t unk_0038;
};

#endif
