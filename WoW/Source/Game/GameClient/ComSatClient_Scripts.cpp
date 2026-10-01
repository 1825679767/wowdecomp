#include "ComSatClient.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Game/GameClient/ComSatThread.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include <cstring>

// STATIC: 0x00D31A90
static int32_t s_currentCaptureDevice;

// VERIFY: f88c26de auto
// OFFSET: 0x007DC910
static int32_t Script_VoiceIsDisabledByClient(lua_State* L) {
    if (s_comsatDisabledByClient) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: ce9f71d6 manual
// OFFSET: 0x007DAFE0
static int32_t Script_VoiceGetCurrentCaptureDevice(lua_State* L) {
    if (ComSatThread::s_comsat) {
        lua_pushnumber(L, s_currentCaptureDevice);
    } else {
        lua_pushnumber(L, -1.0);
    }

    return 1;
}

// VERIFY: d49beb7a auto
// OFFSET: 0x007DAB80
static int32_t Script_VoiceEnumerateOutputDevices(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        lua_pushnil(L);
        s_comsatDeviceLock.Leave();
        return 1;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (comsat->EnumerateOutputDevices(devices, 20) <= index) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, devices[index].m_name);
    }

    s_comsatDeviceLock.Leave();
    return 1;
}

// VERIFY: b093d213 auto
// OFFSET: 0x007DAC40
static int32_t Script_VoiceEnumerateCaptureDevices(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        s_comsatDeviceLock.Leave();
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (comsat->EnumerateCaptureDevices(devices, 20) <= index) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, devices[index].m_name);
    }

    s_comsatDeviceLock.Leave();
    return 1;
}

// VERIFY: ac77a90e manual
// OFFSET: 0x007DACF0
static int32_t Script_VoiceSelectOutputDevice(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isstring(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat) {
        const char* name = lua_tolstring(L, 1, nullptr);
        int32_t count = comsat->EnumerateOutputDevices(devices, 20);

        for (int32_t i = 0; i < count; i++) {
            if (!strcmp(name, devices[i].m_name)) {
                comsat->LocalTalkerChangeOutputDevice(0, i);
                break;
            }
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// VERIFY: 5e76b472 manual
// OFFSET: 0x007DAE40
static int32_t Script_VoiceSelectCaptureDevice(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isstring(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat) {
        const char* name = lua_tolstring(L, 1, nullptr);
        int32_t count = comsat->EnumerateCaptureDevices(devices, 20);

        for (int32_t i = 0; i < count; i++) {
            if (!strcmp(name, devices[i].m_name)) {
                comsat->LocalTalkerChangeCaptureDevice(0, i);
                break;
            }
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// VERIFY: f43789b0 manual
// OFFSET: 0x007DB030
static int32_t Script_GetNumVoiceSessions(lua_State* L) {
    int32_t count = 0;

    for (COMSATDATA* data = s_comsatData; data < s_comsatData + sizeof(s_comsatData) / sizeof(s_comsatData[0]); data++) {
        if (data->m_sessionID) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: d3e3d9ba manual
// OFFSET: 0x007DB1F0
static int32_t Script_GetVoiceCurrentSessionID(lua_State* L) {
    if (s_currentSession < 0) {
        lua_pushnil(L);
        return 1;
    }

    int32_t id = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (static_cast<uint32_t>(s_comsatData[i].m_sessionID)) {
            id++;

            if (i == s_currentSession) {
                lua_pushnumber(L, id);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: fe6382b3 manual
// OFFSET: 0x007DB0B0
static int32_t Script_GetVoiceSessionInfo(lua_State* L) {
    uint32_t id;

    if (!lua_isnumber(L, 1) || (id = static_cast<uint32_t>(lua_tonumber(L, 1))) > 31) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t count = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (!static_cast<uint32_t>(s_comsatData[i].m_sessionID) || ++count != id) {
            continue;
        }

        switch (s_comsatData[i].m_type) {
            case 2:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_PARTY", -1, GENDER_NOT_APPLICABLE));
                break;

            case 3:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_RAID", -1, GENDER_NOT_APPLICABLE));
                break;

            case 1:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_BATTLEGROUND", -1, GENDER_NOT_APPLICABLE));
                break;

            default:
                lua_pushstring(L, s_comsatData[i].m_name);
                break;
        }

        if (i == s_currentSession) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: c6d24d38 manual
// OFFSET: 0x007DB370
static int32_t Script_GetNumVoiceSessionMembersBySessionID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (id > 31) {
        return 0;
    }

    uint32_t count = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (static_cast<uint32_t>(s_comsatData[i].m_sessionID) && ++count == id) {
            lua_pushnumber(L, s_comsatData[i].m_numMembers);
            return 1;
        }
    }

    return 0;
}

// VERIFY: 09dafb32 auto
// OFFSET: 0x007DF0B0
static int32_t Script_UnitIsTalking(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid = { 0 };
    CGChat::ChatTokenToGUID(lua_tolstring(L, 1, nullptr), guid);

    if (guid && unk_D37D88.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00AF29C0
static FrameScript_Method s_ScriptFunctions[] = {
    { "VoiceEnumerateOutputDevices", &Script_VoiceEnumerateOutputDevices },
    { "VoiceEnumerateCaptureDevices", &Script_VoiceEnumerateCaptureDevices },
    { "VoiceSelectOutputDevice", &Script_VoiceSelectOutputDevice },
    { "VoiceSelectCaptureDevice", &Script_VoiceSelectCaptureDevice },
    { "VoiceGetCurrentOutputDevice", &Script_VoiceGetCurrentOutputDevice },
    { "VoiceGetCurrentCaptureDevice", &Script_VoiceGetCurrentCaptureDevice },
    { "GetVoiceStatus", &Script_GetVoiceStatus },
    { "GetNumVoiceSessions", &Script_GetNumVoiceSessions },
    { "GetVoiceSessionInfo", &Script_GetVoiceSessionInfo },
    { "GetVoiceCurrentSessionID", &Script_GetVoiceCurrentSessionID },
    { "SetActiveVoiceChannelBySessionID", &Script_SetActiveVoiceChannelBySessionID },
    { "GetNumVoiceSessionMembersBySessionID", &Script_GetNumVoiceSessionMembersBySessionID },
    { "GetVoiceSessionMemberInfoBySessionID", &Script_GetVoiceSessionMemberInfoBySessionID },
    { "VoiceIsDisabledByClient", &Script_VoiceIsDisabledByClient },
    { "UnitIsTalking", &Script_UnitIsTalking },
};

// VERIFY: e606eb4c auto
// OFFSET: 0x007DB9B0
void VoiceRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
