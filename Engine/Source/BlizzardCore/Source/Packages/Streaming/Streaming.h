#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"

namespace Blizzard {
namespace Streaming {

extern bool s_streamingReady;
extern bool s_isStreamingTrial;

int32_t CheckFileRange(const char* path, int64_t offset, uint32_t count, const char* name, int32_t flags);
uint32_t GetIsLocalAmount(const char* path, int64_t offset, uint32_t size, const char* name);
bool IsStreamedFile(const char* path);
bool IsStreamingMode();
bool ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms);
void MakePartFileName(char* name, uint32_t size);

}
}

bool CheckIfStreamingAndTrial();
char* GetComponentVersionData(uint32_t* size);

#endif
