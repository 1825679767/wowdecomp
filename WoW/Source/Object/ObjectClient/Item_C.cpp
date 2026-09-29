#include "Item_C.h"
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
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include <ctime>

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
