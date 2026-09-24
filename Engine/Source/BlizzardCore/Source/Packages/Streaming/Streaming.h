#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H

#include <cstdint>

namespace Blizzard {
namespace Streaming {

extern bool s_streamingReady;
extern bool s_isStreamingTrial;

int32_t CheckFileRange(const char* path, int64_t offset, uint32_t count, const char* name, int32_t flags);
bool IsStreamedFile(const char* path);
void MakePartFileName(char* name, uint32_t size);

}
}

#endif
