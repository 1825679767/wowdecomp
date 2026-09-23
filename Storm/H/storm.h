#ifndef STORM_H_STORM_H
#define STORM_H_STORM_H

#include <cstdarg>
#include <cstddef>
#include <cstdint>

#define STORMAPI __stdcall

class BigData;
struct HINSTANCE__;
struct HWND__;
struct SSignatureData;

void STORMAPI SBigAdd(BigData* a, const BigData* b, const BigData* c);
void STORMAPI SBigBitLen(BigData* a, uint32_t* bits);
int32_t STORMAPI SBigCompare(const BigData* a, const BigData* b);
void STORMAPI SBigDel(BigData* num);
void STORMAPI SBigFromBinary(BigData* num, const void* data, uint32_t bytes);
void STORMAPI SBigFromUnsigned(BigData* num, uint32_t val);
void STORMAPI SBigMod(BigData* a, const BigData* b, const BigData* c);
void STORMAPI SBigMul(BigData* a, const BigData* b, const BigData* c);
void STORMAPI SBigNew(BigData** num);
void STORMAPI SBigPowMod(BigData* a, const BigData* b, const BigData* c, const BigData* d);
void STORMAPI SBigSub(BigData* a, const BigData* b, const BigData* c);
void STORMAPI SBigToBinaryBuffer(const BigData* num, void* data, uint32_t maxBytes, uint32_t* bytes);

void STORMAPI SErrCatchUnhandledExceptions();
int32_t STORMAPI SErrDestroy();
[[noreturn]] void SErrDisplayAppFatal(const char* format, ...);
[[noreturn]] void SErrDisplayAppFatalCustom(uint32_t errorcode, const char* format, ...);
[[noreturn]] void SErrDisplayAppFatalCustomV(uint32_t errorcode, const char* format, va_list args);
int32_t STORMAPI SErrDisplayError(uint32_t errorcode, const char* filename, int32_t linenumber, const char* description, int32_t recoverable, uint32_t exitcode, uint32_t unknown);
int32_t SErrDisplayErrorFmt(uint32_t errorcode, const char* filename, int32_t linenumber, int32_t recoverable, uint32_t exitcode, const char* format, ...);
int32_t STORMAPI SErrGetErrorStr(uint32_t errorcode, char* buffer, uint32_t bufferchars);
uint32_t STORMAPI SErrGetLastError();
int32_t STORMAPI SErrGetLogLastPath(char* buf, uint32_t size);
void SErrInitialize();
int32_t STORMAPI SErrIsDisplayingError();
void STORMAPI SErrPrepareAppFatal(const char* filename, int32_t linenumber);
void STORMAPI SErrRegisterHandler(int32_t (STORMAPI* handler)(uint32_t, const char*, const char*, int32_t, const char*));
void STORMAPI SErrReportNamedResourceLeak(const char* handlename, const char* resourcename);
void STORMAPI SErrSetLastError(uint32_t errorcode);
void STORMAPI SErrSetLogCallback(int32_t (STORMAPI* callback)(char*, uint32_t));
void STORMAPI SErrSetLogTitleString(const char* title);

void SDebugPrintf(const char* format, ...);

struct MEMREPORTENTRY {
    /* 0000 */ uint8_t unk_0000[16];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ char unk_0024[1];
};

extern int32_t g_memFullError;

void* STORMAPI SMemAlloc(size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
int32_t STORMAPI SMemFree(void* ptr, const char* filename, int32_t linenumber, uint32_t flags);
void* STORMAPI SMemReAlloc(void* ptr, size_t bytes, const char* filename, int32_t linenumber, uint32_t flags);
void STORMAPI SMemSetDebugFlags(uint32_t flags, uint8_t changeMask);
void SMemGenerateReport(uint32_t type, uint32_t flags, void (STORMAPI* callback)(void*, const MEMREPORTENTRY*), void* context);

int32_t STORMAPI SRegGetBaseKey(uint32_t flags, char* buffer, uint32_t bufferchars);
int32_t STORMAPI SRegLoadString(const char* keyname, const char* valuename, uint32_t flags, char* buffer, uint32_t buffersize);
int32_t STORMAPI SRegLoadValue(const char* keyname, const char* valuename, uint32_t flags, uint32_t* value);
int32_t STORMAPI SRegSaveString(const char* keyname, const char* valuename, uint32_t flags, const char* string);
int32_t STORMAPI SRegSaveValue(const char* keyname, const char* valuename, uint32_t flags, uint32_t value);

int32_t SSignatureVerify(const uint8_t* data, uint32_t size, const uint8_t* modulus, uint32_t modulusSize, const uint8_t* pubExponent, uint32_t pubExponentSize);
void SSignatureVerifyStream_Begin(SSignatureData** token, uint32_t modulusSize, uint32_t pubExponentSize);
int32_t SSignatureVerifyStream_Finish(SSignatureData* token, const uint8_t* modulus, const uint8_t* pubExponent);
uint32_t SSignatureVerifyStream_GetSignatureLength(SSignatureData* token);
void SSignatureVerifyStream_ProvideData(SSignatureData* token, const uint8_t* data, uint32_t size);

char* SStrChrR(char* string, char search);
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

HWND__* STORMAPI SMsgGetDefaultWindow();

HINSTANCE__* STORMAPI StormGetInstance();

void DebugLog(const char* format, ...);
void DebugLog(int32_t target, const char* format, ...);
void DebugValidate(const char* expression);

#endif
