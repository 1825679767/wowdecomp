#include "TradeSkillFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"

// STATIC: 0x00C235B8
int32_t CGTradeSkillInfo::m_filteredSkills;

// STATIC: 0x00C235BC
char* CGTradeSkillInfo::m_itemNameFilter;

// STATIC: 0x00C235D4
int32_t CGTradeSkillInfo::m_recastSpellID;

// STATIC: 0x00C235DC
int32_t CGTradeSkillInfo::m_pendingRecastSpellID;

// STATIC: 0x00C235C0
int32_t CGTradeSkillInfo::m_itemMinLevelFilter;

// STATIC: 0x00C235C4
int32_t CGTradeSkillInfo::m_itemMaxLevelFilter;

// STATIC: 0x00C235A0
int32_t CGTradeSkillInfo::m_spellID;

// STATIC: 0x00C235A4
int32_t CGTradeSkillInfo::m_skillLineID;
// STATIC: 0x00C235A8
int32_t CGTradeSkillInfo::m_selectedSkill;

// STATIC: 0x00C235D8
uint32_t CGTradeSkillInfo::m_recastTimes;

// STATIC: 0x00C235E0
uint32_t CGTradeSkillInfo::m_pendingRecastTimes;

// STATIC: 0x00C235E4
void* CGTradeSkillInfo::m_linkData;

// VERIFY: c0b1eab0 manual
// OFFSET: 0x005D9FF0
void CGTradeSkillInfo::StopRecast() {
    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }
}

// VERIFY: da469b43 auto
// OFFSET: 0x005DA020
void UpdateTradeSkillRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_recastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }

    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }
}

// VERIFY: e58a5a53 manual
// OFFSET: 0x005DA070
void CGTradeSkillInfo::DoRecast(int32_t spellId) {
    if (!CGTradeSkillInfo::m_recastSpellID) {
        if (!CGTradeSkillInfo::m_pendingRecastSpellID) {
            return;
        }

        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
        return;
    }

    if (!CGTradeSkillInfo::m_recastTimes || CGTradeSkillInfo::m_recastSpellID != spellId) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
        return;
    }

    if (--CGTradeSkillInfo::m_recastTimes) {
        Spell_C::CastSpell(spellId, nullptr, WOWGUID{ 0 }, false);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    } else {
        CGTradeSkillInfo::m_recastSpellID = 0;
    }
}

// VERIFY: 22405cf4 auto
// OFFSET: 0x005DA0F0
void CGTradeSkillInfo::CommitPendingRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = CGTradeSkillInfo::m_pendingRecastSpellID;
        CGTradeSkillInfo::m_recastTimes = CGTradeSkillInfo::m_pendingRecastTimes;
    }

    CGTradeSkillInfo::m_pendingRecastTimes = 0;
    CGTradeSkillInfo::m_pendingRecastSpellID = 0;
}
