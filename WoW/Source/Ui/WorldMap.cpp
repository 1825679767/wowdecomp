#include "WorldMap.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: 3e6cfe40 auto
// OFFSET: 0x00547120
void CGWorldMap::EnterWorld() {
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, 0xDF4, 0x200, PlayerExploredZonesUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}
