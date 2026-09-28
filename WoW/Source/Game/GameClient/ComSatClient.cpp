#include "ComSatClient.h"
#include <new>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "WoW/Source/Game/GameClient/ComSatThread.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: 0x00D31AA0
COMSATDATA s_comsatData[32];

// STATIC: 0x00D37CA0
int32_t s_voiceActivation;

// STATIC: 0x00AF2A48
static ComSatClientAllocator s_comsatAllocator;

// STATIC: 0x00D37CA4
static float unk_D37CA4;

// STATIC: 0x00D37CA8
static float unk_D37CA8;

// STATIC: 0x00D37CAC
static float unk_D37CAC;

// STATIC: 0x00D37CB0
static int32_t unk_D37CB0;

// STATIC: 0x00D37CB8
static CVar* unk_D37CB8;

// STATIC: 0x00D37CBC
static CVar* unk_D37CBC;

// STATIC: 0x00D37CC0
static CVar* unk_D37CC0;

// STATIC: 0x00D37CD0
static float unk_D37CD0;

// STATIC: 0x00D37CD4
static float unk_D37CD4;

// STATIC: 0x00D37CD8
static float unk_D37CD8;

// STATIC: 0x00D37CDC
static float unk_D37CDC;

// STATIC: 0x00D37CE0
static float unk_D37CE0;

// STATIC: 0x00D37CE4
static float unk_D37CE4;

// STATIC: 0x00D37D04
int32_t s_voiceChatWasEnabled;

// STATIC: 0x00D37D34
static TSFixedArray<ComSatCallback> s_comsatCallbacks;

// STATIC: 0x00D37D54
static SMutex unk_D37D54;

// STATIC: 0x00D37D88
static TSHashTable<TALKINGPLAYER, CHashKeyGUID> unk_D37D88;

// STATIC: 0x00D37DB0
static TSHashTable<MUTEDPLAYER, CHashKeyGUID> unk_D37DB0;

// STATIC: 0x00D37D44
static TSGrowableArray<ComSatEvent> s_comSatEvents;

// VERIFY: 4b73a2c5 manual
// OFFSET: 0x007DCA90
void AddComSatEvent(uint32_t type, float value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_floatValue = value;
        event->m_type = type;
        event->m_guid = guid;
    }
}

// VERIFY: ebc8d780 manual
// OFFSET: 0x007DCB10
void AddComSatEvent(uint32_t type, uint32_t value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_type = type;
        event->m_guid = guid;
        event->m_uintValue = value;
    }
}

// VERIFY: 3ca2706b auto
// OFFSET: 0x007DC5C0
static void ResetVoiceVolumeState() {
    unk_D37CA4 = 0.0f;
    unk_D37CA8 = 0.0f;
    unk_D37CB0 = 0;
    unk_D37CAC = 0.0f;
    unk_D37CDC = 0.0f;
    unk_D37CE0 = 0.0f;
    unk_D37CE4 = 0.0f;

    if (unk_D37CB8) {
        unk_D37CD0 = unk_D37CB8->m_floatValue;
        SI2::SetChannelGroupVolume(0, unk_D37CD0);
    } else {
        unk_D37CD0 = 0.0f;
    }

    if (unk_D37CBC) {
        unk_D37CD4 = unk_D37CBC->m_floatValue;
        SI2::SetChannelGroupVolume(1, unk_D37CD4);
    } else {
        unk_D37CD4 = 0.0f;
    }

    if (unk_D37CC0) {
        unk_D37CD8 = unk_D37CC0->m_floatValue;
        SI2::SetChannelGroupVolume(2, unk_D37CD8);
    } else {
        unk_D37CD8 = 0.0f;
    }
}

// VERIFY: 3564923b manual
// OFFSET: 0x007DC730
void* ComSatClientAllocator::Alloc(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// VERIFY: 0d60f70a manual
// OFFSET: 0x007DC750
void ComSatClientAllocator::Free(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// VERIFY: bf8e209f manual
// OFFSET: 0x007DF770
static void ProcessComSatCallbacks() {
    for (uint32_t i = 0; i < s_comsatCallbacks.Count(); i++) {
        ComSatCallback& callback = s_comsatCallbacks[i];

        switch (callback.m_type) {
            case 0:
                Comsat::VoiceStart();
                break;

            case 1:
                Comsat::VoiceStop();
                break;

            case 2:
                Comsat::VoicePlateStart(callback.unk_0008, callback.unk_0010);
                break;

            case 3:
                Comsat::VoicePlateStop(callback.unk_0008, callback.unk_0010);
                break;

            default:
                break;
        }
    }

    s_comsatCallbacks.Clear();
}

// VERIFY: 7b6b6b9a auto
// OFFSET: 0x007DF900
void ComSatClient_Shutdown() {
    if (ComSatThread::s_threadRunning) {
        ComSatThread::s_threadRunning = 0;
        ComSatThread::s_threadShutdownEvent.Set();
        ComSatThread::s_comsatThread.Wait(0xFFFFFFFF);
    }

    ProcessComSatCallbacks();
    EventUnregister(EVENT_ID_IDLE, VoiceChat_TickWrapper);
    ComSat::Interface::Destroy(ComSatThread::s_comsat, s_comsatAllocator);
    ComSatThread::s_comsat = nullptr;
    unk_D37DB0.InternalClear(0);
    unk_D37D88.InternalClear(0);

    if (unk_D37D54.Valid()) {
        unk_D37D54.Close();
        unk_D37D54.Release();
    }

    ResetVoiceVolumeState();
    SI2::VoiceChat_DestroyChatSystem();
}
