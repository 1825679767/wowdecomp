#ifndef WOW_COMMON_TEXTURECACHE_H
#define WOW_COMMON_TEXTURECACHE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/ComponentCore/Types.h"

class CACHEENTRY;
class CAsyncObject;
struct HASHKEY_NONE;

class CACHEENTRY : public TSHashObject<CACHEENTRY, HASHKEY_NONE> {
    public:
    /* 0018 */ CAsyncObject* m_asyncObject;
    /* 001C */ TCTEXTUREINFO m_info;
    /* 0024 */ char m_fileName [128];
    /* 00A4 */ uint32_t m_refCount;
    /* 00A8 */ uint32_t m_memHandle;
    /* 00AC */ BLPHeader* m_data;
    /* 00B0 */ uint32_t m_flags;
};

#endif
