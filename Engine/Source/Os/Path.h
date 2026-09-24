#ifndef ENGINE_SOURCE_OS_PATH_H
#define ENGINE_SOURCE_OS_PATH_H

#include <cstdint>

uint32_t GetModuleFileNameUTF8(void* module, char* path, uint32_t size);
int32_t OsCreateDirectory(const char* path, int32_t recursive);
void OsGetExePath(char* path, uint32_t size);
uint32_t OsPathGetRootChars(const char* path);
void OsPathStripFilename(char* path);
bool OsSetCurrentDirectory(const char* path);

#endif
