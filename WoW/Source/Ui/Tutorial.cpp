#include "Tutorial.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>

// STATIC: 0x00BD19F0
static int32_t s_tutorialsCompleted[60];

// STATIC: 0x00BD1AE0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialShown;

// STATIC: 0x00BD1AF0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialFlags;

// VERIFY: 101b433c manual
// OFFSET: 0x00530920
int32_t CGTutorial::OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    CGTutorial::m_tutorialShown.Load(data, bytes, -1);
    CGTutorial::m_tutorialFlags.Load(data, bytes, -1);

    return 1;
}

// VERIFY: fc48c140 auto
// OFFSET: 0x005EEB70
void CGTutorial::InitializeGame() {
}

// VERIFY: 964f2a5a auto
// OFFSET: 0x00530100
static int32_t AddCompletedTutorial(uint32_t tutorial) {
    int32_t index = 0;

    do {
        int32_t completed = s_tutorialsCompleted[index];

        if (completed == static_cast<int32_t>(tutorial + 1)) {
            break;
        }

        if (!completed) {
            s_tutorialsCompleted[index] = tutorial + 1;
            return index;
        }

        index++;
    } while (index < 60);

    return index;
}

// VERIFY: 6889e9a0 manual
// OFFSET: 0x00530450
void CGTutorial::FlagTutorial(uint32_t tutorial) {
    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialFlags.m_bits.m_data[index] & bit) {
        return;
    }

    CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
    CGTutorial::m_tutorialFlags.m_bits.m_data[index] |= bit;
    AddCompletedTutorial(tutorial);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TUTORIAL_FLAG));
    msg.Put(tutorial);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 2e754cd0 manual
// OFFSET: 0x00530840
void CGTutorial::TriggerTutorial(uint32_t tutorial) {
    if (!CGTutorial::m_tutorialShown.m_count) {
        return;
    }

    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialShown.m_bits.m_data[index] & bit) {
        return;
    }

    if (CVar::Lookup("showTutorials")->m_intValue) {
        SI2::PlaySoundKit("TutorialPopup", nullptr, nullptr, nullptr);
        FrameScript_SignalEvent(SCRIPT_EVENT_TUTORIAL_TRIGGER, "%d", tutorial + 1);
        CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
    } else {
        CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
        CGTutorial::FlagTutorial(tutorial);
    }
}

// VERIFY: fab94e36 manual
// OFFSET: 0x005303A0
void CGTutorial::LoadFlaggedTutorials() {
    memset(s_tutorialsCompleted, 0, sizeof(s_tutorialsCompleted));
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("flaggedTutorials");
    list.m_pos = 2;
    list.m_state = 0;
    int32_t* tutorial = s_tutorialsCompleted;

    for (uint32_t value = list.GetNextValue(); value; value = list.GetNextValue()) {
        *tutorial++ = value;
    }

    if (list.m_state != 1) {
        char text[256];
        text[0] = 'v';
        text[1] = 1;
        text[list.m_pos] = '\0';
        list.m_cvar->Set(text, true, false, false, true);
    }
}
