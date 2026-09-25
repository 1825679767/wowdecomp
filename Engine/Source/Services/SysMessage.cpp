#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: e98afe5a manual
// OFFSET: 0x004B4F60
int32_t SysMsgAdd(const char* text, STATUS_TYPE severity) {
    if (!text || severity >= STATUS_NUMTYPES) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return 1;
}

// VERIFY: 852e1d4d manual
// OFFSET: 0x004B4F90
int32_t SysMsgAdd(CStatus* status) {
    if (status->IsEmpty()) {
        return 1;
    }

    char* text = status->GetErrorStrAlloc(STATUS_INFO, nullptr);
    int32_t result = SysMsgAdd(text, status->m_highestSeverity);
    SMemFree(text, __FILE__, __LINE__, 0);

    return result;
}

// VERIFY: 53a762bf manual
// OFFSET: 0x004B4FF0
int32_t SysMsgVPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, va_list args) {
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    buffer[255] = '\0';

    if (severity < STATUS_NUMTYPES) {
        return 1;
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}

// VERIFY: 7b8df386 manual
// OFFSET: 0x004B5040
int32_t SysMsgPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (format) {
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        buffer[255] = '\0';

        if (severity < STATUS_NUMTYPES) {
            return 1;
        }
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}

// VERIFY: fe511d8e auto
// OFFSET: 0x004D8520
int32_t SysMsgPrintf(STATUS_TYPE severity, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int32_t result = SysMsgVPrintf(severity, 1, format, args);
    va_end(args);

    return result;
}
