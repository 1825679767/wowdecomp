#include "Item_C.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/MaterialRec.h"
#include "WoW/Source/DB/Rec/RandPropPointsRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include <ctime>

// STATIC: 0x00CA1140
static TSHashTable<INVENTORYART, HASHKEY_NONE> s_inventoryArt;

// STATIC: 0x00CA1168
static TSHashTable<ITEMBYNAME, HASHKEY_UTF8I> s_itemByName;

// VERIFY: 1390e3e9 manual
// OFFSET: 0x00707720
static int32_t OnUpdateStackCount(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        WOWGUID requester = WOWGUID();
        ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, requester, nullptr, nullptr, false);

        if (stats && static_cast<int32_t>(stats->m_stackable) > 1) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                player->m_bag.UnlockItemsOfType(item->m_obj->m_entryID, 8);
            }
        } else {
            CGGameUI::UnlockItem(item->m_obj->m_guid);
        }

        CGTradeSkillInfo::RefreshList(0);
        CGActionBar::UpdateItem(item->m_obj->m_entryID);
        CGTradeInfo::UpdatePlayerItem(item->m_obj->m_guid);
        CGContainerInfo::UpdateItem(item->m_obj->m_guid);
        CGCharacterInfo::UpdateItem(item->m_obj->m_guid);
        CGQuestLog::UpdateQuestItem(item->m_obj->m_entryID);

        if (stats && stats->m_class == 10) {
            CGCurrencyTypes::EnterWorld();
        }

        ItemRec* rec = g_itemDB.GetRecord(item->m_obj->m_entryID);

        if (rec && rec->m_inventoryType == 24) {
            CGCharacterInfo::UpdateInventoryAlerts();
        }
    }

    return 1;
}

// VERIFY: 8b8a4f8e manual
// OFFSET: 0x00707880
static int32_t OnUpdateSpellCharges(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGActionBar::UpdateItem(item->m_obj->m_entryID);
        CGContainerInfo::UpdateCooldowns();
    }

    return 1;
}

// VERIFY: ca82e43c manual
// OFFSET: 0x007078E0
static void ItemIDChangedCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            int32_t slot = player->m_bag.FindItemSlot(guid);

            if (slot == 15 || slot == 16) {
                CGActionBar::UpdateAttackAction();
            } else if (slot == 17) {
                CGActionBar::UpdateRangedAttackAction();
            }
        }

        CGActionBar::UpdateItem(id);
        CGContainerInfo::UpdateItem(guid);
        CGCharacterInfo::UpdateItem(guid);
    }
}

// VERIFY: c3dbda02 manual
// OFFSET: 0x00707960
static int32_t OnUpdateItemID(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        int32_t oldItemId = *static_cast<const int32_t*>(oldValue);
        CGActionBar::UpdateItem(oldItemId);
        g_itemDBCache.Invalidate(oldItemId);

        WOWGUID itemGuid = item->m_obj->m_guid;

        if (g_itemDBCache.GetRecord(item->m_obj->m_entryID, itemGuid, ItemIDChangedCacheCallback, nullptr, false)) {
            itemGuid = item->m_obj->m_guid;
            ItemIDChangedCacheCallback(item->m_obj->m_entryID, itemGuid, nullptr, true);
        }

        Script_SendUniqueSignal(293);
    }

    return 1;
}

// VERIFY: 8b4c986d manual
// OFFSET: 0x00707A30
static int32_t OnUpdateDynamicFlags(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && ((*static_cast<const uint8_t*>(oldValue) ^ item->m_item->m_itemFieldFlags) & 0x10)) {
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner && owner->m_obj->m_guid == ClntObjMgrGetActivePlayer() && static_cast<uint32_t>(owner->GetBag()->FindItemSlot(item->m_obj->m_guid)) <= 18) {
            CGCharacterInfo::UpdateInventoryAlerts();
        }
    }

    return 1;
}

// VERIFY: 0a592725 manual
// OFFSET: 0x00707AE0
static int32_t OnUpdateDurability(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner) {
            int32_t oldDurability = *static_cast<const int32_t*>(oldValue);

            if ((item->m_item->m_itemFieldFlags & 0x8) || !item->m_item->m_itemFieldDurability) {
                int32_t invSlot = -1;
                int32_t slot = owner->GetBag()->FindItemSlot(item->m_obj->m_guid);

                if (slot == 15) {
                    invSlot = 15;
                } else if (slot == 16) {
                    invSlot = 16;
                } else if (slot == 17) {
                    invSlot = (item->m_virtualItem.m_inventoryType != 26) + 15;
                }

                if (oldDurability > 0) {
                    CCharacterComponent::RemoveHandItem(owner->m_worldModel, static_cast<INVENTORY_SLOTS>(invSlot), static_cast<SHEATHE_TYPE>(item->m_virtualItem.m_sheatheType), item->m_virtualItem.m_inventoryType == 14);

                    WOWGUID ownerGuid = owner->m_obj->m_guid;
                    CGGameUI::UnitModelUpdate(ownerGuid, 3);
                }
            } else if (!(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldDurability && !oldDurability) {
                owner->EquipItem(item);

                WOWGUID ownerGuid = owner->m_obj->m_guid;
                CGGameUI::UnitModelUpdate(ownerGuid, 3);
            }
        }
    }

    CGCharacterInfo::UpdateInventoryAlerts();

    return 1;
}

// VERIFY: fa692b57 manual
// OFFSET: 0x00708BD0
static int32_t OnUpdateEnchantments(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->UpdateEnchantments();
    }

    return 1;
}

// VERIFY: 6840b2f7 auto
// OFFSET: 0x00709440
void CGItem_C::Initialize() {
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldStackCount), sizeof(uint32_t), OnUpdateStackCount, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldSpellCharges), sizeof(CGItemData::m_itemFieldSpellCharges), OnUpdateSpellCharges, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldEnchantment), sizeof(CGItemData::m_itemFieldEnchantment), OnUpdateEnchantments, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), sizeof(uint32_t), OnUpdateItemID, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldFlags), sizeof(uint16_t), OnUpdateDynamicFlags, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldDurability), sizeof(uint32_t), OnUpdateDurability, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: 90fb2c28 auto
// OFFSET: 0x00709A10
void CGItem_C::Shutdown() {
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldStackCount), OnUpdateStackCount, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldSpellCharges), OnUpdateSpellCharges, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldEnchantment), OnUpdateEnchantments, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), OnUpdateItemID, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldFlags), OnUpdateDynamicFlags, nullptr);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldDurability), OnUpdateDurability, nullptr);
    s_inventoryArt.Clear();
    s_itemByName.Clear();
}

// VERIFY: 546a5abe manual
// OFFSET: 0x0070AE90
static void LoadItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || !success) {
        return;
    }

    item->PostInitWithStats();

    CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (owner) {
        owner->unk_18B8--;
    }
}

// VERIFY: 00c0838b manual
// OFFSET: 0x00709840
CGItem_C::CGItem_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create) {
    this->m_itemFlags = 0;
    this->m_expirationTime = 0;
    this->unk_03D8 = 0;

    ClntObjMgrHideObject(this->m_obj->m_guid);

    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_classID = rec ? rec->m_classID : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_subclassID = rec ? rec->m_subclassID : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_material = rec ? rec->m_material : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_inventoryType = rec ? rec->m_inventoryType : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_sheatheType = rec ? rec->m_sheatheType : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    this->m_virtualItem.m_soundOverrideSubclassID = rec ? rec->m_soundOverrideSubclassID : -1;

    memset(this->m_enchantmentExpiration, 0, sizeof(this->m_enchantmentExpiration));
    this->m_itemPurchaseData = nullptr;
}

// VERIFY: 6bc496ea manual
// OFFSET: 0x0070B120
CGItem_C::~CGItem_C() {
    if (!(this->m_itemFlags & 0x2)) {
        g_itemDBCache.CancelCallback(this->m_obj->m_entryID, &LoadItemCacheCallback, this);
    }

    if (this->m_itemPurchaseData) {
        delete this->m_itemPurchaseData;
        this->m_itemPurchaseData = nullptr;
    }
}

// VERIFY: e95d6600 manual
// OFFSET: 0x0070AF00
void CGItem_C::RequestItemCacheInfo() {
    WOWGUID guid = this->m_obj->m_guid;

    if (g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, &LoadItemCacheCallback, this, false)) {
        this->PostInitWithStats();
        return;
    }

    CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (owner) {
        owner->unk_18B8++;
    }
}

// VERIFY: fffdf723 auto
// OFFSET: 0x0070AF80
void CGItem_C::PostInit(uint32_t time, const CClientObjCreate& create, bool a3) {
    this->CGObject_C::PostInit(time, create, a3);
    this->RequestItemCacheInfo();
}

// VERIFY: 5b9cdc6d manual
// OFFSET: 0x00706CF0
void CGItem_C::SetItemPurchaseData(ItemPurchaseData* data) {
    if (this->m_itemPurchaseData) {
        delete this->m_itemPurchaseData;
    }

    this->m_itemPurchaseData = data;

    if (data) {
        this->m_itemFlags &= ~0x4;
    }
}

// VERIFY: 25290e05 auto
// OFFSET: 0x00706EC0
uint32_t CGItem_C::GetSpellItemEnchantmentId() const {
    for (uint32_t i = 0; i < 12; i++) {
        uint32_t id = this->m_item->m_itemFieldEnchantment[i].m_id;

        if (!id) {
            continue;
        }

        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (!rec) {
            continue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            if (rec->m_effectArg[j] && rec->m_effect[j] == 7) {
                return this->m_item->m_itemFieldEnchantment[i].m_id;
            }
        }
    }

    return 0;
}

// VERIFY: 414ca99f auto
// OFFSET: 0x00706FA0
int32_t CGItem_C::IsMetal(int32_t materialId) {
    MaterialRec* rec = g_materialDB.GetRecord(materialId);

    if (rec && (rec->m_flags & 0x1)) {
        return 1;
    }

    return 0;
}

// VERIFY: 01abc0a5 auto
// OFFSET: 0x00707060
void CGItem_C::SetTranslated() {
    this->m_item->m_itemFieldFlags |= 0x2;
}

// VERIFY: e174df88 auto
// OFFSET: 0x00707070
void CGItem_C::UpdateExpirationTime(int32_t seconds) {
    if (seconds > 0) {
        this->m_expirationTime = seconds + static_cast<uint32_t>(time(nullptr));
    } else {
        this->m_expirationTime = 0;
    }
}

// VERIFY: d4c997df manual
// OFFSET: 0x007070B0
uint32_t CGItem_C::GetExpirationTimeLeft() const {
    if (!this->m_expirationTime) {
        return 0;
    }

    int32_t left = this->m_expirationTime - static_cast<uint32_t>(time(nullptr));

    return left > 0 ? left : 0;
}

// VERIFY: dc762275 auto
// OFFSET: 0x007070E0
void CGItem_C::UpdateEnchantmentTime(int32_t slot, int32_t seconds) {
    if (seconds > 0) {
        this->m_enchantmentExpiration[slot] = FrameTime::s_curTimeMs + 1000 * seconds;
    } else {
        this->m_enchantmentExpiration[slot] = 0;
    }
}

// VERIFY: 3cedd503 auto
// OFFSET: 0x00707120
uint32_t CGItem_C::GetEnchantmentTimeLeft(int32_t slot) const {
    uint32_t expiration = this->m_enchantmentExpiration[slot];

    if (expiration && static_cast<int32_t>(FrameTime::s_curTimeMs - expiration) < 0) {
        return expiration - FrameTime::s_curTimeMs;
    }

    return 0;
}

// VERIFY: 947c950c manual
// OFFSET: 0x00707150
int32_t CGItem_C::GetSelectionHighlightColor(CImVector* color) const {
    CImVector white;
    white.b = 0xFF;
    white.g = 0xFF;
    white.r = 0xFF;
    white.a = 0xFF;
    *color = white;

    return 1;
}

// VERIFY: 3f2f878c auto
// OFFSET: 0x00707220
uint32_t CGItem_C::GetClassID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);

    return rec ? rec->m_classID : 0;
}

// VERIFY: 955a9609 auto
// OFFSET: 0x00707250
uint32_t CGItem_C::GetSubtypeID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);

    return rec ? rec->m_subclassID : 0;
}

// VERIFY: 6c114c7d auto
// OFFSET: 0x00707280
uint32_t CGItem_C::GetInventoryType() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);

    return rec ? rec->m_inventoryType : 0;
}

// VERIFY: b59c1299 auto
// OFFSET: 0x00707300
uint32_t CGItem_C::GetDisplayID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);

    return rec ? rec->m_displayInfoID : 0;
}

// VERIFY: 8c746859 auto
// OFFSET: 0x00707330
uint32_t CGItem_C::GetSheatheType() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);

    return rec ? rec->m_sheatheType : 0;
}

// INLINED
bool CGItem_C::IsBoundToAccount() {
    return (this->GetItemStaticFlags(0) >> 13) & 1;
}

// VERIFY: 6b7047e1 manual
// OFFSET: 0x007073E0
bool CGItem_C::HasSoulboundEnchant() {
    if (this->IsBoundToAccount()) {
        return false;
    }

    for (uint32_t i = 0; i < 12; i++) {
        uint32_t id = this->m_item->m_itemFieldEnchantment[i].m_id;

        if (!id) {
            continue;
        }

        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (rec && (rec->m_flags & 0x1)) {
            return true;
        }
    }

    return false;
}

// VERIFY: a5c81637 auto
// OFFSET: 0x00707500
int32_t CGItem_C::GetRandomItemSlot() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID);
    uint32_t inventoryType = rec ? rec->m_inventoryType : 0;

    switch (inventoryType) {
        case 0:
        case 18:
        case 19:
        case 24:
        case 27:
        case 28:
            return -1;

        case 1:
        case 4:
        case 5:
        case 7:
        case 17:
        case 20:
            return 0;

        case 2:
        case 9:
        case 11:
        case 14:
        case 16:
        case 23:
            return 2;

        case 3:
        case 6:
        case 8:
        case 10:
        case 12:
            return 1;

        case 13:
        case 21:
        case 22:
            return 3;

        case 15:
        case 25:
        case 26:
            return 4;

        default:
            return -1;
    }
}

// VERIFY: 7676d8ac manual
// OFFSET: 0x00708140
int32_t CGItem_C::GetPrismaticSocketCount() {
    if (this->IsBoundToAccount()) {
        return 0;
    }

    uint32_t id = this->m_item->m_itemFieldEnchantment[6].m_id;

    if (!id) {
        return 0;
    }

    SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

    if (!rec) {
        return 0;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (rec->m_effect[i] == 8) {
            return rec->m_effectPointsMin[i];
        }
    }

    return 0;
}

// VERIFY: 74178378 manual
// OFFSET: 0x00708200
void CGItem_C::OnRightClick() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !player->IsDeadOrGhost()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_AUTOSTORE_GROUND_ITEM));
        msg.Put(this->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// VERIFY: 0f1e7b39 manual
// OFFSET: 0x007089E0
void CGItem_C::RequestItemPurchaseData() {
    if (this->m_itemFlags & 0x4) {
        return;
    }

    this->m_itemFlags |= 0x4;

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GET_ITEM_PURCHASE_DATA));
    msg.Put(this->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: afbf928a manual
// OFFSET: 0x00708AC0
bool CGItem_C::HasEnchantments() {
    if (!this->IsBoundToAccount() && this->m_item->m_itemFieldEnchantment[0].m_id) {
        return true;
    }

    for (uint32_t i = 2; i < 4; i++) {
        if (!this->IsBoundToAccount() && this->m_item->m_itemFieldEnchantment[i].m_id) {
            return true;
        }
    }

    return !this->IsBoundToAccount() && this->m_item->m_itemFieldEnchantment[6].m_id;
}

// VERIFY: 104e4ba4 auto
// OFFSET: 0x00706BF0
int32_t CGItem_C::NumBaseCharges(int32_t itemId) {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, guid, nullptr, nullptr, false);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellId[i] && !stats->m_spellTrigger[i]) {
            return stats->m_spellCharges[i];
        }
    }

    return 0;
}

// VERIFY: 85394430 auto
// OFFSET: 0x00706C50
void CGItem_C::GetCooldowns(int32_t itemId, uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category) {
    cooldown = 0;
    categoryCooldown = 0;
    category = 0;

    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, guid, nullptr, nullptr, false);

    if (!stats) {
        return;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellId[i] && !stats->m_spellTrigger[i]) {
            cooldown = stats->m_spellCooldown[i];
            categoryCooldown = stats->m_spellCategoryCooldown[i];
            category = stats->m_spellCategory[i];
            return;
        }
    }
}

// VERIFY: d25d8e6d manual
// OFFSET: 0x00706D70
char* CGItem_C::BuildItemName(char* buffer, uint32_t size, int32_t itemId, int32_t randomPropertiesId) {
    *buffer = '\0';

    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, guid, nullptr, nullptr, false);

    if (!stats) {
        return nullptr;
    }

    const char* name = stats->m_name[0];

    if (!name) {
        return nullptr;
    }

    if (randomPropertiesId > 0) {
        ItemRandomPropertiesRec* rec = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (rec && rec->m_nameLang && *rec->m_nameLang) {
            SStrPrintf(buffer, size, FrameScript_GetText("ITEM_SUFFIX_TEMPLATE", -1, GENDER_NOT_APPLICABLE), name, rec->m_nameLang);

            return buffer;
        }
    }

    if (randomPropertiesId < 0) {
        ItemRandomSuffixRec* rec = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);
        const char* suffix = rec ? rec->m_name : "";
        SStrPrintf(buffer, size, FrameScript_GetText("ITEM_SUFFIX_TEMPLATE", -1, GENDER_NOT_APPLICABLE), name, suffix);

        return buffer;
    }

    SStrCopy(buffer, name, size);

    return buffer;
}

// VERIFY: 0695924e auto
// OFFSET: 0x00706E60
int32_t CGItem_C::GetOnUseSlot() const {
    WOWGUID guid = ClntObjMgrGetActivePlayer();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    if (!stats) {
        return -1;
    }

    for (int32_t i = 0; i < 5; i++) {
        if (stats->m_spellId[i] && !stats->m_spellTrigger[i]) {
            return i;
        }
    }

    return -1;
}

// VERIFY: d7e003d5 auto
// OFFSET: 0x00706F40
int32_t CGItem_C::GetUsableSpellCharges() const {
    uint32_t id = this->GetSpellItemEnchantmentId();

    if (id) {
        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (rec) {
            for (uint32_t i = 0; i < 3; i++) {
                if (rec->m_effectArg[i] && rec->m_effect[i] == 7) {
                    return rec->m_charges;
                }
            }
        }
    }

    return CGItem_C::NumBaseCharges(this->m_obj->m_entryID);
}

// VERIFY: adcaf162 auto
// OFFSET: 0x00707180
int32_t CGItem_C::GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const {
    WOWGUID guid = this->m_obj->m_guid;
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, callback, nullptr, false);

    return stats ? stats->m_pageText : 0;
}

// VERIFY: b12c7a20 auto
// OFFSET: 0x007071D0
const char* CGItem_C::GetObjectName() const {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    return stats ? stats->m_name[0] : nullptr;
}

// VERIFY: ae01919f auto
// OFFSET: 0x007072B0
uint32_t CGItem_C::GetBagFamily() const {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    return stats ? stats->m_bagFamily : 0;
}

// VERIFY: bef4c8ce auto
// OFFSET: 0x00707360
ItemStats_C* CGItem_C::GetStats() const {
    WOWGUID guid = WOWGUID();

    return g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);
}

// VERIFY: 8b84f216 auto
// OFFSET: 0x00707390
uint32_t CGItem_C::GetItemStaticFlags(int32_t index) {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    return stats ? stats->m_flags[index] : 0;
}

// VERIFY: 4418573f auto
// OFFSET: 0x00707460
bool CGItem_C::IsZoneBound() const {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    return stats && stats->m_area;
}

// VERIFY: f1daf4fc auto
// OFFSET: 0x007074B0
bool CGItem_C::IsInstanceBound() const {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    return stats && stats->m_map;
}

// VERIFY: b81de831 manual
// OFFSET: 0x00707C20
char* CGItem_C::GetFullItemName(char* buffer, uint32_t size) const {
    int32_t randomPropertiesId = ((this->m_item->m_itemFieldFlags >> 3) & 1) ? 0 : this->m_item->m_itemFieldRandomPropertiesId;

    return CGItem_C::BuildItemName(buffer, size, this->m_obj->m_entryID, randomPropertiesId);
}

// VERIFY: de006961 auto
// OFFSET: 0x00706D30
void CGItem_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_item = reinterpret_cast<CGItemData*>(reinterpret_cast<uint8_t*>(storage) + sizeof(CGObjectData));
    this->m_itemMirror = saved + CGObject::TotalFieldsSaved();
}

// VERIFY: a528b6b7 manual
// OFFSET: 0x007075A0
int32_t CGItem_C::GetRandomPropertyPoints() const {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    if (!stats) {
        return 0;
    }

    uint32_t itemLevel = stats->m_itemLevel;
    uint32_t quality = stats->m_quality;
    uint32_t slot = this->GetRandomItemSlot();

    if (slot > 4) {
        return 0;
    }

    RandPropPointsRec* rec = g_randPropPointsDB.GetRecord(itemLevel);

    if (!rec) {
        return 0;
    }

    switch (quality) {
        case 2:
            return rec->m_good[slot];

        case 3:
            return rec->m_superior[slot];

        case 4:
            return rec->m_epic[slot];

        default:
            return 0;
    }
}

// VERIFY: 6de9acae auto
// OFFSET: 0x00707DB0
int32_t CGItem_C::HasOnUseEnchant() const {
    return this->GetSpellItemEnchantmentId() != 0;
}

// VERIFY: 56fb9e40 manual
// OFFSET: 0x00707DC0
int32_t CGItem_C::GetCurrentSpellCharges() {
    if (this->GetSpellItemEnchantmentId()) {
        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(this->GetSpellItemEnchantmentId());

        if (rec) {
            for (uint32_t i = 0; i < 3; i++) {
                if (rec->m_effect[i] == 7 && rec->m_effectArg[i]) {
                    if (!this->IsBoundToAccount()) {
                        return this->m_item->m_itemFieldEnchantment[i].m_charges;
                    }

                    return 0;
                }
            }
        }
    }

    int32_t slot = this->GetOnUseSlot();

    if (slot >= 0) {
        return this->m_item->m_itemFieldSpellCharges[slot];
    }

    return 0;
}

// VERIFY: a124b25b manual
// OFFSET: 0x00707E60
int32_t CGItem_C::HasUseEffect() {
    if (this->GetItemStaticFlags(0) & 0x40) {
        return 1;
    }

    return this->GetSpellItemEnchantmentId() ? 1 : 0;
}

// VERIFY: 36aaddde manual
// OFFSET: 0x00708520
bool CGItem_C::IsBound() {
    if (this->m_item->m_itemFieldFlags & 0x1) {
        return true;
    }

    return this->HasSoulboundEnchant();
}

// VERIFY: 60e081ae manual
// OFFSET: 0x00708B40
bool CGItem_C::IsPermanentlyBoundForTrade() {
    if (!(this->m_item->m_itemFieldFlags & 0x1) && !this->HasSoulboundEnchant()) {
        return false;
    }

    if ((this->m_item->m_itemFieldFlags >> 8) & 1) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && (this->m_item->m_itemFieldOwner) == player->m_obj->m_guid) {
            int32_t createPlayedTime = this->m_item->m_itemFieldCreatePlayedTime;

            return createPlayedTime - player->GetPlayedTime() + 7200 <= 0;
        }
    }

    return true;
}

// VERIFY: 68c11c7d auto
// OFFSET: 0x007094E0
int32_t CGItem_C::GetNumSockets() {
    int32_t sockets = 0;

    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(this->m_obj->m_entryID, guid, nullptr, nullptr, false);

    if (stats) {
        if (stats->m_socketColor[0]) {
            sockets = 1;
        }

        if (stats->m_socketColor[1]) {
            sockets++;
        }

        if (stats->m_socketColor[2]) {
            sockets++;
        }
    }

    return sockets + this->GetPrismaticSocketCount();
}

// VERIFY: 134df63d auto
// OFFSET: 0x00709550
bool CGItem_C::IsBoundForTrade() {
    return this->IsPermanentlyBoundForTrade() || this->HasSoulboundEnchant();
}

// VERIFY: 92d02457 manual
// OFFSET: 0x00706B90
int32_t CGItem_C::GetUseSpell(int32_t itemId) {
    WOWGUID none;
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, nullptr, nullptr, false);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < sizeof(stats->m_spellId) / sizeof(stats->m_spellId[0]); i++) {
        if (stats->m_spellId[i] && !stats->m_spellTrigger[i]) {
            return stats->m_spellId[i];
        }
    }

    return 0;
}

// VERIFY: 35a89283 auto
// OFFSET: 0x00709E20
void CGItem_C::RemoveNameFromCache(const char* name) {
    ITEMBYNAME* entry = s_itemByName.Ptr(name);

    if (entry) {
        s_itemByName.Delete(entry);
    }
}

// VERIFY: 22bc7913 auto
// OFFSET: 0x00707E90
void CGItem_C::GetSpellCooldownInfo(uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category) {
    cooldown = 0;
    categoryCooldown = 0;
    category = 0;

    if (this->GetSpellItemEnchantmentId()) {
        SpellRec spell;

        if (g_spellDB.GetRecord(this->GetEnchantmentSpell(0), &spell)) {
            cooldown = spell.m_recoveryTime;
            categoryCooldown = spell.m_categoryRecoveryTime;
            category = spell.m_category;
            return;
        }
    }

    CGItem_C::GetCooldowns(this->m_obj->m_entryID, cooldown, categoryCooldown, category);
}

// VERIFY: 93d9343c manual
// OFFSET: 0x0070A850
static void AddInventoryArtHash(int32_t displayId, const char* texture) {
    if (s_inventoryArt.Ptr(displayId, HASHKEY_NONE())) {
        return;
    }

    INVENTORYART* art = s_inventoryArt.New(displayId, HASHKEY_NONE(), 0, 0);

    if (art->m_texture) {
        SMemFree(art->m_texture, __FILE__, __LINE__, 0);
        art->m_texture = nullptr;
    }

    if (texture && *texture) {
        art->m_texture = SStrDupA(texture, __FILE__, __LINE__);
    }
}

// VERIFY: a1157b4b auto
// OFFSET: 0x0070A910
const char* CGItem_C::GetInventoryArt(int32_t displayId) {
    INVENTORYART* art = s_inventoryArt.Ptr(displayId, HASHKEY_NONE());

    if (art && art->m_texture) {
        return art->m_texture;
    }

    ItemDisplayInfoRec display;

    if (g_itemDisplayInfoDB.GetRecord(displayId, &display)) {
        const char* icon = ViolenceGetLevel() >= 2 || !*display.m_inventoryIcon[1] ? display.m_inventoryIcon[0] : display.m_inventoryIcon[1];

        if (icon) {
            if (*icon) {
                char buffer[260];

                if (TextureDiscoverFileType(icon) == TEXFILE_TGA) {
                    TexturePickAlternateFilename(icon, TEXFILE_TGA, buffer, sizeof(buffer));
                    icon = buffer;
                }

                AddInventoryArtHash(displayId, icon);
                art = s_inventoryArt.Ptr(displayId, HASHKEY_NONE());
                icon = art ? art->m_texture : nullptr;
            }

            if (icon && *icon) {
                return icon;
            }
        }
    }

    SysMsgPrintf(STATUS_ERROR, 2, "NOINVENTORYICON|%d", displayId);
    return "INV_Misc_QuestionMark";
}
