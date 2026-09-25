#ifndef ENGINE_SOURCE_SERVICES_SYSMESSAGE_H
#define ENGINE_SOURCE_SERVICES_SYSMESSAGE_H

#include <cstdint>
#include "Engine/Source/Base/Status.h"

int32_t SysMsgAdd(const char* text, STATUS_TYPE severity);
int32_t SysMsgAdd(CStatus* status);

#endif
