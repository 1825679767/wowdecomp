#ifndef STORM_H_STORM_H
#define STORM_H_STORM_H

#include <cstddef>
#include <cstdint>

#define STORMAPI __stdcall

void* STORMAPI SMemAlloc(size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
int32_t STORMAPI SMemFree(void* ptr, const char* filename, int32_t linenumber, uint32_t flags);
void* STORMAPI SMemReAlloc(void* ptr, size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);

int32_t STORMAPI SStrCmp(const char* string1, const char* string2, size_t maxchars);
int32_t STORMAPI SStrCmpI(const char* string1, const char* string2, size_t maxchars);
int32_t STORMAPI SStrCmpUTF8I(const char* string1, const char* string2, int32_t maxchars);
char* STORMAPI SStrDupA(const char* string, const char* filename, int32_t linenumber);
uint32_t STORMAPI SStrHashHT(const char* string);

void DebugLog(const char* format, ...);
void DebugValidate(const char* expression);

#endif
