#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

// STATIC: 0x00D38B00
DayNight::DNInfo DayNight::g_dnInfo;

// VERIFY: 900bfd47 auto
// OFFSET: 0x007ECEF0
DayNight::DNInfo* DayNight::GetInfo() {
    return &DayNight::g_dnInfo;
}
