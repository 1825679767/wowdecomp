#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_LCDUTILS_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_LCDUTILS_H

#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"

struct BITMAPBYICON : public TSHashObject<BITMAPBYICON, HASHKEY_NONE> {
    /* 0018 */ void* m_bitmap;
    /* 001C */ C2iVector m_size;
};

class CLCDUtils {
    public:
    static TSHashTable<BITMAPBYICON, HASHKEY_NONE> m_bitmapCache;

    static void Shutdown();
};

#endif
