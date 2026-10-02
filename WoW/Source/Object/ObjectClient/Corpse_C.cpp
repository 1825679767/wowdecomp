#include "Corpse_C.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"
#include "_Triage/CClientObjCreate.h"

// VERIFY: 91aabd2d manual
// OFFSET: 0x00705950
static void GuildTabardCallback(int32_t guildID, const WOWGUID& guid, void* param, bool success) {
    WOWGUID* corpseGuid = static_cast<WOWGUID*>(param);
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(*corpseGuid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->ApplyGuildColor(0);
    }

    delete corpseGuid;
}

// VERIFY: 9d2370ce manual
// OFFSET: 0x00706430
CGCorpse_C::CGCorpse_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_move.m_status.m_transportGuid, create.m_move.m_status.m_transportPosition, this->m_obj->m_guid), m_component(nullptr), m_groundNormal(0.0f, 0.0f, 1.0f), m_corpseFlags(0), m_selectionRadius(0.0f), m_lootEffect(nullptr) {
    this->m_passenger.m_facing = create.m_move.m_status.m_transportFacing;
}

// VERIFY: 02b25264 manual
// OFFSET: 0x007059A0
void CGCorpse_C::ApplyGuildColor(int32_t request) {
    if (!this->m_component) {
        return;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, this->m_corpse->m_corpseFieldGuild, nullptr, nullptr, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        this->m_component->ApplyGuildColor(emblemStyle, emblemColor, borderStyle, borderColor, background);
    } else if (request) {
        WOWGUID* corpseGuid = new (SMemAlloc(sizeof(WOWGUID), __FILE__, __LINE__, 0)) WOWGUID(this->m_obj->m_guid);
        GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, this->m_corpse->m_corpseFieldGuild, &GuildTabardCallback, corpseGuid, emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// VERIFY: 54fbf329 manual
// OFFSET: 0x00705B20
void CGCorpse_C::PostInit(uint32_t time, const CClientObjCreate& create, bool a3) {
    this->m_passenger.PostInit(create.m_move.m_status.m_position);

    if (!this->GetTransportGUID()) {
        this->m_passenger.m_facing = create.m_move.m_status.m_facing;
    }

    this->CGObject_C::PostInit(time, create, a3);

    CM2Model* model = this->m_worldModel;
    int32_t useComponent = 1;
    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldDisplayId);
    CreatureModelDataRec* modelRec = nullptr;

    if (displayRec) {
        modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

        if (modelRec && !(modelRec->m_flags & 0x4)) {
            useComponent = 0;
        }
    }

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1)) {
        if (useComponent) {
            this->m_component = CCharacterComponent::AllocComponent();

            ComponentData data;
            data.m_preferences.m_raceID = this->m_corpse->m_corpseFieldBytes1[1];
            data.m_preferences.m_sexID = this->m_corpse->m_corpseFieldBytes1[2];
            data.m_preferences.m_skinID = this->m_corpse->m_corpseFieldBytes1[3];
            data.m_preferences.m_faceId = this->m_corpse->m_corpseFieldBytes2[0];
            data.m_preferences.m_hairStyleId = this->m_corpse->m_corpseFieldBytes2[1];
            data.m_preferences.m_hairColorId = this->m_corpse->m_corpseFieldBytes2[2];
            data.m_preferences.m_facialHairId = this->m_corpse->m_corpseFieldBytes2[3];
            data.m_model = model;

            uint32_t activePlayer = this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer();
            model->m_refCount++;
            data.m_flags ^= (data.m_flags ^ (activePlayer << 1)) & 0x2;

            this->m_component->Init(&data, nullptr);
        } else {
            CCharacterComponent::ReplaceMonsterSkin(model, displayRec, modelRec);
        }
    }

    CMapEntity* entity = this->m_worldObject;
    C3Vector position = this->GetPosition();
    int32_t liquidType;
    float height;
    int32_t liquidFlags;

    if (World::QueryObjectLiquid(entity, &liquidType, &height, &liquidFlags) && height - position.z > 0.6666667f) {
        this->m_corpseFlags |= 0x2;
        model->SetBoneSequence(0xFFFFFFFF, 132, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else {
        this->m_corpseFlags &= ~0x2;
        model->SetBoneSequence(0xFFFFFFFF, 6, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }

    if (this->m_component) {
        for (uint32_t slot = 0; slot < 19; slot++) {
            ItemDisplayInfoRec rec;

            if (!g_itemDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldItem[slot] & 0xFFFFFF, &rec) || slot == 17) {
                continue;
            }

            if (slot == 15 || slot == 16) {
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID{ this->m_corpse->m_corpseFieldItem[slot] }, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item) {
                    CM2Model* worldModel = this->m_worldModel;
                    bool shield = item->GetInventoryType() == 14;
                    CCharacterComponent::AddHandItem(worldModel, &rec, static_cast<INVENTORY_SLOTS>(slot), static_cast<SHEATHE_TYPE>(item->GetSheatheType()), false, shield, false, 0);
                }

                continue;
            }

            if (slot == 0) {
                if (this->m_corpse->m_corpseFieldFlags & 0x8) {
                    continue;
                }
            } else if (slot == 14 && (this->m_corpse->m_corpseFieldFlags & 0x10)) {
                continue;
            }

            this->m_component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), rec.m_id, 0);

            if (slot == 18 && (rec.m_flags & 0x1)) {
                this->ApplyGuildColor(1);
            }
        }
    }

    if ((this->m_corpse->m_corpseFieldDynamicFlags & 0x1) && !this->m_lootEffect) {
        const SpellVisualEffectNameRec* effect = GetSpecialSpellVisualEffectNameRec(4);
        this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(effect->m_fileName, 0);

        if (this->m_lootEffect) {
            this->m_lootEffect->AttachToParent(this->m_worldModel, 19, nullptr, 0);
        }
    }

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1) && this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetActiveCorpse(this->m_obj->m_guid);
    }
}

// VERIFY: 638a971b manual
// OFFSET: 0x007065B0
CGCorpse_C::~CGCorpse_C() {
    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
    }

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = nullptr;
    }
}

// VERIFY: e8b3bd0d auto
// OFFSET: 0x007057A0
const char* CGCorpse_C::GetPortraitTexture() const {
    uint32_t displayId = this->m_corpse->m_corpseFieldDisplayId;

    if (!displayId) {
        return nullptr;
    }

    CreatureDisplayInfoRec* rec = g_creatureDisplayInfoDB.GetRecord(displayId);

    return rec ? rec->m_portraitTextureName : nullptr;
}

// VERIFY: e4a2c418 auto
// OFFSET: 0x007058B0
int32_t CGCorpse_C::ShouldFadeout() {
    return !this->m_component || (TextureIsReadyNow(this->m_component->m_baseSkinTexture) && this->m_component->RenderPrep(0));
}

// VERIFY: 761d4ea2 auto
// OFFSET: 0x007058F0
int32_t CGCorpse_C::CanBeLooted() const {
    return this->m_corpse->m_corpseFieldDynamicFlags & 0x1;
}

// VERIFY: 78f995ee auto
// OFFSET: 0x00705A80
uint32_t CGCorpse_C::GetFactionTemplate() const {
    ChrRacesRec* rec = g_chrRacesDB.GetRecord(this->m_corpse->m_corpseFieldBytes1[1]);

    return rec ? rec->m_factionID : 0;
}

// VERIFY: 1cfc7c8d auto
// OFFSET: 0x00705AC0
float CGCorpse_C::GetDisplayScale() const {
    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldDisplayId);

    if (displayRec) {
        CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

        if (modelRec) {
            return modelRec->m_modelScale * displayRec->m_creatureModelScale;
        }
    }

    return 1.0f;
}

// VERIFY: 242bdafd auto
// OFFSET: 0x00706570
float CGCorpse_C::GetTrueScale() const {
    return this->GetDisplayScale() * this->m_scaleModifier * this->m_scale;
}

// VERIFY: e46b7590 manual
// OFFSET: 0x00706590
int32_t CGCorpse_C::CanHighlight() const {
    return !(this->m_corpse->m_corpseFieldFlags & 0x1) || (this->m_corpse->m_corpseFieldDynamicFlags & 0x1);
}

// VERIFY: 5ae4217a auto
// OFFSET: 0x007062C0
void CGCorpse_C::Initialize() {
    ClntObjMgrSetTypeMirrorHandler(ID_CORPSE, offsetof(CGCorpseData, m_corpseFieldFlags), sizeof(uint32_t), CorpseFlagUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_CORPSE, offsetof(CGCorpseData, m_corpseFieldDynamicFlags), sizeof(uint32_t), CorpseDynamicFlagUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}

// VERIFY: b1c4d0d1 auto
// OFFSET: 0x00705900
void CGCorpse_C::ShowLootEffect() {
    if (this->m_lootEffect) {
        return;
    }

    const SpellVisualEffectNameRec* effect = GetSpecialSpellVisualEffectNameRec(4);
    this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(effect->m_fileName, 0);

    if (this->m_lootEffect) {
        this->m_lootEffect->AttachToParent(this->m_worldModel, 19, nullptr, 0);
    }
}

// VERIFY: 865254c2 auto
// OFFSET: 0x00706120
void CGCorpse_C::OnFlagChanged(uint32_t oldFlags) {
    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = nullptr;
    }

    this->Fadeout();

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = nullptr;
    }

    this->m_corpseFlags &= ~0x1;
    const char* fileName;

    if (!this->GetModelFileName(&fileName)) {
        return;
    }

    CM2Model* model = World::s_m2Scene->CreateModel(fileName, 0);
    this->SetModel(model);

    if (this->m_corpse->m_corpseFieldDynamicFlags & 0x1) {
        this->ShowLootEffect();
    }

    if (model) {
        model->Release();
    }

    this->AddWorldObject();
}

// VERIFY: b8476d46 auto
// OFFSET: 0x007061E0
void CGCorpse_C::OnDynamicFlagChanged(uint32_t oldFlags) {
    if (!((static_cast<uint8_t>(oldFlags) ^ this->m_corpse->m_corpseFieldDynamicFlags) & 0x1)) {
        return;
    }

    if (this->m_corpse->m_corpseFieldDynamicFlags & 0x1) {
        this->ShowLootEffect();
        return;
    }

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = nullptr;
    }
}

// VERIFY: bab5ab7f manual
// OFFSET: 0x00706240
int32_t CorpseFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->OnFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// VERIFY: c5f353fa manual
// OFFSET: 0x00706280
int32_t CorpseDynamicFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->OnDynamicFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// VERIFY: 12175e66 auto
// OFFSET: 0x00706530
float CGCorpse_C::GetFacing() const {
    return this->m_passenger.GetFacing(this->m_passenger.m_facing);
}

// VERIFY: 63ae9671 manual
// OFFSET: 0x00705850
void CGCorpse_C::ModelLoaded(CM2Model* model) {
    this->CGObject_C::ModelLoaded(model);

    if (this->m_corpseFlags & 0x2) {
        model->SetBoneSequence(0xFFFFFFFF, 132, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else {
        model->SetBoneSequence(0xFFFFFFFF, 6, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }

    this->m_selectionRadius = model->GetBoundingSphere().r * 0.5f;
}
