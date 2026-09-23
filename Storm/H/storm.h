#ifndef STORM_H_STORM_H
#define STORM_H_STORM_H

#include <cstddef>
#include <cstdint>

void* __stdcall SMemAlloc(size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
int32_t __stdcall SMemFree(void* ptr, const char* filename, int32_t linenumber, uint32_t flags);
void* __stdcall SMemReAlloc(void* ptr, size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);

int32_t __stdcall SStrCmp(const char* string1, const char* string2, size_t maxchars);
int32_t __stdcall SStrCmpI(const char* string1, const char* string2, size_t maxchars);
int32_t __stdcall SStrCmpUTF8I(const char* string1, const char* string2, int32_t maxchars);
char* __stdcall SStrDupA(const char* string, const char* filename, int32_t linenumber);
uint32_t __stdcall SStrHashHT(const char* string);

#endif
