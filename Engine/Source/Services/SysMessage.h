#ifndef ENGINE_SOURCE_SERVICES_SYSMESSAGE_H
#define ENGINE_SOURCE_SERVICES_SYSMESSAGE_H

#include <cstdarg>
#include <cstdint>
#include "Engine/Source/Base/Status.h"

int32_t SysMsgAdd(const char* text, STATUS_TYPE severity);
int32_t SysMsgAdd(CStatus* status);
int32_t SysMsgPrintf(STATUS_TYPE severity, const char* format, ...);
int32_t SysMsgPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, ...);
int32_t SysMsgVPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, va_list args);

#endif
