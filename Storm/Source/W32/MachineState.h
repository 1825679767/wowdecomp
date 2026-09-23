#ifndef STORM_SOURCE_W32_MACHINESTATE_H
#define STORM_SOURCE_W32_MACHINESTATE_H

#include <cstdint>
#include <windows.h>

void LoadMachineStateSymbols();
void LogComputerInfoHeader(uint32_t flags, void (*callback)(void*, const char*, ...), void* param, const char* title, const SYSTEMTIME* time);
void LogMachineState(uint32_t flags, void (*callback)(void*, const char*, ...), void* param, uint32_t framestoskip, CONTEXT* context);
int32_t LogMiniDump(void* logfile, EXCEPTION_POINTERS* exceptionpointers, uint32_t count, const char** strings);
int32_t LogMiniDumpIsAvailable();
void UnloadMachineStateSymbols();

#endif
