#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include <windows.h>
#include "_Triage/PendingSpellCastData.h"

class PendingSpellCast {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ PendingSpellCastData m_data;
    /* 0138 */ uint32_t unk_0138;
    /* 013C */ uint32_t m_dwordC;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint32_t unk_0144;
};

class SpellHistory {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C[3];
};

class Spell_C {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
};

int32_t Spell_C_IsSpellInTransit(WOWGUID caster, uint32_t spellId, uint32_t castTime);

void Spell_C_CancelSpellByTarget(const WOWGUID& target, int32_t reason);

void Spell_C_CancelSpellByCaster(const WOWGUID& caster);
void Spell_C_RemovePeriodicClientTrigger(WOWGUID guid, int32_t a2);

#endif
