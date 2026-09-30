#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_PLAYER_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"

class CClientObjCreate;
class CDataStore;
class CGItem_C;
class CGPetition;
class CreatureModelDataRec;
class SpellRec;
class SpellVisualKitRec;

struct InspectItem {
    /* 0000 */ WOWGUID m_creator;
    /* 0008 */ int32_t m_itemId;
    /* 000C */ int32_t m_enchants [12];
    /* 003C */ int16_t m_randomPropertiesId;
    /* 003E */ uint16_t m_flags;
    /* 0040 */ uint32_t m_propertySeed;
    /* 0044 */ uint32_t unk_0044;
};

struct InspectData {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ InspectItem m_items [19];
};

class ItemSetRec;

struct ITEMSWAP {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008 [6];
};

struct TRADESKILLLINE : public TSHashObject<TRADESKILLLINE, HASHKEY_NONE> {
    /* 0018 */ TSGrowableArray<int32_t> m_spells;
};

struct PROFICIENCYSPELL : public TSHashObject<PROFICIENCYSPELL, HASHKEY_NONE> {
    /* 0018 */ int32_t m_spellId;
};

struct PlayerPendingItemExpiration : public TSLinkedNode<PlayerPendingItemExpiration> {
    /* 0008 */ WOWGUID m_itemGuid;
    /* 0010 */ int32_t m_slot;
    /* 0014 */ int32_t m_duration;
};

class CGPlayer_C : public CGUnit_C, public CGPlayer {
    public:
    void DanceMoveAnimComplete();
    static int32_t IsXRayVisionActive();
    void AddKnownSpell(int32_t spellId, int32_t a2, int32_t a3, int32_t a4);
    void AutoEquipItem(WOWGUID container, uint32_t slot, int32_t a3);
    void TurnOffGuildTabardPurchase();
    static int32_t GetNewContinentID();
    void CountEquippedGems();
    static void Initialize();
    static void Shutdown();
    static uint32_t s_displayId;
    static WOWGUID s_lastUpdateGroup;
    static WOWGUID s_lastUpdatePVPGroup;
    static uint32_t s_lastUpdatePVPSequence;
    static uint32_t s_lastUpdatePVPTime;
    static uint32_t s_lastUpdateSequence;
    static uint32_t s_lastUpdateTime;
    static WOWGUID s_lastVendorListReceived;
    static WOWGUID s_resurrectOfferer;
    static uint32_t s_resurrectOfferSickness;
    static uint32_t s_resurrectOfferUseTimer;

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
    static bool CanDualWield();
    int32_t CanLoot(const CGObject_C* object);
    static void __stdcall GetQuestReward(const WOWGUID& questGiver, int32_t questId, int32_t reward);
    static void __stdcall GiveQuestItems(const WOWGUID& questGiver, int32_t questId);
    static int32_t IsGiftWrapping();
    static void CancelGiftWrap();
    static int32_t AutoCombatModeEventHandler(const void* data, void* param);
    static void __stdcall LeaveGroup();
    static void __stdcall OnTaxiNodeStatus(CDataStore* msg);
    static int32_t __stdcall OnLootItemNotify(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverStatus(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverStatusMultiple(CDataStore* msg);
    static int32_t __stdcall OnRefundInfo(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverQuestList(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverQuestDetails(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverRequestItems(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverOfferReward(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverQuestInvalid(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverQuestComplete(CDataStore* msg);
    static int32_t __stdcall OnQuestGiverQuestFailed(CDataStore* msg);
    static int32_t __stdcall OnTrainerList(CDataStore* msg);
    static int32_t __stdcall OnSellResponse(CDataStore* msg);
    static int32_t __stdcall OnBuySucceeded(CDataStore* msg);
    static int32_t __stdcall OnBuyFailed(CDataStore* msg);
    static int32_t __stdcall OnPetitionShowList(CDataStore* msg);
    static int32_t __stdcall OnPetitionShowSignatures(CDataStore* msg);
    static int32_t __stdcall OnPetitionDecline(CDataStore* msg);
    static int32_t __stdcall OnTurnInPetitionResults(CDataStore* msg);
    static int32_t __stdcall OnPetitionRename(CDataStore* msg);
    static int32_t __stdcall OnLootMoneyNotify(CDataStore* msg);
    static void OnLootClearMoney();
    static void __stdcall ReadItemResult(NETMESSAGE msgId, CDataStore* msg);
    static void __stdcall PushQuestToParty(int32_t questId);
    static void __stdcall QueryQuest(const WOWGUID& questGiver, int32_t questId);
    static void __stdcall QueryQuestGiverStatus(const WOWGUID& questGiver);
    static void QueryQuestGiverStatusMultiple();
    bool IsRAFPlayer(WOWGUID guid);
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
    static void PlayerInitializeMessageHandlers();
    static void __stdcall TalkToAuctioneer(const WOWGUID& guid);
    static void __stdcall TalkToBanker(const WOWGUID& guid);
    static void __stdcall TalkToBattleMaster(const WOWGUID& guid);
    static void __stdcall TalkToPetitionVendor(const WOWGUID& guid);
    static void __stdcall TalkToRideVehicle(const WOWGUID& guid);
    static void __stdcall TalkToSpellClick(const WOWGUID& guid);
    static void __stdcall TalkToStableMaster(const WOWGUID& guid);
    static void __stdcall TalkToGossip(const WOWGUID& guid);
    static void __stdcall TalkToQuestUnit(const WOWGUID& guid);
    static void __stdcall TalkToTrainer(const WOWGUID& guid);
    static void __stdcall TalkToGuildBanker(const WOWGUID& guid);
    static void __stdcall ShopFromMerchant(const WOWGUID& guid);
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
    /* 18CC */ TSGrowableArray<int32_t> m_companionSpells;
    /* 18DC */ uint32_t unk_18DC;
    /* 18E0 */ WOWGUID m_lootTarget;
    /* 18E8 */ uint8_t m_autoLoot;
    /* 18E9 */ uint8_t unk_18E9 [3];
    /* 18EC */ uint32_t unk_18EC;
    /* 18F0 */ CGBag_C m_bag;
    /* 1908 */ uint32_t unk_1908 [6];
    /* 1920 */ uint32_t m_clientStandState;
    /* 1924 */ int32_t m_gemCounts [4];
    /* 1934 */ uint32_t m_combatModeTime;
    /* 1938 */ WOWGUID m_combatTarget;
    /* 1940 */ uint32_t m_pvpCooldownTimestamp;
    /* 1944 */ uint32_t unk_1944;
    /* 1948 */ uint32_t unk_1948;
    /* 194C */ int32_t m_playedTime;
    /* 1950 */ uint32_t m_playedTimeBase;
    /* 1954 */ uint32_t unk_1954;

    const CGBag_C* GetBag() const override;
    CGBag_C* GetBag() override;
    void GetAFKText(char* buffer, uint32_t size) const override;
    void GetDNDText(char* buffer, uint32_t size) const override;
    void GetGMText(char* buffer, uint32_t size) const override;
    void GetDevText(char* buffer, uint32_t size) const override;
    uint32_t GetImpactType() const override;
    const CGUnitVirtualItem* GetDefendingItem() const override;
    bool IsDeadOrGhost() const override;
    uint32_t GetClientStandState() const override;
    void Reenable() override;
    void PostReenable() override;
    void PreAnimate(CGWorldFrame* frame) override;
    void ShouldRender(uint32_t flags, int32_t& culled, int32_t& hidden) override;

    void AutoBankItem(WOWGUID bagGuid, uint32_t slot);
    void AutoStoreBankItem(WOWGUID bagGuid, uint32_t slot);
    bool CanTrackObject(CGObject_C* object);
    bool CanTrackUnit(const CGUnit_C* unit);
    int32_t CheckBinderDistance();
    int32_t CheckSpiritHealerDistance();
    int32_t CheckTalentMasterDistance();
    uint8_t FindSlotIndex(WOWGUID guid) const;
    uint32_t GetCoinage() const;
    uint32_t GetCreatureTracking() const;
    uint32_t GetCurrentXP() const;
    uint32_t GetNextLevelXP() const;
    uint32_t GetDefaultLanguage() const;
    int32_t GetLanguageSkill(uint32_t language, uint32_t& skill);
    int16_t GetSkillModifier(int32_t index);
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
    int32_t GetModDamageDonePos(int32_t school) const;
    bool MeetsItemSetSkill(const ItemSetRec* set) const;
    InspectItem* GetInspectedItem(int32_t slot);
    int32_t GetModDamageDoneNeg(int32_t school) const;
    float GetModDamageDonePct(int32_t school) const;
    const CGItem_C* EquippedItemMeetSpellRequirements(const SpellRec* spell, uint32_t slotMask);
    int32_t HasZoneExplored(int32_t zone);
    int32_t IsCommentatorUberOrInArena() const;
    int32_t IsInMelee() const;
    void OnLootGameObject(const WOWGUID& guid);
    bool HasEquipBypassSkill(int32_t skillId) const;
    void LootMoney();
    void PutLootInSlot(WOWGUID bagGuid, uint8_t slot, uint8_t lootSlot);
    void QueueItemEnchantTime(WOWGUID item, int32_t slot, int32_t duration);
    void ReadItem(WOWGUID guid, uint8_t slot);
    void SetFarSightFocus(CGObject_C* object);
    void SetPlayedTime(int32_t playedTime);
    void ChangeStandState(uint32_t standState);
    void SetAFK(const char* message);
    void UpdateActiveMoverDisplay(uint32_t time);
    void SetPvpCooldownTimestamp(uint32_t flags);
    void TalkToBinder(const WOWGUID& guid);
    void TalkToTabardVendor(const WOWGUID& guid);
    void TalkToSpiritHealer(const WOWGUID& guid);
    void TalkToSpiritGuide(const WOWGUID& guid);
    void ToggleFarSight(CGObject_C* object);
    void UpdateAttackPosition(const CGUnit_C* target);
    void UpdateQuestStatus(CGObject_C* object);
    void UpdateTaxiStatus(CGUnit_C* unit);
    void HandleRepopRequest(bool a2);
    void SetClientStandState(uint32_t standState);
    void SetCombatMode();
    CGUnit_C* GetPossessedUnit();
    int32_t OnLootResponse(uint32_t time, CDataStore* msg);
    int32_t OnLootReleaseResponse(CDataStore* msg);
    int32_t OnLootRemoved(CDataStore* msg);
    int32_t OnLootSlotChanged(CDataStore* msg);
    int32_t OnSignedResults(CDataStore* msg);
    int32_t OnListInventory(CDataStore* msg);
    int32_t OnItemRefundResult(CDataStore* msg);
    void DelKnownSpell(int32_t spellId, int32_t a3);
    bool VehicleAllowsInteraction() const;
    void SetAutoLoot(int32_t autoLoot);
    bool CanInteract(const CGUnit_C* unit) const;
    void LootUnit(CGUnit_C* unit, bool autoInteract);
    void HandleSkinning(CGUnit_C* unit, bool autoInteract, bool a4);
    void HandleOnRightClick(CGUnit_C* unit, bool autoInteract);
    void CTMAttackUnit(CGUnit_C* unit);
    void CombatModeEnter(const WOWGUID& target, int32_t queueRanged, int32_t silent);
    void MeleeModeEnter(const WOWGUID& target);
    void CTMClickTerrain(const C3Vector& position);
    bool IsInFlyableZone(int32_t a1);
    int32_t SpellUsesWeaponSkill(const SpellRec* spell);
    CGPlayer_C(uint32_t time, const CClientObjCreate& create);
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable);
    void SetStorage(uint32_t* storage, uint32_t* saved);
    int32_t RefreshVisibleItems();
    void OnDeath();
    void EquipItem(CGItem_C* item);
    void OnGuildChanged(int32_t a2);
    void TogglePlayerFlag(uint32_t oldFlags);
    void UpdateInventoryComponent(INVENTORY_SLOTS slot, const VisibleItem_C* oldItem);
    uint32_t GetEquippedItemDisplayId();
    void PostInitActivePlayer();
    void UpdatePartyMemberState();
    void SetActiveMirrorHandlers();
    void MeleeModeLeave();
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
int32_t Packet_Group_9(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_10(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t PH_LOOT_OPCODES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_LEARNED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_REMOVED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SUPERCEDED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_INITIAL_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ACTION_BUTTONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_CANCEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_UNINVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_SET_LEADER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_DESTROYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PARTY_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUP_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_REAL_GROUP_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_12(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_13(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RESURRECT_REQUEST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAYER_SKINNED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_INSPECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_INSPECT_TALENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_14(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CANCEL_COMBAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GUILD_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GUILD_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GUILD_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GUILD_EVENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GUILD_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_SAVE_GUILD_EMBLEM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_TABARDVENDOR_ACTIVATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_15(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_EMOTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_HandlePartyMemberStats(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_17(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_QUEST_CONFIRM_ACCEPT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_18(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLLOGEXECUTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLLOGMISS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLDAMAGESHIELD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLINSTAKILLLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLNONMELEEDAMAGELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLHEALLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLENERGIZELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_19(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RESURRECT_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELLORDAMAGE_IMMUNE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DAMAGE_CALC_LOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SUMMON_REQUEST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAY_TIME_WARNING(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ARENA_TEAM_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ARENA_TEAM_EVENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ARENA_TEAM_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ARENA_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_TIME_SYNC_REQ(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_VOICE_SESSION_ROSTER_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_VOICE_SESSION_LEAVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_VOICE_SET_TALKER_MUTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_VOICE_PARENTAL_CONTROLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CROSSED_INEBRIATION_THRESHOLD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AVAILABLE_VOICE_CHANNEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CALENDAR_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GROUPACTION_THROTTLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SEND_UNLEARN_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_BATTLEGROUND_INFO_THROTTLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_TALENTS_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_QUEST_POI_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_DEBUG_SERVER_GEO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_QUEST_FORCE_REMOVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_COMBAT_LOG_MULTIPLE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Player_C_SetPlayerRenderDisabled(int32_t disabled);
int32_t Player_C_TogglePlayerRender();
void SetAttackSwingError(int32_t error);
void __stdcall SignalPlayerAttackEvent(int32_t a1);
void SignalPlayerSkillStatEvents();
void PlayerSignalSkillStatChanges();
int32_t SignalPetSpellPowerUpdate(WOWGUID guid);

void Unk7FE510(uint32_t guidLow);

class LootRoll {
    public:
    static int32_t AllPassed(CDataStore* msg);
    static int32_t Won(CDataStore* msg);
    static int32_t Start(CDataStore* msg);
    static int32_t Roll(CDataStore* msg);
    static int32_t MasterLoot(CDataStore* msg);
};

void PlayerClientInitialize();
void PlayerInitializeSounds();
void AreaTriggersInitialize(uint32_t mapId);
void PlayerClientShutdown();
int32_t Player_C_ZoneUpdateHandler(const void* data, void* param);
int32_t GetModalNextSpell();

#endif
