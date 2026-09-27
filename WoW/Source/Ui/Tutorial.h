#ifndef WOW_SOURCE_UI_TUTORIAL_H
#define WOW_SOURCE_UI_TUTORIAL_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

class CDataStore;

class CGTutorial {
    public:
    static int32_t OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
};

#endif
