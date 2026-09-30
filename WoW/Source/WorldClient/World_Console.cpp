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

// VERIFY: 6b7e55e5 auto
// OFFSET: 0x0077F650
int32_t CWorld::ConsoleCommand_ShowCull(const char* command, const char* arguments) {
    if (CWorld::enables & 0x20) {
        ConsoleWrite("Terrain culling disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~0x20;
    } else {
        ConsoleWrite("Terrain culling enabled.", DEFAULT_COLOR);
        CWorld::enables |= 0x20;
    }

    return 1;
}

// VERIFY: 70dab189 auto
// OFFSET: 0x0077F7E0
int32_t CWorld::ConsoleCommand_ShowShadow(const char* command, const char* arguments) {
    if (CWorld::enables & 0x40) {
        ConsoleWrite("Terrain shadow disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~0x40;
    } else {
        ConsoleWrite("Terrain shadow enabled.", DEFAULT_COLOR);
        CWorld::enables |= 0x40;
    }

    return 1;
}

// VERIFY: c47cd1e6 auto
// OFFSET: 0x0077F870
int32_t CWorld::ConsoleCommand_ShowSimpleDoodads(const char* command, const char* arguments) {
    if (CWorld::bShowSimpleDoodads) {
        ConsoleWrite("Simple doodads disabled.", DEFAULT_COLOR);
        CWorld::bShowSimpleDoodads = 0;
        return 1;
    }

    ConsoleWrite("Simple doodads enabled.", DEFAULT_COLOR);
    int32_t result = 1;
    CWorld::bShowSimpleDoodads = result;
    return result;
}

// VERIFY: e24c8564 auto
// OFFSET: 0x0077F5B0
int32_t CWorld::ConsoleCommand_ShowDetailDoodads(const char* command, const char* arguments) {
    if (CWorld::enables & 0x100000) {
        ConsoleWrite("Detail doodads disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~0x100000;
    } else {
        ConsoleWrite("Detail doodads enabled.", DEFAULT_COLOR);
        CWorld::enables |= 0x100000;
    }

    return 1;
}

// VERIFY: b924ea7d auto
// OFFSET: 0x0077F820
int32_t CWorld::ConsoleCommand_ShowLowDetail(const char* command, const char* arguments) {
    if (CWorld::enables & 0x4000000) {
        ConsoleWrite("Terrain low detail disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~0x4000000;
    } else {
        ConsoleWrite("Terrain low detail enabled.", DEFAULT_COLOR);
        CWorld::enables |= 0x4000000;
    }

    return 1;
}
