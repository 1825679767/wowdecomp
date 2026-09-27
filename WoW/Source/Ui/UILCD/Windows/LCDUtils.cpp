#include "WoW/Source/Ui/UILCD/Windows/LCDUtils.h"

// STATIC: 0x00D41374
TSHashTable<BITMAPBYICON, HASHKEY_NONE> CLCDUtils::m_bitmapCache;

// VERIFY: 8f6ac590 manual
// OFFSET: 0x0084B400
void CLCDUtils::Shutdown() {
    CLCDUtils::m_bitmapCache.Clear();
}
