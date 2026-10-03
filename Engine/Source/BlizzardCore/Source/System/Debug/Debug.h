#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_DEBUG_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_DEBUG_DEBUG_H

#include <cstdint>

namespace Blizzard {
namespace Debug {

typedef void (*AssertCallback)(const char* expression, const char* message, const char* fileName, uint32_t lineNumber);

void Breakpoint();
char* CanonicalStr(const char* src, char* dst, uint32_t maxchars);
void DefaultCallback(const char* expression, const char* message, const char* fileName, uint32_t lineNumber);
void Printf(void (*output)(const char*), const char* format, ...);
void SetAssertHandler(AssertCallback handler);
void StderrOutput(const char* text);

}
}

namespace System_Debug {

extern Blizzard::Debug::AssertCallback s_AssertCallback;
extern char s_CanonBuffer[1024];
extern char* s_CanonHead;
extern char* s_CanonTail;

}

#endif
