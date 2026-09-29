#include "Player_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Net/Types.h"
#include "_Triage/CClientObjCreate.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include <new>
#include <ctime>

// STATIC: 0x00C9D4F0
static uint32_t s_proficiency[17];

// STATIC: 0x00C9D534
static uint32_t s_bindAreaId;

// STATIC: 0x00C9D538
static uint32_t s_bindMapId;

// STATIC: 0x00C9D53C
static int32_t s_bindPointSet;

// STATIC: 0x00C9D540
static int32_t s_playerRenderDisabled;

// STATIC: 0x00C9D544
static uint32_t s_attackSwingErrorTime;

// STATIC: 0x00C9EAA8
static int32_t s_attackSwingError;

// STATIC: 0x00C9EAAC
static uint32_t s_pendingMovie;

// STATIC: 0x00C9EAB8
WOWGUID CGPlayer_C::s_resurrectOfferer;

// STATIC: 0x00C9EAD0
static WOWGUID s_giftWrapItem;

// STATIC: 0x00C9EB10
static WOWGUID s_spiritHealer;

// STATIC: 0x00C9EB18
static WOWGUID s_talentMaster;

// STATIC: 0x00C9EB20
static WOWGUID s_binder;

// STATIC: 0x00C9EB2C
static C3Vector s_bindPosition;

// STATIC: 0x00C9EB50
static int32_t* s_weaponSkills;

// STATIC: 0x00C9EB64
static TSGrowableArray<ITEMSWAP> s_itemSwaps;

// STATIC: 0x00A31BB8
static const int32_t s_trivialLevelDifference[20] = {
    4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

// VERIFY: 486d0703 auto
// OFFSET: 0x006D5C30
uint32_t GetDisplayIdFromRaceAndGender(uint32_t race, uint32_t gender) {
    if (gender >= 3) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);

    if (!raceRec) {
        return 0;
    }

    switch (gender) {
        case 0: {
            return raceRec->m_maleDisplayID;
        }

        case 1: {
            return raceRec->m_femaleDisplayID;
        }

        default: {
            return 0;
        }
    }
}

// VERIFY: 24f1b2a8 manual
// OFFSET: 0x006DC810
CreatureModelDataRec* GetModelDataRec(uint32_t race, uint32_t gender) {
    if (gender >= 3) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    uint32_t displayID = GetDisplayIdFromRaceAndGender(race, gender);

    if (!displayID) {
        return nullptr;
    }

    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(displayID);

    if (!displayRec) {
        DebugLog("Error, unknown displayInfo %d specified for player race %d sex %d!", displayID, race, gender);
    }

    CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

    if (!modelRec) {
        DebugLog("Error, unknown model record %d specified for player race %d sex %d!", displayRec->m_modelID, race, gender);
    }

    return modelRec;
}

// VERIFY: c3ce918a manual
// OFFSET: 0x004F7310
uint64_t CGPlayer_C::GetFarSightGuid() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerFarsight.m_guid;
}

// VERIFY: 274a51a9 auto
// OFFSET: 0x006E4940
void CGPlayer_C::SetFarSightFocus(CGObject_C* object) {
    this->ToggleFarSight(object);
}

// VERIFY: ab2cdd89 manual
// OFFSET: 0x0051A250
uint32_t CGPlayer_C::GetSkillValue(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    uint32_t rank = this->m_player->m_playerSkillInfo[index].m_rank;

    if (rank) {
        rank += this->m_player->m_playerSkillInfo[index].m_permBonus;
    }

    return rank;
}

// VERIFY: afe2eea5 auto
// OFFSET: 0x0051A2B0
uint32_t CGPlayer_C::GetCoinage() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerFieldCoinage;
}

// VERIFY: cd4355a9 manual
// OFFSET: 0x0052E6D0
const char* CGPlayer_C::GetHairCustomizationName() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        ChrRacesRec* rec = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);

        if (rec) {
            return rec->m_hairCustomization;
        }
    }

    return "NORMAL";
}

// VERIFY: 08168323 manual
// OFFSET: 0x0052E730
const char* CGPlayer_C::GetFacialHairCustomizationName() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        ChrRacesRec* rec = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);

        if (rec) {
            return rec->m_facialHairCustomization[player->m_player->m_playerBytes3[0]];
        }
    }

    return "NORMAL";
}

// VERIFY: ad2d261f manual
// OFFSET: 0x00594710
uint32_t CGPlayer_C::GetSkillMaxRank(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    uint32_t rank = this->m_player->m_playerSkillInfo[index].m_maxRank;

    if (rank) {
        rank += this->m_player->m_playerSkillInfo[index].m_permBonus;
    }

    return rank;
}

// VERIFY: 637f9a95 auto
// OFFSET: 0x0060A5D0
uint32_t CGPlayer_C::GetCurrentXP() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerXp;
}

// VERIFY: ad4453ec auto
// OFFSET: 0x006CBCF0
int32_t OnSummonCancel(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(455, nullptr);

    return 1;
}

// VERIFY: 0993e05f auto
// OFFSET: 0x006CDDD0
int32_t OnUpdatePlayerScriptEvent(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    FrameScript_SignalEvent(reinterpret_cast<uintptr_t>(param), "%s", "player");

    return 1;
}

// VERIFY: 9f64b8a4 auto
// OFFSET: 0x006CDE90
uint32_t CGPlayer_C::GetProficiency(uint8_t itemClass) {
    if (itemClass >= sizeof(s_proficiency) / sizeof(s_proficiency[0])) {
        return 0;
    }

    return s_proficiency[itemClass];
}

// VERIFY: 340d9418 auto
// OFFSET: 0x006CDEB0
int32_t Packet_SMSG_SET_PROFICIENCY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t itemClass;
    uint32_t mask;
    msg->Get(itemClass);
    msg->Get(mask);
    s_proficiency[itemClass] = mask;
    ConsolePrintf("Proficiency in item class %d set to %08x", itemClass, mask);

    return 1;
}

// VERIFY: 37ec0ce6 auto
// OFFSET: 0x006CED80
int32_t __stdcall CGPlayer_C::OnLootItemNotify(CDataStore* msg) {
    WOWGUID guid;
    uint8_t slot;
    uint8_t unk;
    uint32_t itemId;
    char name[64];
    msg->Get(guid.m_guid);
    msg->Get(slot);
    msg->Get(unk);
    msg->Get(itemId);
    msg->GetString(name, sizeof(name));

    return 1;
}

// VERIFY: dbeea1b7 auto
// OFFSET: 0x006CEE30
int32_t Player_C_TogglePlayerRender() {
    s_playerRenderDisabled = s_playerRenderDisabled == 0;

    return s_playerRenderDisabled;
}

// VERIFY: 45c68f81 auto
// OFFSET: 0x006CEEC0
void CGPlayer_C::SaveBindPoint(CDataStore* msg) {
    msg->Get(s_bindPosition.x);
    msg->Get(s_bindPosition.y);
    msg->Get(s_bindPosition.z);
    msg->Get(s_bindMapId);
    msg->Get(s_bindAreaId);
    s_bindPointSet = 1;
}

// VERIFY: 5ce7e8a8 auto
// OFFSET: 0x006CEF10
uint32_t CGPlayer_C::GetBindAreaID() {
    return s_bindAreaId;
}

// VERIFY: cdd8b8b9 auto
// OFFSET: 0x006CEF20
void __stdcall SignalPlayerAttackEvent(int32_t a1) {
    FrameScript_SignalEvent(318, "%s", "player");
}

// VERIFY: 14438ea5 auto
// OFFSET: 0x006CEF40
void SignalPlayerSkillStatEvents() {
    FrameScript_SignalEvent(318, "%s", "player");
    FrameScript_SignalEvent(319, "%s", "player");
    FrameScript_SignalEvent(120, "%s", "player");
}

// VERIFY: 7ca6e35b auto
// OFFSET: 0x006CEFB0
int32_t CGPlayer_C::IsGiftWrapping() {
    return static_cast<bool>(s_giftWrapItem);
}

// VERIFY: 4cbc468b auto
// OFFSET: 0x006CF240
PlaySpellVisuaKitData::PlaySpellVisuaKitData(SpellRec* spellRec, SpellVisualKitRec* impactVisualKit, uint32_t a4) {
    this->m_spellRec = spellRec;
    this->unk_0008 = a4;
    this->m_impactVisualKit = impactVisualKit;
    this->m_position = nullptr;
    this->m_transportGUID = WOWGUID();
    this->unk_0018 = 1;
    this->unk_001C = 0;
    this->unk_0020 = 0;
    this->unk_0024 = -1;
    this->m_someGUID = WOWGUID();
    this->unk_0030 = 0;
}

// VERIFY: 6bc8ba4b auto
// OFFSET: 0x006CF290
uint32_t CGPlayer_C::GetPendingMovie() {
    return s_pendingMovie;
}

// VERIFY: 6650af23 auto
// OFFSET: 0x006CF2D0
int32_t CGPlayer_C::GetCombatModeFlashDuration() {
    return 500;
}

// VERIFY: 14843d40 auto
// OFFSET: 0x006CF440
int32_t CGPlayer_C::GetPlayedTime() const {
    if (this->m_playedTime < 0) {
        return 0;
    }

    return this->m_playedTime + static_cast<int32_t>(time(nullptr)) - this->m_playedTimeBase;
}

// VERIFY: e7f0a743 auto
// OFFSET: 0x006CF470
void CGPlayer_C::SetPlayedTime(int32_t playedTime) {
    this->m_playedTime = playedTime;
    this->m_playedTimeBase = static_cast<uint32_t>(time(nullptr));
}

// VERIFY: d1c43e20 manual
// OFFSET: 0x006D1CB0
int32_t SignalPetSpellPowerUpdate(WOWGUID guid) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(674, nullptr);
    }

    return 1;
}

// VERIFY: e266ed2d manual
// OFFSET: 0x006D1D30
void __stdcall CGPlayer_C::AcceptResurrectRequest(int32_t accept) {
    if (!CGPlayer_C::s_resurrectOfferer) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RESURRECT_RESPONSE));
    msg.Put(CGPlayer_C::s_resurrectOfferer.m_guid);
    msg.Put(static_cast<uint8_t>(accept));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPlayer_C::s_resurrectOfferer = WOWGUID();
}

// VERIFY: a7e8dc5b manual
// OFFSET: 0x006D1FC0
int32_t CGPlayer_C::CheckSpiritHealerDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_spiritHealer, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// VERIFY: fdee41de manual
// OFFSET: 0x006D2070
int32_t CGPlayer_C::CheckTalentMasterDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_talentMaster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// VERIFY: 9db57599 manual
// OFFSET: 0x006D2290
int32_t CGPlayer_C::CheckBinderDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_binder, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// VERIFY: 76878623 manual
// OFFSET: 0x006D2340
void __stdcall CGPlayer_C::TalkToBanker(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BANKER_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 201bfc5e manual
// OFFSET: 0x006D2480
void __stdcall CGPlayer_C::TalkToPetitionVendor(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_SHOW_LIST));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: c275d25e manual
// OFFSET: 0x006D25C0
void __stdcall CGPlayer_C::TalkToBattleMaster(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BATTLEMASTER_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: eb868649 manual
// OFFSET: 0x006D2640
void __stdcall CGPlayer_C::TalkToAuctioneer(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_AUCTION_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 553fae36 manual
// OFFSET: 0x006D26C0
void __stdcall CGPlayer_C::TalkToStableMaster(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_LIST_STABLED_PETS));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: a0931e1c manual
// OFFSET: 0x006D2740
void __stdcall CGPlayer_C::TalkToSpellClick(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SPELL_CLICK));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d56325e8 manual
// OFFSET: 0x006D27C0
void __stdcall CGPlayer_C::TalkToRideVehicle(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PLAYER_VEHICLE_ENTER));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 1e5377fe manual
// OFFSET: 0x006D29F0
uint8_t CGPlayer_C::FindSlotIndex(WOWGUID guid) const {
    uint32_t numSlots = this->m_bag.m_numSlots;

    if (!numSlots) {
        return 0xFF;
    }

    for (uint8_t slot = 0; slot < numSlots; slot++) {
        WOWGUID slotGuid = slot < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[slot] : WOWGUID();

        if (slotGuid == guid) {
            return slot;
        }
    }

    return 0xFF;
}

// VERIFY: da1b57c4 manual
// OFFSET: 0x006D2C20
void __stdcall CGPlayer_C::AutoStoreLootItem(uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOSTORE_LOOT_ITEM));
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 5a267c7f manual
// OFFSET: 0x006D2D40
void CGPlayer_C::SellItem(WOWGUID vendor, WOWGUID item, uint32_t count) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SELL_ITEM));
    msg.Put(vendor.m_guid);
    msg.Put(item.m_guid);
    msg.Put(count);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 9095114f manual
// OFFSET: 0x006D43C0
void __stdcall CGPlayer_C::Uninvite(WOWGUID guid, const char* name) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_UNINVITE_GUID));
    msg.Put(guid.m_guid);
    msg.PutString(name);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: c3f83a27 manual
// OFFSET: 0x006D4450
void __stdcall CGPlayer_C::AcceptGroup(uint32_t roles) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_ACCEPT));
    msg.Put(roles);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 662f9a0a manual
// OFFSET: 0x006D44D0
void __stdcall CGPlayer_C::DeclineGroup() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_DECLINE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 1e5ae87e manual
// OFFSET: 0x006D4540
void __stdcall CGPlayer_C::LeaveGroup() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_DISBAND));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 8d68652a manual
// OFFSET: 0x006D45B0
void __stdcall CGPlayer_C::SilencePlayer(WOWGUID guid, bool silence) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PARTY_SILENCE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(silence));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 321e8245 manual
// OFFSET: 0x006D4640
void __stdcall CGPlayer_C::UnSilencePlayer(WOWGUID guid, bool silence) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PARTY_UNSILENCE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(silence));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 2afbda5e manual
// OFFSET: 0x006D46D0
void __stdcall CGPlayer_C::SetLootMethod(uint32_t method, WOWGUID master, uint32_t threshold) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_LOOT_METHOD));
    msg.Put(method);
    msg.Put(master.m_guid);
    msg.Put(threshold);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 12aae8b7 manual
// OFFSET: 0x006D4760
void __stdcall CGPlayer_C::AcceptGuild() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_ACCEPT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 26140604 manual
// OFFSET: 0x006D47D0
void __stdcall CGPlayer_C::DeclineGuild() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_DECLINE_INVITATION));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d46256ad manual
// OFFSET: 0x006D4840
void __stdcall CGPlayer_C::AcceptArenaTeam() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_ACCEPT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 40a1eebd manual
// OFFSET: 0x006D48B0
void __stdcall CGPlayer_C::DeclineArenaTeam() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_DECLINE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 69a73445 auto
// OFFSET: 0x006D49C0
bool CGPlayer_C::HasEquipBypassSkill(int32_t skillId) const {
    for (uint32_t i = 0; i < this->m_equipBypassSkills.m_count; i++) {
        if (this->m_equipBypassSkills.m_data[i] == skillId) {
            return true;
        }
    }

    return false;
}

// VERIFY: c02a1585 manual
// OFFSET: 0x006D4B90
void CGPlayer_C::LootMoney() {
    if (!this->m_lootTarget) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LOOT_MONEY));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 7a72f86c manual
// OFFSET: 0x006D4C10
void __stdcall CGPlayer_C::QueryQuest(const WOWGUID& questGiver, int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_QUERY_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint8_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: c799eb8b manual
// OFFSET: 0x006D4CA0
void __stdcall CGPlayer_C::AcceptQuest(const WOWGUID& questGiver, int32_t questId, int32_t a3) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_ACCEPT_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint32_t>(a3));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: aaae61a9 manual
// OFFSET: 0x006D4D40
void __stdcall CGPlayer_C::CompleteQuest(const WOWGUID& questGiver, int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_COMPLETE_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d323253c manual
// OFFSET: 0x006D4DD0
void __stdcall CGPlayer_C::GiveQuestItems(const WOWGUID& questGiver, int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_REQUEST_REWARD));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 922d1ead manual
// OFFSET: 0x006D4E60
void __stdcall CGPlayer_C::GetQuestReward(const WOWGUID& questGiver, int32_t questId, int32_t reward) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_CHOOSE_REWARD));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint32_t>(reward));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 3e324e1c manual
// OFFSET: 0x006D4F00
void __stdcall CGPlayer_C::PushQuestToParty(int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PUSH_QUEST_TO_PARTY));
    msg.Put(static_cast<uint32_t>(questId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 2068fcba manual
// OFFSET: 0x006D4F80
void __stdcall CGPlayer_C::QuestLogRemoveQuest(uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_LOG_REMOVE_QUEST));
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 513fa025 manual
// OFFSET: 0x006D5000
void __stdcall CGPlayer_C::QueryQuestGiverStatus(const WOWGUID& questGiver) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_QUERY));
    msg.Put(questGiver.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 8b0806a0 manual
// OFFSET: 0x006D50C0
void CGPlayer_C::QueryQuestGiverStatusMultiple() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d5ef3b86 manual
// OFFSET: 0x006D5BE0
const CGUnitVirtualItem* CGPlayer_C::GetDefendingItem() const {
    const CGBag_C* bag = this->GetBag();
    WOWGUID guid = static_cast<uint32_t>(bag->m_numSlots) > 4 ? bag->m_slots[4] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        return &item->m_virtualItem;
    }

    return this->CGUnit_C::GetDefendingItem();
}

// VERIFY: b515a05c manual
// OFFSET: 0x006D5C90
void __stdcall CGPlayer_C::ReadItem(uint8_t bag, uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_READ_ITEM));
    msg.Put(bag);
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 9f933d46 auto
// OFFSET: 0x006D5E70
uint32_t CGPlayer_C::GetDefaultLanguage() const {
    ChrRacesRec* rec = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    return rec ? rec->m_baseLanguage : 0;
}

// VERIFY: 3cb1369e manual
// OFFSET: 0x006D62A0
void __stdcall CGPlayer_C::EnableTaxiNode(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ENABLE_TAXI_NODE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 726c606a manual
// OFFSET: 0x006D6320
void __stdcall CGPlayer_C::QueryTaxiNodes(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TAXI_QUERY_AVAILABLE_NODES));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 440a3a74 manual
// OFFSET: 0x006D63A0
void __stdcall CGPlayer_C::StartTaxi(WOWGUID guid, uint32_t source, uint32_t destination) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACTIVATE_TAXI));
    msg.Put(guid.m_guid);
    msg.Put(source);
    msg.Put(destination);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: b5718ddf manual
// OFFSET: 0x006D6430
void __stdcall CGPlayer_C::StartTaxiExpress(WOWGUID guid, const uint32_t* nodes, int32_t count) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACTIVATE_TAXI_EXPRESS));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint32_t>(count));

    for (int32_t i = 0; i < count; i++) {
        msg.Put(nodes[i]);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: daeba6e5 auto
// OFFSET: 0x006D64E0
uint32_t CGPlayer_C::GetClientStandState() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_clientStandState;
    }

    return this->m_unit->m_unitFieldBytes1[0];
}

// VERIFY: 8b250d1e manual
// OFFSET: 0x006D6E00
void __stdcall CGPlayer_C::RequestPetitionSignatures(WOWGUID petition) {
    if (!petition) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_SHOW_SIGNATURES));
    msg.Put(petition.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 1f8b3e30 auto
// OFFSET: 0x006D6E90
int32_t CGPlayer_C::GetPVPFactionIndex() const {
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (!raceRec) {
        return -1;
    }

    FactionTemplateRec* factionRec = g_factionTemplateDB.GetRecord(raceRec->m_factionID);

    if (!factionRec) {
        return -1;
    }

    if (factionRec->m_factionGroup & 0x4) {
        return 0;
    }

    return (factionRec->m_factionGroup & 0x2) ? 1 : -1;
}

// VERIFY: 101d568d manual
// OFFSET: 0x006D7120
void __stdcall CGPlayer_C::RemoveTempEnchantment(int32_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_TEMP_ENCHANTMENT));
    msg.Put(static_cast<uint32_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 0f647542 manual
// OFFSET: 0x006D7200
void __stdcall CGPlayer_C::SetPartyAssignment(uint8_t assignment, WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_PARTY_ASSIGNMENT));
    msg.Put(assignment);
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 501f6a3e manual
// OFFSET: 0x006D7290
void __stdcall CGPlayer_C::ClearPartyAssignment(uint8_t assignment, WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_PARTY_ASSIGNMENT));
    msg.Put(assignment);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 42e7c0e2 auto
// OFFSET: 0x006D7B70
uint32_t CGPlayer_C::GetCreatureTracking() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerTrackCreatures;
}

// VERIFY: 5b662924 manual
// OFFSET: 0x006DAC10
int32_t CGPlayer_C::IsDeadOrGhost() const {
    return this->m_unit->m_unitFieldHealth <= 0 || ((this->m_player->m_playerFlags >> 4) & 1);
}

// VERIFY: 3028b833 auto
// OFFSET: 0x006DC1C0
int32_t CGPlayer_C::GetSkillIndex(int32_t skillId) const {
    int32_t index = 0;

    for (uint32_t i = 0; i < 128; i++) {
        uint16_t id = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillInfo[i].m_skillId : 0;

        if (id == skillId) {
            break;
        }

        index++;
    }

    if (index == 128) {
        return -1;
    }

    return index;
}

// VERIFY: 43bc2c57 manual
// OFFSET: 0x006DC8D0
void CGPlayer_C::GetAFKText(char* buffer, uint32_t size) const {
    if ((this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGChat::m_afk) || (this->m_player->m_playerFlags & 0x2)) {
        const char* text = FrameScript_GetText("CHAT_FLAG_AFK", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// VERIFY: 817b7a91 manual
// OFFSET: 0x006DC940
void CGPlayer_C::GetDNDText(char* buffer, uint32_t size) const {
    if (this->m_player->m_playerFlags & 0x4) {
        const char* text = FrameScript_GetText("CHAT_FLAG_DND", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// VERIFY: aa26e3f6 manual
// OFFSET: 0x006DC980
void CGPlayer_C::GetGMText(char* buffer, uint32_t size) const {
    uint32_t flags = this->m_player->m_playerFlags;

    if ((flags & 0x8) && !((flags >> 15) & 1)) {
        const char* text = FrameScript_GetText("CHAT_FLAG_GM", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// VERIFY: 66d20b22 manual
// OFFSET: 0x006DC9D0
void CGPlayer_C::GetDevText(char* buffer, uint32_t size) const {
    if (this->m_player->m_playerFlags & 0x8000) {
        SStrPack(buffer, "<Dev>", size);
    }
}

// VERIFY: 67d51c6d manual
// OFFSET: 0x006DE130
bool CGPlayer_C::UnitIsTrivial(const CGUnit_C* unit) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !unit || ((unit->m_obj->m_type >> 4) & 1)) {
        return false;
    }

    int32_t level = player->m_unit->m_unitFieldLevel;
    int32_t difference = level - unit->m_unit->m_unitFieldLevel;

    if (difference <= 0) {
        return false;
    }

    if (static_cast<uint32_t>(level / 5) >= sizeof(s_trivialLevelDifference) / sizeof(s_trivialLevelDifference[0])) {
        return difference > 8;
    }

    return difference > s_trivialLevelDifference[level / 5];
}

// VERIFY: 57e71b87 manual
// OFFSET: 0x006DE330
const VisibleItem_C* CGPlayer_C::GetVisibleItem(int32_t slot) const {
    if (slot < 0 || slot >= 19) {
        return nullptr;
    }

    return &this->m_player->m_playerVisibleItems[slot];
}

// VERIFY: 4d46d50f manual
// OFFSET: 0x006DE750
void CGPlayer_C::SetPvpCooldownTimestamp(uint32_t flags) {
    uint32_t playerFlags = this->m_player->m_playerFlags;

    if (playerFlags & 0x200) {
        this->m_pvpCooldownTimestamp = 0;
    } else if (!((playerFlags >> 18) & 1) || !(flags & 0x40000)) {
        this->m_pvpCooldownTimestamp = FrameTime::s_curTimeMs + 300000;
    }
}

// VERIFY: c787d6b7 manual
// OFFSET: 0x006DE7A0
int32_t CGPlayer_C::GetPVPToggleTimer() const {
    if (!(this->m_player->m_playerFlags & 0x40000)) {
        return 300000;
    }

    uint32_t timestamp = this->m_pvpCooldownTimestamp;

    if (timestamp && static_cast<int32_t>(FrameTime::s_curTimeMs - timestamp) < 0) {
        return timestamp - FrameTime::s_curTimeMs;
    }

    return -1;
}

// VERIFY: 8ab92411 auto
// OFFSET: 0x006DE7E0
uint32_t CGPlayer_C::GetEquippedAmmoDisplayId() const {
    int32_t ammoId = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerAmmoId : 0;
    ItemRec* rec = g_itemDB.GetRecord(ammoId);

    return rec ? rec->m_displayInfoID : 0;
}

// VERIFY: f842d174 manual
// OFFSET: 0x006DE980
int32_t CGPlayer_C::IsCommentatorUberOrInArena() const {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    uint32_t flags = this->m_player->m_playerFlags;

    return ((flags >> 19) & 1) && (((flags >> 22) & 1) || (map && map->m_instanceType == 4));
}

// VERIFY: 881f2596 manual
// OFFSET: 0x006E61B0
void CGPlayer_C::QueueItemEnchantTime(WOWGUID item, int32_t slot, int32_t duration) {
    PlayerPendingItemExpiration* expiration = this->m_pendingItemExpirations.NewNode(2, 0, 0);
    expiration->m_itemGuid = item;
    expiration->m_slot = slot;
    expiration->m_duration = duration;
}

// VERIFY: 585bb9b6 auto
// OFFSET: 0x006CEE50
int32_t Player_C_SetPlayerRenderDisabled(int32_t disabled) {
    int32_t previous = s_playerRenderDisabled;
    s_playerRenderDisabled = disabled;

    return previous;
}

// VERIFY: 45bf8d28 auto
// OFFSET: 0x006D4920
int32_t CGPlayer_C::SpellUsesWeaponSkill(const SpellRec* spell) {
    if (spell->m_targets & 0x10) {
        return spell->m_id;
    }

    if ((spell->m_attributesExB & 0x8000000) || spell->m_effect[0] == 25 || spell->m_equippedItemClass != 2 || !spell->m_equippedItemSubclass) {
        return spell->m_id;
    }

    for (uint32_t slot = 0; slot < 3; slot++) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(slot, false);

        if (item && item->m_classID == spell->m_equippedItemClass && ((1 << item->m_subclassID) & spell->m_equippedItemSubclass) && s_weaponSkills[item->m_subclassID]) {
            return s_weaponSkills[item->m_subclassID];
        }
    }

    return spell->m_id;
}

// VERIFY: 22b545a2 manual
// OFFSET: 0x006D5FC0
void __stdcall CGPlayer_C::OnTaxiNodeStatus(CDataStore* msg) {
    WOWGUID guid = WOWGUID();
    msg->Get(guid.m_guid);

    uint8_t status = 0;
    msg->Get(status);

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && ((unit->m_unit->m_unitNpcFlags >> 13) & 1)) {
        unit->m_interactIconType = status ? 0 : 7;
        unit->UpdateInteractIcon();
    }
}

// VERIFY: 2a53878e manual
// OFFSET: 0x006D71A0
int32_t CGItem::IsUsable() const {
    uint32_t flags = this->m_item->m_itemFieldFlags;

    return (((flags >> 3) & 1) || !this->m_item->m_itemFieldMaxdurability || this->m_item->m_itemFieldDurability) && !((flags >> 4) & 1);
}

// VERIFY: 53463c6b manual
// OFFSET: 0x006DA3C0
int32_t OnHonorCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerFieldHonorCurrency < *static_cast<const uint32_t*>(value)) {
        SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);
    }

    FrameScript_SignalEvent(497, nullptr);
    FrameScript_SignalEvent(605, nullptr);

    return 1;
}

// VERIFY: d721193e manual
// OFFSET: 0x006DA430
int32_t OnArenaCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerFieldArenaCurrency < *static_cast<const uint32_t*>(value)) {
        SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);
    }

    FrameScript_SignalEvent(497, nullptr);
    FrameScript_SignalEvent(605, nullptr);

    return 1;
}

// VERIFY: d04fc29e manual
// OFFSET: 0x006DAA80
int32_t OnUpdatePlayerGlyph(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (offset - offsetof(CGPlayerLocalData, m_playerFieldGlyphs1)) / sizeof(uint32_t);

    if (value && g_glyphPropertiesDB.GetRecord(*static_cast<const int32_t*>(value))) {
        if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
            FrameScript_SignalEvent(613, "%d", slot + 1);
        } else {
            FrameScript_SignalEvent(612, "%d", slot + 1);
        }
    } else if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
        FrameScript_SignalEvent(611, "%d", slot + 1);
    }

    return 1;
}

// VERIFY: 389560f2 manual
// OFFSET: 0x006DAB80
int32_t OnUpdatePlayerGlyphSlotsEnabled(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t enabled = *static_cast<const uint32_t*>(value);

        for (uint32_t i = 0; i < 6; i++) {
            uint32_t bit = 1 << i;

            if (player->m_player->m_playerGlyphsEnabled & bit) {
                if (!(bit & enabled)) {
                    FrameScript_SignalEvent(614, "%d", i + 1);
                }
            } else if (bit & enabled) {
                FrameScript_SignalEvent(615, "%d", i + 1);
            }
        }
    }

    return 1;
}

// VERIFY: e11e8b77 auto
// OFFSET: 0x006DF850
uint32_t FindEmptySwapIndex() {
    uint32_t index = 0;

    for (; index < s_itemSwaps.m_count; index++) {
        if (!s_itemSwaps.m_data[index].m_guid) {
            break;
        }
    }

    if (index == s_itemSwaps.m_count) {
        s_itemSwaps.SetCount(s_itemSwaps.m_count + 1);
    }

    return index;
}

// VERIFY: fb3e8173 manual
// OFFSET: 0x007076F0
int32_t CGPlayer_C::GetEquipBypassSkill(uint32_t index) const {
    if (index < this->m_equipBypassSkills.m_count) {
        return this->m_equipBypassSkills.m_data[index];
    }

    return 0;
}

// VERIFY: 2b34eb63 auto
// OFFSET: 0x005140E0
int32_t CGPlayer_C::IsInMelee() const {
    return (this->m_combatModeFlags & 0x2) || this->CGUnit_C::IsInMelee();
}

// VERIFY: 03afa9dc manual
// OFFSET: 0x00546150
int32_t CGPlayer_C::HasZoneExplored(int32_t zone) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->CGPlayer::HasZoneExplored(zone);
}

// VERIFY: e1bf01f5 manual
// OFFSET: 0x006D2A40
void CGPlayer_C::AutoStoreBankItem(WOWGUID bagGuid, uint32_t slot) {
    CGObject_C* bagObject = ClntObjMgrObjectPtr(bagGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!bagObject || !bagObject->GetBag()) {
        return;
    }

    CGBag_C* bag = bagObject->GetBag();

    if (slot >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[slot]) {
        return;
    }

    uint8_t bagIndex = this->FindSlotIndex(bagGuid);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOSTORE_BANK_ITEM));
    msg.Put(bagIndex);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 6ce36b74 manual
// OFFSET: 0x006D2B30
void CGPlayer_C::AutoBankItem(WOWGUID bagGuid, uint32_t slot) {
    CGObject_C* bagObject = ClntObjMgrObjectPtr(bagGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!bagObject || !bagObject->GetBag()) {
        return;
    }

    CGBag_C* bag = bagObject->GetBag();

    if (slot >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[slot]) {
        return;
    }

    uint8_t bagIndex = this->FindSlotIndex(bagGuid);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOBANK_ITEM));
    msg.Put(bagIndex);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: b0bc7cf8 manual
// OFFSET: 0x006D2CA0
void CGPlayer_C::PutLootInSlot(WOWGUID bagGuid, uint8_t slot, uint8_t lootSlot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_STORE_LOOT_IN_SLOT));
    msg.Put(this->FindSlotIndex(bagGuid));
    msg.Put(slot);
    msg.Put(lootSlot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: b4f04b3e auto
// OFFSET: 0x006D5D10
void CGPlayer_C::ReadItem(WOWGUID guid, uint8_t slot) {
    if (guid == this->m_obj->m_guid) {
        CGPlayer_C::ReadItem(static_cast<uint8_t>(255), slot);
        return;
    }

    for (uint32_t index = 19; index < 23; index++) {
        WOWGUID bagGuid = index < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[index] : WOWGUID();

        if (bagGuid == guid) {
            CGPlayer_C::ReadItem(static_cast<uint8_t>(index), slot);
            return;
        }
    }
}

// VERIFY: b5faad94 manual
// OFFSET: 0x006D6B10
void __stdcall CGPlayer_C::BuyPetition(const WOWGUID& vendor, CGPetition* petition) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_BUY));
    msg.Put(vendor.m_guid);
    petition->Pack(&msg);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 027e5ec0 auto
// OFFSET: 0x006D6F00
const char* CGPlayer_C::GetPVPRankString(int32_t rank) const {
    char token[32];
    SStrPrintf(token, sizeof(token), "PVP_RANK_%d_%d", rank, this->GetPVPFactionIndex());

    return this->GetFrameScriptText(token, -1);
}

// VERIFY: 24dda942 manual
// OFFSET: 0x006DC230
bool CGPlayer_C::GetExpandedSkillRank(int32_t skillId, int32_t& rank, int32_t& bonus) const {
    int32_t index = this->GetSkillIndex(skillId);

    if (index < 0) {
        return false;
    }

    rank = this->GetSkillValue(index);
    bonus = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillInfo[index].m_tempBonus : 0;

    return true;
}

// VERIFY: 0f64563c manual
// OFFSET: 0x006DC2C0
int32_t CGPlayer_C::GetSkillRank(int32_t skillId) const {
    int32_t index = this->GetSkillIndex(skillId);

    if (index < 0) {
        return 0;
    }

    int16_t bonus = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillInfo[index].m_tempBonus : 0;
    int32_t rank = bonus + this->GetSkillValue(index);

    return rank < 0 ? 0 : rank;
}

// VERIFY: ca1d9a08 manual
// OFFSET: 0x006DCA00
bool CGPlayer_C::CanTrackUnit(const CGUnit_C* unit) {
    if (this->IsDeadOrGhost()) {
        return false;
    }

    uint8_t flags = unit->m_unit->m_unitFieldBytes1[2];

    if (flags & 0x4) {
        return false;
    }

    if (((unit->m_unit->m_unitDynamicFlags >> 1) & 1) || ((flags & 0x2) && (this->m_player->m_playerFieldBytes[0] & 0x2))) {
        return true;
    }

    int32_t creatureType = unit->GetCreatureType();

    return creatureType && (this->GetCreatureTracking() & (1 << (creatureType - 1)));
}

// VERIFY: 1b989ff9 manual
// OFFSET: 0x006E6250
void CGPlayer_C::GetPendingEnchantTime(CGItem_C* item) {
    if (!item) {
        return;
    }

    PlayerPendingItemExpiration* expiration = this->m_pendingItemExpirations.Head();

    while (expiration) {
        PlayerPendingItemExpiration* next = expiration->Next();

        if (expiration->m_itemGuid == item->m_obj->m_guid) {
            item->UpdateEnchantmentTime(expiration->m_slot, expiration->m_duration);
            expiration->m_link.Unlink();
            delete expiration;
        }

        expiration = next;
    }
}

// VERIFY: b9d03315 auto
// OFFSET: 0x006CEE70
void SetAttackSwingError(int32_t error) {
    s_attackSwingError = error;
    s_attackSwingErrorTime = FrameTime::s_curTimeMs;
}

// VERIFY: 9d0eb63c manual
// OFFSET: 0x006D11C0
int32_t __stdcall CGPlayer_C::OnQuestGiverStatus(CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    uint8_t status;
    msg->Get(status);

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && object->IsQuestGiver()) {
        object->SetQuestStatus(status);
    }

    return 1;
}

// VERIFY: a22feec7 manual
// OFFSET: 0x006D1230
int32_t __stdcall CGPlayer_C::OnQuestGiverStatusMultiple(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid;
        msg->Get(guid.m_guid);

        uint8_t status;
        msg->Get(status);

        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && object->IsQuestGiver()) {
            object->SetQuestStatus(status);
        }
    }

    return 1;
}

// VERIFY: 3e1e45a2 manual
// OFFSET: 0x006D1650
int32_t __stdcall CGPlayer_C::OnRefundInfo(CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        msg->m_read = msg->m_size;
        return 1;
    }

    CGItem_C::ItemPurchaseData* data = item->m_itemPurchaseData;

    if (!data) {
        data = new (__FILE__, __LINE__) CGItem_C::ItemPurchaseData;

        if (!data) {
            msg->m_read = msg->m_size;
            return 1;
        }

        item->SetItemPurchaseData(data);
    }

    msg->Get(data->m_money);
    msg->Get(data->m_honorPoints);
    msg->Get(data->m_arenaPoints);

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(data->m_itemId[i]);
        msg->Get(data->m_itemCount[i]);

        WOWGUID requester = WOWGUID();
        g_itemDBCache.GetRecord(data->m_itemId[i], requester, nullptr, nullptr, true);
    }

    msg->Get(data->unk_0034);
    msg->Get(data->m_refundTimeLeft);

    return 1;
}

// VERIFY: a104b077 manual
// OFFSET: 0x006D4AD0
void CGPlayer_C::UpdateAttackPosition(const CGUnit_C* target) {
    s_attackSwingError = 0;
    s_attackSwingErrorTime = FrameTime::s_curTimeMs;

    if (!target) {
        return;
    }

    float range = this->m_unit->m_unitFieldCombatreach + target->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        range = 5.0f;
    }

    range *= 4.0f;

    C3Vector targetPosition = target->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = position.x - targetPosition.x;
    float dy = position.y - targetPosition.y;
    float dz = position.z - targetPosition.z;

    if (range * range <= dx * dx + dz * dz + dy * dy) {
        this->StopAttack();
    }
}

// VERIFY: f794ab61 manual
// OFFSET: 0x006D52D0
void __stdcall CGPlayer_C::ClearAFK(int32_t force) {
    if (!CGChat::m_afk || (!force && !g_autoClearAFKCVar->m_intValue)) {
        return;
    }

    CGChat::m_afk = 0;
    CGChat::AddChatMessage(FrameScript_GetText("CLEARED_AFK", -1, GENDER_NOT_APPLICABLE), 0, 0, 0, 0, 0, nullptr, WOWGUID(), 0, WOWGUID(), 0, 0, 0);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MESSAGECHAT));
    msg.Put(static_cast<uint32_t>(23));
    msg.Put(static_cast<uint32_t>(0));
    msg.PutString("");
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d8710bc2 manual
// OFFSET: 0x006E6B40
CGPlayer_C::CGPlayer_C(uint32_t time, const CClientObjCreate& create)
    : CGUnit_C(time, create),
      unk_1854(-1),
      m_combatModeFlags(0),
      unk_18B8(0),
      unk_18CC(),
      m_lootTarget(),
      unk_18E8(0) {
    CGPlayerLocalData* player = this->m_player;
    bool activePlayer = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
    this->m_bag.m_guid = this->m_obj->m_guid;
    this->m_bag.m_hasBank = 1;
    this->m_bag.m_numSlots = activePlayer ? 150 : 0;
    this->m_bag.m_slots = reinterpret_cast<WOWGUID*>(player->m_playerFieldInvSlotHead);
    this->unk_1938[0] = 0;
    this->unk_1938[1] = 0;

    if (!this->m_unit->m_unitFieldDisplayid) {
        this->GetUnitName(nullptr, 1);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        for (uint32_t i = 0; i < 1002; i++) {
            new (&this->m_playerLocalMirrorLists[i]) TSList<void>();
        }

        if (this->m_unit->m_unitFieldHealth <= 0) {
            this->Unk716040();
        }

        CGWorldFrame::UpdateScreenEffect();
    }

    this->unk_1934 = FrameTime::s_curTimeMs - 500;
    this->m_pvpCooldownTimestamp = 0;
    this->unk_1924[0] = 0;
    this->unk_1924[1] = 0;
    this->unk_1924[2] = 0;
    this->unk_1924[3] = 0;
    this->unk_1944 = 0;
    this->unk_1948 = 0;

    if (create.m_updateFlags & 0x1) {
        Unk7FE510(static_cast<uint32_t>(this->m_obj->m_guid.m_guid));
    }

    this->m_playedTime = -1;
    this->m_playedTimeBase = 0;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PLAYED_TIME));
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    this->unk_1954 = 0;
}

// VERIFY: 1bf91d47 auto
// OFFSET: 0x006D1CF0
void CGPlayer_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGUnit_C::SetStorage(storage, saved);
    this->m_player = reinterpret_cast<CGPlayerLocalData*>(storage + OBJECT_END + sizeof(CGUnitData) / sizeof(uint32_t));
    this->m_playerMirror = saved + CGUnit::TotalFields();
}

// VERIFY: aa03190b auto
// OFFSET: 0x006E8280
void CGPlayer_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_clientStandState = this->m_unit->m_unitFieldBytes1[0];
    this->CGUnit_C::PostInit(time, create, reenable);
    this->GetUnitName(nullptr, 1);
    this->OnGuildChanged(0);
    CGChat::UpdateGuildStatus();
    this->Animate(0.0f);
    this->UpdateAmmoModel(this->GetEquippedItemDisplayId(), 30);
    CGPartyInfo::EnableMember(this, 1);
    CGRaidInfo::EnableMember(this, 1);

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->m_playerBytes3[3] != CGBattlefieldInfo::m_arenaFaction) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddArenaOpponent(guid);

        int32_t index;
        guid = this->m_obj->m_guid;

        if (CGBattlefieldInfo::GetArenaOpponentIndex(guid, &index)) {
            CGBattlefieldInfo::m_arenaOpponentSeen[index] = 1;
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->PostInitActivePlayer();
    } else {
        this->UpdatePartyMemberState();
    }

    this->UpdatePetReaction();
    this->OnMoveUpdate(time, 1, 1);
}

// VERIFY: 5414e18a auto
// OFFSET: 0x006E6150
void CGPlayer_C::Reenable() {
    this->m_clientStandState = this->m_unit->m_unitFieldBytes1[0];
    this->CGUnit_C::Reenable();
    this->unk_18B8 = 0;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->SetActiveMirrorHandlers();
        CGGameUI::EnterWorld();
    }
}

// VERIFY: 2463b98f auto
// OFFSET: 0x006E1D70
void CGPlayer_C::PostReenable() {
    this->CGUnit_C::PostReenable();
    CGPartyInfo::EnableMember(this, 1);
    CGRaidInfo::EnableMember(this, 1);

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->m_playerBytes3[3] != CGBattlefieldInfo::m_arenaFaction) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddArenaOpponent(guid);

        int32_t index;
        WOWGUID guid2 = this->m_obj->m_guid;

        if (CGBattlefieldInfo::GetArenaOpponentIndex(guid2, &index)) {
            CGBattlefieldInfo::m_arenaOpponentSeen[index] = 1;
        }
    }

    this->UpdatePartyMemberState();
    this->UpdatePetReaction();
}
