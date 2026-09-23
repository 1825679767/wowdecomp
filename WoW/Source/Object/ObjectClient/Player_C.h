#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/WGUID.h"

class C3Vector;
class SpellRec;
struct SpellVisualKitRec;

class CGPlayer_C : public CGUnit_C, public CGPlayer {
    public:
    /* 1010 */ TSList<void> m_playerMirrorLists [176];
    /* 1850 */ TSList<void>* m_playerLocalMirrorLists;
    /* 1854 */ uint32_t unk_1854 [35];
    /* 18E0 */ WGUID m_lootTarget;
    /* 18E8 */ uint32_t unk_18E8 [28];
};

class PlaySpellVisuaKitData {
    public:
    /* 0000 */ SpellRec* m_spellRec;
    /* 0004 */ SpellVisualKitRec* m_impactVisualKit;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ C3Vector* m_position;
    /* 0010 */ WGUID m_transportGUID;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ WGUID m_someGUID;
    /* 0030 */ uint32_t unk_0030;
};

#endif
