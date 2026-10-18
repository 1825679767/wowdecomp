#ifndef WOW_SOURCE_SOUNDINTERFACE_TYPES_H
#define WOW_SOURCE_SOUNDINTERFACE_TYPES_H

#include <cstddef>
#include <cstdint>

enum PARRYMATERIALS : int32_t {
    PARRYMATERIAL_WOOD = 0,
    PARRYMATERIAL_METAL = 1,
    NUM_PARRYMATERIALS = 2,
};

enum AMBIENCE : int32_t {
    AMB_DAY = 0,
    AMB_NIGHT = 1,
    NUM_AMBIENCES = 2,
};

enum WEAPONSWING_SOUNDTYPES : int32_t {
    WEAPONSWING_LIGHT = 0,
    WEAPONSWING_MEDIUM = 1,
    WEAPONSWING_HEAVY = 2,
    NUM_WEAPONSWINGSOUNDTYPES = 3,
    // WEAPONSWING_UNUSED = -1, // Unused reference member.
};

enum ITEMSOUNDTYPE : int32_t {
    ITEMSOUND_PICKUP = 0,
    ITEMSOUND_DROP = 1,
    ITEMSOUND_USE = 2,
    ITEMSOUND_UNKNOWN_3 = 3, // Historical close operation is unconfirmed in 3.3.5.
    NUM_ITEMSOUNDS = 4,

    // Unused 0.5.3 name for slot 3; its 3.3.5 meaning is unverified.
    // ITEMSOUND_CLOSE = 3,
};

enum SI2_DSP_TYPE {
    SI2_DSP_TYPE_NONE = 0,
    SI2_DSP_TYPE_PITCHSHIFT = 1,
    SI2_DSP_TYPE_HIGHPASS = 2,
    SI2_DSP_TYPE_LOWPASS = 3,
    SI2_DSP_TYPE_CHORUS = 4,
    SI2_DSP_TYPE_ECHO = 5,
    SI2_DSP_TYPE_VOLUME = 6,
};

#endif
