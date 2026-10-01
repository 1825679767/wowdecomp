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
#include "WoW/Source/Ui/UIBindings.h"

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

// STATIC: 0x00AF29BC
int32_t s_pushToTalkKey = -1;

// STATIC: 0x00D31A88
uint32_t s_pushToTalkModifiers;

// STATIC: 0x00D37D00
CVar* s_cvarPushToTalkButton;

// STATIC: 0x00D37D04
int32_t s_voiceChatWasEnabled;

// STATIC: 0x00D37D1C
uint32_t s_pushToTalkMouseButton;

// STATIC: 0x00D37D34
static TSFixedArray<ComSatCallback> s_comsatCallbacks;

// STATIC: 0x00D37D54
static SMutex unk_D37D54;

// STATIC: 0x00D37D88
TSHashTable<TALKINGPLAYER, CHashKeyGUID> unk_D37D88;

// STATIC: 0x00D37DB0
static TSHashTable<MUTEDPLAYER, CHashKeyGUID> unk_D37DB0;

// STATIC: 0x00D37D44
static TSGrowableArray<ComSatEvent> s_comSatEvents;

// STATIC: 0x00D37D18
int32_t s_comsatDisabledByClient;

// STATIC: 0x00D37D5C
SCritSect s_comsatDeviceLock;

// STATIC: 0x00AF29B4
int32_t s_currentSession = -1;

// VERIFY: df726b6e auto
// OFFSET: 0x007DBC70
static int32_t ParseKeyNameToKeyCode(const char* name) {
    if (!name || !*name) {
        return -1;
    }

    if (!name[1] && name[0] > ' ') {
        return name[0];
    }

    if (!SStrCmpI(name, "LSHIFT", STORM_MAX_STR)) {
        return KEY_LSHIFT;
    }

    if (!SStrCmpI(name, "RSHIFT", STORM_MAX_STR)) {
        return KEY_RSHIFT;
    }

    if (!SStrCmpI(name, "LCTRL", STORM_MAX_STR)) {
        return KEY_LCONTROL;
    }

    if (!SStrCmpI(name, "RCTRL", STORM_MAX_STR)) {
        return KEY_RCONTROL;
    }

    if (!SStrCmpI(name, "LALT", STORM_MAX_STR)) {
        return KEY_LALT;
    }

    if (!SStrCmpI(name, "RALT", STORM_MAX_STR)) {
        return KEY_RALT;
    }

    if (!SStrCmpI(name, "SPACE", STORM_MAX_STR)) {
        return KEY_SPACE;
    }

    if (!SStrCmpI(name, "NUMPADPLUS", STORM_MAX_STR)) {
        return KEY_NUMPAD_PLUS;
    }

    if (!SStrCmpI(name, "NUMPADMINUS", STORM_MAX_STR)) {
        return KEY_NUMPAD_MINUS;
    }

    if (!SStrCmpI(name, "NUMPADMULTIPLY", STORM_MAX_STR)) {
        return KEY_NUMPAD_MULTIPLY;
    }

    if (!SStrCmpI(name, "NUMPADDIVIDE", STORM_MAX_STR)) {
        return KEY_NUMPAD_DIVIDE;
    }

    if (!SStrCmpI(name, "NUMPADDECIMAL", STORM_MAX_STR)) {
        return KEY_NUMPAD_DECIMAL;
    }

    if (!SStrCmpI(name, "NUMPADEQUALS", STORM_MAX_STR)) {
        return KEY_NUMPAD_EQUALS;
    }

    if (SStrStrI(name, "NUMPAD")) {
        if (name[6] >= 0 && name[6] <= '9') {
            return name[6] - '0' + KEY_NUMPAD0;
        }

        return -1;
    }

    if (!SStrCmpI(name, "ESCAPE", STORM_MAX_STR)) {
        return KEY_ESCAPE;
    }

    if (!SStrCmpI(name, "ENTER", STORM_MAX_STR)) {
        return KEY_ENTER;
    }

    if (!SStrCmpI(name, "BACKSPACE", STORM_MAX_STR)) {
        return KEY_BACKSPACE;
    }

    if (!SStrCmpI(name, "TAB", STORM_MAX_STR)) {
        return KEY_TAB;
    }

    if (!SStrCmpI(name, "LEFT", STORM_MAX_STR)) {
        return KEY_LEFT;
    }

    if (!SStrCmpI(name, "UP", STORM_MAX_STR)) {
        return KEY_UP;
    }

    if (!SStrCmpI(name, "RIGHT", STORM_MAX_STR)) {
        return KEY_RIGHT;
    }

    if (!SStrCmpI(name, "DOWN", STORM_MAX_STR)) {
        return KEY_DOWN;
    }

    if (!SStrCmpI(name, "INSERT", STORM_MAX_STR)) {
        return KEY_INSERT;
    }

    if (!SStrCmpI(name, "DELETE", STORM_MAX_STR)) {
        return KEY_DELETE;
    }

    if (!SStrCmpI(name, "HOME", STORM_MAX_STR)) {
        return KEY_HOME;
    }

    if (!SStrCmpI(name, "END", STORM_MAX_STR)) {
        return KEY_END;
    }

    if (!SStrCmpI(name, "PAGEUP", STORM_MAX_STR)) {
        return KEY_PAGEUP;
    }

    if (!SStrCmpI(name, "PAGEDOWN", STORM_MAX_STR)) {
        return KEY_PAGEDOWN;
    }

    if (!SStrCmpI(name, "NUMLOCK", STORM_MAX_STR)) {
        return KEY_NUMLOCK;
    }

    if (!SStrCmpI(name, "CAPSLOCK", STORM_MAX_STR)) {
        return KEY_CAPSLOCK;
    }

    if (!SStrCmpI(name, "PRINTSCREEN", STORM_MAX_STR)) {
        return KEY_PRINTSCREEN;
    }

    if (*name == 'F') {
        return SStrToInt(name + 1) + KEY_F1 - 1;
    }

    return -1;
}

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

// VERIFY: 9c9c6262 auto
// OFFSET: 0x007DC9C0
void UpdatePushToTalkKey() {
    s_pushToTalkKey = -1;
    s_pushToTalkMouseButton = 0;
    s_pushToTalkModifiers = 0;

    if (!s_cvarPushToTalkButton) {
        return;
    }

    const char* binding = s_cvarPushToTalkButton->m_stringValue.m_str;

    if (!binding || !*binding) {
        return;
    }

    s_pushToTalkModifiers = 0;
    CGUIBindings::AddModifiers(s_pushToTalkModifiers, binding);
    s_pushToTalkMouseButton = StringToMouseButton(binding);

    if (!s_pushToTalkMouseButton) {
        s_pushToTalkKey = ParseKeyNameToKeyCode(binding);
    }
}

// VERIFY: bc5e270c auto
// OFFSET: 0x007DBAB0
int32_t ComSatClient_IsComsatDisabledByClient() {
    return s_comsatDisabledByClient;
}

// VERIFY: fe028376 auto
// OFFSET: 0x007DADE0
void ComSatClient_SetOutputDevice(int32_t index) {
    ComSat::DeviceInfo devices[20];
    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat && index < comsat->EnumerateOutputDevices(devices, 20)) {
        comsat->LocalTalkerChangeOutputDevice(0, index);
    }

    s_comsatDeviceLock.Leave();
}

// VERIFY: a22afe03 auto
// OFFSET: 0x007DAF30
void ComSatClient_SetCaptureDevice(int32_t index) {
    ComSat::DeviceInfo devices[20];
    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat && index < comsat->EnumerateCaptureDevices(devices, 20)) {
        comsat->LocalTalkerChangeCaptureDevice(0, index);
    }

    s_comsatDeviceLock.Leave();
}

// VERIFY: 6dafafe1 manual
// OFFSET: 0x007DB5C0
int32_t ComSatClient_GetCurrentSessionType() {
    if (s_currentSession >= 0) {
        return s_comsatData[s_currentSession].m_type;
    }

    return 4;
}

// VERIFY: 706dd5de manual
// OFFSET: 0x007DB5E0
const char* ComSatClient_GetCurrentSessionName() {
    if (s_currentSession >= 0) {
        return s_comsatData[s_currentSession].m_name;
    }

    return nullptr;
}
