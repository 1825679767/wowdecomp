#include "UIMacros.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/UIMacroOptions.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"

// STATIC: 0x00ACE2A0
static TSExplicitList<MACRONODE, offsetof(MACRONODE, m_dynamicLink)> s_dynamicMacros;

// STATIC: 0x00BEAE20
uint32_t CGUIMacros::m_sortedMacros[72];

// STATIC: 0x00BEAF68
static TSHashTable<MACRONODE, HASHKEY_NONE> s_macros;

// STATIC: 0x00ACE1E8
static const char* s_macroIconPath = "Interface\\Icons\\";

// STATIC: 0x00BEAF44
uint32_t CGUIMacros::m_state;

// STATIC: 0x00BEAF48
const char* CGUIMacros::m_macroRunning;

// STATIC: 0x00BEAF4C
MACRONODE* CGUIMacros::m_macroButton;

// STATIC: 0x00BEAF50
uint32_t CGUIMacros::m_numMacros;

// STATIC: 0x00BEAF54
uint32_t CGUIMacros::m_numCharacterMacros;

// STATIC: 0x00BEAF60
static uint16_t s_initCounter;

// STATIC: 0x00BEAF64
static uint32_t s_pendingLoads;

// VERIFY: 395f9dc1 auto
// OFFSET: 0x00564AB0
int32_t CGUIMacros::GetMacroIndexByID(uint32_t id) {
    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(CGUIMacros::m_sortedMacros) / sizeof(CGUIMacros::m_sortedMacros[0])); i++) {
        if (CGUIMacros::m_sortedMacros[i] == id) {
            return i;
        }
    }

    return -1;
}

// VERIFY: 3cc88c82 auto
// OFFSET: 0x005660E0
void MACRONODE::IncrementActionBarCount() {
    if (this->m_dynamic && !this->m_actionBarCount) {
        s_dynamicMacros.LinkToTail(this);
    }

    this->m_actionBarCount++;
}

// VERIFY: 462f140c manual
// OFFSET: 0x00566110
void MACRONODE::DecrementActionBarCount() {
    this->m_actionBarCount--;

    if (this->m_dynamic && !this->m_actionBarCount) {
        s_dynamicMacros.UnlinkNode(this);
    }
}

// VERIFY: 83d22513 auto
// OFFSET: 0x00566160
void CGUIMacros::UpdateMacroSpells() {
    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
        if (macro->m_spellID < 0 && macro->FindSpellID()) {
            CGActionBar::UpdateMacro(macro->m_id);
        }
    }
}

// VERIFY: e9bab52e manual
// OFFSET: 0x00566660
void CGUIMacros::UpdateDynamicMacros() {
    for (MACRONODE* macro = s_dynamicMacros.Head(); macro; macro = s_dynamicMacros.Next(macro)) {
        if (macro->FindSpellID()) {
            CGActionBar::UpdateMacro(macro->m_id);
        }
    }
}

// VERIFY: 67f7b9f9 manual
// OFFSET: 0x00566A70
MACRONODE* CGUIMacros::GetMacroByIndex(uint32_t index) {
    if (index < sizeof(CGUIMacros::m_sortedMacros) / sizeof(CGUIMacros::m_sortedMacros[0])) {
        return s_macros.Ptr(CGUIMacros::m_sortedMacros[index], HASHKEY_NONE());
    }

    return nullptr;
}

// VERIFY: 01debf71 auto
// OFFSET: 0x00566AA0
MACRONODE* CGUIMacros::GetMacroByID(uint32_t id) {
    return s_macros.Ptr(id, HASHKEY_NONE());
}

// VERIFY: 1c1a9d37 manual
// OFFSET: 0x00566AC0
void CGUIMacros::GetFullIconPath(uint32_t id, char* buffer, uint32_t size) {
    *buffer = 0;
    MACRONODE* macro = s_macros.Ptr(id, HASHKEY_NONE());

    if (!macro) {
        return;
    }

    if (macro->m_dynamic && !macro->m_actionBarCount) {
        macro->FindSpellID();
    }

    if (!SStrCmp(macro->m_icon, "INV_Misc_QuestionMark", STORM_MAX_STR)) {
        if (macro->m_itemID) {
            const char* path = ClientDBStringLookup(3);
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(macro->m_itemID, none, nullptr, nullptr, false);
            int32_t displayId = stats ? stats->m_displayId : 0;
            SStrPrintf(buffer, size, "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
            return;
        }

        if (macro->m_spellID) {
            SpellRec spell;

            if (g_spellDB.GetRecord(macro->m_spellID, &spell)) {
                int32_t iconId = spell.m_spellIconID;

                if (spell.m_activeIconID) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(macro->m_spellFlags ? CGPetInfo::GetPet(0) : ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (unit) {
                        uint32_t count = unit->GetAuraCount();

                        for (uint32_t i = 0; i < count; i++) {
                            if (unit->GetAuraSpellId(i) == spell.m_id && (unit->GetAuraFlags(i) & 0x10)) {
                                iconId = spell.m_activeIconID;
                                break;
                            }
                        }
                    }
                }

                SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

                if (icon) {
                    SStrCopy(buffer, icon->m_textureFilename, size);
                    return;
                }
            }
        }
    }

    SStrPrintf(buffer, size, "%s%s", s_macroIconPath, macro->m_icon);
}

// VERIFY: ee9d285c manual
// OFFSET: 0x00566330
MACRONODE* CGUIMacros::FindMacro(const char* name) {
    if (!name || !*name) {
        return nullptr;
    }

    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
        if (!SStrCmpI(name, macro->m_name, STORM_MAX_STR)) {
            return macro;
        }
    }

    return nullptr;
}

// VERIFY: ebb01234 auto
// OFFSET: 0x0050F4D0
bool CGUIMacros::IsActionAllowed(uint32_t action) {
    return !CGUIMacros::m_macroButton || ((1 << action) & CGUIMacros::m_state);
}

// VERIFY: 2f0aa8c8 auto
// OFFSET: 0x00565AD0
void CGUIMacros::RemoveDynamicMacro(MACRONODE* macro) {
    s_dynamicMacros.UnlinkNode(macro);
}

// VERIFY: 4406f3d1 auto
// OFFSET: 0x00566140
void CGUIMacros::Initialize() {
    do {
        s_initCounter++;
    } while (!s_initCounter);

    InitializeMacroOptions();
    CGUIMacros::LoadIcons();
}

// VERIFY: 7d331447 auto
// OFFSET: 0x00567FD0
void CGUIMacros::LoadMacros() {
    s_pendingLoads = 0x30000;
    LoadAccountData(4, CGUIMacros::LoadMacrosCallback, reinterpret_cast<void*>(static_cast<uintptr_t>(s_initCounter | 0x10000)));
    LoadAccountData(5, CGUIMacros::LoadMacrosCallback, reinterpret_cast<void*>(static_cast<uintptr_t>(s_initCounter | 0x20000)));
}
