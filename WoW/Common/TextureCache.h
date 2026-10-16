#ifndef WOW_COMMON_TEXTURECACHE_H
#define WOW_COMMON_TEXTURECACHE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/ComponentCore/Types.h"

class CACHEENTRY;
class CAsyncObject;

class CACHEENTRY : public TSHashObject<CACHEENTRY, HASHKEY_NONE> {
    public:
    /* 0018 */ CAsyncObject* m_asyncObject;
    /* 001C */ TCTEXTUREINFO m_info;
    /* 0024 */ char m_fileName [260];
    int32_t m_refCount;
    uint32_t m_memHandle;
    BLPHeader* m_data;
    uint32_t m_size;
    uint32_t m_loadFailed : 1;
    uint32_t m_unused : 31;

    CACHEENTRY();
    ~CACHEENTRY();
    int32_t LoadTexture();
};

void TextureCacheComputeLoadProgress(void* handle, int64_t& local, int64_t& total);
void* TextureCacheCopyTexture(void* handle);
void* TextureCacheCreateTexture(const char* fileName);
void TextureCacheDestroy();
void TextureCacheDestroyTexture(void* handle);
int32_t TextureCacheGetInfo(void* handle, TCTEXTUREINFO& info, int32_t wait);
void* TextureCacheGetMip(void* handle, uint32_t mipLevel);
BlpPalPixel* TextureCacheGetPal(void* handle);
int32_t TextureCacheHasMips(void* handle);
void TextureCacheResetLoadCount();

#endif
