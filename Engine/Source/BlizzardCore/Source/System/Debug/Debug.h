#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_DEBUG_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_DEBUG_H

#include <cstdint>

namespace Blizzard {
namespace Debug {

typedef void (*AssertCallback)(const char* expression, const char* message, const char* fileName, uint32_t lineNumber);

char* CanonicalStr(const char* src, char* dst, uint32_t maxchars);
void DefaultCallback(const char* expression, const char* message, const char* fileName, uint32_t lineNumber);
void SetAssertHandler(AssertCallback handler);

}
}

namespace System_Debug {

extern Blizzard::Debug::AssertCallback s_AssertCallback;
extern char s_CanonBuffer[1024];
extern char* s_CanonHead;
extern char* s_CanonTail;

}

#endif
