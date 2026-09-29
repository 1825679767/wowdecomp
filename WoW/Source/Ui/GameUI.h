#ifndef WOW_SOURCE_UI_GAMEUI_H
#define WOW_SOURCE_UI_GAMEUI_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/PartyFrame.h"

class CGGameObject_C;
class CSimpleTop;
struct CVar;
class FrameScript_Object;

extern CVar* g_autoClearAFKCVar;
extern const char* g_macroTaint;

class TotemInfo {
    public:
    /* 0000 */ int32_t m_slot;
    /* 0004 */ uint32_t unk_0004 [3];
    /* 0010 */ const char* m_name;
};

class CGGameUI {
    public:
    static WOWGUID m_currentObjectTrack;
    static WOWGUID m_interactTarget;
    static WOWGUID m_lockedTarget;
    static WOWGUID m_lastEnemyTarget;
    static WOWGUID m_corpseGUID;
    static int32_t m_initialized;
    static int32_t m_inMovie;
    static int32_t m_iCurrentMapID;
    static WOWGUID unk_BD07D0;
    static CSimpleTop* m_simpleTop;

    static int32_t CameraMusicIsPlaying();
    static int32_t CanPerformAction(uint32_t action);
    static void SetLastInstanceTime(int32_t time, int32_t currentMapId, int32_t newMapId);
    static void ShowBlockedActionFeedback(FrameScript_Object* object, uint32_t type);
    static void Reload();
    static void ResetCamera(WOWGUID guid);
    static void SetActiveCorpse(WOWGUID guid);
    static void SetAspect();
    static WOWGUID GetCursorItem();
    static WOWGUID GetPartyMember(uint32_t index);
    static WOWGUID GetPartyMemberPet(uint32_t index);
    static WOWGUID GetRaidMemberPet(uint32_t index);
    static void InvalidatePlayer(const WOWGUID& guid);
    static int32_t SequenceMusicIsPlaying();
    static TotemInfo* GetTotemInfo(WOWGUID guid);
    static int32_t IsRaidMemberOrPet(const WOWGUID& guid);
    static void ClearInteractTarget(const WOWGUID& guid, int32_t a2);
    static void EnterWorld();
    static void BreakTarget(WOWGUID guid);
    static void DestroyTotem(int32_t slot);
};

class CSpriteClickEvent {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ MOUSEBUTTON m_button;
};

class CTerrainClickEvent {
    public:
    /* 0000 */ WOWGUID m_guid;
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

    void AddCapturePoint(CGGameObject_C* object, uint32_t a2, uint32_t a3);
    void RemoveCapturePoint(CGGameObject_C* object);
};

extern CCapturePointUIManager g_capturePointUIManager;

WOWGUID GetRaidMemberGUID(uint32_t index);

#endif
