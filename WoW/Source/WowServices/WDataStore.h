#ifndef WOW_SOURCE_WOWSERVICES_WDATASTORE_H
#define WOW_SOURCE_WOWSERVICES_WDATASTORE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"

class WDataStore : public CDataStore {
    public:
    static void StaticInitialize();

    /* 0018 */ void* m_bufferObj;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
};

#endif
