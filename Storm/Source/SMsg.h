#ifndef STORM_SOURCE_SMSG_H
#define STORM_SOURCE_SMSG_H

#include <windows.h>
#include "Storm/H/stpl.h"

struct WNDREC : TSLinkedNode<WNDREC> {
    /* 0008 */ HWND window;
};

#endif
