#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CGCorpse_C : public CGObject_C, public CGCorpse {
    public:
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ uint32_t unk_00DC;
    /* 00E0 */ uint8_t unk_00E0 [24];
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ uint8_t unk_00FC [372];
    /* 0270 */ uint32_t unk_0270;
    /* 0274 */ uint32_t unk_0274;
    /* 0278 */ uint32_t unk_0278;
    /* 027C */ uint32_t unk_027C;
    /* 0280 */ uint32_t unk_0280;
    /* 0284 */ uint32_t unk_0284;
    /* 0288 */ uint32_t unk_0288;
    /* 028C */ uint8_t unk_028C [4];
};

#endif
