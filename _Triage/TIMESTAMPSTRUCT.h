#ifndef _TRIAGE_TIMESTAMPSTRUCT_H
#define _TRIAGE_TIMESTAMPSTRUCT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class TIMESTAMPSTRUCT : public TSHashObject<TIMESTAMPSTRUCT, HASHKEY_NONE> {
    public:
    /* 0018 */ uint32_t unk_0018[6];
};

#endif
