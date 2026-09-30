#ifndef ENGINE_SOURCE_COMSAT_COMSATINTERFACE_H
#define ENGINE_SOURCE_COMSAT_COMSATINTERFACE_H

#include <cstddef>
#include <cstdint>

namespace ComSat {

class Allocator {
    public:
    virtual ~Allocator();
    virtual void* Alloc(uint32_t size) = 0;
    virtual void Free(void* ptr) = 0;
};

struct DeviceInfo {
    /* 0000 */ char m_name[256];
};

enum MuteSetting {
};

class Interface {
    public:
    class Callback;

    static void Destroy(Interface* comsat, Allocator& allocator);

    virtual ~Interface();
    virtual void Work(uint32_t elapsed) = 0;
    virtual int32_t EnumerateOutputDevices(DeviceInfo* devices, int32_t maxDevices) = 0;
    virtual int32_t EnumerateCaptureDevices(DeviceInfo* devices, int32_t maxDevices) = 0;
    virtual void CallbackSet(Interface::Callback& callback) = 0;
    virtual void CallbackClear() = 0;
    virtual int32_t SessionCreate(uint64_t session) = 0;
    virtual void SessionDestroy(uint64_t session) = 0;
    virtual void SessionSetNetworkId(uint64_t session, uint16_t networkId) = 0;
    virtual int32_t SessionAddLocal(uint64_t session, uint8_t localTalker, uint64_t talker) = 0;
    virtual int32_t SessionAddRemote(uint64_t session, uint64_t talker, uint8_t priority, const uint8_t (&key)[16], const char* address) = 0;
    virtual void SessionSetTalkerNetworkIdLocal(uint64_t session, uint64_t talker, uint8_t networkId) = 0;
    virtual void SessionSetTalkerNetworkIdRemote(uint64_t session, uint64_t talker, uint8_t networkId) = 0;
    virtual void SessionRemove(uint64_t session, uint64_t talker) = 0;
    virtual int32_t SessionNumMembers(uint64_t session) = 0;
    virtual int32_t SessionGetTalkerByIndex(uint64_t session, int32_t index, uint64_t& talker, uint8_t& flags) = 0;
    virtual void SessionSetTalkerPriority(uint64_t session, uint64_t talker, uint8_t priority) = 0;
    virtual int32_t LocalTalkerCreate(uint8_t localTalker, uint64_t talker, float threshold, int32_t outputDevice, int32_t captureDevice) = 0;
    virtual void LocalTalkerDestroy(uint8_t localTalker) = 0;
    virtual void LocalTalkerChangeOutputDevice(uint8_t localTalker, int32_t device) = 0;
    virtual void LocalTalkerChangeCaptureDevice(uint8_t localTalker, int32_t device) = 0;
    virtual int32_t LocalTalkerGetCurrentOutputDeviceIndex(uint8_t localTalker) = 0;
    virtual int32_t LocalTalkerGetCurrentCaptureDeviceIndex(uint8_t localTalker) = 0;
    virtual void LocalTalkerEnable(uint8_t localTalker, bool enable) = 0;
    virtual void LocalTalkerThreshold(uint8_t localTalker, float threshold) = 0;
    virtual void LocalTalkerEnableVoiceLevelCallback(uint8_t localTalker, bool enable) = 0;
    virtual void LocalTalkerSelectSession(uint8_t localTalker, uint64_t session) = 0;
    virtual void LocalTalkerSetSessionPriority(uint8_t localTalker, uint64_t session, uint8_t priority) = 0;
    virtual void LocalTalkerSetRemoteTalkerVolume(uint8_t localTalker, uint64_t talker, float volume) = 0;
    virtual void LocalTalkerSetSessionVolume(uint8_t localTalker, uint64_t session, float volume) = 0;
    virtual void MuteTalker(uint8_t localTalker, uint64_t talker, MuteSetting setting) = 0;
    virtual int32_t IsTalkerMuted(uint8_t localTalker, uint64_t talker) = 0;
    virtual void MuteSession(uint8_t localTalker, uint64_t session, MuteSetting setting) = 0;
    virtual int32_t IsSessionMuted(uint8_t localTalker, uint64_t session) = 0;
    virtual int32_t IsTalkerTalkingInSession(uint64_t talker, uint8_t localTalker, uint64_t session) = 0;
    virtual int32_t DoesTalkerHaveHeadset(uint64_t talker) = 0;
    virtual int32_t DoesLocalPlayerHaveHeadset(uint8_t localTalker) = 0;
    virtual void DebugSetSequenceDelay(int32_t delay) = 0;
    virtual void DebugSetSoundOutputDelay(uint8_t localTalker, int32_t delay) = 0;
};

namespace Network {

class Manager {
    public:
    ~Manager();
};

}

}

#endif
