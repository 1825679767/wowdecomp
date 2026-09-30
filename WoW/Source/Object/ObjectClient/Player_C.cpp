#include "Player_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
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
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "Engine/Source/Event/Event.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "_Triage/CClientObjCreate.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemSetRec.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include <new>
#include <ctime>
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "_Triage/AreaList.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include <malloc.h>
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/UIUtil/Tooltip.h"

struct GUILDUPDATETIMESTAMP {
    /* 0000 */ uint32_t m_guildId;
    /* 0004 */ uint32_t m_timestamp;
};

struct InitialSpellStruct {
    /* 0000 */ uint32_t m_spellId;
    /* 0004 */ uint16_t unk_0004;
    /* 0006 */ uint16_t unk_0006;
};

// STATIC: 0x00C9D33C
static int32_t s_unk_C9D33C;

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

// STATIC: 0x00C9EA90
WOWGUID CGPlayer_C::s_lastVendorListReceived;

// STATIC: 0x00C9EAA0
static WOWGUID s_unk_C9EAA0;

// STATIC: 0x00C9EAA8
static int32_t s_attackSwingError;

// STATIC: 0x00C9EAAC
static uint32_t s_pendingMovie;

// STATIC: 0x00C9EAB8
WOWGUID CGPlayer_C::s_resurrectOfferer;

// STATIC: 0x00C9EAB0
static uint32_t s_unk_C9EAB0;

// STATIC: 0x00C9EAC0
uint32_t CGPlayer_C::s_resurrectOfferSickness;

// STATIC: 0x00C9EAC4
uint32_t CGPlayer_C::s_resurrectOfferUseTimer;

// STATIC: 0x00C9EAB4
static uint32_t s_unk_C9EAB4;

// STATIC: 0x00C9EACC
static uint8_t s_tabardSavePending;

// STATIC: 0x00C9EAD0
static WOWGUID s_giftWrapItem;

// STATIC: 0x00C9EAE0
static InspectData* s_inspectData;

// STATIC: 0x00C9EC78
static InspectItem s_emptyInspectItem;

// STATIC: 0x00C9EAE4
uint32_t CGPlayer_C::s_lastUpdatePVPSequence;

// STATIC: 0x00C9EAE8
WOWGUID CGPlayer_C::s_lastUpdatePVPGroup;

// STATIC: 0x00C9EAF0
uint32_t CGPlayer_C::s_lastUpdatePVPTime;

// STATIC: 0x00C9EAF4
uint32_t CGPlayer_C::s_lastUpdateSequence;

// STATIC: 0x00C9EAF8
WOWGUID CGPlayer_C::s_lastUpdateGroup;

// STATIC: 0x00C9EB00
uint32_t CGPlayer_C::s_lastUpdateTime;

// STATIC: 0x00C9EAD8
static WOWGUID s_lastTrainerRequest;

// STATIC: 0x00C9D7D0
static MerchantItem s_vendorItems[150];

// STATIC: 0x00C9EB08
static int32_t s_questsAccepted;

// STATIC: 0x00C9D548
static WOWGUID s_questListNpc;

// STATIC: 0x00C9D750
static uint32_t s_questListIds[32];

// STATIC: 0x00C9D6D0
static uint32_t s_questListStatus[32];

// STATIC: 0x00C9D650
static uint32_t s_questListLevels[32];

// STATIC: 0x00C9D5D0
static uint32_t s_questListFlags[32];

// STATIC: 0x00C9D550
static uint32_t s_questListRepeatable[32];

// STATIC: 0x00C9EB04
static uint32_t s_questFailedReason;

// STATIC: 0x00C9EB10
static WOWGUID s_spiritHealer;

// STATIC: 0x00C9EB18
static WOWGUID s_talentMaster;

// STATIC: 0x00C9EB20
static WOWGUID s_binder;

// STATIC: 0x00C9EB2C
static C3Vector s_bindPosition;

// STATIC: 0x00AD9F6C
static int32_t s_actionButtons[144];

// STATIC: 0x00C9EB38
static TSGrowableArray<InitialSpellStruct> s_initialSpells;

// STATIC: 0x00C9EB48
static TSFixedArray<uint32_t> s_weaponSkills;

// STATIC: 0x00C9EB54
static TSGrowableArray<GUILDUPDATETIMESTAMP> s_guildUpdateTimestamps;

// STATIC: 0x00C9EB64
static TSGrowableArray<ITEMSWAP> s_itemSwaps;

// STATIC: 0x00A31BB8
static const int32_t s_trivialLevelDifference[20] = {
    4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

// STATIC: 0x00C9ECC0
static uint32_t s_combatModeExitCount;

// STATIC: 0x00C9EAC8
static int32_t s_dualWieldSpell;

// STATIC: 0x00D3F4EC
static uint32_t unk_D3F4EC;

// STATIC: 0x00C9EC68
static int32_t s_zoneUpdateCounter;

// VERIFY: a84630a0 auto
// OFFSET: 0x006CDD80
static int32_t OnUpdatePVPTitle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameUI::UnitNameUpdate(guid);

    return 1;
}

// VERIFY: 88fa79ea manual
// OFFSET: 0x006D1970
static int32_t PetChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid == ClntObjMgrGetActivePlayer() || CGGameUI::IsPartyMember(guid)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        for (int32_t i = 0; i < 2; i++) {
            WOWGUID oldGuid = static_cast<const WOWGUID*>(oldValue)[i];
            WOWGUID newGuid = i ? player->m_unit->m_unitFieldSummon : player->m_unit->m_unitFieldCharm;

            if (oldGuid) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(oldGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit) {
                    unit->UpdateScriptRegistration();
                } else if (oldGuid != newGuid) {
                    CGGameUI::ClearTarget(oldGuid, 1);

                    if (CGGameUI::unk_BD07D0 == oldGuid) {
                        WOWGUID focus = WOWGUID();
                        CGGameUI::SetFocusTarget(focus);
                    }
                }
            }

            if (newGuid) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(newGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit) {
                    unit->UpdateScriptRegistration();
                }
            }
        }

        if (guid == ClntObjMgrGetActivePlayer()) {
            CGPetInfo::FindPossessedSpell();
        }
    }

    return 1;
}

// VERIFY: 0af6d015 auto
// OFFSET: 0x006D1AE0
static int32_t GuildRankUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        CGGuildInfo::QueryPermissions();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_GUILD_UPDATE, nullptr);

    return 1;
}

// VERIFY: c6201d41 manual
// OFFSET: 0x006D1B20
static int32_t DuelTeamUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateReaction(true);
    }

    return 1;
}

// VERIFY: 02a74688 auto
// OFFSET: 0x006D3010
static int32_t GetWeaponSubClassCount(int32_t* count) {
    if (!count) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    int32_t maxSubClass = 0;
    int32_t found = 0;
    int32_t index = g_itemSubClassDB.m_numRecords;

    while (index--) {
        const ItemSubClassRec* rec = index >= 0 && index < g_itemSubClassDB.m_numRecords ? &g_itemSubClassDB.m_records[index] : nullptr;

        if (rec->m_classID == 2) {
            found = 1;

            if (maxSubClass <= rec->m_subClassID) {
                maxSubClass = rec->m_subClassID;
            }
        }
    }

    *count = maxSubClass + 1;

    return found;
}

// VERIFY: d6521b3e manual
// OFFSET: 0x006DA6B0
static int32_t OnUpdateInebriation(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        float drunkness = player->GetDrunknessTurnValue();

        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (target && player->CanAttack(target)) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_LEVEL, "%s", "target");
            }

            CGUnit_C::NamePlateUpdateLevels(true);
            CGWorldFrame::GetActiveCamera()->UpdateInebriation(drunkness, 0);
        }
    }

    return 1;
}

// VERIFY: 3507ab43 manual
// OFFSET: 0x006DA7F0
static int32_t OnUpdatePlayerHairColor(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (player->m_characterComponent) {
            player->m_characterComponent->SetHairColor(player->m_player->m_playerBytes[3], true, nullptr);
        }

        if (ClntObjMgrGetActivePlayer() == guid) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// VERIFY: f44b3c3d manual
// OFFSET: 0x006DA870
static int32_t OnUpdatePlayerSkinID(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (player->m_characterComponent) {
            player->m_characterComponent->SetSkinColor(player->m_player->m_playerBytes[0], true, false, nullptr);
        }

        if (ClntObjMgrGetActivePlayer() == guid) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// VERIFY: d2633809 manual
// OFFSET: 0x006DF530
static int32_t OnUpdatePlayerHairStyle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        bool activePlayer = ClntObjMgrGetActivePlayer() == guid;
        CCharacterComponent* component = player->m_characterComponent;

        if (component) {
            component->SetHairStyle(player->m_player->m_playerBytes[2], nullptr);

            if (!(player->m_player->m_playerFlags & 0x400) && (!activePlayer || !CGBarberShop::m_barberShopEnabled)) {
                VisibleItem_C* head = &player->m_player->m_playerVisibleItems[0];

                if (head && head->GetDisplayID() && player->CanEquipItemsInThisForm()) {
                    component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(0), head->GetDisplayID(), 0);
                }
            }
        }

        if (activePlayer) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// VERIFY: 77d10d5a manual
// OFFSET: 0x006DF620
static int32_t OnUpdatePlayerFacialStyle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        bool activePlayer = ClntObjMgrGetActivePlayer() == guid;
        CCharacterComponent* component = player->m_characterComponent;

        if (component) {
            component->SetBeardStyle(player->m_player->m_playerBytes2[0], true, nullptr);

            if (!(player->m_player->m_playerFlags & 0x400) && (!activePlayer || !CGBarberShop::m_barberShopEnabled)) {
                VisibleItem_C* head = &player->m_player->m_playerVisibleItems[0];

                if (head && head->GetDisplayID() && player->CanEquipItemsInThisForm()) {
                    component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(0), head->GetDisplayID(), 0);
                }
            }
        }

        if (activePlayer) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// VERIFY: ae08522d manual
// OFFSET: 0x006E0340
static void InitWeaponSkills() {
    int32_t count;

    if (!GetWeaponSubClassCount(&count)) {
        return;
    }

    s_weaponSkills.SetCount(count);

    for (uint32_t i = 0; i < static_cast<uint32_t>(count); i++) {
        s_weaponSkills.m_data[i] = 0;
    }

    s_unk_C9D33C = 0;

    SpellRec rec;
    int32_t index = g_spellDB.m_numRecords;

    while (index--) {
        if (!g_spellDB.GetRecordByIndex(index, &rec)) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        if (!(rec.m_attributes & 0x40)) {
            continue;
        }

        if (!s_unk_C9D33C && rec.m_effect[0] == 26) {
            s_unk_C9D33C = rec.m_id;
        }

        if (rec.m_equippedItemClass != 2) {
            continue;
        }

        int32_t mask = rec.m_equippedItemSubclass;
        int32_t singleBit = 1;

        if (!mask || ((mask - 1) & mask)) {
            singleBit = 0;
        }

        int32_t subClass = 0x7FFFFFFF;

        for (int32_t bit = 0; static_cast<uint32_t>(bit) < 32; bit++) {
            if (((1 << bit) & mask) && bit < subClass) {
                subClass = bit;
            }
        }

        if (!mask) {
            subClass = -1;
        }

        if (singleBit && subClass < count) {
            s_weaponSkills.m_data[subClass] = rec.m_id;
        }
    }
}

// VERIFY: 07dc9718 manual
// OFFSET: 0x006E1B40
static int32_t GuildUpdateHandler(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(object)->m_player->m_playerGuildid == reinterpret_cast<uint32_t>(param)) {
        static_cast<CGPlayer_C*>(object)->OnGuildChanged(0);
    }

    return 1;
}

// VERIFY: 4f0aa2b8 auto
// OFFSET: 0x006E1B90
static void GuildTimestampChanged(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        ClntObjMgrEnumVisibleObjects(GuildUpdateHandler, reinterpret_cast<void*>(id));
    }
}

// VERIFY: 5687d777 manual
// OFFSET: 0x006E1BB0
static int32_t GuildIDUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->OnGuildChanged(1);

        if (guid == ClntObjMgrGetActivePlayer()) {
            CGChat::UpdateGuildStatus();
            CGGuildInfo::QueryPermissions();

            if (CGMerchantInfo::m_merchant) {
                FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, nullptr);
            }

            CGGuildBankInfo::ClearAllTabs();
        }
    }

    return 1;
}

// VERIFY: cdac6d5b manual
// OFFSET: 0x006E1C20
static int32_t OnUpdatePlayerFlags(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TogglePlayerFlag(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// VERIFY: 3504720c manual
// OFFSET: 0x006E1C60
static int32_t OnUpdateGuildTimeStamp(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t timestamp = player->m_player->m_playerGuildTimestamp;
    uint32_t guildId = player->m_player->m_playerGuildid;
    int32_t index = -1;

    for (uint32_t i = 0; i < s_guildUpdateTimestamps.m_count; i++) {
        if (s_guildUpdateTimestamps.m_data[i].m_guildId == guildId) {
            if (s_guildUpdateTimestamps.m_data[i].m_timestamp == timestamp) {
                return 1;
            }

            index = i;
            break;
        }
    }

    if (index == -1) {
        index = s_guildUpdateTimestamps.m_count;
        s_guildUpdateTimestamps.SetCount(index + 1);
        s_guildUpdateTimestamps.m_data[index].m_guildId = guildId;
    }

    s_guildUpdateTimestamps.m_data[index].m_timestamp = timestamp;
    g_guildInfoCache.Invalidate(guildId);
    g_guildInfoCache.GetRecord(guildId, guid, GuildTimestampChanged, nullptr, false);

    return 1;
}

// VERIFY: bdd9f9e6 manual
// OFFSET: 0x006E4580
static int32_t OnUpdateInventoryComponent(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateInventoryComponent(static_cast<INVENTORY_SLOTS>((fieldOffset - offsetof(CGPlayerData, m_playerVisibleItems)) / sizeof(VisibleItem_C)), static_cast<const VisibleItem_C*>(oldValue));
    }

    return 1;
}

// VERIFY: e141464d auto
// OFFSET: 0x006E45D0
void CGPlayer_C::Initialize() {
    CGPlayer_C::s_lastVendorListReceived = WOWGUID();
    s_giftWrapItem = WOWGUID();
    s_bindPointSet = 0;
    s_unk_C9EAB0 = 0;
    s_unk_C9EAB4 = 0;
    CGPlayer_C::s_lastUpdatePVPGroup = WOWGUID();
    CGPlayer_C::s_lastUpdatePVPSequence = 0;
    CGPlayer_C::s_lastUpdatePVPTime = 0;
    CGPlayer_C::s_lastUpdateGroup = WOWGUID();
    CGPlayer_C::s_lastUpdateSequence = 0;
    CGPlayer_C::s_lastUpdateTime = 0;
    InitWeaponSkills();
    s_attackSwingError = 0;
    s_attackSwingErrorTime = FrameTime::s_curTimeMs;

    for (uint32_t offset = offsetof(CGPlayerData, m_playerVisibleItems); offset <= offsetof(CGPlayerData, m_playerVisibleItems) + sizeof(CGPlayerData::m_playerVisibleItems) - sizeof(VisibleItem_C); offset += sizeof(VisibleItem_C)) {
        ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offset, sizeof(VisibleItem_C), OnUpdateInventoryComponent, nullptr, static_cast<HANDLER_PRIORITY>(1), 0);
    }

    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldCharm), 2 * sizeof(WOWGUID), PetChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildid), sizeof(uint32_t), GuildIDUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildrank), sizeof(uint32_t), GuildRankUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerDuelTeam), sizeof(uint32_t), DuelTeamUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerFlags), sizeof(uint32_t), OnUpdatePlayerFlags, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildTimestamp), sizeof(uint32_t), OnUpdateGuildTimeStamp, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes3) + 2, sizeof(uint8_t), OnUpdatePVPTitle, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes3) + 1, sizeof(uint8_t), OnUpdateInebriation, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerChosenTitle), sizeof(uint32_t), OnUpdatePVPTitle, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes) + 3, sizeof(uint8_t), OnUpdatePlayerHairColor, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes) + 2, sizeof(uint8_t), OnUpdatePlayerHairStyle, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes2), sizeof(uint8_t), OnUpdatePlayerFacialStyle, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes), sizeof(uint8_t), OnUpdatePlayerSkinID, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: b276ec2e manual
// OFFSET: 0x006E4780
void CGPlayer_C::Shutdown() {
    s_guildUpdateTimestamps.Clear();
    s_initialSpells.Clear();
    s_itemSwaps.Clear();
    s_weaponSkills.Clear();
    s_unk_C9EAA0 = WOWGUID();
    s_tabardSavePending = 0;

    if (s_inspectData) {
        delete s_inspectData;
        s_inspectData = nullptr;
    }

    for (uint32_t offset = offsetof(CGPlayerData, m_playerVisibleItems); offset <= offsetof(CGPlayerData, m_playerVisibleItems) + sizeof(CGPlayerData::m_playerVisibleItems) - sizeof(VisibleItem_C); offset += sizeof(VisibleItem_C)) {
        ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offset, OnUpdateInventoryComponent, nullptr);
    }

    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldCharm), PetChangeHandler, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildid), GuildIDUpdateHandler, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildrank), GuildRankUpdateHandler, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerDuelTeam), DuelTeamUpdateHandler, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerFlags), OnUpdatePlayerFlags, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerGuildTimestamp), OnUpdateGuildTimeStamp, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes3) + 2, OnUpdatePVPTitle, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes3) + 1, OnUpdateInebriation, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerChosenTitle), OnUpdatePVPTitle, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes) + 3, OnUpdatePlayerHairColor, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes) + 2, OnUpdatePlayerHairStyle, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, offsetof(CGPlayerData, m_playerBytes2), OnUpdatePlayerFacialStyle, nullptr);
}

// VERIFY: 78b02f45 auto
// OFFSET: 0x006E8EE0
void PlayerClientInitialize() {
    CGPlayer_C::PlayerInitializeMessageHandlers();

    for (uint32_t i = 0; i < 17; i++) {
        s_proficiency[i] = 0;
    }

    CGPlayer_C::s_resurrectOfferer = WOWGUID{ 0 };
    CGPlayer_C::s_resurrectOfferSickness = 0;
    CGPlayer_C::s_resurrectOfferUseTimer = 0;
    s_playerRenderDisabled = 1;
    PlayerInitializeSounds();
    AreaTriggersInitialize(ClntObjMgrGetMapID());
}

// VERIFY: f3fc01f5 auto
// OFFSET: 0x005EEB70
void PlayerInitializeSounds() {
}

// VERIFY: 9cc59dbb auto
// OFFSET: 0x006D5080
void CGPlayer_C::UpdateQuestStatus(CGObject_C* object) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID guid = object->m_obj->m_guid;
    CGPlayer_C::QueryQuestGiverStatus(guid);
}

// VERIFY: c64646b5 manual
// OFFSET: 0x006D5130
void CGPlayer_C::UpdateTaxiStatus(CGUnit_C* unit) {
    if (unit->UnitReaction(this) <= 1 || !(unit->m_unit->m_unitNpcFlags & 0x2000)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TAXI_NODE_STATUS_QUERY));
    msg.Put(unit->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: ed82bd8b manual
// OFFSET: 0x006D2950
void CGPlayer_C::HandleRepopRequest(bool a2) {
    if (!this->IsDeadOrGhost() || !this->CheckSelfResurrect()) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REPOP_REQUEST));
    msg.Put(static_cast<uint8_t>(a2 != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 63c9a7c3 manual
// OFFSET: 0x006DCB40
void CGPlayer_C::ChangeStandState(uint32_t standState) {
    if (standState >= 11) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000)) {
        return;
    }

    CGUnitData* unit = this->m_unit;

    if ((unit->m_unitFieldFlags & 0x100000) || unit->m_unitFieldHealth <= 0 || (unit->m_unitFieldFlags2 & 0x1) || this->IsInVehicle() || (this->m_animationState & 0x400) || this->m_castSpellId || !this->IsLocalClientControlled()) {
        return;
    }

    if (this->m_unit->m_unitChannelSpell) {
        SpellRec spellRec;
        g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell, &spellRec);

        if (!(spellRec.m_attributes & 0x8000000)) {
            return;
        }
    }

    uint32_t moveFlags = this->m_movement->m_flags;

    if ((moveFlags & 0x30) && (standState == 3 || standState == 1)) {
        return;
    }

    if (standState && (moveFlags & 0x2E0100F)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_STAND_STATE_CHANGE));
    msg.Put(standState);
    msg.Finalize();
    ClientServices::Send2(&msg);
    this->UpdateClientStandState(standState);
}

// VERIFY: b464ce44 auto
// OFFSET: 0x006E2B30
void CGPlayer_C::SetClientStandState(uint32_t standState) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (this->m_clientStandState == standState) {
        return;
    }

    this->m_clientStandState = standState;

    if (!standState) {
        CGInputControl::GetActive()->UpdatePlayer(OsGetAsyncTimeMs(), 1);
    } else {
        if (this->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }

        if (this->CGUnit_C::IsInMelee()) {
            this->SetCombatMode();
        }

        if (standState >= 4 && standState <= 6) {
            return;
        }
    }

    if (CGBarberShop::m_barberShopEnabled) {
        CGBarberShop::DisableBarberShop();
    }
}

// VERIFY: c0e35919 manual
// OFFSET: 0x006D5EB0
void CGPlayer_C::PreAnimate(CGWorldFrame* frame) {
    if (!frame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    this->CGUnit_C::PreAnimate(frame);

    uint32_t time = FrameTime::s_curTimeMs;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && this->m_combat.m_attacking.m_guid && static_cast<int32_t>(time - s_attackSwingErrorTime) >= 0) {
        if (s_attackSwingError == 1) {
            s_attackSwingErrorTime = time + 4000;
            CGGameUI::DisplayError(ERR_BADATTACKPOS);
        } else if (s_attackSwingError == 2) {
            s_attackSwingErrorTime = time + 4000;
            CGGameUI::DisplayError(ERR_BADATTACKFACING);
        }
    }
}

// VERIFY: a6c965e0 manual
// OFFSET: 0x006E0840
void CGPlayer_C::ShouldRender(uint32_t flags, int32_t& culled, int32_t& hidden) {
    this->CGUnit_C::ShouldRender(flags, culled, hidden);

    if (this->IsCommentatorUberOrInArena() || ((this->m_clientFlags & 0x400000) && this->unk_18B8)) {
        hidden = 1;
    } else if (!s_playerRenderDisabled && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        hidden = 1;
    }
}

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

// VERIFY: e9fe3eb9 manual
// OFFSET: 0x006D7AA0
bool CGPlayer_C::VehicleAllowsInteraction() const {
    if (!this->HasVehicleTransport()) {
        return true;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_movementData.m_transportGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport) {
        return true;
    }

    if (!transport->m_vehicle) {
        return false;
    }

    VehicleSeatRec* seat = transport->m_vehicle->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return seat && (seat->m_flags & 0x80000000);
}

// VERIFY: eb153e9f auto
// OFFSET: 0x006CDE50
void CGPlayer_C::SetAutoLoot(int32_t autoLoot) {
    this->m_autoLoot = autoLoot != 0;

    if (autoLoot && this->m_lootTarget.m_guid) {
        CGLootInfo::DoAutoLoot(0);
    }
}

// VERIFY: 21ce503c manual
// OFFSET: 0x00729530
bool CGPlayer_C::CanInteract(const CGUnit_C* unit) const {
    bool isPlayer = (this->m_obj->m_type & TYPEMASK_PLAYER) != 0;

    if (isPlayer && (this->m_player->m_playerFlags & 0x10) && (!unit->m_creatureCacheEntry || !(unit->m_creatureCacheEntry->m_typeFlags & 0x2))) {
        return false;
    }

    CGUnitData* data = unit->m_unit;
    uint32_t npcFlags = data->m_unitNpcFlags;

    if ((npcFlags & 0x1) && isPlayer && !(unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->m_creatureCacheEntry && (unit->m_creatureCacheEntry->m_typeFlags & 0x800000) && data->m_unitFieldCreatedby != this->m_obj->m_guid) {
        return false;
    }

    return !(data->m_unitFieldFlags & 0x2000000) && npcFlags && ((unit->UnitReaction(this) >= 3 && this->UnitReaction(unit) >= 3) || (unit->m_unit->m_unitFieldFlags2 & 0x4000));
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

// VERIFY: fc18c204 auto
// OFFSET: 0x006CBCF0
int32_t OnSummonCancel(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, nullptr);

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

// VERIFY: dfb6fe0a manual
// OFFSET: 0x006CEB00
bool CGPlayer_C::CanDualWield() {
    return s_dualWieldSpell != 0;
}

// VERIFY: 3a070edb manual
// OFFSET: 0x006D5A60
int32_t CGPlayer_C::CanLoot(const CGObject_C* object) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    bool autoInteract = this->CanAutoInteract();
    float rangeSq = 0.0f;
    bool checkRange = !autoInteract;

    if (checkRange) {
        float range;

        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            range = this->GetMeleeRange(static_cast<const CGUnit_C*>(object));
        } else {
            range = 5.0f;
        }

        rangeSq = range * range;
    }

    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (form && (form->m_flags & 0x100)) {
            return 0;
        }
    }

    C3Vector objectPosition = object->GetPosition();
    C3Vector position = this->GetPosition();

    if (checkRange) {
        float dx = position.x - objectPosition.x;
        float dy = position.y - objectPosition.y;
        float dz = position.z - objectPosition.z;

        if (!(dx * dx + dz * dz + dy * dy <= rangeSq)) {
            return 0;
        }
    }

    if (this->m_lootTarget == object->m_obj->m_guid || this->m_lootTarget) {
        return 0;
    }

    return !this->IsDeadOrGhost() && !(this->m_movement->m_flags & 0x1000) && !(this->m_unit->m_unitFieldFlags & 0x40000) && this->IsLocalClientControlled();
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

// VERIFY: f0168031 auto
// OFFSET: 0x006CEF20
void __stdcall SignalPlayerAttackEvent(int32_t a1) {
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
}

// VERIFY: 1b4218d9 auto
// OFFSET: 0x006CEF40
void SignalPlayerSkillStatEvents() {
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_DEFENSE, "%s", "player");
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_RANGED_ATTACK_POWER, "%s", "player");
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

// VERIFY: 14d81fae manual
// OFFSET: 0x006D1CB0
int32_t SignalPetSpellPowerUpdate(WOWGUID guid) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_SPELL_POWER_UPDATE, nullptr);
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

// VERIFY: 43e0888c manual
// OFFSET: 0x006DAC10
bool CGPlayer_C::IsDeadOrGhost() const {
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

// VERIFY: d4574fd6 auto
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

        if (item && item->m_classID == spell->m_equippedItemClass && ((1 << item->m_subclassID) & spell->m_equippedItemSubclass) && s_weaponSkills.m_data[item->m_subclassID]) {
            return s_weaponSkills.m_data[item->m_subclassID];
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

// VERIFY: f7a9f1f8 manual
// OFFSET: 0x006DA3C0
int32_t OnHonorCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerFieldHonorCurrency < *static_cast<const uint32_t*>(value)) {
        SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_HONOR_CURRENCY_UPDATE, nullptr);
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, nullptr);

    return 1;
}

// VERIFY: d617a5c3 manual
// OFFSET: 0x006DA430
int32_t OnArenaCurrencyChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerFieldArenaCurrency < *static_cast<const uint32_t*>(value)) {
        SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_HONOR_CURRENCY_UPDATE, nullptr);
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, nullptr);

    return 1;
}

// VERIFY: fa2916ef manual
// OFFSET: 0x006DAA80
int32_t OnUpdatePlayerGlyph(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (offset - offsetof(CGPlayerLocalData, m_playerFieldGlyphs1)) / sizeof(uint32_t);

    if (value && g_glyphPropertiesDB.GetRecord(*static_cast<const int32_t*>(value))) {
        if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
            FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_UPDATED, "%d", slot + 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_REMOVED, "%d", slot + 1);
        }
    } else if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_ADDED, "%d", slot + 1);
    }

    return 1;
}

// VERIFY: 22efc675 manual
// OFFSET: 0x006DAB80
int32_t OnUpdatePlayerGlyphSlotsEnabled(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t enabled = *static_cast<const uint32_t*>(value);

        for (uint32_t i = 0; i < 6; i++) {
            uint32_t bit = 1 << i;

            if (player->m_player->m_playerGlyphsEnabled & bit) {
                if (!(bit & enabled)) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_ENABLED, "%d", i + 1);
                }
            } else if (bit & enabled) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_DISABLED, "%d", i + 1);
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

// VERIFY: f4bf6af2 manual
// OFFSET: 0x006E6B40
CGPlayer_C::CGPlayer_C(uint32_t time, const CClientObjCreate& create)
    : CGUnit_C(time, create),
      unk_1854(-1),
      m_combatModeFlags(0),
      unk_18B8(0),
      m_companionSpells(),
      m_lootTarget(),
      m_autoLoot(0) {
    CGPlayerLocalData* player = this->m_player;
    bool activePlayer = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
    this->m_bag.m_guid = this->m_obj->m_guid;
    this->m_bag.m_hasBank = 1;
    this->m_bag.m_numSlots = activePlayer ? 150 : 0;
    this->m_bag.m_slots = reinterpret_cast<WOWGUID*>(player->m_playerFieldInvSlotHead);
    this->m_combatTarget.m_guid = 0;

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

    this->m_combatModeTime = FrameTime::s_curTimeMs - 500;
    this->m_pvpCooldownTimestamp = 0;
    this->m_gemCounts[0] = 0;
    this->m_gemCounts[1] = 0;
    this->m_gemCounts[2] = 0;
    this->m_gemCounts[3] = 0;
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

// VERIFY: 46563e0e auto
// OFFSET: 0x006CF3A0
static float ClampPitch(float pitch) {
    if (pitch > 1.5707964f) {
        return 1.5707964f;
    }

    if (pitch < -1.5707964f) {
        return -1.5707964f;
    }

    return pitch;
}

// VERIFY: 83512664 manual
// OFFSET: 0x006CF3D0
static float GetDrunkTurnOffset(uint32_t time, float drunkness) {
    if (drunkness == 0.0f) {
        return 0.0f;
    }

    float phase = time * 0.017453292f * drunkness;
    return (cosf(0.154f * phase) + cosf(phase * 0.195f) + cosf(phase * 0.093f)) * 0.333333f * (drunkness * 0.01308997f);
}

// VERIFY: 192067b6 auto
// OFFSET: 0x006DEB30
void CGPlayer_C::UpdateActiveMoverDisplay(uint32_t time) {
    float offset = 0.0f;
    float drunkness = this->GetDrunknessTurnValue();

    if (drunkness > 0.0f) {
        offset = GetDrunkTurnOffset(time, drunkness);
    }

    if (this->IsAutoTracking()) {
        this->HandleTracking(drunkness, offset);
        return;
    }

    if (drunkness <= 0.0f || !(this->m_movement->m_flags & 0xF)) {
        return;
    }

    if (!CGInputControl::GetActive()->CanTurn(this)) {
        return;
    }

    if (!(this->m_movement->m_flags & 0x30) || CGInputControl::GetActive()->CameraCanTurnPlayer()) {
        this->OnSetRawFacingLocal(time, CMath::normalizeangle0to2pi_(this->GetRawFacing() + offset));
    }

    if ((this->m_movement->m_flags & 0x2200000) || (this->m_movement->m_flags2 & 0x20)) {
        if (CGInputControl::GetActive()->CameraCanTurnPlayer() || !(this->m_movementData.m_flags & 0xC0)) {
            this->OnSetPitchLocal(time, ClampPitch(this->GetPitch() + offset * 4.0f));
        }
    }
}

// VERIFY: 1d09cacb manual
// OFFSET: 0x006D5290
static int32_t TrackingMaskUpdateProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        static_cast<CGGameObject_C*>(object)->CheckForPassiveHighlight();
    }

    return 1;
}

// VERIFY: 93a46999 manual
// OFFSET: 0x006D7030
static int32_t AuraVisionUpdateHandler(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->AuraVisionUpdated(*static_cast<uint8_t*>(param));
    }

    return 1;
}

// VERIFY: ef96589a manual
// OFFSET: 0x006E7F50
void CGPlayer_C::PostInitActivePlayer() {
    this->m_movementData.m_flags |= 0x200;
    this->SetActiveMirrorHandlers();
    SI2::InitVocalErrorSounds(this->m_unit->m_unitFieldBytes0[0], this->m_player->m_playerBytes3[0]);
    CGSpellBook::ClearSpells();

    for (uint32_t i = 0; i < s_initialSpells.Count(); i++) {
        this->AddKnownSpell(s_initialSpells[i].m_spellId, static_cast<int16_t>(s_initialSpells[i].unk_0004), 0, 1);
    }

    CGSpellBook::UpdateSpells(1, 0, 1);
    CGSpellBook::UpdateCompanions();
    CGClassTrainer::RefreshList();

    if (!ClntObjMgrGetPlayerType()) {
        for (uint32_t i = 0; i < sizeof(s_actionButtons) / sizeof(s_actionButtons[0]); i++) {
            if ((s_actionButtons[i] & 0xF0000000) || this->IsSpellKnown(s_actionButtons[i])) {
                CGActionBar::SetAction(i, s_actionButtons[i], 0, 1);
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", 0);
    }

    if (this->IsShapeShifted()) {
        CGSpellBook::UpdateUsable();
        CGSpellBook::UpdateSelection();
        CGActionBar::UpdateShapeShiftBar();
        CGActionBar::UpdateBonusBar();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, nullptr);
    }

    uint8_t auraVision = 0xFF;
    CGWorldFrame::UpdateScreenEffect();
    ClntObjMgrEnumVisibleObjects(AuraVisionUpdateHandler, &auraVision);
    CGWorldFrame::GetActiveCamera()->SetTelescope(0);

    if (!ClntObjMgrGetPlayerType()) {
        this->SetLocalClientControl(true);

        WOWGUID farsight = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerFarsight : WOWGUID();
        WOWGUID charm = this->m_unit->m_unitFieldCharm;
        CGUnit_C* charmed = nullptr;
        WOWGUID controlled;

        if (charm) {
            charmed = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(charm, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (charmed && (charmed->m_unit->m_unitFieldFlags & 0x1000000) && farsight == charm) {
            charmed->SetLocalClientControl(true);
            controlled = charm;
        } else {
            controlled = this->m_obj->m_guid;
        }

        CGGameUI::InitClientControlState(controlled);

        if (controlled != this->m_obj->m_guid) {
            this->ToggleFarSight(charmed);
        }

        CGGameUI::EnterWorld();
        CGGameUI::UpdateActivePlayer();
    }

    ClientServices::Connection()->CharacterSetInGame(1);

    if (s_pendingMovie) {
        CGGameUI::StartCinematic(s_pendingMovie);
        s_pendingMovie = 0;
    } else if (this->GetTransportGUID()) {
        LoadingScreenSetTransparent(true);
    } else {
        LoadingScreenDisable();
    }

    Spell_C_SetPlayerClass(this->m_unit->m_unitFieldBytes0[1]);
    this->CountEquippedGems();

    if (this->m_player->m_playerFlags & 0x200) {
        this->m_pvpCooldownTimestamp = 0;
    } else if (!(this->m_player->m_playerFlags & 0x40000)) {
        this->m_pvpCooldownTimestamp = FrameTime::s_curTimeMs + 300000;
    }

    ClntObjMgrEnumVisibleObjects(TrackingMaskUpdateProc, nullptr);
    CGCommentator::PostInit(this);

    if (SFile::IsStreamingMode() && this->GetTransportGUID()) {
        LoadingScreenDisable();
        World::Preload(this->GetPosition(), 1);
    }
}


// VERIFY: 87cc1c54 auto
// OFFSET: 0x006CEF80
void CGPlayer_C::CancelGiftWrap() {
    CGGameUI::UnlockItem(s_giftWrapItem);
    s_giftWrapItem = WOWGUID();
    CursorSetResetMode(1);
    CursorSetMode(1);
}

// VERIFY: e37e66de manual
// OFFSET: 0x006DD060
CGUnit_C* CGPlayer_C::GetPossessedUnit() {
    if (!(this->m_combatModeFlags & 0x1)) {
        return nullptr;
    }

    WOWGUID farsight;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        farsight = this->m_player->m_playerFarsight;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(farsight, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return nullptr;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (!unit->IsPossessedBy(guid)) {
        return nullptr;
    }

    return unit;
}

// VERIFY: 3e8a1a90 manual
// OFFSET: 0x006E1660
void CGPlayer_C::SetCombatMode() {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && (this->CGUnit_C::IsInMelee() || this->m_combat.m_attackSent)) {
        this->StopAttack();
        Spell_C_CancelMeleeSpells();
    }

    if (Spell_C_GetAutoRangedCombatSpell()) {
        Spell_C_SetAutoRangedCombatSpell(0);
        int32_t autoRangedSpell = s_cvAutoRangedCombat->m_intValue ? CGSpellBook::m_autoRangedSpell : 0;

        if (Spell_C_GetAutoRepeatingSpell() == autoRangedSpell) {
            Spell_C_CancelAutoRepeat(true);
        }
    }

    Spell_C_ClearLastAutoRangedCombatError();
    Spell_C_SetAutoRangedCombatSpell(0);
    EventUnregister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags &= ~0xEu;
    CGGameUI::UpdateCombatMode();

    if (static_cast<int32_t>(this->m_unit->m_unitFieldLevel) < 2) {
        return;
    }

    if (CGTutorial::m_tutorialFlags.m_count && (reinterpret_cast<uint8_t*>(CGTutorial::m_tutorialFlags.m_bits.m_data)[7] & 0x1)) {
        return;
    }

    uint32_t count = ++s_combatModeExitCount;

    if (CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[1] & 0x800000) && count > 4) {
        if (IsTutorialFlagged(5) && count > 10) {
            CGTutorial::TriggerTutorial(56);
        }

        CGTutorial::TriggerTutorial(5);
    }

    CGTutorial::TriggerTutorial(55);
}

// VERIFY: b85de264 auto
// OFFSET: 0x006CBC50
int32_t Packet_SMSG_QUEST_CONFIRM_ACCEPT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId == SMSG_QUEST_CONFIRM_ACCEPT) {
        uint32_t questId;
        char title[1024];
        WOWGUID guid;
        msg->Get(questId);
        msg->GetString(title, sizeof(title));
        msg->Get(guid.m_guid);
        CGQuestInfo::ConfirmAcceptQuest(questId, title, &guid);
    }

    return 1;
}

// VERIFY: 36fc807e auto
// OFFSET: 0x006CBCB0
int32_t Packet_SMSG_QUEST_POI_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId != SMSG_QUEST_POI_QUERY_RESPONSE) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGQuestLog::HandlePOIQueryResponse(msg);
    return 1;
}

// VERIFY: 445c4b27 auto
// OFFSET: 0x006CBD10
int32_t Packet_SMSG_GROUP_CANCEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    CGGameUI::CancelPartyInvite();
    return 1;
}

// VERIFY: 7c272313 auto
// OFFSET: 0x006CBD40
int32_t Packet_SMSG_GROUP_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    CGGameUI::DisplayError(ERR_DECLINE_GROUP_S, name);
    return 1;
}

// VERIFY: 19b171e9 auto
// OFFSET: 0x006CBE10
int32_t Packet_SMSG_GROUP_UNINVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPartyInfo::RemoveAll(1);
    CGGameUI::DisplayError(ERR_UNINVITE_YOU);
    CGLookingForGroup::ClearPartyInfo();
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, nullptr);
    return 1;
}

// VERIFY: 83ee7b00 auto
// OFFSET: 0x006CBE40
int32_t Packet_SMSG_GROUP_DESTROYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (CGPartyInfo::GetNumPartyMembers()) {
        CGGameUI::DisplayError(ERR_GROUP_DISBANDED);
    }

    CGPartyInfo::RemoveAll(1);
    CGLookingForGroup::ClearPartyInfo();
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, nullptr);
    return 1;
}

// VERIFY: 6ce7a822 auto
// OFFSET: 0x006CBE80
int32_t Packet_SMSG_GROUPACTION_THROTTLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_GROUP_ACTION_THROTTLED);
    return 1;
}

// VERIFY: 9e39a3ab auto
// OFFSET: 0x006CBEA0
int32_t Packet_SMSG_BATTLEGROUND_INFO_THROTTLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_BATTLEGROUND_INFO_THROTTLED);
    return 1;
}

// VERIFY: 1cf32762 auto
// OFFSET: 0x006CD770
int32_t Packet_SMSG_TALENTS_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t pet = 0;
    msg->Get(pet);

    if (pet) {
        CGTalentInfo::UpdatePetTalentData(msg);
    } else {
        CGTalentInfo::UpdateTalentData(msg);
    }

    return 1;
}

// VERIFY: 8c7e06a6 auto
// OFFSET: 0x006CDF00
int32_t Packet_SMSG_PLAYER_SKINNED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t freeRepop;
    msg->Get(freeRepop);
    CGGameUI::OpenPlayerSkinned(freeRepop != 0);
    return 1;
}

// VERIFY: 3b2739cc auto
// OFFSET: 0x006CE260
int32_t Packet_SMSG_SPELLINSTAKILLLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID casterGuid;
    WOWGUID targetGuid;
    msg->Get(casterGuid.m_guid);
    msg->Get(targetGuid.m_guid);
    uint32_t spellId = 0;
    msg->Get(spellId);
    UnitCombatLogInstakill(casterGuid, targetGuid, spellId);
    return 1;
}

// VERIFY: 1f59b848 auto
// OFFSET: 0x006CE3B0
int32_t Packet_Group_19(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellRemoveLog(msgId, msg, 0);
}

// VERIFY: 0e8f27d4 auto
// OFFSET: 0x006CE3D0
int32_t Packet_SMSG_SPELLORDAMAGE_IMMUNE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID casterGuid = WOWGUID{ value };
    value = 0;
    msg->Get(value);
    WOWGUID targetGuid = WOWGUID{ value };
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint8_t debug = 0;
    msg->Get(debug);
    bool isDebug = debug != 0;
    UnitCombatLogSpellMissed(casterGuid, targetGuid, spellId, 7, !isDebug, isDebug, 0);
    return 1;
}

// VERIFY: 62c887fa auto
// OFFSET: 0x006CE470
int32_t Packet_SMSG_RESURRECT_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::UpdateActivePlayer();
    return 1;
}

// VERIFY: c9571cc5 manual
// OFFSET: 0x006CFE70
int32_t Packet_SMSG_EMOTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t emote;
    WOWGUID guid;
    msg->Get(emote);
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_unit->m_unitFieldBytes1[0] != 3 && !(unit->m_movement->m_flags & 0x200000)) {
        unit->PlayEmoteAnimation(emote);
    }

    return 1;
}

// VERIFY: d53bbf7b auto
// OFFSET: 0x006D3200
int32_t Packet_SMSG_SPELLLOGMISS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogMiss(msg, 0);
}

// VERIFY: 3b3f5c71 auto
// OFFSET: 0x006D3730
int32_t Packet_SMSG_SPELLLOGEXECUTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogExecute(msg, 0);
}

// VERIFY: 2a0b9a11 manual
// OFFSET: 0x006D3750
int32_t Packet_SMSG_SPELLDAMAGESHIELD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID victimGuid = WOWGUID{ value };
    value = 0;
    msg->Get(value);
    WOWGUID casterGuid = WOWGUID{ value };
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint32_t damage = 0;
    msg->Get(damage);
    uint32_t overkill = 0;
    msg->Get(overkill);
    uint32_t schoolMask = 0;
    msg->Get(schoolMask);
    UnitCombatLogDamageShield(victimGuid, casterGuid, spellId, damage, overkill, schoolMask);
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (caster) {
        caster->AddWorldDamageText(victimGuid, damage, nullptr, false, false, true);
    }

    CGGameUI::ShowCombatFeedback(casterGuid, damage, schoolMask, 0);
    return 1;
}

// VERIFY: dd286bdb auto
// OFFSET: 0x006D3C10
int32_t Packet_SMSG_SPELLNONMELEEDAMAGELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellNonMeleeDamageLog(msg, 0);
}

// VERIFY: 759667b2 auto
// OFFSET: 0x006D3DD0
int32_t Packet_SMSG_SPELLHEALLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellHealLog(msg, 0);
}

// VERIFY: f2c3971c auto
// OFFSET: 0x006D3EF0
int32_t Packet_SMSG_SPELLENERGIZELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogEnergizeLog(msg, 0);
}

// VERIFY: 3850fbbd auto
// OFFSET: 0x006D4110
int32_t Packet_SMSG_COMBAT_LOG_MULTIPLE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t count;
    msg->Get(reinterpret_cast<uint32_t&>(count));
    uint32_t baseTime = 0;
    msg->Get(baseTime);

    for (int32_t i = 0; i < count; i++) {
        uint32_t entryTime = 0;
        msg->Get(entryTime);
        int32_t timeOffset = baseTime - entryTime;
        uint32_t type;
        msg->Get(type);

        switch (type) {
            case SMSG_SPELL_MISS_LOG:
                CombatLog::HandleSpellLogMiss(msg, timeOffset);
                break;
            case SMSG_SPELL_BREAK_LOG:
            case SMSG_SPELL_DISPELL_LOG:
            case SMSG_SPELL_STEAL_LOG:
                CombatLog::HandleSpellRemoveLog(static_cast<NETMESSAGE>(type), msg, timeOffset);
                break;
            case SMSG_SPELL_HEAL_LOG:
                CombatLog::HandleSpellHealLog(msg, timeOffset);
                break;
            case SMSG_SPELL_ENERGIZE_LOG:
                CombatLog::HandleSpellLogEnergizeLog(msg, timeOffset);
                break;
            case SMSG_SPELL_EXECUTE_LOG:
                CombatLog::HandleSpellLogExecute(msg, timeOffset);
                break;
            case SMSG_SPELL_PERIODIC_AURA_LOG:
                CombatLog::HandlePeriodicAuraLog(msg, timeOffset);
                break;
            case SMSG_SPELL_NON_MELEE_DAMAGE_LOG:
                CombatLog::HandleSpellNonMeleeDamageLog(msg, timeOffset);
                break;
        }
    }

    return 1;
}

// VERIFY: 4accacac manual
// OFFSET: 0x006D7F10
int32_t Packet_Group_12(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid || !ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    switch (msgId) {
        case SMSG_QUEST_GIVER_STATUS:
            return CGPlayer_C::OnQuestGiverStatus(msg);
        case SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE:
            return CGPlayer_C::OnQuestGiverQuestList(msg);
        case SMSG_QUEST_GIVER_QUEST_DETAILS:
            return CGPlayer_C::OnQuestGiverQuestDetails(msg);
        case SMSG_QUEST_GIVER_REQUEST_ITEMS:
            return CGPlayer_C::OnQuestGiverRequestItems(msg);
        case SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE:
            return CGPlayer_C::OnQuestGiverOfferReward(msg);
        case SMSG_QUEST_GIVER_INVALID_QUEST:
            return CGPlayer_C::OnQuestGiverQuestInvalid(msg);
        case SMSG_QUEST_GIVER_QUEST_COMPLETE:
            return CGPlayer_C::OnQuestGiverQuestComplete(msg);
        case SMSG_QUEST_GIVER_QUEST_FAILED:
            return CGPlayer_C::OnQuestGiverQuestFailed(msg);
        case SMSG_QUEST_LOG_FULL:
            CGGameUI::DisplayError(ERR_QUEST_LOG_FULL);
            return 1;
        case SMSG_QUEST_GIVER_STATUS_MULTIPLE:
            return CGPlayer_C::OnQuestGiverStatusMultiple(msg);
        default:
            return 0;
    }
}

// VERIFY: 7f42c5ae manual
// OFFSET: 0x006D8370
int32_t Packet_SMSG_QUEST_FORCE_REMOVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId != SMSG_QUEST_FORCE_REMOVED) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    uint32_t questId;
    msg->Get(questId);
    QuestCache* quest = g_questDBCache.GetRecord(questId, WOWGUID{ 0 }, nullptr, nullptr, false);

    if (quest) {
        CGGameUI::DisplayError(ERR_QUEST_FORCE_REMOVED_S, quest->m_name);
    }

    return 1;
}

// VERIFY: 4ed46ae3 manual
// OFFSET: 0x006D8410
int32_t Packet_Group_13(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid || !ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (msgId == SMSG_TRAINER_LIST) {
        return CGPlayer_C::OnTrainerList(msg);
    }

    if (msgId != SMSG_TRAINER_BUY_FAILED) {
        return 0;
    }

    WOWGUID trainer;
    uint32_t service;
    uint32_t reason;
    msg->Get(trainer.m_guid);
    msg->Get(service);
    msg->Get(reason);

    if (reason == 0) {
        ConsoleWriteA("Trainer service %d unavailable", DEFAULT_COLOR, service);
    } else if (reason == 1) {
        ConsoleWriteA("Not enough money for trainer service %d", DEFAULT_COLOR, service);
    } else if (reason == 2) {
        ConsoleWriteA("Not enough skill points for trainer service %d", DEFAULT_COLOR, service);
    }

    return 1;
}

// VERIFY: a7e484bb manual
// OFFSET: 0x006D84F0
int32_t PH_LOOT_OPCODES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    switch (msgId) {
        case SMSG_LOOT_RESPONSE:
            return player->OnLootResponse(time, msg);
        case SMSG_LOOT_RELEASE:
            return player->OnLootReleaseResponse(msg);
        case SMSG_LOOT_REMOVED:
            return player->OnLootRemoved(msg);
        case SMSG_LOOT_MONEY_NOTIFY:
            return CGPlayer_C::OnLootMoneyNotify(msg);
        case SMSG_LOOT_ITEM_NOTIFY:
            return CGPlayer_C::OnLootItemNotify(msg);
        case SMSG_LOOT_CLEAR_MONEY:
            CGPlayer_C::OnLootClearMoney();
            return 1;
        case SMSG_TEST_DROP_RATE_RESULT: {
            char text[1024];
            msg->GetString(text, sizeof(text));
            ConsoleWrite(text, DEFAULT_COLOR);
            return 1;
        }
        case SMSG_LOOT_ALL_PASSED:
            return LootRoll::AllPassed(msg);
        case SMSG_LOOT_ROLL_WON:
            return LootRoll::Won(msg);
        case SMSG_LOOT_START_ROLL:
            return LootRoll::Start(msg);
        case SMSG_LOOT_ROLL:
            return LootRoll::Roll(msg);
        case SMSG_LOOT_MASTER_LIST:
            return LootRoll::MasterLoot(msg);
        case SMSG_LOOT_SLOT_CHANGED:
            return player->OnLootSlotChanged(msg);
        default:
            return 0;
    }
}

// VERIFY: b3f1870a manual
// OFFSET: 0x006D8680
int32_t Packet_SMSG_SUMMON_REQUEST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID summoner;
    uint32_t area;
    uint32_t timeout;
    msg->Get(summoner.m_guid);
    msg->Get(area);
    msg->Get(timeout);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (!player->IsDeadOrGhost()) {
        CGGameUI::SetSummonRequest(summoner, area, timeout);
        return 1;
    }

    WDataStore response;
    response.Put(static_cast<uint32_t>(CMSG_SUMMON_RESPONSE));
    response.Put(summoner.m_guid);
    response.Put(static_cast<uint8_t>(0));
    response.Finalize();
    ClientServices::Send2(&response);
    return 0;
}

// VERIFY: 5a3c9684 manual
// OFFSET: 0x006D8750
int32_t Packet_SMSG_ACTION_BUTTONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t state;
    msg->Get(state);
    uint32_t mode = state;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (mode == 2) {
        if (player) {
            player->unk_1954 = 1;
        }

        return 1;
    }

    if (mode > 1) {
        return 1;
    }

    for (uint32_t i = 0; i < sizeof(s_actionButtons) / sizeof(s_actionButtons[0]); i++) {
        msg->Get(reinterpret_cast<uint32_t&>(s_actionButtons[i]));
    }

    if (mode == 1 && player) {
        player->unk_1954 = 0;

        for (uint32_t i = 0; i < sizeof(s_actionButtons) / sizeof(s_actionButtons[0]); i++) {
            int32_t action = s_actionButtons[i];

            if (((action & 0xF0000000) || player->IsSpellKnown(action)) && ((action & 0xF0000000) != 0x20000000 || CGActionBar::IsEquipmentSetValid(action & 0xDFFFFFFF))) {
                CGActionBar::SetAction(i, action, 0, 1);
            } else {
                CGActionBar::RemoveAction(i);
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", 0);
    }

    return 1;
}

// VERIFY: b8ad26b7 manual
// OFFSET: 0x006DBDF0
int32_t Packet_Group_14(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::ReadItemResult(msgId, msg);
    }

    return 1;
}

// VERIFY: fe6d19d8 manual
// OFFSET: 0x006DC010
int32_t Packet_SMSG_TIME_SYNC_REQ(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t counter = 0;
    msg->Get(counter);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->m_movementData.OnTimeSyncRequest(OsGetAsyncTimeMs(), counter);
    }

    return 1;
}

// VERIFY: 74f608d7 manual
// OFFSET: 0x006DEEF0
int32_t Packet_Group_15(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    switch (msgId) {
        case SMSG_PETITION_SHOW_LIST:
            return CGPlayer_C::OnPetitionShowList(msg);
        case SMSG_PETITION_SHOW_SIGNATURES:
            return CGPlayer_C::OnPetitionShowSignatures(msg);
        case SMSG_PETITION_SIGN_RESULTS:
            return player->OnSignedResults(msg);
        case MSG_PETITION_DECLINE:
            return CGPlayer_C::OnPetitionDecline(msg);
        case SMSG_TURN_IN_PETITION_RESULT:
            return CGPlayer_C::OnTurnInPetitionResults(msg);
        case MSG_PETITION_RENAME:
            return CGPlayer_C::OnPetitionRename(msg);
        default:
            return 0;
    }
}

// VERIFY: 9f33c465 manual
// OFFSET: 0x006DEFA0
int32_t Packet_Group_10(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    switch (msgId) {
        case SMSG_VENDOR_INVENTORY:
            return player->OnListInventory(msg);
        case SMSG_SELL_ITEM:
            return CGPlayer_C::OnSellResponse(msg);
        case SMSG_BUY_SUCCEEDED:
            return CGPlayer_C::OnBuySucceeded(msg);
        case SMSG_BUY_FAILED:
            return CGPlayer_C::OnBuyFailed(msg);
        case SMSG_ITEM_REFUND_INFO_RESPONSE:
            return CGPlayer_C::OnRefundInfo(msg);
        case SMSG_ITEM_PURCHASE_REFUND_RESULT:
            return player->OnItemRefundResult(msg);
        default:
            return 0;
    }
}

// VERIFY: 5322b599 manual
// OFFSET: 0x006DF050
int32_t Packet_SMSG_INITIAL_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t talentSpec;
    msg->Get(talentSpec);
    uint16_t count;
    msg->Get(count);
    s_initialSpells.SetCount(count);

    for (uint16_t i = 0; i < count; i++) {
        msg->Get(s_initialSpells[i].m_spellId);
        msg->Get(s_initialSpells[i].unk_0004);
    }

    msg->Get(count);

    for (int32_t i = 0; i < count; i++) {
        uint32_t spellId;
        uint16_t itemId;
        uint16_t category;
        uint32_t spellCooldown;
        uint32_t categoryCooldown;
        msg->Get(spellId);
        msg->Get(itemId);
        msg->Get(category);
        msg->Get(spellCooldown);
        msg->Get(categoryCooldown);
        bool onHold = (categoryCooldown >> 31) & 0x1;
        categoryCooldown &= 0x7FFFFFFF;
        Spell_C_SetCooldownLeft(spellId, itemId, category, spellCooldown, categoryCooldown, onHold, 0);
    }

    return 1;
}

// VERIFY: 8fa31e36 manual
// OFFSET: 0x006DF1A0
int32_t Packet_SMSG_GROUP_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t canAccept = 0;
    msg->Get(canAccept);
    char name[48];
    msg->GetString(name, sizeof(name));
    uint32_t dungeonType = 0;
    msg->Get(dungeonType);
    uint8_t count = 0;
    TSGrowableArray<uint32_t> dungeons;
    msg->Get(count);
    dungeons.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(dungeons[i]);
    }

    uint32_t unk = 0;
    msg->Get(unk);

    if (canAccept) {
        SignalEvent_EVENT_PARTY_INVITE_REQUEST(name);
        CGGameUI::DisplayError(ERR_INVITED_TO_GROUP_SS, name, name);
    } else {
        CGGameUI::DisplayError(ERR_INVITED_ALREADY_IN_GROUP_SS, name, name);
    }

    return 1;
}

// VERIFY: 5526bdf2 manual
// OFFSET: 0x006E2210
int32_t Packet_SMSG_CANCEL_COMBAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->SetCombatMode();
    }

    return 1;
}

// VERIFY: 8c7c4c56 manual
// OFFSET: 0x006E6330
int32_t Packet_Group_18(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!msg) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID itemGuid;
    msg->Get(itemGuid.m_guid);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    switch (msgId) {
        case SMSG_ITEM_TIME_UPDATE: {
            uint32_t seconds;
            msg->Get(seconds);

            if (item) {
                item->UpdateExpirationTime(seconds);
            }

            return 1;
        }
        case SMSG_ITEM_ENCHANT_TIME_UPDATE: {
            uint32_t slot;
            uint32_t seconds;
            WOWGUID playerGuid;
            msg->Get(slot);
            msg->Get(seconds);
            msg->Get(playerGuid.m_guid);

            if (item) {
                item->UpdateEnchantmentTime(slot, seconds);
                return 1;
            }

            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(playerGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player || (player = ClntObjMgrGetActivePlayerObj())) {
                player->QueueItemEnchantTime(itemGuid, slot, seconds);
            }

            return 1;
        }
        case SMSG_SOCKET_GEMS: {
            uint32_t a2 = 0;
            msg->Get(a2);
            uint32_t a3 = 0;
            msg->Get(a3);
            uint32_t a4 = 0;
            msg->Get(a4);
            uint32_t a5 = 0;
            msg->Get(a5);
            item->UpdateSockets(a2, a3, a4, a5);
            return 1;
        }
        default:
            return 0;
    }
}

// VERIFY: 33f6984f manual
// OFFSET: 0x006E7840
int32_t Packet_SMSG_REMOVED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t spellId;
    msg->Get(spellId);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->DelKnownSpell(spellId, 0);
        return 1;
    }

    for (uint32_t i = 0; i < s_initialSpells.Count(); i++) {
        if (s_initialSpells[i].m_spellId == spellId) {
            s_initialSpells[i].m_spellId = 0;
            s_initialSpells[i].unk_0004 = 0;
        }
    }

    return 1;
}

// VERIFY: 299341f9 manual
// OFFSET: 0x006E7D60
int32_t Packet_SMSG_LEARNED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t spellId;
    uint16_t slot;
    msg->Get(spellId);
    msg->Get(slot);

    if (SFile::IsStreamingMode() && !LoadingScreenDrawing()) {
        PrefetchSpellVisuals(spellId);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->AddKnownSpell(spellId, static_cast<int16_t>(slot), 1, 1);
    } else {
        InitialSpellStruct* spell = s_initialSpells.New();
        spell->m_spellId = spellId;
        spell->unk_0004 = slot;
    }

    return 1;
}

// VERIFY: 84e6a81b manual
// OFFSET: 0x006E7E00
int32_t Packet_SMSG_SUPERCEDED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t oldSpellId;
    uint32_t newSpellId;
    msg->Get(oldSpellId);
    msg->Get(newSpellId);

    if (SFile::IsStreamingMode() && !LoadingScreenDrawing()) {
        PrefetchSpellVisuals(newSpellId);
    }

    CGActionBar::ReplaceSpell(oldSpellId, newSpellId);
    CGSpellBook::ReplaceSpell(oldSpellId, newSpellId);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->DelKnownSpell(oldSpellId, 1);
        player->AddKnownSpell(newSpellId, 0, 0, 0);
        CGClassTrainer::RefreshList();
        return 1;
    }

    uint32_t i;

    for (i = 0; i < s_initialSpells.Count(); i++) {
        if (s_initialSpells[i].m_spellId == oldSpellId) {
            s_initialSpells[i].m_spellId = newSpellId;
            break;
        }
    }

    if (i == s_initialSpells.Count()) {
        InitialSpellStruct* spell = s_initialSpells.New();
        spell->m_spellId = newSpellId;
        spell->unk_0004 = 0;
    }

    CGClassTrainer::RefreshList();
    return 1;
}

int32_t Packet_SMSG_DEBUG_SERVER_GEO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return 1;
}

// VERIFY: 97769787 auto
// OFFSET: 0x006E83B0
void CGPlayer_C::PlayerInitializeMessageHandlers() {
    ClientServices::SetMessageHandler(SMSG_MOUNT_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_DISMOUNT_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_INVENTORY_CHANGE_FAILURE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_OPEN_CONTAINER, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_PUSH_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_VENDOR_INVENTORY, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_BUY_FAILED, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_BUY_SUCCEEDED, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_SELL_ITEM, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_REFUND_INFO_RESPONSE, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_PURCHASE_REFUND_RESULT, Packet_Group_10, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_RESPONSE, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_RELEASE, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_REMOVED, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_SLOT_CHANGED, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_MONEY_NOTIFY, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_ITEM_NOTIFY, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_CLEAR_MONEY, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LEARNED_SPELL, Packet_SMSG_LEARNED_SPELL, nullptr);
    ClientServices::SetMessageHandler(SMSG_UNLEARNED_SPELLS, Packet_SMSG_REMOVED_SPELL, nullptr);
    ClientServices::SetMessageHandler(SMSG_SUPERCEDED_SPELLS, Packet_SMSG_SUPERCEDED_SPELL, nullptr);
    ClientServices::SetMessageHandler(SMSG_SEND_KNOWN_SPELLS, Packet_SMSG_INITIAL_SPELLS, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_ACTION_BUTTONS, Packet_SMSG_ACTION_BUTTONS, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_INVITE, Packet_SMSG_GROUP_INVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_CANCEL, Packet_SMSG_GROUP_CANCEL, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_DECLINE, Packet_SMSG_GROUP_DECLINE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_UNINVITE, Packet_SMSG_GROUP_UNINVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_SET_LEADER, Packet_SMSG_GROUP_SET_LEADER, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_DESTROYED, Packet_SMSG_GROUP_DESTROYED, nullptr);
    ClientServices::SetMessageHandler(SMSG_PARTY_COMMAND_RESULT, Packet_SMSG_PARTY_COMMAND_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_LIST, Packet_SMSG_GROUP_LIST, nullptr);
    ClientServices::SetMessageHandler(SMSG_REAL_GROUP_UPDATE, Packet_SMSG_REAL_GROUP_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_INVALID_QUEST, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_DETAILS, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_REQUEST_ITEMS, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_COMPLETE, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_FAILED, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_STATUS, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_STATUS_MULTIPLE, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_LOG_FULL, Packet_Group_12, nullptr);
    ClientServices::SetMessageHandler(SMSG_TRAINER_LIST, Packet_Group_13, nullptr);
    ClientServices::SetMessageHandler(SMSG_TRAINER_BUY_FAILED, Packet_Group_13, nullptr);
    ClientServices::SetMessageHandler(SMSG_SET_PROFICIENCY, Packet_SMSG_SET_PROFICIENCY, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESURRECT_REQUEST, Packet_SMSG_RESURRECT_REQUEST, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAYER_SKINNED, Packet_SMSG_PLAYER_SKINNED, nullptr);
    ClientServices::SetMessageHandler(SMSG_INSPECT_RESULTS_UPDATE, Packet_SMSG_INSPECT, nullptr);
    ClientServices::SetMessageHandler(SMSG_INSPECT_TALENT, Packet_SMSG_INSPECT_TALENT, nullptr);
    ClientServices::SetMessageHandler(SMSG_READ_ITEM_RESULT_OK, Packet_Group_14, nullptr);
    ClientServices::SetMessageHandler(SMSG_READ_ITEM_RESULT_FAILED, Packet_Group_14, nullptr);
    ClientServices::SetMessageHandler(SMSG_CANCEL_COMBAT, Packet_SMSG_CANCEL_COMBAT, nullptr);
    ClientServices::SetMessageHandler(SMSG_TAXI_NODE_STATUS, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_SHOW_TAXI_NODES, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_ACTIVATE_TAXI_REPLY, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_INVITE, Packet_SMSG_GUILD_INVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_DECLINE, Packet_SMSG_GUILD_DECLINE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_INFO, Packet_SMSG_GUILD_INFO, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_EVENT, Packet_SMSG_GUILD_EVENT, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_COMMAND_RESULT, Packet_SMSG_GUILD_COMMAND_RESULT, nullptr);
    ClientServices::SetMessageHandler(MSG_SAVE_GUILD_EMBLEM, Packet_MSG_SAVE_GUILD_EMBLEM, nullptr);
    ClientServices::SetMessageHandler(MSG_TABARDVENDOR_ACTIVATE, Packet_MSG_TABARDVENDOR_ACTIVATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PETITION_SHOW_LIST, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(SMSG_PETITION_SHOW_SIGNATURES, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(SMSG_PETITION_SIGN_RESULTS, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(MSG_PETITION_DECLINE, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(SMSG_TURN_IN_PETITION_RESULT, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(MSG_PETITION_RENAME, Packet_Group_15, nullptr);
    ClientServices::SetMessageHandler(SMSG_BIND_POINT_UPDATE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_BINDZONEREPLY, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_EMOTE, Packet_SMSG_EMOTE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAYER_BOUND, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAYERBINDERROR, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_NEW_TAXI_PATH, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_NAME_INVALID, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_EXPLORATION_EXPERIENCE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_PARTY_MEMBER_STATS, Packet_HandlePartyMemberStats, nullptr);
    ClientServices::SetMessageHandler(SMSG_PARTY_MEMBER_STATS_FULL, Packet_HandlePartyMemberStats, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_FAILED, Packet_Group_17, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_FAILED_TIMER, Packet_Group_17, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_COMPLETE, Packet_Group_17, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_ADD_KILL, Packet_Group_17, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_ADD_PVP_CREDIT, Packet_Group_17, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_CONFIRM_ACCEPT, Packet_SMSG_QUEST_CONFIRM_ACCEPT, nullptr);
    ClientServices::SetMessageHandler(SMSG_SHOW_BANK, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_BUY_BANK_SLOT_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_LEVEL_UP_INFO, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(MSG_MINIMAP_PING, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_TRIGGER_CINEMATIC, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_TIME_UPDATE, Packet_Group_18, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_ENCHANT_TIME_UPDATE, Packet_Group_18, nullptr);
    ClientServices::SetMessageHandler(MSG_RANDOM_ROLL, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_FISH_NOT_HOOKED, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_FISH_ESCAPED, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_FORCEACTIONSHOW, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_GOD_MODE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_PETGODMODE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_IGNORE_REQUIREMENTS_CHEAT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_IGNORE_DIMINISHING_RETURNS_CHEAT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_CLEAR_FAR_SIGHT_IMMEDIATE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHAT_WRONG_FACTION, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHAT_PLAYER_NOTFOUND, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHAT_RESTRICTED, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHAT_NOT_IN_PARTY, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_GET_SYSTEM_STATUS, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPIRIT_HEALER_CONFIRM, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(MSG_TALENT_WIPE_CONFIRM, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_BINDER_CONFIRM, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_EXECUTE_LOG, Packet_SMSG_SPELLLOGEXECUTE, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_MISS_LOG, Packet_SMSG_SPELLLOGMISS, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_DAMAGE_SHIELD, Packet_SMSG_SPELLDAMAGESHIELD, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_INSTAKILL_LOG, Packet_SMSG_SPELLINSTAKILLLOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_NON_MELEE_DAMAGE_LOG, Packet_SMSG_SPELLNONMELEEDAMAGELOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_HEAL_LOG, Packet_SMSG_SPELLHEALLOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_ENERGIZE_LOG, Packet_SMSG_SPELLENERGIZELOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_DISPELL_LOG, Packet_Group_19, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_STEAL_LOG, Packet_Group_19, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_BREAK_LOG, Packet_Group_19, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESURRECT_FAILED, Packet_SMSG_RESURRECT_FAILED, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_OR_DAMAGE_IMMUNE, Packet_SMSG_SPELLORDAMAGE_IMMUNE, nullptr);
    ClientServices::SetMessageHandler(MSG_QUEST_PUSH_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_DAMAGE_CALC_LOG, Packet_SMSG_DAMAGE_CALC_LOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_RAID_GROUP_ONLY, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_INSTANCE_LOCK_WARNING_QUERY, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_START_ROLL, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_ALL_PASSED, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_ROLL_WON, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_ROLL, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_LOOT_MASTER_LIST, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_TEST_DROP_RATE_RESULT, PH_LOOT_OPCODES, nullptr);
    ClientServices::SetMessageHandler(SMSG_SUMMON_REQUEST, Packet_SMSG_SUMMON_REQUEST, nullptr);
    ClientServices::SetMessageHandler(SMSG_SUMMON_CANCEL, OnSummonCancel, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_TIME_WARNING, Packet_SMSG_PLAY_TIME_WARNING, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_INVITE, Packet_SMSG_ARENA_TEAM_INVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_EVENT, Packet_SMSG_ARENA_TEAM_EVENT, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_COMMAND_RESULT, Packet_SMSG_ARENA_TEAM_COMMAND_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_ERROR, Packet_SMSG_ARENA_ERROR, nullptr);
    ClientServices::SetMessageHandler(SMSG_DEATH_RELEASE_LOC, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_FORCED_DEATH_UPDATE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_TIME_SYNC_REQUEST, Packet_SMSG_TIME_SYNC_REQ, nullptr);
    ClientServices::SetMessageHandler(SMSG_VOICE_SESSION_ROSTER_UPDATE, Packet_SMSG_VOICE_SESSION_ROSTER_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_VOICE_SESSION_LEAVE, Packet_SMSG_VOICE_SESSION_LEAVE, nullptr);
    ClientServices::SetMessageHandler(SMSG_VOICE_SET_TALKER_MUTED, Packet_SMSG_VOICE_SET_TALKER_MUTED, nullptr);
    ClientServices::SetMessageHandler(SMSG_VOICE_PARENTAL_CONTROLS, Packet_SMSG_VOICE_PARENTAL_CONTROLS, nullptr);
    ClientServices::SetMessageHandler(SMSG_CROSSED_INEBRIATION_THRESHOLD, Packet_SMSG_CROSSED_INEBRIATION_THRESHOLD, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMPLAINT_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_FEATURE_SYSTEM_STATUS, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_AVAILABLE_VOICE_CHANNEL, Packet_SMSG_AVAILABLE_VOICE_CHANNEL, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_COMMAND_RESULT, Packet_SMSG_CALENDAR_COMMAND_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_REPORT_PVP_AFK_RESULT, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_GROUP_ACTION_THROTTLED, Packet_SMSG_GROUPACTION_THROTTLED, nullptr);
    ClientServices::SetMessageHandler(SMSG_SEND_UNLEARN_SPELLS, Packet_SMSG_SEND_UNLEARN_SPELLS, nullptr);
    ClientServices::SetMessageHandler(SMSG_CONVERT_RUNE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESYNC_RUNES, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_ADD_RUNE_POWER, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEGROUND_INFO_THROTTLED, Packet_SMSG_BATTLEGROUND_INFO_THROTTLED, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_TALENT_DATA, Packet_SMSG_TALENTS_INFO, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_CHANGE_FAILED_QUEUED, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_POI_QUERY_RESPONSE, Packet_SMSG_QUEST_POI_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_TOGGLE_XP_GAIN, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_DEBUG_SERVER_GEO, Packet_SMSG_DEBUG_SERVER_GEO, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_FORCE_REMOVED, Packet_SMSG_QUEST_FORCE_REMOVE, nullptr);
    ClientServices::SetMessageHandler(SMSG_CAMERA_SHAKE, Packet_Group_9, nullptr);
    ClientServices::SetMessageHandler(SMSG_SOCKET_GEMS, Packet_Group_18, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMBAT_LOG_MULTIPLE, Packet_SMSG_COMBAT_LOG_MULTIPLE, nullptr);
}

// VERIFY: ea8acbe5 manual
// OFFSET: 0x006E4950
void CGPlayer_C::CombatModeEnter(const WOWGUID& target, int32_t queueRanged, int32_t silent) {
    if (!CGGameUI::CanPerformAction(1)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    int32_t rangedSpell = s_cvAutoRangedCombat->m_intValue ? CGSpellBook::m_autoRangedSpell : 0;

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !this->MountedCombatAllowed()) {
        float minRange;
        float maxRange;

        if (rangedSpell) {
            Spell_C::GetSpellRange(this, rangedSpell, &minRange, &maxRange, unit);
        } else {
            maxRange = this->CGUnit::GetMeleeRange(unit);
        }

        C3Vector position = unit->GetPosition();

        if (maxRange * maxRange < this->GetDistanceToPos(position)) {
            if (!silent) {
                CGGameUI::DisplayError(ERR_OUT_OF_RANGE);
            }

            return;
        }

        this->Dismount();
    }

    if ((!(this->m_combatModeFlags & 0x2) && !this->CGUnit_C::IsInMelee()) || target != this->m_combatTarget) {
        this->m_combatModeTime = FrameTime::s_curTimeMs;
    }

    this->m_combatTarget = target;

    if (!rangedSpell) {
        this->MeleeModeEnter(target);
        return;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || (this->m_unit->m_unitFieldFlags & 0x20000) || !CGGameUI::m_hasControl) {
        return;
    }

    if (this->m_player->m_playerFlags & 0x1000000) {
        CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
        return;
    }

    if (!this->CanAttackNow(unit)) {
        if (this->IsInMelee()) {
            this->SetCombatMode();
        }

        return;
    }

    EventRegister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags |= 0x2;
    CGGameUI::UpdateCombatMode();

    if (!this->IsInMeleeRange(unit)) {
        if (queueRanged) {
            this->m_combatModeFlags |= 0x8;
        }

        this->m_combatModeFlags |= 0x4;
    }
}

// VERIFY: bbf5e390 manual
// OFFSET: 0x006E0640
int32_t CGPlayer_C::GetLanguageSkill(uint32_t language, uint32_t& skill) {
    skill = 0;

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    int32_t languageSpell = CGSpellBook::m_languageSpells.m_data[language];

    if (!languageSpell) {
        if (!(this->m_unit->m_unitFieldFlags2 & 0x8)) {
            return 0;
        }

        SpellRec spell;

        for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
            if (!g_spellDB.GetRecord(this->m_auras[i].m_spellId, &spell)) {
                continue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                if (spell.m_effectAura[j] == 244 && spell.m_effectMiscValue[j] == static_cast<int32_t>(language)) {
                    skill = 300;
                    return 1;
                }
            }
        }

        return 0;
    }

    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], languageSpell);

    if (!ability) {
        return 0;
    }

    for (int32_t i = 0; i < 128; i++) {
        uint16_t skillId = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillInfo[i].m_skillId : 0;

        if (skillId == ability->m_skillLine) {
            if (!g_skillLineDB.GetRecord(ability->m_skillLine)) {
                return 0;
            }

            int32_t modifier = this->GetSkillModifier(i);
            skill = this->GetSkillValue(i) + modifier;
            return 1;
        }
    }

    return 0;
}

// VERIFY: e61db75c manual
// OFFSET: 0x005CD920
int16_t CGPlayer_C::GetSkillModifier(int32_t index) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerSkillInfo[index].m_tempBonus;
}

// VERIFY: 30fe9fdf auto
// OFFSET: 0x006E7B00
void CGPlayer_C::AddKnownSpell(int32_t spellId, int32_t a2, int32_t a3, int32_t a4) {
    if (spellId > g_spellDB.m_maxID) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", spellId);
        return;
    }

    if (a4) {
        if (a3) {
            CGActionBar::ReplaceSpell(0, spellId);
        }

        CGSpellBook::AddKnownSpell(spellId, a3, 0);
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return;
    }

    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spellId);
    SkillLineRec* skillLine = ability ? g_skillLineDB.GetRecord(ability->m_skillLine) : nullptr;

    if (skillLine && skillLine->m_id == 778) {
        this->m_companionSpells.Add(1, &spellId);
    }

    if (spell.m_attributes & 0x20) {
        if (skillLine) {
            HASHKEY_NONE key;
            TRADESKILLLINE* line = this->m_tradeSkillLines.Ptr(skillLine->m_id, key);

            if (!line) {
                line = this->m_tradeSkillLines.InternalNewNode(skillLine->m_id, 0, 0);
                line->m_hashval = skillLine->m_id;
            }

            *line->m_spells.New() = spellId;
        }

        CGTradeSkillInfo::RefreshList(1);
    } else if (spell.m_effect[0] == 101) {
        SpellSetFeedPetSpell(spell.m_id);
    } else if (spell.m_effect[0] == 155) {
        if (!this->HasEquipBypassSkill(spellId)) {
            this->m_equipBypassSkills.Add(1, &spellId);
        }
    } else {
        bool proficiency = false;

        for (uint32_t i = 0; i < 3; i++) {
            if (spell.m_effect[i] == 60) {
                proficiency = true;
                break;
            }
        }

        if (proficiency) {
            for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
                ItemSubClassRec* rec = (i < 0 || i >= g_itemSubClassDB.m_numRecords) ? nullptr : &g_itemSubClassDB.m_records[i];

                if (rec->m_classID <= 0 || rec->m_classID != spell.m_equippedItemClass || 1 << rec->m_subClassID != spell.m_equippedItemSubclass) {
                    continue;
                }

                int32_t skill = rec->m_generatedID;

                if (skill) {
                    HASHKEY_NONE key;
                    PROFICIENCYSPELL* proficiencySpell = this->m_proficiencySpells.Ptr(skill, key);

                    if (!proficiencySpell) {
                        proficiencySpell = this->m_proficiencySpells.InternalNewNode(skill, 0, 0);
                        proficiencySpell->m_hashval = skill;
                    }

                    proficiencySpell->m_spellId = spellId;
                }

                break;
            }
        }
    }

    if (a3) {
        CGClassTrainer::RefreshList();
    }

    if (spell.m_effect[0] == 40) {
        s_dualWieldSpell = spellId;
    }
}

// VERIFY: f0445597 auto
// OFFSET: 0x006D71E0
int32_t GetModalNextSpell() {
    return s_cvAutoRangedCombat->m_intValue ? CGSpellBook::m_autoRangedSpell : 0;
}

// VERIFY: 3f2316b1 manual
// OFFSET: 0x006DE840
int32_t CGPlayer_C::IsXRayVisionActive() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    return (player->m_auraEffectMask[0x22] >> 1) & 1;
}


// VERIFY: a9d2639d auto
// OFFSET: 0x006CEAF0
int32_t CGPlayer_C::GetNewContinentID() {
    return ClntObjMgrGetMapID();
}

// VERIFY: fc173622 auto
// OFFSET: 0x007FE510
void Unk7FE510(uint32_t guidLow) {
    unk_D3F4EC = guidLow;
}

// VERIFY: 3100171d manual
// OFFSET: 0x0060A600
uint32_t CGPlayer_C::GetNextLevelXP() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_player->m_playerNextLevelXp;
    }

    return 0;
}

// VERIFY: 67b36da5 auto
// OFFSET: 0x006CC410
int32_t Packet_SMSG_GUILD_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    CGGameUI::DisplayError(ERR_GUILD_DECLINE_S, name);
    return 1;
}

// VERIFY: 9b9205f5 auto
// OFFSET: 0x006D6520
void CGPlayer_C::OnLootGameObject(const WOWGUID& guid) {
    this->m_lootTarget = guid;

    if (this->ShouldKneelForLoot()) {
        this->PlayBaseAnimation(ANIM_LOOT, 0);
    }
}

// VERIFY: f3a09cad auto
// OFFSET: 0x006CC910
int32_t Packet_SMSG_ARENA_TEAM_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char inviter[48];
    char team[96];
    msg->GetString(inviter, sizeof(inviter));
    msg->GetString(team, sizeof(team));
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_INVITE_REQUEST, "%s%s", inviter, team);
    CGGameUI::DisplayError(ERR_INVITED_TO_GUILD_SSS, inviter, inviter, team);
    return 1;
}

// VERIFY: 3830e7e6 manual
// OFFSET: 0x006CCE90
int32_t Packet_SMSG_ARENA_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t error;
    msg->Get(error);

    if (error == 0) {
        uint8_t type;
        msg->Get(type);
        CGGameUI::DisplayError(ERR_ARENA_NO_TEAM_II, type, type);
    } else if (error == 1) {
        CGGameUI::DisplayError(ERR_ARENA_EXPIRED_CAIS);
    } else if (error == 2) {
        CGGameUI::DisplayError(ERR_LFG_CANT_USE_BATTLEGROUND);
    }

    return 1;
}

// VERIFY: 2d2bd7b2 manual
// OFFSET: 0x006CBD70
int32_t Packet_SMSG_GROUP_SET_LEADER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));

    if (SStrCmpUTF8I(ClientServices::GetCharacterName(), name, STORM_MAX_STR)) {
        if (CGPartyInfo::m_lfgRestricted) {
            CGGameUI::DisplayError(ERR_NEW_GUIDE_S, name);
        } else {
            CGGameUI::DisplayError(ERR_NEW_LEADER_S, name);
        }
    } else {
        if (CGPartyInfo::m_lfgRestricted) {
            CGGameUI::DisplayError(ERR_NEW_GUIDE_YOU);
        } else {
            CGGameUI::DisplayError(ERR_NEW_LEADER_YOU);
        }
    }

    return 1;
}

// VERIFY: 454dfb59 manual
// OFFSET: 0x006CF1A0
int32_t __stdcall CGPlayer_C::OnTurnInPetitionResults(CDataStore* msg) {
    uint32_t result;
    msg->Get(result);

    switch (result) {
        case 0: {
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REGISTRAR_CLOSED, nullptr);
            break;
        }

        case 2: {
            CGGameUI::DisplayError(ERR_PETITION_IN_GUILD);
            break;
        }

        case 4: {
            CGGameUI::DisplayError(ERR_PETITION_NOT_ENOUGH_SIGNATURES);
            break;
        }

        case 11: {
            CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
            break;
        }

        default: {
            ConsoleWrite("Petition error", DEFAULT_COLOR);
            break;
        }
    }

    return 1;
}

// VERIFY: fc2ddd1d auto
// OFFSET: 0x006CCB20
int32_t Packet_SMSG_ARENA_TEAM_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t command;
    char team[96];
    char player[48];
    uint32_t error;
    msg->Get(command);
    msg->GetString(team, sizeof(team));
    msg->GetString(player, sizeof(player));
    msg->Get(error);

    if (error == 0) {
        switch (command) {
            case 0: {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_CREATE_S, team);
                break;
            }

            case 1: {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_INVITE_SS, player, team);
                break;
            }

            case 3: {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_QUIT_S, team);
                break;
            }

            case 14: {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_FOUNDER_S, team);
                break;
            }
        }

        return 1;
    }

    switch (error) {
        case 1: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_INTERNAL);
            break;
        }

        case 2: {
            CGGameUI::DisplayError(ERR_ALREADY_IN_ARENA_TEAM);
            break;
        }

        case 3: {
            CGGameUI::DisplayError(ERR_ALREADY_IN_ARENA_TEAM_S, player);
            break;
        }

        case 4: {
            CGGameUI::DisplayError(ERR_INVITED_TO_ARENA_TEAM);
            break;
        }

        case 5: {
            CGGameUI::DisplayError(ERR_ALREADY_INVITED_TO_ARENA_TEAM_S, player);
            break;
        }

        case 6: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NAME_INVALID);
            break;
        }

        case 7: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NAME_EXISTS_S, team);
            break;
        }

        case 8: {
            if (command == 3) {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_LEADER_LEAVE_S, team);
            } else {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_PERMISSIONS);
            }

            break;
        }

        case 9: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM);
            break;
        }

        case 10: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS, player, team);
            break;
        }

        case 11: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S, player);
            break;
        }

        case 12: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NOT_ALLIED);
            break;
        }

        case 19: {
            CGGameUI::DisplayError(ERR_IGNORING_YOU_S, player);
            break;
        }

        case 21: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TARGET_TOO_LOW_S, player);
            break;
        }

        case 22: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TARGET_TOO_HIGH_S, player);
            break;
        }

        case 23: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S, team);
            break;
        }

        case 27: {
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NOT_FOUND);
            break;
        }

        case 30: {
            CGGameUI::DisplayError(ERR_ARENA_TEAMS_LOCKED);
            break;
        }
    }

    return 1;
}

// VERIFY: 068abde1 manual
// OFFSET: 0x006DCA90
bool CGPlayer_C::CanTrackObject(CGObject_C* object) {
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);

    if (this->IsDeadOrGhost() || (gameObject->m_gameObject->m_typeID != 25 && !gameObject->CanUse())) {
        return false;
    }

    uint32_t trackResources = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerTrackResources : 0;
    LockRec* lock = gameObject->GetLockRec();

    if (!lock) {
        return false;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (lock->m_type[i] == 2 && ((1 << (static_cast<uint8_t>(lock->m_index[i]) - 1)) & trackResources)) {
            return true;
        }
    }

    return false;
}

// VERIFY: c6a9b523 auto
// OFFSET: 0x006E2240
int32_t Packet_SMSG_SEND_UNLEARN_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!msg) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (msgId != SMSG_SEND_UNLEARN_SPELLS) {
        return 1;
    }

    uint32_t count;
    msg->Get(count);
    CGSpellBook::m_unlearnedSpells.SetCount(count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        uint32_t spellId;
        msg->Get(spellId);
        CGSpellBook::m_unlearnedSpells.m_data[i] = spellId;
    }

    return 1;
}

// VERIFY: c9e9e3d7 manual
// OFFSET: 0x006D7590
bool CGPlayer_C::IsRAFPlayer(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return (player->m_unit->m_unitDynamicFlags >> 6) & 1;
    }

    CGPartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid);

    if (!stats) {
        stats = CGRaidInfo::GetRemoteStats(guid);
    }

    if (stats && (stats->m_flags & 0x100)) {
        return true;
    }

    if (FriendList::s_friendList) {
        FriendInfo* info = FriendList::s_friendList->GetFriend(guid);

        if (info) {
            return (info->m_flags >> 3) & 1;
        }
    }

    return false;
}

// VERIFY: d74d5ba5 auto
// OFFSET: 0x006E6FD0
const CGBag_C* CGPlayer_C::GetBag() const {
    return &this->m_bag;
}

// VERIFY: 66a7ab59 auto
// OFFSET: 0x006E6FD0
CGBag_C* CGPlayer_C::GetBag() {
    return &this->m_bag;
}

// VERIFY: a02355d6 manual
// OFFSET: 0x006D7BB0
int32_t Player_C_ZoneUpdateHandler(const void* data, void* param) {
    if (++s_zoneUpdateCounter >= 10) {
        s_zoneUpdateCounter -= 10;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit || (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != nullptr) {
            AreaListRegisterLocation(ClntObjMgrGetMapID(), unit->m_worldObject);
        }
    }

    return 1;
}

// VERIFY: 042060de manual
// OFFSET: 0x006DE230
const CGItem_C* CGPlayer_C::EquippedItemMeetSpellRequirements(const SpellRec* spell, uint32_t slotMask) {
    if ((this->m_unit->m_unitFieldFlags & 0x200000) || (this->m_unit->m_unitFieldFlags2 & 0x80)) {
        if (this->IsDisarmed(0)) {
            slotMask &= ~0x8000;
        }

        if (this->IsDisarmed(1)) {
            slotMask &= ~0x10000;
        }
    }

    if (this->m_unit->m_unitFieldFlags2 & 0x400) {
        slotMask &= ~0x20000;
    }

    for (int32_t slot = 0; slot < 23; slot++) {
        if (!((1 << slot) & slotMask)) {
            continue;
        }

        CGItem_C* item = this->m_bag.GetItemPointer(slot);

        if (item && item->IsUsable() && spell->m_equippedItemClass == static_cast<int32_t>(item->GetClassID()) && ((1 << item->GetSubtypeID()) & spell->m_equippedItemSubclass)) {
            return item;
        }
    }

    return nullptr;
}

// VERIFY: 6a792bb7 manual
// OFFSET: 0x00578210
int32_t CGPlayer_C::GetModDamageDonePos(int32_t school) const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_player->m_playerFieldModDamageDonePos[school];
    }

    return 0;
}

// VERIFY: 816c9e22 manual
// OFFSET: 0x00578250
int32_t CGPlayer_C::GetModDamageDoneNeg(int32_t school) const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_player->m_playerFieldModDamageDoneNeg[school];
    }

    return 0;
}

// VERIFY: 55676844 manual
// OFFSET: 0x00578290
float CGPlayer_C::GetModDamageDonePct(int32_t school) const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_player->m_playerFieldModDamageDonePct[school];
    }

    return 0.0f;
}

// VERIFY: 68b495c6 manual
// OFFSET: 0x006DC550
bool CGPlayer_C::MeetsItemSetSkill(const ItemSetRec* set) const {
    if (set->m_requiredSkill) {
        int32_t index = this->GetSkillIndex(set->m_requiredSkill);

        if (index < 0 || static_cast<int32_t>(this->GetSkillValue(index)) < set->m_requiredSkillRank) {
            return false;
        }
    }

    return true;
}

// VERIFY: 95312415 manual
// OFFSET: 0x006DE360
InspectItem* CGPlayer_C::GetInspectedItem(int32_t slot) {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        if (slot < 0) {
            return nullptr;
        }

        if (static_cast<uint32_t>(slot) < 19) {
            return &s_inspectData->m_items[slot];
        }
    }

    if (slot < 0 || static_cast<uint32_t>(slot) >= 19) {
        return nullptr;
    }

    VisibleItem_C* visible = &this->m_player->m_playerVisibleItems[slot];

    if (!visible) {
        return nullptr;
    }

    memset(&s_emptyInspectItem, 0, sizeof(s_emptyInspectItem));
    s_emptyInspectItem.m_itemId = abs(visible->m_itemId);
    s_emptyInspectItem.m_enchants[0] = visible->m_enchantment[0];
    s_emptyInspectItem.m_enchants[1] = visible->m_enchantment[1];

    if (visible->m_itemId <= 0) {
        s_emptyInspectItem.m_flags |= 1;
    }

    return &s_emptyInspectItem;
}

// VERIFY: bd15a857 manual
// OFFSET: 0x006DE490
static void CountGemsItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->CountEquippedGems();
    }
}

// VERIFY: 456c3dda manual
// OFFSET: 0x006DE4D0
void CGPlayer_C::CountEquippedGems() {
    this->m_gemCounts[0] = 0;
    this->m_gemCounts[1] = 0;
    this->m_gemCounts[2] = 0;
    this->m_gemCounts[3] = 0;

    for (uint32_t slot = 0; slot <= 18; slot++) {
        CGItem_C* item = nullptr;
        InspectItem* inspect = nullptr;

        if (ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) {
            item = this->m_bag.GetItemPointer(slot);

            if (!item || !item->IsUsable()) {
                continue;
            }
        } else {
            inspect = this->GetInspectedItem(slot);

            if (!inspect || !(inspect->m_flags & 1)) {
                continue;
            }
        }

        for (uint32_t socket = 2; socket <= 4; socket++) {
            int32_t enchantId;

            if (ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) {
                enchantId = (item->GetItemStaticFlags(0) & 0x2000) ? 0 : item->m_item->m_itemFieldEnchantment[socket].m_id;
            } else if (socket <= 11) {
                enchantId = inspect->m_enchants[socket];
            } else {
                enchantId = 0;
            }

            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);

            if (!enchant || !enchant->m_srcItemID) {
                continue;
            }

            ItemStats_C* gem = g_itemDBCache.GetRecord(enchant->m_srcItemID, WOWGUID{ 0 }, &CountGemsItemCacheCallback, nullptr, false);

            if (!gem) {
                continue;
            }

            GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(gem->m_gemProperties);

            if (!properties || !properties->m_type) {
                continue;
            }

            if (properties->m_type & 1) {
                this->m_gemCounts[0]++;
            }

            if (properties->m_type & 2) {
                this->m_gemCounts[1]++;
            }

            if (properties->m_type & 4) {
                this->m_gemCounts[2]++;
            }

            if (properties->m_type & 8) {
                this->m_gemCounts[3]++;
            }
        }
    }
}

// VERIFY: 106512a4 manual
// OFFSET: 0x006D1E20
void __stdcall CGPlayer_C::TalkToGossip(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GOSSIP_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 99b0048a manual
// OFFSET: 0x006D1EA0
void __stdcall CGPlayer_C::TalkToQuestUnit(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 6f91a651 manual
// OFFSET: 0x006D1F20
void __stdcall CGPlayer_C::TalkToTrainer(const WOWGUID& guid) {
    s_lastTrainerRequest = guid;
    WOWGUID none = { 0 };
    CGClassTrainer::SetTrainer(none, 0);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TRAINER_LIST));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 1d12cc15 manual
// OFFSET: 0x006D2120
void CGPlayer_C::TalkToBinder(const WOWGUID& guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid ? guid : s_binder, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (dy * dy + dx * dx + dz * dz > range * range) {
        return;
    }

    if (guid) {
        s_binder = guid;
        AreaTableRec* area = CGGameUI::GetCurrentSubzoneRec();
        const char* name = area ? area->m_areaName : FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE);
        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_BINDER, "%s", name);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BINDER_ACTIVATE));
    msg.Put(s_binder.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 525e9f8e manual
// OFFSET: 0x006D23C0
void __stdcall CGPlayer_C::TalkToGuildBanker(const WOWGUID& guid) {
    if (CGGuildBankInfo::m_banker && CGGuildBankInfo::m_banker == guid) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(CGGuildBankInfo::m_guildBankTabInfo[0].unk_2140 != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGuildBankInfo::m_pendingBanker = guid;
}

// VERIFY: 7ea073a1 manual
// OFFSET: 0x006D2500
void CGPlayer_C::TalkToTabardVendor(const WOWGUID& guid) {
    if (CGTabardCreationFrame::m_vendor == guid) {
        return;
    }

    if (!this->CanEquipItemsInThisForm()) {
        CGGameUI::DisplayError(ERR_EMBLEMERROR_NOTABARDGEOSET);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_TABARDVENDOR_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: eb9c79a5 manual
// OFFSET: 0x006DAF60
void __stdcall CGPlayer_C::ShopFromMerchant(const WOWGUID& guid) {
    WOWGUID item = CGGameUI::GetCursorItem();

    if (item) {
        CGPlayer_C::SellItem(guid, item, 0);
        WOWGUID none = { 0 };
        CGGameUI::SetCursorItem(none, none, 0, 0, 0, 1);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LIST_INVENTORY));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: e4ffe0d2 manual
// OFFSET: 0x006DB010
void CGPlayer_C::TalkToSpiritHealer(const WOWGUID& guid) {
    if (!(this->m_player->m_playerFlags & 0x10)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid ? guid : s_spiritHealer, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (dy * dy + dx * dx + dz * dz > range * range) {
        return;
    }

    if (guid) {
        s_spiritHealer = guid;
        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_XP_LOSS, "%d", static_cast<int32_t>(static_cast<int32_t>(this->GetNextLevelXP()) * 0.05f));
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SPIRIT_HEALER_ACTIVATE));
    msg.Put(s_spiritHealer.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 25092c88 manual
// OFFSET: 0x006DB180
void CGPlayer_C::TalkToSpiritGuide(const WOWGUID& guid) {
    if (this->m_player->m_playerFlags & 0x10) {
        WOWGUID none = { 0 };
        CGGameUI::SetAreaSpiritHealer(none);
        CGGameUI::SetAreaSpiritHealer(guid);
    }
}

// VERIFY: 856bb8dc manual
// OFFSET: 0x006DDBB0
void CGPlayer_C::HandleOnRightClick(CGUnit_C* unit, bool autoInteract) {
    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (!form || !(form->m_flags & 0x8)) {
            if (!this->CanAutoUnshift()) {
                CGGameUI::DisplayError(ERR_CANT_INTERACT_SHAPESHIFTED);
                return;
            }

            this->CancelShapeshift();
        }
    }

    C3Vector unitPosition = unit->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = position.x - unitPosition.x;
    float dy = position.y - unitPosition.y;
    float dz = position.z - unitPosition.z;
    float distanceSq = dz * dz + dy * dy + dx * dx;

    if (autoInteract) {
        float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;

        if (reach * reach < distanceSq) {
            WOWGUID guid = unit->m_obj->m_guid;

            if (this->SetTrackingTarget(guid, 5)) {
                return;
            }
        }
    }

    float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (reach * reach < distanceSq) {
        if (!autoInteract) {
            CGGameUI::DisplayError(ERR_TOO_FAR_TO_INTERACT);
        }

        return;
    }

    if (unit->m_obj->m_guid == CGGameUI::m_interactTarget) {
        return;
    }

    if (unit->m_questStatus == 1) {
        CGTutorial::TriggerTutorial(0x2A);
    }

    WOWGUID guid = unit->m_obj->m_guid;

    if (unit->m_interactIconType == 7) {
        CGPlayer_C::EnableTaxiNode(guid);

        if (!CGUnit_C::UnitCanGossip(unit)) {
            return;
        }
    }

    if (unit->m_unit->m_unitNpcFlags & 0x1) {
        CGPlayer_C::TalkToGossip(guid);
        this->RequestTalkEmote(0);
        return;
    }

    if (CGUnit_C::UnitCanGossip(unit)) {
        CGPlayer_C::TalkToQuestUnit(guid);
        this->RequestTalkEmote(0);
        return;
    }

    uint32_t npcFlags = unit->m_unit->m_unitNpcFlags;

    if (npcFlags & 0x2000) {
        CGPlayer_C::QueryTaxiNodes(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x80) {
        CGPlayer_C::ShopFromMerchant(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x10) {
        CGPlayer_C::TalkToTrainer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x4000) {
        this->TalkToSpiritHealer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x8000) {
        this->TalkToSpiritGuide(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x10000) {
        this->TalkToBinder(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x20000) {
        CGPlayer_C::TalkToBanker(guid);
        this->RequestTalkEmote(0);
    } else if (unit->IsGuildMaster()) {
        CGPlayer_C::TalkToPetitionVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x40000) {
        CGPlayer_C::TalkToPetitionVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x80000) {
        this->TalkToTabardVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x100000) {
        CGPlayer_C::TalkToBattleMaster(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x200000) {
        CGPlayer_C::TalkToAuctioneer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x400000) {
        CGPlayer_C::TalkToStableMaster(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x800000) {
        CGPlayer_C::TalkToGuildBanker(guid);
        this->RequestTalkEmote(0);
    } else if (unit->IsInPartyOrRaid()) {
        CGPlayer_C::TalkToSpellClick(guid);
    } else if (unit->CanBeSpellClickedByLocalPlayer(this)) {
        CGPlayer_C::TalkToRideVehicle(guid);
    } else if (unit->m_unit->m_unitNpcFlags & 0x4000000) {
        CGPlayer_C::TalkToMailbox(unit->m_obj->m_guid);
    }
}

// VERIFY: 2014c9e0 manual
// OFFSET: 0x006D1760
void CGPlayer_C::RequestAllItemPurchaseData() {
    for (int32_t i = 0; i <= 18; i++) {
        if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(this->m_bag.m_numSlots) || !this->m_bag.m_slots[i]) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(this->m_bag.m_slots[i], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
            item->RequestItemPurchaseData();
        }
    }

    for (int32_t i = 23; i <= 38; i++) {
        if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(this->m_bag.m_numSlots) || !this->m_bag.m_slots[i]) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(this->m_bag.m_slots[i], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
            item->RequestItemPurchaseData();
        }
    }

    for (int32_t i = 19; i <= 22; i++) {
        WOWGUID guid = static_cast<uint32_t>(i) < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[i] : WOWGUID();
        CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!container) {
            continue;
        }

        for (uint32_t j = 0; j < static_cast<uint32_t>(container->GetBag()->m_numSlots); j++) {
            CGBag_C* bag = container->GetBag();

            if (j >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[j]) {
                continue;
            }

            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(bag->m_slots[j], TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
                item->RequestItemPurchaseData();
            }
        }
    }
}

// VERIFY: 1167de4b manual
// OFFSET: 0x006D9960
int32_t CGPlayer_C::OnListInventory(CDataStore* msg) {
    for (int32_t i = 0; i < 150; i++) {
        s_vendorItems[i].m_slot = 0;
    }

    WOWGUID vendor;
    msg->Get(vendor.m_guid);

    uint8_t count;
    msg->Get(count);

    if (count > 150) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGPlayer_C::s_lastVendorListReceived = vendor;
    uint8_t error = 0xFF;

    if (count) {
        for (uint8_t i = 0; i < count; i++) {
            MerchantItem& item = s_vendorItems[i];
            msg->Get(reinterpret_cast<uint32_t&>(item.m_slot));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_itemID));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_displayID));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_maxCount));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_price));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_durability));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_buyCount));
            msg->Get(reinterpret_cast<uint32_t&>(item.m_extendedCostID));
        }
    } else {
        msg->Get(error);

        switch (error) {
            case 0:
                ConsoleWrite("Vendor has no inventory", DEFAULT_COLOR);
                break;

            case 1:
                ConsoleWrite("I don't think he likes you very much", DEFAULT_COLOR);
                break;

            case 2:
                ConsoleWrite("You are too far away", DEFAULT_COLOR);
                break;

            case 3:
                ConsoleWrite("Vendor is dead", DEFAULT_COLOR);
                break;

            case 4:
                ConsoleWrite("You can't shop while dead.", DEFAULT_COLOR);
                break;

            default:
                break;
        }
    }

    if (count || !error) {
        this->RequestAllItemPurchaseData();
        CGMerchantInfo::SetMerchant(vendor, s_vendorItems, count);
    }

    return 1;
}

// VERIFY: 56e2df00 manual
// OFFSET: 0x006D0460
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestDetails(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);

    WOWGUID inform;
    msg->Get(inform.m_guid);

    uint32_t questId;
    msg->Get(questId);

    char title[512];
    msg->GetString(title, sizeof(title));

    char details[3000];
    msg->GetString(details, sizeof(details));

    char objectives[3000];
    msg->GetString(objectives, sizeof(objectives));

    uint8_t autoLaunched;
    msg->Get(autoLaunched);

    uint32_t flags;
    msg->Get(flags);

    uint32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);

    uint8_t startCheat;
    msg->Get(startCheat);

    uint32_t numChoices;
    msg->Get(numChoices);

    uint32_t choiceItems[6] = { 0 };
    uint32_t choiceDisplays[6] = { 0 };
    uint32_t choiceCounts[6];

    for (int32_t i = 0; i < static_cast<int32_t>(numChoices); i++) {
        msg->Get(choiceItems[i]);
        msg->Get(choiceCounts[i]);
        msg->Get(choiceDisplays[i]);
    }

    uint32_t numRewards;
    msg->Get(numRewards);

    uint32_t rewardItems[4] = { 0 };
    uint32_t rewardDisplays[4] = { 0 };
    uint32_t rewardCounts[4];

    for (int32_t i = 0; i < static_cast<int32_t>(numRewards); i++) {
        msg->Get(rewardItems[i]);
        msg->Get(rewardCounts[i]);
        msg->Get(rewardDisplays[i]);
    }

    uint32_t money;
    msg->Get(money);

    uint32_t xp;
    msg->Get(xp);

    uint32_t honor;
    msg->Get(honor);

    float honorMultiplier;
    msg->Get(honorMultiplier);

    uint32_t spell;
    msg->Get(spell);

    uint32_t spellCast;
    msg->Get(spellCast);

    uint32_t rewardTitle;
    msg->Get(rewardTitle);

    uint32_t talents;
    msg->Get(talents);

    uint32_t arenaPoints;
    msg->Get(arenaPoints);

    uint32_t factionMask;
    msg->Get(factionMask);

    uint32_t factionIDs[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionIDs[i]);
    }

    uint32_t factionValues[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionValues[i]);
    }

    uint32_t factionOverrides[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionOverrides[i]);
    }

    uint32_t emoteCount;
    msg->Get(emoteCount);

    QUESTGIVEREMOTEQUEUE emotes;
    emotes.m_nodes = static_cast<QUESTGIVEREMOTENODE*>(alloca(emoteCount * sizeof(QUESTGIVEREMOTENODE)));
    emotes.m_count = emoteCount;

    for (int32_t i = 0; i < static_cast<int32_t>(emoteCount); i++) {
        msg->Get(reinterpret_cast<uint32_t&>(emotes.m_nodes[i].m_emote));
        msg->Get(emotes.m_nodes[i].m_delay);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            unit->PlayEmotes(emotes);
        }

        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && CGQuestInfo::m_npc) {
            CDataStore response;
            response.Put(static_cast<uint32_t>(MSG_QUEST_PUSH_RESULT));
            response.Put(inform ? inform.m_guid : npc.m_guid);
            response.Put(questId);
            response.Put(static_cast<uint8_t>(4));
            response.Finalize();
            ClientServices::Send2(&response);
            return 1;
        }
    }

    CGQuestInfo::SetState(npc, inform, 1, details, questId, startCheat);
    CGQuestInfo::SetLogDescription(objectives);
    CGQuestInfo::AddReward(title, choiceItems, choiceDisplays, choiceCounts, numChoices, rewardItems, rewardDisplays, rewardCounts, numRewards, money, xp, honor, honorMultiplier, spell, spellCast, autoLaunched, suggestedGroupNum, rewardTitle, talents, arenaPoints, factionMask, factionIDs, factionValues, factionOverrides, flags);
    return 1;
}

// VERIFY: ba5bd839 manual
// OFFSET: 0x006D2DE0
void CGPlayer_C::BuyItem(WOWGUID vendor, const MerchantItem* item, uint32_t count, uint8_t a5) {
    if (!item) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BUY_ITEM));
    msg.Put(vendor.m_guid);
    msg.Put(static_cast<uint32_t>(item->m_itemID));
    msg.Put(static_cast<uint32_t>(item->m_slot));
    msg.Put(count);
    msg.Put(a5);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 604e7050 manual
// OFFSET: 0x006DC3F0
int32_t CGPlayer_C::CanUseItem(const ItemStats_C* stats, int32_t& error) {
    error = ERR_MAX_VALUE;

    if (!stats) {
        error = ERR_ITEM_NOT_FOUND;
        return 0;
    }

    CGUnitData* unit = this->m_unit;

    if (static_cast<int32_t>(stats->m_requiredLevel) > static_cast<int32_t>(unit->m_unitFieldLevel) || !((1 << (unit->m_unitFieldBytes0[1] - 1)) & stats->m_allowableClass) || !((1 << (unit->m_unitFieldBytes0[0] - 1)) & stats->m_allowableRace)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    uint8_t itemClass = static_cast<uint8_t>(stats->m_class);

    if (itemClass < 17 && s_proficiency[itemClass] && !(s_proficiency[itemClass] & (1 << stats->m_subClass))) {
        error = ERR_PROFICIENCY_NEEDED;
        return 0;
    }

    if (stats->m_requiredSkill && !(stats->m_flags[0] & 0x20040000)) {
        int32_t index = this->GetSkillIndex(stats->m_requiredSkill);

        if (index < 0 || static_cast<int32_t>(this->GetSkillValue(index)) < static_cast<int32_t>(stats->m_requiredSkillRank)) {
            error = ERR_CANT_USE_ITEM;
            return 0;
        }
    }

    if (stats->m_requiredSpell && !this->IsSpellKnown(stats->m_requiredSpell) && !this->IsSpellSuperceded(stats->m_requiredSpell)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredHonorRank && static_cast<int32_t>(this->m_player->m_playerFieldBytes[3]) < static_cast<int32_t>(stats->m_requiredHonorRank)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredCityRank && !this->HasPvpMedal(static_cast<uint8_t>(stats->m_requiredCityRank))) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredReputationFaction && GetRepListRepValue(stats->m_requiredReputationFaction) < CGReputationInfo::m_reactionThresholds[stats->m_requiredReputationRank]) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    return 1;
}

// VERIFY: c1869d85 manual
// OFFSET: 0x006D0AB0
int32_t __stdcall CGPlayer_C::OnQuestGiverOfferReward(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);

    uint32_t questId;
    msg->Get(questId);

    char title[512];
    msg->GetString(title, sizeof(title));

    char text[3000];
    msg->GetString(text, sizeof(text));

    uint8_t autoLaunched;
    msg->Get(autoLaunched);

    uint32_t flags;
    msg->Get(flags);

    uint32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);

    uint32_t emoteCount;
    msg->Get(emoteCount);

    QUESTGIVEREMOTEQUEUE emotes;
    emotes.m_nodes = static_cast<QUESTGIVEREMOTENODE*>(alloca(emoteCount * sizeof(QUESTGIVEREMOTENODE)));
    emotes.m_count = emoteCount;

    for (int32_t i = 0; i < static_cast<int32_t>(emoteCount); i++) {
        msg->Get(emotes.m_nodes[i].m_delay);
        msg->Get(reinterpret_cast<uint32_t&>(emotes.m_nodes[i].m_emote));
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->PlayEmotes(emotes);
    }

    uint32_t numChoices;
    msg->Get(numChoices);

    uint32_t choiceItems[6] = { 0 };
    uint32_t choiceDisplays[6] = { 0 };
    uint32_t choiceCounts[6];

    for (int32_t i = 0; i < static_cast<int32_t>(numChoices); i++) {
        msg->Get(choiceItems[i]);
        msg->Get(choiceCounts[i]);
        msg->Get(choiceDisplays[i]);
    }

    uint32_t numRewards;
    msg->Get(numRewards);

    uint32_t rewardItems[4] = { 0 };
    uint32_t rewardDisplays[4] = { 0 };
    uint32_t rewardCounts[4];

    for (int32_t i = 0; i < static_cast<int32_t>(numRewards); i++) {
        msg->Get(rewardItems[i]);
        msg->Get(rewardCounts[i]);
        msg->Get(rewardDisplays[i]);
    }

    uint32_t money;
    msg->Get(money);

    uint32_t xp;
    msg->Get(xp);

    uint32_t honor;
    msg->Get(honor);

    float honorMultiplier;
    msg->Get(honorMultiplier);

    uint32_t unknown;
    msg->Get(unknown);

    uint32_t spell;
    msg->Get(spell);

    uint32_t spellCast;
    msg->Get(spellCast);

    uint32_t rewardTitle;
    msg->Get(rewardTitle);

    uint32_t talents;
    msg->Get(talents);

    uint32_t arenaPoints;
    msg->Get(arenaPoints);

    uint32_t factionMask;
    msg->Get(factionMask);

    uint32_t factionIDs[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionIDs[i]);
    }

    uint32_t factionValues[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionValues[i]);
    }

    uint32_t factionOverrides[5];

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(factionOverrides[i]);
    }

    WOWGUID none = { 0 };
    CGQuestInfo::SetState(npc, none, 3, text, questId, 0);
    CGQuestInfo::AddReward(title, choiceItems, choiceDisplays, choiceCounts, numChoices, rewardItems, rewardDisplays, rewardCounts, numRewards, money, xp, honor, honorMultiplier, spell, spellCast, autoLaunched, suggestedGroupNum, rewardTitle, talents, arenaPoints, factionMask, factionIDs, factionValues, factionOverrides, flags);
    return 1;
}

// VERIFY: 2d06226f manual
// OFFSET: 0x007246A0
float CGPlayer_C::GetQuestXPMultiplier() {
    float multiplier = 1.0f;

    if (!(this->m_auraEffectMask[291 >> 3] & (1 << (291 & 7)))) {
        return multiplier;
    }

    SpellRec spell;

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        if (!g_spellDB.GetRecord(this->GetAura(i)->m_spellId, &spell)) {
            continue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            if (spell.m_effectAura[j] == 291) {
                multiplier = (spell.m_effectBasePoints[j] + 1) * 0.0099999998f + multiplier;
            }
        }
    }

    return multiplier;
}

// VERIFY: 6e33a08e auto
// OFFSET: 0x006DF2B0
static void QuestAcceptCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(id, none, nullptr, nullptr, false);

    if (!quest) {
        return;
    }

    CGGameUI::DisplayError(ERR_QUEST_ACCEPTED_S, quest->m_name);

    if (quest->m_infoId == 1) {
        CGTutorial::TriggerTutorial(0x28);
        CGTutorial::TriggerTutorial(0x26);
    }

    if (!IsTutorialFlagged(0x14) && ++s_questsAccepted >= 4) {
        CGTutorial::TriggerTutorial(0x14);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ACCEPTED, "%d", CGQuestLog::GetQuestIndex(quest->m_id));
}

// VERIFY: ac232887 manual
// OFFSET: 0x006DF370
int32_t OnUpdateQuest(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    CGQuestLog::Update(1);
    uint32_t slot = (offset - 0x28) / sizeof(PlayerQuestLogEntry);
    PlayerQuestLogEntry* entry = slot > 25 ? nullptr : &player->m_player->m_playerQuestLog[slot];
    const PlayerQuestLogEntry* old = static_cast<const PlayerQuestLogEntry*>(value);
    uint32_t questId = entry->m_questId;
    WOWGUID none = { 0 };

    if (questId && (questId != old->m_questId || (!(entry->m_state & 2) && (old->m_state & 2)))) {
        QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestAcceptCallback, nullptr, false);

        if (quest) {
            CGGameUI::DisplayError(ERR_QUEST_ACCEPTED_S, quest->m_name);

            if (quest->m_infoId == 1) {
                CGTutorial::TriggerTutorial(0x28);
                CGTutorial::TriggerTutorial(0x26);
            }

            if (!IsTutorialFlagged(0x14) && ++s_questsAccepted >= 4) {
                CGTutorial::TriggerTutorial(0x14);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ACCEPTED, "%d", CGQuestLog::GetQuestIndex(quest->m_id));
        }
    }

    QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, none, nullptr, nullptr, false);

    if (quest) {
        int32_t index = CGQuestLog::GetQuestIndex(quest->m_id) - 1;

        if (index >= 0 && (quest->m_flags & 0x10000) && CGQuestLog::IsQuestCompleteEx(index, 1)) {
            WOWGUID self = player->m_obj->m_guid;
            CGPlayer_C::CompleteQuest(self, quest->m_id);
        }
    }

    if (old && old->m_questId && entry->m_questId != old->m_questId) {
        RemoveQuestPointDisplay(old->m_questId);
    }

    return 1;
}

// VERIFY: 1103b817 manual
// OFFSET: 0x006E5180
void CGPlayer_C::SetActiveMirrorHandlers() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x10, 8, CritterUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x2C0; i < 0x378; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x3F8; i <= 0x4D0; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x4D8; i <= 0x508; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x570; i <= 0x668; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x670; i <= 0x768; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x28; i < 0x21C; i += 20) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 20, OnUpdateQuest, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x11B0; i < 0x1214; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, ReportDailyQuestsRemaining, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFF8, 4, OnUpdateMoney, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x18, 8, CharmChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x1D3, 1, OnUpdateShapeshiftForm, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x770, 8, FarsightChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);

    for (uint32_t i = 0x778; i < 0x790; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_KNOWN_TITLES_UPDATE)), static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x790, 8, SignalKnownCurrencyTypesUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);

    for (uint32_t i = 0x7A6; i < 0xDA6; i += 12) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i - 2, 2, SkillRankChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 2, SkillMaxRankChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i + 4, 2, SkillMaxRankChangeHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i + 2, 2, SignalPlayerSkillStatEventsAndRefresh, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1B, 1, OnUpdateRestState, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFF4, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_EXHAUSTION)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x798, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x79C, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFFC, 0x1C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1018, 0x1C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1034, 0x1C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1050, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D4, 2, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E0, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D8, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1C, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_RANK_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E7, 1, OnUpdatePlayerAuraVision, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E4, 2, OnUpdatePlayerOverrideSpellsID, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E8, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_FACTION)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1150, 0x54, OnArenaTeamsChanged, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11A4, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnHonorCurrencyChanged), nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11A8, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnArenaCurrencyChanged), nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10EC, 0x64, SignalCombatRatingUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDEC, 4, SignalCombatRatingUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDB0, 0x34, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x88, 0x38, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x1214; i < 0x1224; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, OnUpdatePlayerRuneRegen, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDAC, 4, OnUpdatePlayerTrackingMask, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1060, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x105C, 4, SignalCombatRatingUpdate, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1224, 0xC, OnUpdatePlayerNoReagentCost, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x1248; i < 0x1260; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyph), nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1260, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyphSlotsEnabled), nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1264, 4, reinterpret_cast<MIRRORHANDLERFUNC>(SignalPetSpellPowerUpdate), nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: 7c2e2f8b manual
// OFFSET: 0x006E5960
void CGPlayer_C::UnsetActiveMirrorHandlers() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x10, CritterUpdate, nullptr);

    for (uint32_t i = 0x2C0; i < 0x378; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, nullptr);
    }

    for (uint32_t i = 0x3F8; i <= 0x4D0; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, nullptr);
    }

    for (uint32_t i = 0x4D8; i <= 0x508; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, nullptr);
    }

    for (uint32_t i = 0x570; i <= 0x668; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, nullptr);
    }

    for (uint32_t i = 0x670; i <= 0x768; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, nullptr);
    }

    for (uint32_t i = 0x28; i < 0x21C; i += 20) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdateQuest, nullptr);
    }

    for (uint32_t i = 0x11B0; i < 0x1214; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, ReportDailyQuestsRemaining, nullptr);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFF8, OnUpdateMoney, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x18, CharmChangeHandler, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x770, FarsightChangeHandler, nullptr);

    for (uint32_t i = 0x778; i < 0x790; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_KNOWN_TITLES_UPDATE)));
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x790, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_KNOWN_CURRENCY_TYPES_UPDATE)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x1D3, OnUpdateShapeshiftForm, nullptr);

    for (uint32_t i = 0x7A6; i < 0xDA6; i += 12) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i - 2, SkillRankChangeHandler, nullptr);
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, SkillMaxRankChangeHandler, nullptr);
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i + 4, SkillMaxRankChangeHandler, nullptr);
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i + 2, SignalPlayerSkillStatEventsAndRefresh, nullptr);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1B, OnUpdateRestState, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFF4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_EXHAUSTION)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x798, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x79C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xFFC, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1018, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1034, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1050, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E0, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D8, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_RANK_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E7, OnUpdatePlayerAuraVision, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E4, OnUpdatePlayerOverrideSpellsID, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E8, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_FACTION)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1150, OnArenaTeamsChanged, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11A4, reinterpret_cast<MIRRORHANDLERFUNC>(OnHonorCurrencyChanged), nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11A8, reinterpret_cast<MIRRORHANDLERFUNC>(OnArenaCurrencyChanged), nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10EC, SignalCombatRatingUpdate, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDEC, SignalCombatRatingUpdate, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDB0, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x88, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));

    for (uint32_t i = 0x1214; i < 0x1224; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdatePlayerRuneRegen, nullptr);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDAC, OnUpdatePlayerTrackingMask, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1060, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x105C, SignalCombatRatingUpdate, nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1224, OnUpdatePlayerNoReagentCost, nullptr);

    for (uint32_t i = 0x1248; i < 0x1260; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyph), nullptr);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1260, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyphSlotsEnabled), nullptr);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1264, reinterpret_cast<MIRRORHANDLERFUNC>(SignalPetSpellPowerUpdate), nullptr);
}

struct QuestCompleteRewards {
    /* 0000 */ int32_t m_xp;
    /* 0004 */ int32_t m_money;
    /* 0008 */ int32_t m_honor;
    /* 000C */ int32_t m_talents;
    /* 0010 */ int32_t m_arenaPoints;
};

// VERIFY: 42e86b51 manual
// OFFSET: 0x006D0240
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestList(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);

    char greeting[2048];
    msg->GetString(greeting, sizeof(greeting));

    QUESTGIVEREMOTENODE emote;
    msg->Get(emote.m_delay);
    msg->Get(reinterpret_cast<uint32_t&>(emote.m_emote));

    uint8_t count;
    msg->Get(count);

    s_questListNpc = npc;
    memset(s_questListIds, 0, sizeof(s_questListIds));
    memset(s_questListStatus, 0, sizeof(s_questListStatus));
    memset(s_questListLevels, 0, sizeof(s_questListLevels));
    memset(s_questListFlags, 0, sizeof(s_questListFlags));
    memset(s_questListRepeatable, 0, sizeof(s_questListRepeatable));
    CGQuestInfo::SetState(npc, WOWGUID(), 0, greeting, 0, 0);

    char titles[32][512];

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(s_questListIds[i]);
        msg->Get(s_questListStatus[i]);
        msg->Get(s_questListLevels[i]);
        msg->Get(s_questListFlags[i]);

        uint8_t repeatable = 0;
        msg->Get(repeatable);
        s_questListRepeatable[i] = repeatable;

        msg->GetString(titles[i], sizeof(titles[i]));

        uint32_t status = s_questListStatus[i];

        if (status == 3 || status == 4) {
            CGQuestInfo::AddQuestInProgress(s_questListIds[i], titles[i], s_questListLevels[i], s_questListFlags[i], s_questListRepeatable[i]);
        } else {
            CGQuestInfo::AddQuest(s_questListIds[i], titles[i], s_questListLevels[i], s_questListFlags[i], status == 0, s_questListRepeatable[i]);
        }
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetEmoteQueue(&emote, 1);
    }

    CGQuestInfo::EndQuestList();
    return 1;
}

// VERIFY: 1dea9fc1 manual
// OFFSET: 0x006D08F0
int32_t __stdcall CGPlayer_C::OnQuestGiverRequestItems(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);

    uint32_t questId;
    msg->Get(questId);

    char title[512];
    msg->GetString(title, sizeof(title));

    char text[3000];
    msg->GetString(text, sizeof(text));

    QUESTGIVEREMOTENODE emote;
    msg->Get(emote.m_delay);
    msg->Get(reinterpret_cast<uint32_t&>(emote.m_emote));

    uint32_t autoLaunched;
    msg->Get(autoLaunched);

    uint32_t flags;
    msg->Get(flags);

    uint32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);

    uint32_t requiredMoney;
    msg->Get(requiredMoney);

    uint32_t numItems;
    msg->Get(numItems);

    uint32_t itemIds[6] = { 0 };
    uint32_t itemCounts[6];
    uint32_t itemDisplays[6];

    for (int32_t i = 0; i < static_cast<int32_t>(numItems); i++) {
        msg->Get(itemIds[i]);
        msg->Get(itemCounts[i]);
        msg->Get(itemDisplays[i]);
    }

    uint32_t completable0;
    msg->Get(completable0);

    uint32_t completable1;
    msg->Get(completable1);

    uint32_t completable2;
    msg->Get(completable2);

    uint32_t completable3;
    msg->Get(completable3);

    CGQuestInfo::SetState(npc, WOWGUID(), 2, text, questId, 0);
    int32_t canComplete = completable0 && completable1 && completable2 && completable3;
    CGQuestInfo::AddItemRequest(title, itemIds, itemCounts, itemDisplays, numItems, requiredMoney, canComplete, autoLaunched, suggestedGroupNum, flags);

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetEmoteQueue(&emote, 1);
    }

    return 1;
}

// VERIFY: e3fa33e0 auto
// OFFSET: 0x006CD7C0
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestInvalid(CDataStore* msg) {
    uint32_t reason;
    msg->Get(reason);

    switch (reason) {
        case 1:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_LOW_LEVEL);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_WRONG_RACE);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_DONE);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_QUEST_ONLY_ONE_TIMED);
            break;

        case 13:
        case 18:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_ON);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_EXPANSION);
            break;

        case 21:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_MISSING_ITEMS);
            break;

        case 23:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_NOT_ENOUGH_MONEY);
            break;

        case 26:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
            break;

        case 27:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_CAIS);
            break;

        case 29:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_DONE_DAILY);
            break;

        default:
            CGGameUI::DisplayError(ERR_QUEST_NEED_PREREQS);
            break;
    }

    CGQuestInfo::QuestGiverFinished(0, 0, 0);
    return 1;
}

// VERIFY: 73d5b3db auto
// OFFSET: 0x006CD8C0
static void QuestRewardItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = { 0 };
    ItemStats_C* stats = g_itemDBCache.GetRecord(id, none, nullptr, nullptr, false);

    if (!stats) {
        return;
    }

    const char* link = CGTooltip::GetItemHyperlinkString(id, stats->m_quality, 0, 0, 0);
    int32_t count = static_cast<int32_t>(reinterpret_cast<intptr_t>(param));

    if (count <= 1) {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_S, link);
    } else {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_MULT_IS, count, link);
    }
}

// VERIFY: bafe4a7d auto
// OFFSET: 0x006CD940
static void QuestFailedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(id, none, nullptr, nullptr, false);

    if (!quest) {
        return;
    }

    switch (s_questFailedReason) {
        case 4:
        case 16:
        case 50:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_BAG_FULL_S, quest->m_name);
            CGGameUI::DisplayError(ERR_INV_FULL);
            break;

        case 17:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_MAX_COUNT_S, quest->m_name);
            break;

        case 74:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
            break;

        default:
            if (!CGQuestLog::IsQuestExpiredByID(id)) {
                CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
            }

            break;
    }
}

// VERIFY: b475afdb auto
// OFFSET: 0x006CDA70
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestFailed(CDataStore* msg) {
    uint32_t questId;
    msg->Get(questId);
    msg->Get(s_questFailedReason);

    QuestCache* quest = g_questDBCache.GetRecord(questId, CGQuestInfo::m_npc, QuestFailedCallback, nullptr, false);

    if (quest) {
        switch (s_questFailedReason) {
            case 4:
            case 16:
            case 50:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_BAG_FULL_S, quest->m_name);
                CGGameUI::DisplayError(ERR_INV_FULL);
                break;

            case 17:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_MAX_COUNT_S, quest->m_name);
                break;

            case 74:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
                break;

            default:
                if (!CGQuestLog::IsQuestExpiredByID(questId)) {
                    CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
                }

                break;
        }
    }

    CGQuestInfo::QuestGiverFinished(0, questId, 0);
    return 1;
}

// VERIFY: 2b919027 manual
// OFFSET: 0x006D0E10
static void QuestCompleteCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    QuestCompleteRewards* rewards = static_cast<QuestCompleteRewards*>(param);

    if (!success) {
        delete rewards;
        return;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(id, none, nullptr, nullptr, false);

    if (!quest) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGGameUI::DisplayError(ERR_QUEST_COMPLETE_S, quest->m_name);
    SI2::PlaySoundKit("igQuestListComplete", nullptr, nullptr, nullptr);

    if (rewards->m_xp) {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_EXP_I, rewards->m_xp);
    }

    if (rewards->m_money > 0) {
        char money[1024];
        GetCurrencyText(rewards->m_money, money, sizeof(money), ", ");
        CGGameUI::DisplayError(ERR_QUEST_REWARD_MONEY_S, money);
    }

    char text[3000];

    if (rewards->m_honor > 0) {
        rewards->m_honor = static_cast<int32_t>(static_cast<uint32_t>(rewards->m_honor) / 10 + 0.5);
        const char* format = FrameScript_GetText("COMBATLOG_HONORAWARD", -1, GENDER_NOT_APPLICABLE);

        if (format && *format) {
            SStrPrintf(text, sizeof(text), format, rewards->m_honor);
            CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_HONOR_GAIN, nullptr, 0, nullptr, nullptr, nullptr, WOWGUID(), 0, WOWGUID(), 0, false, nullptr);
        }

        SendToCombatText(43, rewards->m_honor);
    }

    if (rewards->m_talents > 0) {
        const char* format = FrameScript_GetText("LEVEL_UP_CHAR_POINTS", -1, GENDER_NOT_APPLICABLE);

        if (format && *format) {
            SStrPrintf(text, sizeof(text), format, rewards->m_talents);
            CGChat::AddChatMessage(text, CHAT_MSG_MONEY, nullptr, 0, nullptr, nullptr, nullptr, WOWGUID(), 0, WOWGUID(), 0, false, nullptr);
        }
    }

    if (rewards->m_arenaPoints > 0) {
        const char* format = FrameScript_GetText("COMBATLOG_ARENAPOINTSAWARD", -1, GENDER_NOT_APPLICABLE);

        if (format && *format) {
            SStrPrintf(text, sizeof(text), format, rewards->m_arenaPoints);
            CGChat::AddChatMessage(text, CHAT_MSG_ARENA_POINTS, nullptr, 0, nullptr, nullptr, nullptr, WOWGUID(), 0, WOWGUID(), 0, false, nullptr);
        }

        SendToCombatText(47, rewards->m_arenaPoints);
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();

    for (uint32_t i = 0; i < 4; i++) {
        int32_t itemId = static_cast<int32_t>(quest->m_rewardItem[i]);
        void* count = reinterpret_cast<void*>(static_cast<uintptr_t>(quest->m_rewardItemCount[i]));

        if (itemId > 0 && g_itemDBCache.GetRecord(itemId, player, QuestRewardItemCallback, count, false)) {
            QuestRewardItemCallback(itemId, player, count, true);
        }
    }

    int32_t chosen = CGQuestInfo::m_lastChosenItem;

    if (chosen) {
        if (g_itemDBCache.GetRecord(chosen, player, QuestRewardItemCallback, nullptr, false)) {
            WOWGUID none2 = { 0 };
            ItemStats_C* stats = g_itemDBCache.GetRecord(chosen, none2, nullptr, nullptr, false);

            if (stats) {
                CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_S, CGTooltip::GetItemHyperlinkString(chosen, stats->m_quality, 0, 0, 0));
            }
        }

        CGQuestInfo::m_lastChosenItem = 0;
    }

    CGQuestInfo::QuestGiverFinished(quest->m_nextQuestId != 0, id, 0);
    delete rewards;
}

// VERIFY: bd6214c4 manual
// OFFSET: 0x006D1110
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestComplete(CDataStore* msg) {
    QuestCompleteRewards* rewards = new (__FILE__, __LINE__) QuestCompleteRewards;

    uint32_t questId;
    msg->Get(questId);
    msg->Get(reinterpret_cast<uint32_t&>(rewards->m_xp));
    msg->Get(reinterpret_cast<uint32_t&>(rewards->m_money));
    msg->Get(reinterpret_cast<uint32_t&>(rewards->m_honor));
    msg->Get(reinterpret_cast<uint32_t&>(rewards->m_talents));
    msg->Get(reinterpret_cast<uint32_t&>(rewards->m_arenaPoints));

    WOWGUID none = { 0 };

    if (g_questDBCache.GetRecord(questId, none, QuestCompleteCallback, rewards, false)) {
        QuestCompleteCallback(questId, none, rewards, true);
    }

    return 1;
}
