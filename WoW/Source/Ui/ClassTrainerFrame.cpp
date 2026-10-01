#include "ClassTrainerFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include <cstdlib>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/SpellBookFrame.h"

// STATIC: 0x00C0E490
WOWGUID CGClassTrainer::m_trainer;

// STATIC: 0x00C0DC90
char CGClassTrainer::m_greetingText[2048];

// STATIC: 0x00C0E498
int32_t CGClassTrainer::m_trainerType;

// STATIC: 0x00C0E49C
int32_t CGClassTrainer::m_currentSelection;

// STATIC: 0x00C0E4A0
uint32_t CGClassTrainer::m_numServices;

// STATIC: 0x00C0E4A4
int32_t CGClassTrainer::m_numSkillLines;

// STATIC: 0x00C0E4A8
uint32_t CGClassTrainer::m_filteredServices;

// STATIC: 0x00C0E4AC
uint32_t CGClassTrainer::m_skillLineFilter;

// STATIC: 0x00C0E4B0
uint32_t CGClassTrainer::m_expandedSkillLines;

// STATIC: 0x00C0E4B4
TSGrowableArray<TrainerServiceInfo*> CGClassTrainer::m_services;

// STATIC: 0x00C0E4C4
TSGrowableArray<TrainerSkillLineInfo*> CGClassTrainer::m_skillLines;

// VERIFY: 03233b9e auto
// OFFSET: 0x005EEB70
void CGClassTrainer::InitializeGame() {
}

// VERIFY: a50c2177 auto
// OFFSET: 0x005940E0
void CGClassTrainer::SetTrainer(const WOWGUID& trainer, int32_t type) {
    if (trainer == CGClassTrainer::m_trainer) {
        return;
    }

    if (!trainer) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_CLOSED, nullptr);
        CGGameUI::ClearInteractTarget(CGClassTrainer::m_trainer, 0);
        CGClassTrainer::m_trainer = WOWGUID();
        CGClassTrainer::m_numSkillLines = 0;
        CGClassTrainer::m_numServices = 0;
        return;
    }

    if (trainer != ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetInteractTarget(trainer);
    }

    CGClassTrainer::m_trainer = trainer;
    CGClassTrainer::m_trainerType = type;
}

// VERIFY: d723d8b2 auto
// OFFSET: 0x00593CC0
void CGClassTrainer::EnterWorld() {
    CGClassTrainer::m_trainer = WOWGUID{ 0 };
    CGClassTrainer::m_currentSelection = 0;
    CGClassTrainer::m_numServices = 0;
    CGClassTrainer::m_greetingText[0] = '\0';
}

// VERIFY: 53d4c1b1 manual
// OFFSET: 0x005947D0
static int32_t QSortServices_General(const void* a, const void* b) {
    const TrainerServiceInfo* serviceA = *static_cast<TrainerServiceInfo* const*>(a);
    const TrainerServiceInfo* serviceB = *static_cast<TrainerServiceInfo* const*>(b);

    if (!serviceA->m_visible || !serviceB->m_visible) {
        if (!serviceA->m_visible && !serviceB->m_visible) {
            return 0;
        }

        return 2 * (serviceB->m_visible != 0) - 1;
    }

    if (CGClassTrainer::m_numSkillLines) {
        int32_t lineA = 0;
        int32_t lineB = 0;

        for (uint32_t i = 0; i < static_cast<uint32_t>(CGClassTrainer::m_numSkillLines); i++) {
            if (CGClassTrainer::m_skillLines.m_data[i]->m_skillLine == serviceA->m_skillLine) {
                lineA = i;
            }

            if (CGClassTrainer::m_skillLines.m_data[i]->m_skillLine == serviceB->m_skillLine) {
                lineB = i;
            }
        }

        if (lineA != lineB) {
            return 2 * (lineA > lineB) - 1;
        }
    }

    if (serviceA->m_spellID < 0 || serviceB->m_spellID < 0) {
        return 2 * (serviceA->m_spellID >= 0) - 1;
    }

    SpellRec spellA;
    SpellRec spellB;

    if (!g_spellDB.GetRecord(serviceA->m_spellID, &spellA) || !g_spellDB.GetRecord(serviceB->m_spellID, &spellB)) {
        return 0;
    }

    if (serviceB->m_levelReq != serviceA->m_levelReq) {
        return serviceB->m_levelReq < serviceA->m_levelReq ? 1 : -1;
    }

    if (static_cast<uint32_t>(serviceB->m_skillRankReq) != static_cast<uint32_t>(serviceA->m_skillRankReq)) {
        return static_cast<uint32_t>(serviceB->m_skillRankReq) < static_cast<uint32_t>(serviceA->m_skillRankReq) ? 1 : -1;
    }

    int32_t result = SStrSortUTF8I(spellA.m_name, spellB.m_name, STORM_MAX_STR);

    if (!result) {
        result = SStrSortUTF8I(spellA.m_nameSubtext, spellB.m_nameSubtext, STORM_MAX_STR);
    }

    return result;
}

// VERIFY: 5cc286cb manual
// OFFSET: 0x005949A0
static int32_t QSortServices_Tradeskill(const void* a, const void* b) {
    const TrainerServiceInfo* serviceA = *static_cast<TrainerServiceInfo* const*>(a);
    const TrainerServiceInfo* serviceB = *static_cast<TrainerServiceInfo* const*>(b);

    if (!serviceA->m_visible || !serviceB->m_visible) {
        if (!serviceA->m_visible && !serviceB->m_visible) {
            return 0;
        }

        return 2 * (serviceB->m_visible != 0) - 1;
    }

    if (serviceA->m_skillLine != serviceB->m_skillLine) {
        return 2 * (serviceA->m_skillLine > serviceB->m_skillLine) - 1;
    }

    if (serviceA->m_spellID < 0 || serviceB->m_spellID < 0) {
        return 2 * (serviceA->m_spellID >= 0) - 1;
    }

    SpellRec spellA;
    SpellRec spellB;

    if (!g_spellDB.GetRecord(serviceA->m_spellID, &spellA) || !g_spellDB.GetRecord(serviceB->m_spellID, &spellB)) {
        return 0;
    }

    if (static_cast<uint32_t>(serviceB->m_skillRankReq) != static_cast<uint32_t>(serviceA->m_skillRankReq)) {
        return static_cast<uint32_t>(serviceB->m_skillRankReq) < static_cast<uint32_t>(serviceA->m_skillRankReq) ? 1 : -1;
    }

    return SStrSortUTF8I(spellA.m_name, spellB.m_name, STORM_MAX_STR);
}

// VERIFY: f786a100 manual
// OFFSET: 0x00594BA0
void CGClassTrainer::FilterAndSortServices() {
    CGClassTrainer::m_filteredServices = CGClassTrainer::m_numServices;
    int32_t filter = g_serviceTypeFilterCVar->m_intValue;

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGClassTrainer::m_numSkillLines); i++) {
        int32_t visible = 0;

        for (uint32_t state = 0; state < 6; state++) {
            if (((1 << state) & filter) && CGClassTrainer::m_skillLines.m_data[i]->m_stateCounts[state]) {
                visible = ((1 << i) & CGClassTrainer::m_skillLineFilter) != 0;
                break;
            }
        }

        CGClassTrainer::m_skillLines.m_data[i]->m_visible = visible;
        CGClassTrainer::m_skillLines.m_data[i]->m_collapsed = (CGClassTrainer::m_expandedSkillLines & (1 << i)) == 0;
    }

    for (uint32_t i = 0; i < CGClassTrainer::m_numServices; i++) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

        if (service->m_spellID >= 0 && !((1 << service->m_state) & filter)) {
            service->m_visible = 0;
            CGClassTrainer::m_filteredServices--;
            continue;
        }

        service->m_visible = 1;

        if (!CGClassTrainer::m_numSkillLines) {
            continue;
        }

        uint32_t index = 0;

        for (uint32_t j = 0; j < static_cast<uint32_t>(CGClassTrainer::m_numSkillLines); j++) {
            if (CGClassTrainer::m_skillLines.m_data[j]->m_skillLine == service->m_skillLine) {
                index = j;
                break;
            }
        }

        TrainerSkillLineInfo* line = CGClassTrainer::m_skillLines.m_data[index];

        if (!line->m_visible || (service->m_spellID >= 0 && line->m_collapsed)) {
            service->m_visible = 0;
            CGClassTrainer::m_filteredServices--;
        }
    }

    if (CGClassTrainer::m_trainerType == 2) {
        qsort(CGClassTrainer::m_services.m_data, CGClassTrainer::m_numServices, sizeof(TrainerServiceInfo*), QSortServices_Tradeskill);
    } else {
        qsort(CGClassTrainer::m_services.m_data, CGClassTrainer::m_numServices, sizeof(TrainerServiceInfo*), QSortServices_General);
    }

    uint32_t i;

    for (i = 0; i < CGClassTrainer::m_numServices; i++) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

        if (service && service->m_spellID > 0 && !service->m_state && service->m_visible) {
            break;
        }
    }

    if (i < CGClassTrainer::m_numServices) {
        CGClassTrainer::m_currentSelection = CGClassTrainer::m_services.m_data[i]->m_spellID;

        if (CGClassTrainer::m_currentSelection < 0) {
            CGClassTrainer::m_currentSelection = 0;
        }
    }

    if (i == CGClassTrainer::m_numServices) {
        CGClassTrainer::m_currentSelection = CGClassTrainer::m_numServices > 1 ? CGClassTrainer::m_services.m_data[1]->m_spellID : 0;

        if (CGClassTrainer::m_currentSelection < 0) {
            CGClassTrainer::m_currentSelection = 0;
        }
    }
}

// VERIFY: c9bb5ec3 manual
// OFFSET: 0x00596B00
void CGClassTrainer::RefreshList() {
    if (!CGClassTrainer::m_trainer) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t s = 0; s < CGClassTrainer::m_numServices; s++) {
        if (CGClassTrainer::m_services.m_data[s]->m_spellID == -1) {
            continue;
        }

        SpellRec spell;

        if (!g_spellDB.GetRecord(CGClassTrainer::m_services.m_data[s]->m_spellID, &spell)) {
            continue;
        }

        CGClassTrainer::m_services.m_data[s]->m_state = 0;
        bool teaches = false;
        int32_t total = 0;
        int32_t known = 0;

        for (uint32_t e = 0; e < 3; e++) {
            int32_t effect = spell.m_effect[e];

            if (!effect) {
                continue;
            }

            if (effect == 44) {
                int32_t minPoints;
                int32_t maxPoints;
                Spell_C_GetMinMaxPoints(&spell, e, &minPoints, &maxPoints, 0, 0, 0, 0);
                total++;

                for (int32_t i = 0; i < 128; i++) {
                    uint16_t skillId = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerSkillInfo[i].m_skillId : 0;

                    if (skillId == spell.m_effectMiscValue[e]) {
                        uint16_t step = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerSkillInfo[i].m_step : 0;

                        if (step >= maxPoints) {
                            known++;
                        }

                        break;
                    }
                }

                continue;
            }

            int32_t spellId;

            if (effect == 36 && spell.m_implicitTargetA[e] != 5) {
                spellId = spell.m_effectTriggerSpell[e];
                teaches = true;
            } else {
                if (teaches) {
                    continue;
                }

                spellId = spell.m_id;
            }

            total++;

            if (player->IsSpellKnown(spellId) || player->IsSpellSuperceded(spellId)) {
                known++;
            }

            if (spellId == spell.m_id) {
                break;
            }
        }

        if (total > 0 && known == total) {
            CGClassTrainer::m_services.m_data[s]->m_state = 2;
        }

        if (CGClassTrainer::m_services.m_data[s]->m_state) {
            continue;
        }

        if (CGClassTrainer::m_services.m_data[s]->m_skillReq) {
            int32_t maxRank = 0;

            for (int32_t i = 0; i < 128; i++) {
                uint16_t skillId = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerSkillInfo[i].m_skillId : 0;

                if (skillId == CGClassTrainer::m_services.m_data[s]->m_skillReq) {
                    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                        maxRank = player->m_player->m_playerSkillInfo[i].m_maxRank;

                        if (maxRank > 0) {
                            maxRank += player->m_player->m_playerSkillInfo[i].m_permBonus;
                        }
                    } else {
                        maxRank = 0;
                    }

                    if (static_cast<int32_t>(player->GetSkillValue(i)) < CGClassTrainer::m_services.m_data[s]->m_skillRankReq) {
                        CGClassTrainer::m_services.m_data[s]->m_state = 1;
                    }

                    break;
                }
            }

            SpellRec stepSpell;

            if (g_spellDB.GetRecord(CGClassTrainer::m_services.m_data[s]->m_stepSpellID, &stepSpell)) {
                for (uint32_t e = 0; e < 3; e++) {
                    if (stepSpell.m_effect[e] == 44 && stepSpell.m_effectMiscValue[e] == CGClassTrainer::m_services.m_data[s]->m_skillReq) {
                        int32_t minPoints;
                        int32_t maxPoints;
                        Spell_C_GetMinMaxPoints(&stepSpell, e, &minPoints, &maxPoints, 0, 0, 0, 0);

                        if (maxRank < 5 * maxPoints) {
                            CGClassTrainer::m_services.m_data[s]->m_state = 1;
                        }

                        break;
                    }
                }
            }
        }

        if (CGClassTrainer::m_services.m_data[s]->m_state) {
            continue;
        }

        bool abilitiesKnown = true;

        for (uint32_t i = 0; i < 3; i++) {
            int32_t ability = CGClassTrainer::m_services.m_data[s]->m_abilityReq[i];

            if (ability && !player->IsSpellKnown(ability) && !player->IsSpellSuperceded(ability)) {
                abilitiesKnown = false;
                break;
            }
        }

        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[s];

        if (CGSpellBook::IsUnlearnedSpell(service->m_spellID)) {
            service->m_state = 2;
        } else if (!abilitiesKnown) {
            service->m_state = 1;
        } else if (service->m_levelReq > 1 && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < service->m_levelReq) {
            service->m_state = 1;
        }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGClassTrainer::m_numSkillLines); i++) {
        TrainerSkillLineInfo* line = CGClassTrainer::m_skillLines.m_data[i];

        for (uint32_t state = 0; state < 6; state++) {
            line->m_stateCounts[state] = 0;
        }

        line->unk_0024 = 1;
    }

    for (uint32_t s = 0; s < CGClassTrainer::m_numServices; s++) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[s];

        if (service->m_spellID == -1) {
            continue;
        }

        for (uint32_t i = 0; i < static_cast<uint32_t>(CGClassTrainer::m_numSkillLines); i++) {
            if (CGClassTrainer::m_skillLines.m_data[i]->m_skillLine == service->m_skillLine) {
                CGClassTrainer::m_skillLines.m_data[i]->m_stateCounts[service->m_state]++;
                break;
            }
        }
    }

    CGClassTrainer::FilterAndSortServices();
    FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_UPDATE, nullptr);
}

// VERIFY: a07b4562 manual
// OFFSET: 0x005963C0
void CGClassTrainer::ShutdownGame() {
    for (uint32_t i = 0; i < CGClassTrainer::m_services.m_count; i++) {
        delete CGClassTrainer::m_services.m_data[i];
    }

    CGClassTrainer::m_services.m_count = 0;

    for (uint32_t i = 0; i < CGClassTrainer::m_skillLines.m_count; i++) {
        delete CGClassTrainer::m_skillLines.m_data[i];
    }

    CGClassTrainer::m_skillLines.m_count = 0;
}
