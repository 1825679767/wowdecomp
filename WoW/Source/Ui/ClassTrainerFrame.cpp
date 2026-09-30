#include "ClassTrainerFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

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
