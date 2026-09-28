#ifndef WOW_SOURCE_UI_GAMEUI_H
#define WOW_SOURCE_UI_GAMEUI_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"

class CGGameUI {
    public:
    static uint64_t m_currentObjectTrack;
    static uint64_t m_interactTarget;
    static uint64_t m_lockedTarget;
    static uint64_t m_lastEnemyTarget;
    static int32_t m_initialized;
    static int32_t m_inMovie;
    static uint64_t unk_BD07D0;

    static int32_t CameraMusicIsPlaying();
    static void Reload();
    static void SetAspect();
    static uint64_t GetCursorItem();
    static uint64_t GetPartyMember(uint32_t index);
    static uint64_t GetPartyMemberPet(uint32_t index);
    static uint64_t GetRaidMemberPet(uint32_t index);
    static void InvalidatePlayer(const uint64_t& guid);
    static int32_t SequenceMusicIsPlaying();
};

class CGPartyMemberState {
    public:
    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint8_t unk_0002[342];
    /* 0158 */ WGUID m_petGuid;
    /* 0160 */ uint8_t unk_0160[432];
};

class CSpriteClickEvent {
    public:
    /* 0000 */ WGUID m_guid;
    /* 0008 */ MOUSEBUTTON m_button;
};

class CTerrainClickEvent {
    public:
    /* 0000 */ WGUID m_guid;
    /* 0008 */ C3Vector m_point;
    /* 0014 */ MOUSEBUTTON m_button;
};

struct MirrorTimerInfo {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
};

class CCapturePointUIManager {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
};

extern CGPartyMemberState unk_BD0D10[4];

uint64_t GetRaidMemberGUID(uint32_t index);

#endif
