#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H

#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

namespace System_SFile {

struct RawReadInfo {
    /* 0000 */ bool m_retryNative;

    RawReadInfo();
};

extern Blizzard::Thread::LocalObject<RawReadInfo> s_rawReadInfo;

}

#endif
