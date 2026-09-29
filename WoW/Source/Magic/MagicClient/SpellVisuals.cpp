#include "SpellVisuals.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/Lightning.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00D39750
static CLightningManager* s_lightningManager;

// STATIC: 0x00AF5090
static int32_t s_unk_AF5090 = -1;

// STATIC: 0x00AF5094
static TSList<BlizzardObject> s_blizzardObjects;

// STATIC: 0x00AF50A0
static TSList<LightningObject> s_lightningObjects;

// STATIC: 0x00D3975C
static CDataAllocator s_blizzardObjectHeap(sizeof(BlizzardObject), 8);

// STATIC: 0x00D39770
static CDataAllocator s_shardHeap(sizeof(Shard), 32);

// STATIC: 0x00D39758
static TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>* s_modelAttachLookup;

// VERIFY: 1522f2db manual
// OFFSET: 0x007FBB80
void SpellVisualsShutdownModelAttachLookup() {
    if (!s_modelAttachLookup) {
        return;
    }

    uint32_t count = s_modelAttachLookup->Count();

    for (uint32_t i = 0; i < count; i++) {
        if ((*s_modelAttachLookup)[i]) {
            delete (*s_modelAttachLookup)[i];
        }
    }

    delete s_modelAttachLookup;
    s_modelAttachLookup = nullptr;
}

// VERIFY: f470cb1c manual
// OFFSET: 0x007FBE90
void SpellVisualsInitializeModelAttachLookup() {
    SpellVisualsShutdownModelAttachLookup();

    uint32_t maxKitID = g_spellVisualKitDB.m_maxID <= 0 ? 0 : g_spellVisualKitDB.m_maxID;
    uint32_t maxAttachID = g_spellVisualKitModelAttachDB.m_maxID <= 0 ? 0 : g_spellVisualKitModelAttachDB.m_maxID;

    s_modelAttachLookup = new (SMemAlloc(sizeof(TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>), __FILE__, __LINE__, 0)) TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>();
    s_modelAttachLookup->GrowToFit(maxKitID, 1);

    for (uint32_t id = 1; id <= maxAttachID; id++) {
        const SpellVisualKitModelAttachRec* rec = g_spellVisualKitModelAttachDB.GetRecord(id);

        if (!rec || !rec->m_parentSpellVisualKitID || static_cast<uint32_t>(rec->m_parentSpellVisualKitID) > maxKitID) {
            continue;
        }

        TSGrowableArray<const SpellVisualKitModelAttachRec*>*& kitAttachments = (*s_modelAttachLookup)[rec->m_parentSpellVisualKitID];

        if (!kitAttachments) {
            kitAttachments = new (SMemAlloc(sizeof(TSGrowableArray<const SpellVisualKitModelAttachRec*>), __FILE__, __LINE__, 0)) TSGrowableArray<const SpellVisualKitModelAttachRec*>();
        }

        kitAttachments->New(rec);
    }
}

// VERIFY: 95aa500d manual
// OFFSET: 0x007FC5A0
void SpellVisualsInitialize() {
    s_lightningManager = new (SMemAlloc(sizeof(CLightningManager), __FILE__, __LINE__, 0)) CLightningManager();
    s_spellEffectLevelVar = CVar::Lookup("spellEffectLevel");
    SpellVisualsInitializeModelAttachLookup();
}

// VERIFY: 1702458f manual
// OFFSET: 0x007FC9F0
void SpellVisualsClear() {
    s_lightningObjects.Clear();

    BlizzardObject* object;

    while ((object = s_blizzardObjects.Head()) != nullptr) {
        object->~BlizzardObject();
        s_blizzardObjectHeap.PutData(object, nullptr, 0);
    }
}

// VERIFY: 9c3b0607 auto
// OFFSET: 0x007FCBC0
void SpellVisualsShutdown() {
    SpellVisualsClear();

    if (s_lightningManager) {
        delete s_lightningManager;
    }

    s_unk_AF5090 = -1;
    s_blizzardObjectHeap.Clear(typeid(BlizzardObject).raw_name(), -2, 1);
    s_shardHeap.Clear(typeid(Shard).raw_name(), -2, 1);
    s_spellEffectLevelVar = nullptr;
    SpellVisualsShutdownModelAttachLookup();
}

// VERIFY: c2183fd9 auto
// OFFSET: 0x007F9EE0
void SpellVisualsBlizzardStop(BlizzardObject** object) {
    (*object)->unk_0124 = 0.0f;
    *object = nullptr;
}
