#include "Effect_C.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00ADA764
static const char* s_specialUnitEffectNames[] = {
    "HARDCODED Footstep Water Run Spray",
    "HARDCODED Footstep Water Walk Spray",
    "HARDCODED Breath Underwater",
    "HARDCODED Breath Cold",
    "HARDCODED Loot Art",
    "HARDCODED Unit Level Up",
    "HARDCODED Mount Poof",
    "HARDCODED Inebriated Bubbles",
    "HARDCODED Meeting Stone Join",
    "HARDCODED Reputation",
    "HARDCODED Resist Spell",
    "HARDCODED Achievement Base",
};

// STATIC: 0x00CA0504
static CEffect* s_effects;

// STATIC: 0x00CA0508
static CEffect* s_destroyedEffects;

// STATIC: 0x00CA050C
static int32_t s_specialUnitEffectIDs[sizeof(s_specialUnitEffectNames) / sizeof(s_specialUnitEffectNames[0])];

// VERIFY: 8db32f34 auto
// OFFSET: 0x006F7520
static int32_t BuildSpecialUnitEffectTable() {
    memset(s_specialUnitEffectIDs, 0, sizeof(s_specialUnitEffectIDs));

    for (int32_t i = 0; i < g_spellVisualEffectNameDB.m_numRecords; i++) {
        SpellVisualEffectNameRec* rec = (i < 0 || i >= g_spellVisualEffectNameDB.m_numRecords) ? nullptr : &g_spellVisualEffectNameDB.m_records[i];

        for (uint32_t effect = 0; effect < sizeof(s_specialUnitEffectNames) / sizeof(s_specialUnitEffectNames[0]); effect++) {
            if (!SStrCmp(s_specialUnitEffectNames[effect], rec->m_name, STORM_MAX_STR)) {
                s_specialUnitEffectIDs[effect] = rec->m_id;
                break;
            }
        }
    }

    return 1;
}

// VERIFY: d8c1c6a0 auto
// OFFSET: 0x006F75E0
int32_t SpecialUnitEffectsInitialize() {
    return BuildSpecialUnitEffectTable();
}

// VERIFY: 8276ea70 manual
// OFFSET: 0x006FA3C0
void CEffect::Destroy() {
    while (s_destroyedEffects) {
        CEffect* effect = s_destroyedEffects;

        if (--effect->m_refCount == 0) {
            delete effect;
        }
    }

    while (s_effects) {
        CEffect* effect = s_effects;

        if (--effect->m_refCount == 0) {
            delete effect;
        }
    }
}
