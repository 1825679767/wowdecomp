#ifndef WOW_SOURCE_HARDWARE_H
#define WOW_SOURCE_HARDWARE_H

#include <cstddef>
#include <cstdint>

struct CpuHardware {
    /* 0000 */ uint32_t m_farclipIdx;
    /* 0004 */ uint32_t m_animatingDoodadIdx;
    /* 0008 */ uint32_t m_waterLODIdx;
    /* 000C */ uint32_t m_particleDensityIdx;
    /* 0010 */ uint32_t m_smallCullDistIdx;
    /* 0014 */ uint32_t m_unitDrawDistIdx;
};

struct SoundHardware {
    /* 0000 */ uint32_t m_numChannels;
    /* 0004 */ bool m_fivePointOne;
    /* 0005 */ uint8_t unk_0005[3];
};

#endif
