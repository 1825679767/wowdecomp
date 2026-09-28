#include "World.h"
#include <cstdio>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/WorldClient/Types.h"

// STATIC: 0x00ADF7F0
int32_t CWorld::s_waterRipples = 1;

// VERIFY: ae98adec auto
// OFFSET: 0x0077F690
int32_t CWorld::ConsoleCommand_WaterRipples(const char* command, const char* arguments) {
    sscanf(arguments, "%d", &CWorld::s_waterRipples);
    return 1;
}

// VERIFY: 05445309 auto
// OFFSET: 0x0077F6B0
int32_t CWorld::ConsoleCommand_WaterParticulates(const char* command, const char* arguments) {
    if (CWorld::enables & 0x2000000) {
        ConsoleWrite("Particulates disabled", DEFAULT_COLOR);
        CWorld::enables &= ~0x2000000;
    } else {
        ConsoleWrite("Particulates enabled", DEFAULT_COLOR);
        CWorld::enables |= 0x2000000;
    }

    return 1;
}
