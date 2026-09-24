#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

class IOMpqUnit;
class IOUnit;
class IOUnitContainer;

namespace System_SFile {

class ARCHIVEREC;

struct RawReadInfo {
    /* 0000 */ bool m_retryNative;

    RawReadInfo();
};

extern Blizzard::Thread::LocalObject<RawReadInfo> s_rawReadInfo;
extern uint32_t s_threadingMode;

bool CheckCrc(void* context, uint32_t crc);
IOUnit* CreateRangeUnit(IOUnitContainer* container, int64_t start, int64_t end, IOUnit* next);
IOUnit* GetLockedUnit(IOUnit* file);
bool InsertLockedUnit(IOUnit* file, IOUnit* unit);
IOUnit* OpenFileUnit(IOUnitContainer* container, const char* path, int32_t mode);

}

namespace Blizzard {
namespace Mopaq {

int32_t SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, IOMpqUnit** result);
void SFileSetThreadingMode(uint32_t mode);

}
}

#endif
