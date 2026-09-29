#include "PlayerName.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"

// VERIFY: b96c6600 auto
// OFFSET: 0x007E6150
void PlayerNameRegisterCVars() {
    CVar::Register("UnitNameOwn", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1), false);
    CVar::Register("UnitNameNPC", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2), false);
    CVar::Register("UnitNamePlayerGuild", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x4), false);
    CVar::Register("UnitNamePlayerPVPTitle", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x8), false);
    CVar::Register("UnitNameEnemyPlayerName", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x10), false);
    CVar::Register("UnitNameEnemyPetName", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x20), false);
    CVar::Register("UnitNameEnemyGuardianName", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1000), false);
    CVar::Register("UnitNameEnemyTotemName", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x40), false);
    CVar::Register("UnitNameFriendlyPlayerName", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x80), false);
    CVar::Register("UnitNameFriendlyPetName", nullptr, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x100), false);
    CVar::Register("UnitNameFriendlyGuardianName", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2000), false);
    CVar::Register("UnitNameFriendlyTotemName", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x200), false);
    CVar::Register("UnitNameNonCombatCreatureName", nullptr, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x400), false);
}
