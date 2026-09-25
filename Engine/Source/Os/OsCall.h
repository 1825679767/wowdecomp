#ifndef ENGINE_SOURCE_OS_OSCALL_H
#define ENGINE_SOURCE_OS_OSCALL_H

#include <cstdint>

void OsCallDestroy();
void OsCallDestroyContext(void* context);
void OsCallInitialize(const char* name);
void* OsCallInitializeContext(const char* name);
void OsCallResetContext(void* context);
void OsCallSetContext(void* context);

#endif
