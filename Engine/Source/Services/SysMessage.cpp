#include <cstdint>
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
