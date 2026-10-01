#ifndef WOW_SOURCE_UI_GAMEUI_H
#define WOW_SOURCE_UI_GAMEUI_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CGUnit_C;
class AreaTableRec;
class CImVector;
class CDataStore;
class ATTACKROUNDINFO;
class CFocusEvent;
struct MirrorTimerInfo;
class CGGameObject_C;
class CGTooltip;
class CMouseEvent;
class CScriptRegion;
class CSimpleFrame;
class CSizeEvent;
class CStatus;
class CSimpleTop;
class CObjectTrackEvent;
class CSpriteClickEvent;
class CTerrainClickEvent;
class CWorldClickEvent;
struct CVar;
class FrameScript_Object;
struct lua_State;

extern CVar* g_autoClearAFKCVar;
extern CVar* g_questPOICVar;
extern CVar* g_nameplateShowEnemiesCVar;
extern CVar* g_nameplateShowEnemyPetsCVar;
extern CVar* g_nameplateShowEnemyGuardiansCVar;
extern CVar* g_nameplateShowEnemyTotemsCVar;
extern CVar* g_nameplateShowFriendsCVar;
extern CVar* g_nameplateShowFriendlyPetsCVar;
extern CVar* g_nameplateShowFriendlyGuardiansCVar;
extern CVar* g_nameplateShowFriendlyTotemsCVar;
extern CVar* s_cvNameplateAllowOverlap;
extern CVar* g_ShowClassColorInNameplateCVar;
extern CVar* g_showVKeyCastbarCVar;
extern CVar* g_autoLootDefaultCVar;
extern CVar* g_combatLogRetentionTimeCVar;
extern CVar* g_guildShowOfflineCVar;
extern CVar* g_combatDamageCVar;
extern CVar* g_combatHealingCVar;
extern CVar* s_cvCombatLogPeriodicSpells;
extern CVar* g_petMeleeDamageCVar;
extern CVar* g_petSpellDamageCVar;
extern CVar* g_deselectOnClickCVar;
extern CVar* g_scriptProfileCVar;
extern CVar* g_taintLogCVar;
extern CVar* g_guildMemberNotifyCVar;
extern CVar* g_uiScaleCVar;
extern CVar* g_useUiScaleCVar;
extern CVar* g_rotateMinimapCVar;
extern CVar* g_cvAutoInteract;
extern CVar* g_autoStandCVar;

void SignalEvent_EVENT_PARTY_INVITE_REQUEST(const char* name);
extern CVar* g_unitHighlightsCVar;
extern CVar* g_stopAutoAttackOnTargetChangeCVar;
extern CVar* g_autoDismountCVar;
extern CVar* g_autoDismountFlyingCVar;
extern CVar* g_autoUnshiftCVar;
extern CVar* s_cvQuestLogCollapseFilter;
extern CVar* s_cvAutoRangedCombat;
extern CVar* s_cvAutoFilledMultiCastSlots;
extern CVar* g_threatWarningCVar;
extern CVar* g_threatWorldTextCVar;
extern CVar* g_currencyTokensUnused1CVar;
extern CVar* g_currencyTokensUnused2CVar;
extern CVar* g_currencyTokensBackpack1CVar;
extern CVar* g_currencyTokensBackpack2CVar;
extern CVar* s_cvThreatPlaySounds;
extern CVar* g_autoSelfCastCVar;
extern CVar* g_secureAbilityToggleCVar;
extern CVar* g_predictedHealthCVar;
extern const char* g_macroTaint;

struct UIErrorInfo {
    /* 0000 */ const char* m_token;
    /* 0004 */ int32_t m_type;
    /* 0008 */ const char* m_sound;
    /* 000C */ int32_t m_vocalError;
    /* 0010 */ int32_t m_chatType;
};

struct SAVEDVARIABLE : public TSHashObject<SAVEDVARIABLE, HASHKEY_STR> {
};

class SE2SoundKitObject;
class CGGameUI {
    public:
    static void SendRaidDifficultyMessage();
    static void LeaveWorld();
    static void SaveSavedVariables();
    struct THREATMAP : public TSHashObject<THREATMAP, CHashKeyGUID> {
        /* 0020 */ TSGrowableArray<WOWGUID> m_units;

        uint8_t ComputeMaxThreatLevel(const WOWGUID& guid);
        void RemoveUnit(const WOWGUID& guid);
        void AddUnit(const WOWGUID& guid);
    };

    static TSHashTable<THREATMAP, CHashKeyGUID> s_threatMap;
    static void ThreatMapRemove(const WOWGUID& unit, const WOWGUID& target);
    static void ThreatMapAdd(const WOWGUID& unit, const WOWGUID& target);
    static void UnitTooltipUpdate(const WOWGUID& guid);
    static void UnlockAllItems();
    static void SetRegenEnabled(int32_t enabled);

    static TSHashTable<SAVEDVARIABLE, HASHKEY_STR> s_savedVariables[2];
    static int32_t HandleMouseDown(const CMouseEvent& evt);
    static void OnClientControlChanged(int32_t control);
    static void SetMinimapZoneText(const char* text);
    static void NewZoneFeedback(int32_t areaId, int32_t subzoneId, int32_t wmoGroupId, const char* zoneText, const char* subZoneText, const char* realZoneText, int32_t inside);
    static int32_t HandleSpriteClick(const CSpriteClickEvent& evt);
    static void HandleSpriteTrack(const CObjectTrackEvent& evt);
    static int32_t HandleTerrainClick(const CTerrainClickEvent& evt);
    static int32_t HandleWorldClick(const CWorldClickEvent& evt);
    static int32_t OnSpriteLeftClick(WOWGUID guid);
    static int32_t OnSpriteRightClick(WOWGUID guid);
    static void HandleObjectTrackChange(WOWGUID guid, WOWGUID prevGuid);
    static int32_t OnTerrainClick(const CTerrainClickEvent& evt);
    static void Target(WOWGUID guid);
    static void DeleteCursorItem();
    static int32_t m_cursorItemType;
    static int32_t m_threatWarningEnabled;
    static char m_lastErrorMessage[3000];
    static int32_t m_cursorEquipmentSet;
    static int32_t m_cursorHasAction;
    static int32_t m_cursorVirtualSlot;
    static int32_t m_cursorVirtualDisplay;
    static uint32_t m_cursorItemSlot;
    static WOWGUID m_lastFriendTarget;
    static int32_t m_cursorSpell;
    static int32_t m_cursorPetAction;
    static int32_t m_cursorMacro;
    static int32_t m_cursorPet;
    static void GetCursorItem(WOWGUID& item, WOWGUID& container, uint32_t& slot);
    static void SendTarget(WOWGUID guid);
    static uint8_t GetComboPoints(WOWGUID target);
    static void UpdateCombatMode();
    static int32_t m_cursorVirtualID;
    static WOWGUID m_cursorItemContainer;
    static WOWGUID m_cursorItem;
    static WOWGUID m_lastTarget;
    static int32_t unk_BD08CC;
    static void InitClientControlState(const WOWGUID& guid);
    static void StartCinematic(int32_t cinematic);
    static void SetInstanceLockTime(uint32_t time, uint32_t a2, uint32_t a3);
    static void SetInstanceBootTime(uint32_t time);
    static void DisplayNameError(uint32_t error);
    static void UpdateActivePlayer();
    static void ClearClientControls();
    static void OpenPlayerSkinned(bool a1);
    static void OpenResurrectRequest(const char* name);
    static void CancelPartyInvite();
    static void SetSummonRequest(WOWGUID summoner, uint32_t area, uint32_t timeout);
    static uint32_t m_cursorMoney;
    static int32_t unk_AC80A4;
    static int32_t m_hasControl;
    static uint8_t m_currentlyReloadingUI;
    static uint8_t m_reloadUIRequested;
    static uint8_t m_loggingIn;
    static CGTooltip* m_gameTooltip;
    static CSimpleFrame* m_UISimpleParent;
    static int32_t unk_BD07F0;
    static int32_t unk_BD07F4;
    static int32_t m_inCinematic;
    static WOWGUID m_directionalTarget;
    static WOWGUID m_pendingLevelGranter;
    static uint32_t m_raidDifficulty;
    static const char* m_realZoneText;
    static const char* m_zoneText;
    static WOWGUID m_summonConfirmGUID;
    static uint32_t m_summonConfirmArea;
    static int32_t m_areaSpiritHealerTime;
    static uint32_t m_repopTime;
    static uint8_t m_deadNoRepopTimer;
    static uint8_t m_comboPoints;
    static uint32_t m_corpseReclaimDelay;
    static uint32_t m_instanceBootTime;
    static uint32_t m_instanceLockTime;
    static uint32_t m_instanceLockCompletedMask;
    static uint32_t m_instanceLockExtending;
    static uint32_t m_summonConfirmTime;
    static uint32_t m_billingExpireTime;
    static int32_t unk_BD0878;
    static int32_t unk_BD087C;
    static uint32_t m_instanceDifficulty;
    static uint32_t m_lastRegenTime;
    static WOWGUID m_comboTarget;
    static uint16_t unk_BD08C4;
    static TotemInfo m_totems[4];
    static MirrorTimerInfo m_mirrorTimerInfo[3];

    static int32_t FilterMouseDown(CMouseEvent* evt);
    static int32_t FilterMouseMotion(CMouseEvent* evt);
    static void HandleDisplaySizeChanged(const CSizeEvent& evt);
    static void DisplayError(UIErrors error, ...);
    static int32_t IsAutoLooting();
    static void ShowCombatFeedback(const WOWGUID& guid, int32_t amount, uint32_t schoolMask, uint32_t flags);
    static void ShowSpellMissFeedback(const WOWGUID& guid, int32_t reason);
    static void ShowHealingFeedback(const WOWGUID& guid, int32_t amount, bool crit);
    static void ShowEnergizeFeedback(const WOWGUID& guid, int32_t powerType, int32_t amount);
    static void SignalUnitCombat(const WOWGUID& guid, const char* action, int32_t amount, uint32_t schoolMask, uint32_t flags);
    static void ShowCombatFeedbackAttackRoundInfo(const ATTACKROUNDINFO* info);
    static void HandleFocusChanged(const CFocusEvent& evt);
    static int32_t Idle(const void* data, void* param);
    static int32_t MessageHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnMirrorTimerEvent(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void Initialize();
    static void InitializeFrameXML();
    static void InitializeGame();
    static void RegisterGameCVars();
    static void ScaleToScreenSize(int32_t width, int32_t height, int32_t force);
    static void SetUIScale(float scale);
    static bool UnitHighlightsCVarCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void ShowBlockedFrameFeedback(CScriptRegion* region);
    static void AutoMouseOver();
    static void ClearCursor(int32_t a1, int32_t a2);
    static void UpdateThreatWarningEnabled(int32_t value);
    static void UpdateUnitHighlights();
    static void UpdateAreaSpiritHealerDistance();
    static void UpdateCorpseDistance();
    static void UpdateInteractTarget();
    static void DoPowerRegen();
    static void Shutdown();
    static void ShutdownGame();
    static WOWGUID m_currentObjectTrack;
    static WOWGUID m_pendingItem;
    static WOWGUID m_interactTarget;
    static float m_interactMaxDistSq;
    static WOWGUID m_areaSpiritHealerGUID;
    static void SetInteractTarget(const WOWGUID& guid);
    static void CloseInteraction();
    static AreaTableRec* GetCurrentSubzoneRec();
    static void SetAreaSpiritHealer(WOWGUID guid);
    static void UpdateAreaSpiritHealerTime(WOWGUID guid, int32_t time);
    static void DisplayPvPQueueRemovedError(int32_t reason);
    static void SetCorpseReclaimDelay(uint32_t delay);
    static void SetDungeonDifficulty(uint32_t difficulty, uint32_t unk, uint32_t inGroup);
    static void ShowDungeonDifficultyChanged();
    static void SetRaidDifficulty(uint32_t difficulty, uint32_t unk, uint32_t inGroup);
    static void UpdateComboPoints(const WOWGUID& guid, uint32_t points);
    static void StartMovie(uint32_t movieId);
    static void SetPhaseShift(uint32_t phase);
    static void ProposeLevelGrant(WOWGUID guid);
    static void SetCorpseMapPosition(const C3Vector& position, const float& facing);
    static void CastSpiritHealSpell();
    static void SetCursorItem(WOWGUID item, WOWGUID container, uint32_t slot, int32_t a4, int32_t stackSplit, int32_t playSound);
    static WOWGUID m_lockedTarget;
    static int32_t m_visible;
    static WOWGUID m_lastEnemyTarget;
    static WOWGUID m_corpseGUID;
    static int32_t m_initialized;
    static int32_t m_inMovie;
    static float m_aspect;
    static int32_t m_iCurrentMapID;
    static uint32_t m_uiLastInstanceTime;
    static int32_t m_iMapID;
    static int32_t m_bOwnsInstance;
    static int32_t m_areaID;

    static void ClearInstanceResetMap();
    static uint32_t m_dungeonDifficulty;
    static WOWGUID unk_BD07D0;
    static CSimpleTop* m_simpleTop;

    static int32_t CameraMusicIsPlaying();
    static SE2SoundKitObject m_cinematicMusic;
    static void LoadTrackedState();
    static void LoadMinimapTrackingState(const char* state);
    static int32_t CanPerformAction(uint32_t action);
    static void CastItemSpell(WOWGUID guid, int32_t confirmed);
    static const char* LastErrorMessage();
    static void SetLastEnchantSpellID(int32_t spellId);
    static void SetLastItemSpellTarget(WOWGUID guid);
    static int32_t m_lastEnchantSpellID;
    static WOWGUID m_lastItemSpellTarget;
    static void OnMouseModeNormal();
    static void OnMouseModeRelative();
    static uint32_t GetCurrentDungeonDifficulty();
    static uint32_t GetCurrentRaidDifficulty();
    static void SetLastInstanceTime(uint32_t time, int32_t mapId, int32_t currentMapId);
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
    static TotemInfo* GetTotemInfo(uint32_t slot);
    static int32_t IsKeyDown(KEY key);
    static int32_t IsRaidMemberOrPet(const WOWGUID& guid);
    static void ClearInteractTarget(const WOWGUID& guid, int32_t a2);
    static void CloseLoot(bool a1, bool a2, bool a3);
    static void OpenLoot(CGObject_C* object, int32_t coins, uint32_t lootType);
    static void ClearLootSlot(uint8_t slot);
    static void LootSlotChanged(uint8_t slot);
    static void LockItem(WOWGUID guid);
    static void SetPendingItemSound(WOWGUID guid, int32_t itemId);
    static void EnterWorld();
    static void RegisterSavedVariable(const char* name, int32_t perCharacter);
    static void BreakTarget(WOWGUID guid);
    static void DestroyTotem(int32_t slot);
    static void ClearTarget(WOWGUID guid, int32_t a2);
    static int32_t IsPartyMember(const WOWGUID& guid);
    static int32_t IsInParty(const WOWGUID& guid);
    static void GetSelectionHighlightColor(const WOWGUID& guid, CImVector* color, int32_t a3);
    static void SetFocusTarget(const WOWGUID& guid);
    static void UnitModelUpdate(const WOWGUID& guid, uint32_t flags);
    static CGPartyMemberState* GetPartyMemberStateFromPet(const WOWGUID& petGuid);
    static void UnitNameUpdate(const WOWGUID& guid);
    static void UnlockItem(WOWGUID guid);
    static int32_t m_stackSplit;
    static int32_t GetCursorSpell();
    static int32_t GetCursorVirtualItem();
    static void GetCursorVirtualItem(int32_t& id, int32_t& slot);
    static int32_t GetVirtualItemCount(int32_t id, uint32_t virtualSlot, int32_t a3);
    static void UnlockVirtualItem(int32_t id, uint32_t virtualSlot, int32_t a3);
    static int32_t GetCursorVirtualItemOfType(int32_t type);
    static void DropCursorSpell();
    static void SetCursorSpell(int32_t spellId);
    static void SetCursorPetAction(const uint32_t* action);
    static void SetCursorMacro(int32_t macroId);
    static void SetCursorVirtualItem(int32_t id, int32_t displayId, int32_t slot, int32_t type, int32_t stackSplit);
    static void SetCursorEquipmentSet(int32_t setId);
    static uint8_t s_inWorld;
    static const char* m_minimapZoneText;
    static const char* m_subZoneText;
    static int32_t m_subzoneID;
    static int32_t m_wmoGroupID;
    static uint32_t unk_BD08A4;
    static WOWGUID unk_BD08B0;
    static int32_t m_corpseMapID;
    static int32_t m_corpseActualMapID;
    static int32_t unk_BD0820;
    static WOWGUID m_corpseTransportGUID;
    static int32_t m_deathReleaseMapID;
    static C3Vector m_corpsePos;
    static C3Vector m_deathReleaseLoc;
    static void GetCorpsePosition(C3Vector& position);
    static void SetCorpseLocation(int32_t mapId, const C3Vector& position, int32_t actualMapId, int32_t transport);
    static void UpdateCorpseLocation();
    static void SetDeathReleaseLocation(int32_t mapId, const C3Vector& position);
    static void BeginCinematicInternal(void* param);
};

void EnableFadingScreen(float duration, void (*callback)(void*), void* param);
void SendRequestPetInfo();

class CObjectTrackEvent {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ WOWGUID m_prevGuid;
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
    /* 0000 */ int32_t m_type;
    /* 0004 */ int32_t m_value;
    /* 0008 */ int32_t m_maxValue;
    /* 000C */ int32_t m_scale;
    /* 0010 */ int32_t m_paused;
    /* 0014 */ int32_t m_spellId;
    /* 0018 */ uint32_t unk_0018;
};

struct CapturePointUIManagerNode : public TSLinkedNode<CapturePointUIManagerNode> {
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint32_t m_radiusSquared;
    /* 0014 */ uint32_t m_worldState;
};

class CCapturePointUIManager {
    public:
    /* 0000 */ TSList<CapturePointUIManagerNode, TSGetLink<CapturePointUIManagerNode>> m_nodes;
    /* 000C */ uint32_t m_nextWarningTime;

    void AddCapturePoint(CGGameObject_C* object, uint32_t worldState, uint32_t radius);
    void RemoveCapturePoint(CGGameObject_C* object);
    void Update();
};

extern CCapturePointUIManager g_capturePointUIManager;

extern CVar* g_predictedPowerCVar;

WOWGUID GetRaidMemberGUID(uint32_t index);
bool CanTriggerTutorial(uint32_t tutorial);
bool IsTutorialFlagged(uint32_t tutorial);

bool AutoInteractStateChangedCallback(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_autoCompleteResortNamesOnRecency(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_autoCompleteUseContext(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_autoCompleteWhenEditingFromCenter(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_guildRecruitmentChannel(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_previewTalents(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_threatWarning(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_uiScale(CVar* var, const char* oldValue, const char* value, void* arg);
bool CVarHandler_useUiScale(CVar* var, const char* oldValue, const char* value, void* arg);
void LoadAccountVariables();
void LoadPlacedFrames(HOSFILE localFile, HOSFILE cacheFile, void* param);
void LoadSavedVariables(CStatus* status);
struct TAINTEVENT : public TSHashObject<TAINTEVENT, HASHKEY_STR> {
};

void LoadScriptFunctions();
int32_t IsInFlyableArea();
void UnloadScriptFunctions();
void SavePlacedFrames(HOSFILE localFile, HOSFILE file, void* param);
void SaveGameCVarsCallback(HOSFILE localFile, HOSFILE file, void* param);
void DisableFadingScreen(float duration, void (*callback)(void*), void* param);
extern HSLOG s_taintLog;
bool GuildShowOfflineCallback(CVar* var, const char* oldValue, const char* value, void* arg);
bool SetCalendarFilterCVarCallback(CVar* var, const char* oldValue, const char* value, void* arg);
bool SetTaintLogCallback(CVar* var, const char* oldValue, const char* value, void* arg);
bool ShowClassColorInNameplateCallback(CVar* var, const char* oldValue, const char* value, void* arg);
void TaintEventCallback(lua_State* L, int32_t event, const char* name, const char* taint);

int32_t Script_GetBuildInfo(lua_State* L);
int32_t Script_ReloadUI(lua_State* L);
int32_t Script_SetConsoleKey(lua_State* L);
int32_t Script_Screenshot(lua_State* L);
int32_t Script_TogglePerformanceDisplay(lua_State* L);
int32_t Script_TogglePerformancePause(lua_State* L);
int32_t Script_TogglePerformanceValues(lua_State* L);
int32_t Script_ResetPerformanceValues(lua_State* L);
int32_t Script_GetDebugStats(lua_State* L);
int32_t Script_RegisterCVar(lua_State* L);
int32_t Script_GetCVarAbsoluteMin(lua_State* L);
int32_t Script_GetCVarAbsoluteMax(lua_State* L);
int32_t Script_ToggleTris(lua_State* L);
int32_t Script_TogglePortals(lua_State* L);
int32_t Script_ToggleCollision(lua_State* L);
int32_t Script_ToggleCollisionDisplay(lua_State* L);
int32_t Script_TogglePlayerBounds(lua_State* L);
int32_t Script_Stuck(lua_State* L);
int32_t Script_Logout(lua_State* L);
int32_t Script_EquipCursorItem(lua_State* L);
int32_t Script_DeleteCursorItem(lua_State* L);
int32_t Script_EquipPendingItem(lua_State* L);
int32_t Script_CancelPendingEquip(lua_State* L);
int32_t Script_TargetUnit(lua_State* L);
void TargetNearestEnemy(int32_t reverse, int32_t type);
int32_t UnitMatchesTargetType(CGUnit_C* unit, CGUnit_C* target, int32_t type);
int32_t Script_TargetDirectionEnemy(lua_State* L);
int32_t Script_TargetDirectionFriend(lua_State* L);
int32_t Script_TargetLastTarget(lua_State* L);
int32_t Script_TargetLastEnemy(lua_State* L);
int32_t Script_TargetLastFriend(lua_State* L);
int32_t Script_AttackTarget(lua_State* L);
int32_t Script_AssistUnit(lua_State* L);
int32_t Script_FollowUnit(lua_State* L);
int32_t Script_InteractUnit(lua_State* L);
int32_t Script_ClearFocus(lua_State* L);
int32_t Script_ToggleSheath(lua_State* L);
int32_t Script_InitiateTrade(lua_State* L);
int32_t Script_CanInspect(lua_State* L);
int32_t Script_NotifyInspect(lua_State* L);
int32_t Script_InviteUnit(lua_State* L);
int32_t Script_UninviteUnit(lua_State* L);
int32_t Script_ResurrectGetOfferer(lua_State* L);
int32_t Script_ResurrectHasSickness(lua_State* L);
int32_t Script_ResurrectHasTimer(lua_State* L);
int32_t Script_ForceLogout(lua_State* L);
int32_t Script_PickupPlayerMoney(lua_State* L);
int32_t Script_HasSoulstone(lua_State* L);
int32_t Script_UseSoulstone(lua_State* L);
int32_t Script_GuildSetMOTD(lua_State* L);
int32_t Script_GetInstanceLockTimeRemaining(lua_State* L);
int32_t Script_GetInstanceLockTimeRemainingEncounter(lua_State* L);
int32_t Script_GetSummonConfirmSummoner(lua_State* L);
int32_t Script_ConfirmSummon(lua_State* L);
int32_t Script_StopCinematic(lua_State* L);
int32_t Script_RunScript(lua_State* L);
int32_t Script_CheckInteractDistance(lua_State* L);
int32_t Script_IsWindowsClient(lua_State* L);
int32_t Script_IsMacClient(lua_State* L);
int32_t Script_IsLinuxClient(lua_State* L);
int32_t Script_AcceptXPLoss(lua_State* L);
int32_t Script_BindEnchant(lua_State* L);
int32_t Script_ReplaceEnchant(lua_State* L);
int32_t Script_ReplaceTradeEnchant(lua_State* L);
int32_t Script_GetTimeToWellRested(lua_State* L);
int32_t Script_GMRequestPlayerInfo(lua_State* L);
int32_t Script_GetCoinIcon(lua_State* L);
int32_t Script_GetCoinText(lua_State* L);
int32_t Script_GetCoinTextureString(lua_State* L);
int32_t Script_ConfirmBindOnUse(lua_State* L);
int32_t Script_SetPortraitToTexture(lua_State* L);
int32_t Script_GetLocale(lua_State* L);
int32_t Script_GetGMTicketCategories(lua_State* L);
int32_t Script_DropItemOnUnit(lua_State* L);
int32_t Script_RestartGx(lua_State* L);
int32_t Script_RestoreVideoResolutionDefaults(lua_State* L);
int32_t Script_RestoreVideoEffectsDefaults(lua_State* L);
int32_t Script_RestoreVideoStereoDefaults(lua_State* L);
int32_t Script_ConfirmTalentWipe(lua_State* L);
int32_t Script_ConfirmBinder(lua_State* L);
int32_t Script_ShowHelm(lua_State* L);
int32_t Script_ShowCloak(lua_State* L);
int32_t Script_AcceptAreaSpiritHeal(lua_State* L);
int32_t Script_CancelAreaSpiritHeal(lua_State* L);
int32_t Script_GetItemInfo(lua_State* L);
int32_t Script_GetItemGem(lua_State* L);
int32_t Script_GetExtendedItemInfo(lua_State* L);
int32_t Script_GetItemIcon(lua_State* L);
int32_t Script_GetItemFamily(lua_State* L);
int32_t Script_GetItemCount(lua_State* L);
int32_t Script_PickupItem(lua_State* L);
int32_t Script_IsCurrentItem(lua_State* L);
int32_t Script_IsUsableItem(lua_State* L);
int32_t Script_IsHelpfulItem(lua_State* L);
int32_t Script_IsHarmfulItem(lua_State* L);
int32_t Script_IsConsumableItem(lua_State* L);
int32_t Script_IsEquippableItem(lua_State* L);
int32_t Script_IsEquippedItem(lua_State* L);
int32_t Script_IsEquippedItemType(lua_State* L);
int32_t Script_IsDressableItem(lua_State* L);
int32_t Script_ItemHasRange(lua_State* L);
int32_t Script_IsItemInRange(lua_State* L);
int32_t Script_GetNumAddOns(lua_State* L);
int32_t Script_GetAddOnInfo(lua_State* L);
int32_t Script_GetAddOnMetadata(lua_State* L);
int32_t Script_GetAddOnMemoryUsage(lua_State* L);
int32_t Script_GetAddOnCPUUsage(lua_State* L);
int32_t Script_GetFrameCPUUsage(lua_State* L);
int32_t Script_GetAddOnDependencies(lua_State* L);
int32_t Script_EnableAddOn(lua_State* L);
int32_t Script_EnableAllAddOns(lua_State* L);
int32_t Script_DisableAddOn(lua_State* L);
int32_t Script_DisableAllAddOns(lua_State* L);
int32_t Script_IsAddOnLoadOnDemand(lua_State* L);
int32_t Script_GetBillingTimeRested(lua_State* L);
int32_t Script_CanShowResetInstances(lua_State* L);
int32_t Script_SetDungeonDifficulty(lua_State* L);
int32_t Script_SetRaidDifficulty(lua_State* L);
int32_t Script_ReportBug(lua_State* L);
int32_t Script_ReportSuggestion(lua_State* L);
int32_t Script_GetMirrorTimerProgress(lua_State* L);
int32_t Script_SetCurrentTitle(lua_State* L);
int32_t Script_UseItemByName(lua_State* L);
int32_t Script_EquipItemByName(lua_State* L);
int32_t Script_StartAttack(lua_State* L);
int32_t Script_StopAttack(lua_State* L);
int32_t Script_Dismount(lua_State* L);
int32_t Script_VoicePushToTalkStart(lua_State* L);
int32_t Script_VoicePushToTalkStop(lua_State* L);
int32_t Script_CanGrantLevel(lua_State* L);
int32_t Script_GrantLevel(lua_State* L);
int32_t Script_CanSummonFriend(lua_State* L);
int32_t Script_SummonFriend(lua_State* L);
int32_t Script_GetNumDeclensionSets(lua_State* L);
int32_t Script_DeclineName(lua_State* L);
int32_t Script_AcceptLevelGrant(lua_State* L);
int32_t Script_UploadSettings(lua_State* L);
int32_t Script_DownloadSettings(lua_State* L);
int32_t Script_GetMovieResolution(lua_State* L);
int32_t Script_GameMovieFinished(lua_State* L);
int32_t Script_GetItemUniqueness(lua_State* L);
int32_t Script_EndRefund(lua_State* L);
int32_t Script_EndBoundTradeable(lua_State* L);
int32_t Script_GetAccountExpansionLevel(lua_State* L);

extern CVar* g_profanityFilterCVar;
extern CVar* g_spamFilterCVar;
extern CVar* g_serviceTypeFilterCVar;
extern CVar* g_minimapZoomCVar;
extern CVar* g_minimapInsideZoomCVar;
extern CVar* g_minimapPortalMaxCVar;
extern CVar* g_enablePVPNotifyAFKCVar;
extern CVar* g_chatBubblesCVar;
extern CVar* g_chatBubblesPartyCVar;

void UpdateInstanceOwnershipPacket(int32_t owns);

void UpdateModelForUI(const WOWGUID& guid, uint32_t flags);

extern CVar* s_cvLfgSelectedRoles;

const char* MirrorTimerTypeToken(int32_t type);
const char* GetMirrorTimerLabel(int32_t type, int32_t spellId);

void BroadcastUIError(int32_t error, const char* name);

#endif
