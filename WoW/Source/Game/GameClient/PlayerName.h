#ifndef WOW_SOURCE_GAME_GAMECLIENT_PLAYERNAME_H
#define WOW_SOURCE_GAME_GAMECLIENT_PLAYERNAME_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Object/WGUID.h"

class PLAYERNAMEDESC {
    public:
    /* 0000 */ uint32_t m_link;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_string;
    /* 000C */ uint32_t m_color;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_flags;
    /* 001C */ uint32_t m_renderFrame;
    /* 0020 */ uint32_t m_worldText [4];
    /* 0030 */ float m_zOffset;
    /* 0034 */ uint32_t unk_0034;
};

PLAYERNAMEDESC* PlayerNameCreate(const WOWGUID& guid);
void PlayerNameDelete(PLAYERNAMEDESC* desc);

void PlayerNameTriggerColorUpdate(PLAYERNAMEDESC* desc);

#endif
