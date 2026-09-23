#ifndef STORM_H_STORM_H
#define STORM_H_STORM_H

#include <cstddef>
#include <cstdint>

#define STORMAPI __stdcall

int32_t STORMAPI SErrDisplayError(uint32_t errorcode, const char* filename, int32_t linenumber, const char* description, int32_t recoverable, uint32_t exitcode, uint32_t unknown);
void STORMAPI SErrSetLastError(uint32_t errorcode);

void* STORMAPI SMemAlloc(size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
int32_t STORMAPI SMemFree(void* ptr, const char* filename, int32_t linenumber, uint32_t flags);
void* STORMAPI SMemReAlloc(void* ptr, size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
void STORMAPI SMemSetDebugFlags(uint32_t flags, uint8_t changeMask);

int32_t STORMAPI SRegGetBaseKey(uint32_t flags, char* buffer, uint32_t bufferchars);
int32_t STORMAPI SRegLoadString(const char* keyname, const char* valuename, uint32_t flags, char* buffer, uint32_t buffersize);
int32_t STORMAPI SRegLoadValue(const char* keyname, const char* valuename, uint32_t flags, uint32_t* value);
int32_t STORMAPI SRegSaveString(const char* keyname, const char* valuename, uint32_t flags, const char* string);
int32_t STORMAPI SRegSaveValue(const char* keyname, const char* valuename, uint32_t flags, uint32_t value);

int32_t STORMAPI SStrCmp(const char* string1, const char* string2, size_t maxchars);
int32_t STORMAPI SStrCmpI(const char* string1, const char* string2, size_t maxchars);
int32_t STORMAPI SStrCmpUTF8I(const char* string1, const char* string2, int32_t maxchars);
size_t STORMAPI SStrCopy(char* dest, const char* source, size_t destsize);
char* STORMAPI SStrDupA(const char* string, const char* filename, int32_t linenumber);
uint32_t STORMAPI SStrHashHT(const char* string);
size_t STORMAPI SStrLen(const char* string);
size_t STORMAPI SStrPack(char* dest, const char* source, size_t destsize);
size_t SStrPrintf(char* dest, size_t maxchars, const char* format, ...);

int32_t STORMAPI SUniConvertUTF16to8(char* dst, uint32_t dstmaxchars, const uint16_t* src, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars);
int32_t STORMAPI SUniConvertUTF8to16(uint16_t* dst, uint32_t dstmaxchars, const char* src, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars);
int32_t STORMAPI SUniConvertUTF8to16Len(const char* src, uint32_t srcmaxchars, uint32_t* srcchars);

void DebugLog(const char* format, ...);
void DebugLog(int32_t target, const char* format, ...);
void DebugValidate(const char* expression);

#endif
