#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/WGUID.h"

class CClientObjCreate;
class CDataStore;
class CGItem_C;
class CGPetition;
class CreatureModelDataRec;
class SpellRec;
class SpellVisualKitRec;

struct ITEMSWAP {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008 [6];
};

struct TRADESKILLLINE : public TSHashObject<TRADESKILLLINE, HASHKEY_NONE> {
};

struct PROFICIENCYSPELL : public TSHashObject<PROFICIENCYSPELL, HASHKEY_NONE> {
};

struct PlayerPendingItemExpiration : public TSLinkedNode<PlayerPendingItemExpiration> {
    /* 0008 */ WOWGUID m_itemGuid;
    /* 0010 */ int32_t m_slot;
    /* 0014 */ int32_t m_duration;
};

class CGPlayer_C : public CGUnit_C, public CGPlayer {
    public:
    static uint32_t s_displayId;
    static WOWGUID s_resurrectOfferer;

    static void __stdcall AcceptArenaTeam();
    static void __stdcall AcceptGroup(uint32_t roles);
    static void __stdcall AcceptGuild();
    static void __stdcall AcceptQuest(const WOWGUID& questGiver, int32_t questId, int32_t a3);
    static void __stdcall AcceptResurrectRequest(int32_t accept);
    static void __stdcall AutoStoreLootItem(uint8_t slot);
    static void __stdcall BuyPetition(const WOWGUID& vendor, CGPetition* petition);
    static void __stdcall ClearAFK(int32_t force);
    static void __stdcall ClearPartyAssignment(uint8_t assignment, WOWGUID guid);
    static void __stdcall CompleteQuest(const WOWGUID& questGiver, int32_t questId);
    static void __stdcall DeclineArenaTeam();
    static void __stdcall DeclineGroup();
    static void __stdcall DeclineGuild();
    static void __stdcall EnableTaxiNode(const WOWGUID& guid);
    static uint32_t GetBindAreaID();
    static int32_t GetCombatModeFlashDuration();
    static const char* GetFacialHairCustomizationName();
    static const char* GetHairCustomizationName();
    static uint32_t GetPendingMovie();
    static uint32_t GetProficiency(uint8_t itemClass);
    static void __stdcall GetQuestReward(const WOWGUID& questGiver, int32_t questId, int32_t reward);
    static void __stdcall GiveQuestItems(const WOWGUID& questGiver, int32_t questId);
    static int32_t IsGiftWrapping();
    static void __stdcall LeaveGroup();
    static void __stdcall OnTaxiNodeStatus(CDataStore* msg);
    static int32_t __stdcall OnLootItemNotify(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverStatus(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverStatusMultiple(CDataStore* msg);
    static int32_t __stdcall OnRefundInfo(CDataStore* msg);
    static void __stdcall PushQuestToParty(int32_t questId);
    static void __stdcall QueryQuest(const WOWGUID& questGiver, int32_t questId);
    static void __stdcall QueryQuestGiverStatus(const WOWGUID& questGiver);
    static void QueryQuestGiverStatusMultiple();
    static void __stdcall QueryTaxiNodes(const WOWGUID& guid);
    static void __stdcall QuestLogRemoveQuest(uint8_t slot);
    static void __stdcall ReadItem(uint8_t bag, uint8_t slot);
    static void __stdcall RemoveTempEnchantment(int32_t slot);
    static void __stdcall RequestPetitionSignatures(WOWGUID petition);
    static void SaveBindPoint(CDataStore* msg);
    static void SellItem(WOWGUID vendor, WOWGUID item, uint32_t count);
    static void __stdcall SetLootMethod(uint32_t method, WOWGUID master, uint32_t threshold);
    static void __stdcall SetPartyAssignment(uint8_t assignment, WOWGUID guid);
    static void __stdcall SilencePlayer(WOWGUID guid, bool silence);
    static void __stdcall StartTaxi(WOWGUID guid, uint32_t source, uint32_t destination);
    static void __stdcall StartTaxiExpress(WOWGUID guid, const uint32_t* nodes, int32_t count);
    static void TalkToMailbox(WOWGUID guid);
    static void __stdcall TalkToAuctioneer(const WOWGUID& guid);
    static void __stdcall TalkToBanker(const WOWGUID& guid);
    static void __stdcall TalkToBattleMaster(const WOWGUID& guid);
    static void __stdcall TalkToPetitionVendor(const WOWGUID& guid);
    static void __stdcall TalkToRideVehicle(const WOWGUID& guid);
    static void __stdcall TalkToSpellClick(const WOWGUID& guid);
    static void __stdcall TalkToStableMaster(const WOWGUID& guid);
    static void __stdcall Uninvite(WOWGUID guid, const char* name);
    static bool UnitIsTrivial(const CGUnit_C* unit);
    static void __stdcall UnSilencePlayer(WOWGUID guid, bool silence);

    /* 1010 */ TSList<void> m_playerMirrorLists [176];
    /* 1850 */ TSList<void>* m_playerLocalMirrorLists;
    /* 1854 */ int32_t unk_1854;
    /* 1858 */ uint32_t m_combatModeFlags;
    /* 185C */ TSHashTable<TRADESKILLLINE, HASHKEY_NONE> m_tradeSkillLines;
    /* 1884 */ TSHashTable<PROFICIENCYSPELL, HASHKEY_NONE> m_proficiencySpells;
    /* 18AC */ TSExplicitList<PlayerPendingItemExpiration, 0> m_pendingItemExpirations;
    /* 18B8 */ uint32_t unk_18B8;
    /* 18BC */ TSGrowableArray<int32_t> m_equipBypassSkills;
    /* 18CC */ uint32_t unk_18CC [4];
    /* 18DC */ uint32_t unk_18DC;
    /* 18E0 */ WOWGUID m_lootTarget;
    /* 18E8 */ uint8_t unk_18E8;
    /* 18E9 */ uint8_t unk_18E9 [3];
    /* 18EC */ uint32_t unk_18EC;
    /* 18F0 */ CGBag_C m_bag;
    /* 1908 */ uint32_t unk_1908 [6];
    /* 1920 */ uint32_t m_clientStandState;
    /* 1924 */ uint32_t unk_1924 [4];
    /* 1934 */ uint32_t unk_1934;
    /* 1938 */ uint32_t unk_1938 [2];
    /* 1940 */ uint32_t m_pvpCooldownTimestamp;
    /* 1944 */ uint32_t unk_1944;
    /* 1948 */ uint32_t unk_1948;
    /* 194C */ int32_t m_playedTime;
    /* 1950 */ uint32_t m_playedTimeBase;
    /* 1954 */ uint32_t unk_1954;

    void GetAFKText(char* buffer, uint32_t size) const override;
    void GetDNDText(char* buffer, uint32_t size) const override;
    void GetGMText(char* buffer, uint32_t size) const override;
    void GetDevText(char* buffer, uint32_t size) const override;
    uint32_t GetImpactType() const override;
    const CGUnitVirtualItem* GetDefendingItem() const override;
    int32_t IsDeadOrGhost() const override;
    uint32_t GetClientStandState() const override;
    void Reenable() override;
    void PostReenable() override;

    void AutoBankItem(WOWGUID bagGuid, uint32_t slot);
    void AutoStoreBankItem(WOWGUID bagGuid, uint32_t slot);
    bool CanTrackUnit(const CGUnit_C* unit);
    int32_t CheckBinderDistance();
    int32_t CheckSpiritHealerDistance();
    int32_t CheckTalentMasterDistance();
    uint8_t FindSlotIndex(WOWGUID guid) const;
    uint32_t GetCoinage() const;
    uint32_t GetCreatureTracking() const;
    uint32_t GetCurrentXP() const;
    uint32_t GetDefaultLanguage() const;
    int32_t GetEquipBypassSkill(uint32_t index) const;
    uint32_t GetEquippedAmmoDisplayId() const;
    bool GetExpandedSkillRank(int32_t skillId, int32_t& rank, int32_t& bonus) const;
    uint64_t GetFarSightGuid() const;
    void GetPendingEnchantTime(CGItem_C* item);
    int32_t GetPlayedTime() const;
    int32_t GetPVPFactionIndex() const;
    const char* GetPVPRankString(int32_t rank) const;
    int32_t GetPVPToggleTimer() const;
    int32_t GetSkillIndex(int32_t skillId) const;
    uint32_t GetSkillMaxRank(int32_t index) const;
    int32_t GetSkillRank(int32_t skillId) const;
    uint32_t GetSkillValue(int32_t index) const;
    const VisibleItem_C* GetVisibleItem(int32_t slot) const;
    int32_t HasZoneExplored(int32_t zone);
    int32_t IsCommentatorUberOrInArena() const;
    int32_t IsInMelee() const;
    bool HasEquipBypassSkill(int32_t skillId) const;
    void LootMoney();
    void PutLootInSlot(WOWGUID bagGuid, uint8_t slot, uint8_t lootSlot);
    void QueueItemEnchantTime(WOWGUID item, int32_t slot, int32_t duration);
    void ReadItem(WOWGUID guid, uint8_t slot);
    void SetFarSightFocus(CGObject_C* object);
    void SetPlayedTime(int32_t playedTime);
    void SetPvpCooldownTimestamp(uint32_t flags);
    void TalkToGuildBanker(const WOWGUID& guid);
    void ToggleFarSight(CGObject_C* object);
    void UpdateAttackPosition(const CGUnit_C* target);
    int32_t SpellUsesWeaponSkill(const SpellRec* spell);
    CGPlayer_C(uint32_t time, const CClientObjCreate& create);
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable);
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void RefreshVisibleItems();
    void OnDeath();
    void OnGuildChanged(int32_t a2);
    uint32_t GetEquippedItemDisplayId();
    void PostInitActivePlayer();
    void UpdatePartyMemberState();
    void SetActiveMirrorHandlers();
};

class PlaySpellVisuaKitData {
    public:
    /* 0000 */ SpellRec* m_spellRec;
    /* 0004 */ SpellVisualKitRec* m_impactVisualKit;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ C3Vector* m_position;
    /* 0010 */ WOWGUID m_transportGUID;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ WOWGUID m_someGUID;
    /* 0030 */ uint32_t unk_0030;

    PlaySpellVisuaKitData(SpellRec* spellRec, SpellVisualKitRec* impactVisualKit, uint32_t a4);
};

uint32_t FindEmptySwapIndex();
uint32_t GetDisplayIdFromRaceAndGender(uint32_t race, uint32_t gender);
CreatureModelDataRec* GetModelDataRec(uint32_t race, uint32_t gender);
int32_t OnArenaCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value);
int32_t OnHonorCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value);
int32_t OnSummonCancel(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t OnUpdatePlayerScriptEvent(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param);
int32_t OnUpdatePlayerGlyph(WOWGUID guid, uint32_t offset, uint32_t count, const void* value);
int32_t OnUpdatePlayerGlyphSlotsEnabled(WOWGUID guid, uint32_t offset, uint32_t count, const void* value);
int32_t Packet_SMSG_SET_PROFICIENCY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Player_C_SetPlayerRenderDisabled(int32_t disabled);
int32_t Player_C_TogglePlayerRender();
void SetAttackSwingError(int32_t error);
void __stdcall SignalPlayerAttackEvent(int32_t a1);
void SignalPlayerSkillStatEvents();
int32_t SignalPetSpellPowerUpdate(WOWGUID guid);

void Unk7FE510(uint32_t guidLow);

#endif
