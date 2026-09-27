#ifndef WOW_SOURCE_UI_CKBSYSTEM_H
#define WOW_SOURCE_UI_CKBSYSTEM_H

#include "Engine/Source/Base/DynamicString.h"

extern CDynamicString s_motd;

class CKBSystem {
    public:
    static void Initialize();
    static void Shutdown();
};

#endif
