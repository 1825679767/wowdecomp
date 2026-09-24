#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H

#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

namespace System_SFile {

extern Blizzard::Thread::LocalObject<bool> s_retryNativeRead;

}

#endif
