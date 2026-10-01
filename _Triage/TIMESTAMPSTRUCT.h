#ifndef _TRIAGE_TIMESTAMPSTRUCT_H
#define _TRIAGE_TIMESTAMPSTRUCT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CGameTime;

struct TIMESTAMPCALLBACK : public TSLinkedNode<TIMESTAMPCALLBACK> {
    /* 0008 */ void* m_param;
    /* 000C */ void (__stdcall* m_callback)(CGameTime* time, void* param);
};

class TIMESTAMPSTRUCT : public TSHashObject<TIMESTAMPSTRUCT, HASHKEY_NONE> {
    public:
    /* 0018 */ TSList<TIMESTAMPCALLBACK, TSGetLink<TIMESTAMPCALLBACK>> m_callbacks;
    /* 0024 */ uint32_t unk_0024[3];
};

#endif
