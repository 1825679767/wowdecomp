#include <cstddef>
#include <cstdint>
#include <new>
#include <cstring>
#include <vector>
#include "WXL/BlpCompat.hpp"
#include "WoW/Common/TextureCache.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B6BA48
static HASHKEY_NONE s_cacheKey;

// STATIC: 0x00B6BA4C
static uint32_t* s_entryHeap;

// STATIC: 0x00B6BA50
static int32_t s_loadCount;

// STATIC: 0x00B6BA54
static TSHashTable<CACHEENTRY, HASHKEY_NONE> s_cacheTable;

// VERIFY: dead4cbd manual
// OFFSET: 0x004F2B40
static void FreeRequest(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// VERIFY: d664e324 manual
// OFFSET: 0x004F2B70
static void LoadSuccessCallback(void* param) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(param);
    // The async owner must retire before its SMem buffer can be replaced/freed.
    AsyncFileReadDestroyObject(entry->m_asyncObject);
    entry->m_asyncObject = nullptr;
    wxl::native::ComponentBlpInfo info;
    std::vector<uint8_t> converted;
    bool valid=wxl::native::PrepareComponentBlp(entry->m_data,entry->m_size,converted,info);
    if (valid && !converted.empty()) {
        auto* replacement=static_cast<BLPHeader*>(SMemAlloc(converted.size(),__FILE__,__LINE__,0));
        if (!replacement) valid=false;
        else {
            std::memcpy(replacement,converted.data(),converted.size());
            SMemFree(entry->m_data,__FILE__,__LINE__,0);
            entry->m_data=replacement;
            entry->m_size=static_cast<uint32_t>(converted.size());
        }
    }
    if (!valid) {
        if(entry->m_data)SMemFree(entry->m_data,__FILE__,__LINE__,0);
        entry->m_data=nullptr;entry->m_size=0;entry->m_loadFailed=1;entry->m_info={};entry->m_info.m_opaque=1;
        return;
    }
    entry->m_info.m_alphaSize=info.alphaSize;
    entry->m_info.m_opaque=info.alphaSize==0;
    entry->m_info.m_mipCount=info.mipCount;
    entry->m_info.m_height=info.height;
    entry->m_info.m_width=info.width; // Publish only completely validated palette data.
}

// VERIFY: native typed bounded replacement of legacy async allocation.
int32_t CACHEENTRY::LoadTexture() {
    if (this->m_loadFailed) return 0;
    if (this->m_asyncObject || this->m_data) return 1;
    SFileImpl* file = nullptr;
    if (!SFile::OpenEx(nullptr, this->m_fileName, 0, &file)) {
        char alternate[sizeof(this->m_fileName)]{};
        TEXFILETYPE type = TextureDiscoverFileType(this->m_fileName);
        TexturePickAlternateFilename(this->m_fileName, type, alternate, sizeof(alternate));
        if(alternate[0])SFile::OpenEx(nullptr, alternate, 0, &file);
    }
    if (!file) {this->m_loadFailed=1;return 0;}
    uint32_t high=0;
    const uint32_t size=SFileGetFileSize(file,&high);
    constexpr uint32_t limit=64u*1024u*1024u;
    if(high || size<offsetof(BLPHeader,m_extended) || size>limit){
        SFileCloseFile(file);this->m_loadFailed=1;return 0;
    }
    auto* buffer=static_cast<BLPHeader*>(SMemAlloc(size,__FILE__,__LINE__,0));
    if(!buffer){SFileCloseFile(file);this->m_loadFailed=1;return 0;}
    // The legacy queue has no byte-count/error result in its postload callback.
    // A failed/partial read must not expose uninitialized allocation contents.
    std::memset(buffer,0,size);
    auto* object=AsyncFileReadAllocObject();
    if(!object){SMemFree(buffer,__FILE__,__LINE__,0);SFileCloseFile(file);this->m_loadFailed=1;return 0;}
    this->m_size=size;this->m_data=buffer;this->m_asyncObject=object;
    object->m_userArg=this;
    object->m_userPostloadCallback=reinterpret_cast<void*>(&LoadSuccessCallback);
    object->m_file=file;object->m_size=size;object->m_priority=130;object->m_buffer=buffer;
    AsyncFileReadObject(object,0);
    return 1;
}
// VERIFY: 70cae207 manual
// OFFSET: 0x004F2CE0
void* TextureCacheCopyTexture(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (entry) {
        entry->m_refCount++;
    }

    return entry;
}

// VERIFY: 7bec2c4f manual
// OFFSET: 0x004F2D00
void* TextureCacheGetMip(void* handle, uint32_t mipLevel) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry || entry->m_loadFailed || entry->m_asyncObject || !entry->m_data || mipLevel >= entry->m_info.m_mipCount) {
        return nullptr;
    }

    const uint32_t offset=entry->m_data->m_mipOffsets[mipLevel],size=entry->m_data->m_mipSizes[mipLevel];
    if(offset<sizeof(BLPHeader)||offset>entry->m_size||!size||size>entry->m_size-offset)return nullptr;
    return reinterpret_cast<uint8_t*>(entry->m_data)+offset;
}

// VERIFY: 0bb1b277 manual
// OFFSET: 0x004F2D40
BlpPalPixel* TextureCacheGetPal(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry || entry->m_loadFailed || entry->m_asyncObject || !entry->m_info.m_mipCount || !entry->m_data || entry->m_size<sizeof(BLPHeader) || entry->m_data->m_colorEncoding != 1) {
        return nullptr;
    }

    return entry->m_data->m_extended.m_palette;
}

// VERIFY: 8b89ff06 manual
// OFFSET: 0x004F2D80
int32_t TextureCacheHasMips(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    return entry && entry->m_data && !entry->m_asyncObject && !entry->m_loadFailed && entry->m_info.m_width && entry->m_info.m_mipCount;
}

// VERIFY: 195da797 auto
// OFFSET: 0x004F2DB0
void TextureCacheResetLoadCount() {
    s_loadCount = 0;
}

// VERIFY: c060d573 auto
// OFFSET: 0x004F2DC0
void TextureCacheComputeLoadProgress(void* handle, int64_t& local, int64_t& total) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return;
    }

    if (!entry->m_loadFailed && !entry->m_data) {
        entry->LoadTexture();
    }

    SFile::FileGetIsLocalAmount(entry->m_fileName, local, total);
    total++;

    if (!entry->m_asyncObject) {
        local++;
    }
}

// VERIFY: 89bf2f8e manual
// OFFSET: 0x004F2E20
void TextureCacheDestroy() {
    delete s_entryHeap;
    s_entryHeap = nullptr;
}

// VERIFY: 65c3dd55 auto
// OFFSET: 0x004F2E50
int32_t TextureCacheGetInfo(void* handle, TCTEXTUREINFO& info, int32_t wait) {
    info={};info.m_opaque=1;
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return 0;
    }

    if (!entry->m_loadFailed) {
        if (!entry->m_data) {
            entry->LoadTexture();
        }

        if (!entry->m_loadFailed && !entry->m_info.m_width && entry->m_asyncObject) {
            if (!wait) {
                if (SFile::IsStreamingMode()) {
                    AsyncFile::EnterQueueLock();
                    CAsyncObject* object = entry->m_asyncObject;

                    if (object->IsPending()) {
                        AsyncFileReadIncreasePriority(object);
                    }

                    AsyncFile::LeaveQueueLock();
                }

                return 0;
            }

            AsyncFileReadWait(entry->m_asyncObject);
        }
    }

    info = entry->m_info;

    return 1;
}

// VERIFY: 3bb1eceb manual
// OFFSET: 0x004F2EF0
CACHEENTRY::~CACHEENTRY() {
    int32_t freeData = 1;

    if (this->m_asyncObject) {
        freeData = AsyncFile::Cancel(this->m_asyncObject, &FreeRequest);
        this->m_asyncObject = nullptr;
    }

    if (this->m_data && freeData) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
    }

    this->m_size = 0;
    this->m_data = nullptr;
    this->m_fileName[0] = '\0';
}

// VERIFY: 26c07029 manual
// OFFSET: 0x004F2F60
CACHEENTRY::CACHEENTRY() {
    this->m_info={};this->m_unused=0;
    this->m_info.m_mipCount = 0;
    this->m_info.m_opaque = 1;
    this->m_info.m_width = 0;
    this->m_info.m_height = 0;
    this->m_info.m_alphaSize = 0;
    this->m_size = 0;
    this->m_loadFailed = 0;
    this->m_asyncObject = nullptr;
    this->m_fileName[0] = '\0';
    this->m_refCount = 0;
    this->m_memHandle = 0;
    this->m_data = nullptr;
}

// VERIFY: 5cf66dc0 manual
// OFFSET: 0x004F3110
static CACHEENTRY* TextureCacheAllocEntry() {
    if (!s_entryHeap) {
        s_entryHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CACHEENTRY), 1024, "TCACHEENTRY", true));
    }

    uint32_t memHandle;
    void* object = nullptr;

    if (!s_entryHeap || !ObjectAlloc(*s_entryHeap, &memHandle, &object, false) || !object) {
        return nullptr;
    }

    CACHEENTRY* entry = new (object) CACHEENTRY();
    entry->m_memHandle = memHandle;

    return entry;
}

// VERIFY: 52f7899b auto
// OFFSET: 0x004F31A0
void TextureCacheDestroyTexture(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return;
    }

    if (--entry->m_refCount <= 0) {
        s_cacheTable.Unlink(entry);
        uint32_t* heap = s_entryHeap;
        entry->~CACHEENTRY();
        ObjectFree(*heap, entry->m_memHandle);
    }
}

// VERIFY: 1e1d17fd auto
// OFFSET: 0x004F3930
void* TextureCacheCreateTexture(const char* fileName) {
    constexpr size_t capacity=sizeof(CACHEENTRY::m_fileName);
    if(!fileName || !*fileName || strnlen_s(fileName,capacity)>=capacity)return nullptr;
    uint32_t hashval = SStrHash(fileName, 0, 0);
    CACHEENTRY* entry = s_cacheTable.Ptr(hashval, s_cacheKey);

    if (entry) {
        entry->m_refCount++;
        return entry;
    }

    entry = TextureCacheAllocEntry();
    if(!entry)return nullptr;
    s_cacheTable.Insert(entry, hashval, s_cacheKey);
    SStrCopy(entry->m_fileName, fileName, sizeof(entry->m_fileName));
    entry->m_refCount++;

    return entry;
}
