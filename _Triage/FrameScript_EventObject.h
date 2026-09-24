#ifndef _TRIAGE_FRAMESCRIPT_EVENTOBJECT_H
#define _TRIAGE_FRAMESCRIPT_EVENTOBJECT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

struct EVENTLISTENERNODE;
class FrameScript_EventObject;
struct HASHKEY_STRI;

class FrameScript_EventObject {
    public:
    /* 0000 */ TSHashObject<FrameScript_EventObject, HASHKEY_STRI> m_hashObject;
    /* 0018 */ TSList<EVENTLISTENERNODE> m_listeners;
    /* 0024 */ TSList<EVENTLISTENERNODE> m_unregisterListeners;
    /* 0030 */ TSList<EVENTLISTENERNODE> m_registerListeners;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ int32_t m_signalCount;
    /* 004C */ int32_t m_pendingSignalCount;
};

#endif
