// Native server-authorized Skyriding owner. Adapted from wxl-skyriding.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/Skyriding.hpp"
#include "WXL/NativeFeatures.hpp"
#include "WXL/NativeProtocol.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Event/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Movement.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#ifndef WXL_ENABLE_SKYRIDING
#define WXL_ENABLE_SKYRIDING 1
#endif
namespace wxl::native::skyriding { namespace {
static_assert(std::endian::native==std::endian::little);
uint32_t Now(){return OsGetAsyncTimeMs();}
namespace opcodes {constexpr uint16_t CmsgSkyriding=0x527,SmsgSkyriding=0x528,CmsgMoveAddImpulseAck=0x529,SmsgMoveAddImpulse=0x52a;}
namespace network {bool Send(uint16_t op,std::span<const uint8_t> bytes){return protocol::Send(op,bytes.data(),uint32_t(bytes.size()));}}
CVar* enabled=nullptr;
bool installed=false,resetPending=false;
uint64_t session=0,playerGuid=0;
uint32_t map=0,mountHandle=0;
uint32_t deferredModeOffAt=0;
CM2Model* mountedModel=nullptr;
std::atomic<uint32_t> capturedMap{0};
std::atomic<bool> accepting{false};
std::array<bool,KEY_LAST> keys{};
struct Packet {uint16_t opcode;uint64_t session;uint32_t map;std::vector<uint8_t> data;};
std::mutex queueMutex;std::deque<Packet> pending;
    constexpr uint32_t kMoveForward = 0x00000001;
    constexpr uint32_t kMoveBackward = 0x00000002;
    constexpr uint32_t kMoveDisableGravity = 0x00000400;
    constexpr uint32_t kMoveFalling = 0x00001000;
    constexpr uint32_t kMoveFallingFar = 0x00002000;
    constexpr uint32_t kMovePendingStop = 0x00004000;
    constexpr uint32_t kMoveAscending = 0x00400000;
    constexpr uint32_t kMoveDescending = 0x00800000;
    constexpr uint32_t kMoveFlying = 0x02000000;
    constexpr uint32_t kForwardControlBit = 0x10;

    constexpr int kAnimForward = 1524;
    constexpr int kAnimDown = 1530;
    constexpr int kAnimForwardGlide = 1532;
    // Whirling Surge is a complete one-shot barrel roll on the mounted model.
    // RidingWyvern.m2 authors it as 1558 (2066 ms); the old three-stage path
    // only played the first 350 ms of this sequence and then returned to glide.
    constexpr int kAnimWhirlingSurge = 1558;
    constexpr int kAnimDownStart = 1678;
    constexpr int kAnimFlapBig = 1680;
    constexpr int kAnimFlapUp = 1702;
    constexpr int kAnimSlowFall = 1704;
    constexpr int kAnimForwardGlideSlow = 1722;
    constexpr int kAnimSecondFlapUp = 1726;

    constexpr uint32_t kDownStartDurationMs = 1583;
    constexpr uint32_t kSurgeForwardDurationMs = 2600;
    constexpr uint32_t kWhirlFallbackDurationMs = 2000;
    constexpr uint32_t kFlapUpDurationMs = 750;
    constexpr uint32_t kSecondFlapUpDurationMs = 2083;
    constexpr uint32_t kDoubleJumpWindowMs = 400;
    constexpr uint32_t kTakeoffGraceMs = 400;
    constexpr uint32_t kDefaultGroundLockMs = 450;
    constexpr uint32_t kRecentFlyMs = 1000;
    constexpr uint32_t kRecentDiveIntentMs = 1200;
    constexpr float kDiveEnterPitch = -0.35f;
    constexpr float kDiveExitPitch = -0.28f;
    constexpr float kStallSinkPerSec = 8.0f;
    constexpr float kSurgeLiftPerSec = 6.0f;
    constexpr uint32_t kSurgeLiftMs = 400;
    constexpr uint32_t kTraceHitFlagsGround = 0x00000051;
    constexpr float kCollisionSkinYards = 0.45f;
    constexpr float kCollisionMaxStepYards = 28.0f;
    constexpr float kCollisionProbeZ = 1.2f;
    constexpr float kSettleYardsXY = 0.12f;
    constexpr float kSettleYardsZ = 0.16f;
    constexpr float kSoftLandMaxMoveZ = 0.55f;
    constexpr int kSettleFrames = 2;
    constexpr float kDiveLandProbeUp = 0.50f;
    constexpr float kDiveLandProbeDown = 2.50f;
    constexpr float kDiveLandContactGap = 0.65f;
    constexpr int kDiveLandConfirmFrames = 2;
    constexpr float kSlowRateEnter = 1.55f;
    constexpr float kSlowRateExit = 1.90f;
    constexpr float kBaseFlightYards = 7.0f;
    constexpr uint32_t kImpulseStateDurationMs = 350;
    constexpr uint32_t kSkywardLaunchDurationMs =
        kFlapUpDurationMs + kSecondFlapUpDurationMs;
    constexpr float kSkywardForwardBonusYardsPerSec = 2.0f;
    constexpr float kSkywardMinPitch = 0.48f;
    constexpr float kSkywardMaxPitch = 0.62f;
    constexpr size_t kMaxCommandBytes = 128;

    enum class AnimationPhase : uint8_t
    {
        Idle,
        Cruise,
        DiveStart,
        Dive,
        DiveExit,
        Surge,
        Whirl,
        SkywardFirst,
        SkywardSecond
    };

    const char* PhaseName(AnimationPhase phase) noexcept
    {
        switch (phase)
        {
        case AnimationPhase::Idle: return "idle";
        case AnimationPhase::Cruise: return "cruise";
        case AnimationPhase::DiveStart: return "dive start";
        case AnimationPhase::Dive: return "dive";
        case AnimationPhase::DiveExit: return "dive exit";
        case AnimationPhase::Surge: return "surge forward";
        case AnimationPhase::Whirl: return "whirling surge";
        case AnimationPhase::SkywardFirst: return "skyward flap 1";
        case AnimationPhase::SkywardSecond: return "skyward flap 2";
        }
        return "unknown";
    }

    struct State
    {
        bool active = false;
        bool classicVertical = false;
        bool grounded = false;
        bool braking = false;
        bool stalled = false;
        bool forcedForward = false;
        bool previousFlying = false;
        bool previousFalling = false;
        bool haveFacing = false;
        float lastFacing = 0.0f;
        float turnRate = 0.75f;
        float flightRate = 1.0f;
        float coastYardsPerSec = 2.5f * kBaseFlightYards;
        uint32_t groundLockUntilMs = 0;
        uint32_t takeoffGraceUntilMs = 0;
        uint32_t aerialHaltUntilMs = 0;
        uint32_t lastSpaceMs = 0;
        uint32_t lastFlyingMs = 0;
        uint32_t lastFallingMs = 0;
        uint32_t lastDiveIntentMs = 0;
        uint32_t lastPhysicsMs = 0;
        uint32_t surgeLiftUntilMs = 0;
        int lastBrakeSent = -1;
        std::array<float, 3> lastPosition{};
        bool haveLastPosition = false;
        int settleFrames = 0;
        int diveLandFrames = 0;
        float lastMoveXY = -1.0f;
        float lastMoveZ = -1.0f;
        std::array<float, 3> lastCollisionPosition{};
        bool haveLastCollisionPosition = false;
        bool inDive = false;
        bool surgePitchLocked = false;
        bool skywardPitchLocked = false;
        float surgePitch = 0.0f;
        float skywardPitch = 0.0f;
        bool impulseActive = false;
        uint32_t impulseSequence = 0;
        std::array<float, 3> impulseVelocity{};
        uint32_t impulseStartedMs = 0;
        bool launchActive = false;
        uint32_t launchStartedMs = 0;
        uint32_t launchDurationMs = 0;
        float launchProgress = 0.0f;
        float launchForwardYards = 0.0f;
        float launchRiseYards = 0.0f;
        float launchForwardSpeed = 0.0f;
        float launchPitchStart = 0.0f;
        std::array<float, 3> launchOrigin{};
        std::array<float, 2> launchHeading{};
        std::array<char, 64> lastCommand{};

        AnimationPhase phase = AnimationPhase::Idle;
        uint32_t phaseUntilMs = 0;
        int forcedAnimation = -1;
        int boneAnimation = -1;
        float boneSpeed = 1.0f;
        int cruiseAnimation = kAnimForwardGlide;
        CGUnit_C* forcedUnit = nullptr;
        CM2Model* forcedModel = nullptr;
    };

    State g_state;

CGUnit_C* ActivePlayer() noexcept {return ClntObjMgrGetCurrent()?ClntObjMgrGetActivePlayerObj():nullptr;}
CM2Model* UnitModel(CGUnit_C* unit) noexcept {return unit?unit->GetObjectModel():nullptr;}
bool ModelHasSequence(CM2Model* model,unsigned int id) noexcept {return model&&model->IsLoaded(0,0)&&model->HasSequence(id);}
bool ModelSupportsAdvFly(CM2Model* model) noexcept {return ModelHasSequence(model,kAnimForwardGlide);}
CM2Model* AnimationModel(CGUnit_C* unit) noexcept {auto* body=UnitModel(unit);if(!body)return nullptr;auto* parent=body->m_attachParent;return ModelSupportsAdvFly(parent)?parent:(ModelSupportsAdvFly(body)?body:nullptr);}
bool OnAdvFlyMount(CGUnit_C* unit) noexcept {return AnimationModel(unit)!=nullptr;}
uint32_t MovementFlags(CGUnit_C* unit){return unit?unit->m_movementData.m_flags:0;}
bool IsFlying(CGUnit_C* unit) noexcept {return (MovementFlags(unit)&MOVEMENTFLAG_FLYING)!=0;}
bool IsFalling(CGUnit_C* unit) noexcept {return (MovementFlags(unit)&(MOVEMENTFLAG_FALLING|MOVEMENTFLAG_FALLING_FAR))!=0;}
bool AuthorizedFlight(CGUnit_C* unit){return unit&&unit->IsLocalClientControlled()&&IsFlying(unit)&&(MovementFlags(unit)&MOVEMENTFLAG_CAN_FLY)&&!(MovementFlags(unit)&(MOVEMENTFLAG_ROOT|MOVEMENTFLAG_ONTRANSPORT));}
struct Angle {
 CGUnit_C* unit;bool pitch;
 operator float()const{return pitch?unit->m_movementData.m_pitch:unit->m_movementData.m_facing;}
 Angle& operator=(float value){if(AuthorizedFlight(unit)&&std::isfinite(value)){
  const float current=*this;if(std::abs(current-value)>0.0005f){
   if(pitch)unit->m_movementData.OnSetPitchLocal(Now(),std::clamp(value,-1.45f,1.45f));
   else unit->m_movementData.OnSetRawFacingLocal(Now(),value);
  }}return *this;}
};
Angle Pitch(CGUnit_C* unit){return {unit,true};}Angle Facing(CGUnit_C* unit){return {unit,false};}
float* UnitPosition(CGUnit_C* unit){return &unit->m_movementData.m_position.x;}
void ReleaseForcedForward(){auto* unit=ActivePlayer();if(g_state.forcedForward&&unit&&ClntObjMgrGetActivePlayer().m_guid==playerGuid&&!keys[KEY_W]&&!keys[KEY_UP]&&!keys[KEY_S]&&!keys[KEY_DOWN])unit->m_movementData.OnMoveStopLocal(Now());g_state.forcedForward=false;}
void SetForwardControl(bool value){auto* unit=ActivePlayer();if(value&&AuthorizedFlight(unit))unit->m_movementData.OnMoveStartLocal(Now(),1);else if(!value)ReleaseForcedForward();}
bool AbilityPhase();bool SendCommand(std::string_view);
    void ReleaseAnimation()
    {
        g_state.phase = AnimationPhase::Idle;
        g_state.phaseUntilMs = 0;
        g_state.forcedAnimation = -1;
        g_state.boneAnimation = -1;
        g_state.boneSpeed = 1.0f;
        g_state.cruiseAnimation = kAnimForwardGlide;
        g_state.inDive = false;
        g_state.surgePitchLocked = false;
        g_state.skywardPitchLocked = false;
        g_state.surgeLiftUntilMs = 0;
        g_state.forcedUnit = nullptr;
        g_state.forcedModel = nullptr;
    }

void PlayAnimation(CGUnit_C* unit,CM2Model* model,int animationId,float speed=1.0f,float sequenceTime=-1.0f,bool force=false){
 if(!unit||!model||!ModelSupportsAdvFly(model)||animationId<=0||!ModelHasSequence(model,uint32_t(animationId)))return;
 g_state.forcedAnimation=animationId;
 const bool same=g_state.boneAnimation==animationId&&std::abs(g_state.boneSpeed-speed)<0.01f;
 if(same&&!force)return;
 const uint32_t time=sequenceTime>0?uint32_t(sequenceTime*1000.0f):0;
 unit->SetKeyBoneSequence(model,0xffffffffu,uint32_t(animationId),0,time,speed,1,1,1);
 g_state.boneAnimation=animationId;g_state.boneSpeed=speed;g_state.forcedUnit=unit;g_state.forcedModel=model;
}
void ForceGroundFromServer(){
 ReleaseForcedForward();ReleaseAnimation();auto* unit=ActivePlayer();
 // The explicit server LAND/GLOCK command may end flight through its native
 // event queue. It never grants can-fly or changes gravity permission.
 if(unit&&unit->IsLocalClientControlled()){
  unit->m_movementData.OnAscendDescendStopLocal(Now());
  if(IsFlying(unit))unit->m_movementData.OnFlightLocal(Now(),0);
 }
 g_state.grounded=true;g_state.groundLockUntilMs=Now()+kDefaultGroundLockMs;g_state.haveFacing=false;g_state.launchActive=false;g_state.impulseActive=false;
}
    float NormalizeAngle(float value)
    {
        constexpr float pi = 3.14159265f;
        constexpr float twoPi = 6.2831853f;
        while (value > pi) value -= twoPi;
        while (value < -pi) value += twoPi;
        return value;
    }
    void ApplyTurnInertia(CGUnit_C* unit)
    {
        if (!unit || !IsFlying(unit) || g_state.turnRate >= 0.999f)
        {
            if (unit)
            {
                {
                    g_state.lastFacing = Facing(unit);
                    g_state.haveFacing = true;
                }
            }
            return;
        }

        {
            auto facing = Facing(unit);
            if (!g_state.haveFacing)
            {
                g_state.lastFacing = facing;
                g_state.haveFacing = true;
                return;
            }
            const float delta =
                NormalizeAngle(facing - g_state.lastFacing);
            facing = g_state.lastFacing + delta * g_state.turnRate;
            g_state.lastFacing = facing;
        }
    }
    void BeginPhase(
        AnimationPhase phase, uint32_t durationMs)
    {
        g_state.phase = phase;
        g_state.phaseUntilMs =
            durationMs ? Now() + durationMs : 0;
    }
    bool AbilityPhase()
    {
        return g_state.phase == AnimationPhase::Surge ||
            g_state.phase == AnimationPhase::Whirl ||
            g_state.phase == AnimationPhase::SkywardFirst ||
            g_state.phase == AnimationPhase::SkywardSecond ||
            g_state.phase == AnimationPhase::DiveStart ||
            g_state.phase == AnimationPhase::DiveExit;
    }
    int SupportedOr(
        CM2Model* model, int preferred, int fallback) noexcept
    {
        return ModelHasSequence(
            model, static_cast<unsigned int>(preferred))
            ? preferred : fallback;
    }
    void BeginFlap(int kind)
    {
        if (g_state.grounded) return;
        CGUnit_C* unit = ActivePlayer();
        CM2Model* model = AnimationModel(unit);
        if (!unit) return;

        g_state.stalled = false;
        g_state.boneAnimation = -1;
        if (kind == 1)
        {
            {
                g_state.skywardPitch = Pitch(unit);
                g_state.skywardPitchLocked = true;
            }
            g_state.takeoffGraceUntilMs =
                Now() + kTakeoffGraceMs;
            if (model)
                PlayAnimation(
                    unit, model, kAnimFlapUp, 1.0f, 0.0f, true);
            BeginPhase(
                AnimationPhase::SkywardFirst,
                kFlapUpDurationMs);
        }
        else if (kind == 2)
        {
            const int animation = SupportedOr(
                model, kAnimWhirlingSurge, kAnimFlapBig);
            if (model)
                PlayAnimation(
                    unit, model, animation, 1.0f, 0.0f, true);
            const uint32_t durationMs = kWhirlFallbackDurationMs;
            
            BeginPhase(AnimationPhase::Whirl, durationMs);
        }
        else
        {
            {
                g_state.surgePitch = Pitch(unit);
                g_state.surgePitchLocked = true;
            }
            g_state.surgeLiftUntilMs =
                Now() + kSurgeLiftMs;
            const int animation = kAnimFlapBig;
            if (model)
                PlayAnimation(
                    unit, model, animation, 1.0f, -1.0f, true);
            BeginPhase(
                AnimationPhase::Surge,
                kSurgeForwardDurationMs);
        }
    }
    int CruiseAnimation()
    {
        if (g_state.stalled)
        {
            g_state.cruiseAnimation = kAnimSlowFall;
            return kAnimSlowFall;
        }

        float rate = g_state.coastYardsPerSec / kBaseFlightYards;
        if (rate < 0.1f) rate = 0.1f;
        if (g_state.cruiseAnimation == kAnimSlowFall)
            g_state.cruiseAnimation = kAnimForwardGlideSlow;
        if (g_state.cruiseAnimation == kAnimForwardGlideSlow)
        {
            if (rate >= kSlowRateExit)
                g_state.cruiseAnimation = kAnimForwardGlide;
        }
        else if (rate < kSlowRateEnter)
            g_state.cruiseAnimation = kAnimForwardGlideSlow;
        else
            g_state.cruiseAnimation = kAnimForwardGlide;
        return g_state.cruiseAnimation;
    }
    void TickAnimation(CGUnit_C* unit, CM2Model* model)
    {
        if (!unit || !model) return;
        const uint32_t now = Now();

        if (g_state.phase == AnimationPhase::Surge)
        {
            PlayAnimation(
                unit, model, kAnimFlapBig);
            if (now >= g_state.phaseUntilMs)
            {
                g_state.surgePitchLocked = false;
                g_state.surgeLiftUntilMs = 0;
                PlayAnimation(
                    unit, model, kAnimForwardGlide);
                BeginPhase(AnimationPhase::Cruise, 0);
            }
            return;
        }
        else if (g_state.phase == AnimationPhase::Whirl)
        {
            PlayAnimation(
                unit, model,
                SupportedOr(model, kAnimWhirlingSurge, kAnimFlapBig));
            if (now < g_state.phaseUntilMs) return;
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimForwardGlide,
                1.0f, 0.0f, true);
            BeginPhase(AnimationPhase::Cruise, 0);
            return;
        }
        else if (g_state.phase == AnimationPhase::SkywardFirst)
        {
            if (g_state.skywardPitchLocked)
            {
                {
                    Pitch(unit) = g_state.skywardPitch;
                }
            }
            PlayAnimation(unit, model, kAnimFlapUp);
            if (now < g_state.phaseUntilMs) return;
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimSecondFlapUp,
                1.0f, 0.0f, true);
            BeginPhase(
                AnimationPhase::SkywardSecond,
                kSecondFlapUpDurationMs);
            return;
        }
        else if (g_state.phase == AnimationPhase::SkywardSecond)
        {
            if (g_state.skywardPitchLocked)
            {
                {
                    Pitch(unit) = g_state.skywardPitch;
                }
            }
            PlayAnimation(unit, model, kAnimSecondFlapUp);
            if (now >= g_state.phaseUntilMs)
            {
                g_state.skywardPitchLocked = false;
                PlayAnimation(
                    unit, model, kAnimForwardGlide);
                BeginPhase(AnimationPhase::Cruise, 0);
            }
            return;
        }

        float pitch = 0.0f;
        {
            pitch = Pitch(unit);
        }

        if (!g_state.inDive && pitch < kDiveEnterPitch &&
            !AbilityPhase())
        {
            g_state.inDive = true;
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimDownStart,
                1.0f, 0.0f, true);
            BeginPhase(
                AnimationPhase::DiveStart,
                kDownStartDurationMs);
            return;
        }
        else if (g_state.inDive && pitch > kDiveExitPitch &&
            (g_state.phase == AnimationPhase::Dive ||
             g_state.phase == AnimationPhase::DiveStart))
        {
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimDownStart, -1.0f,
                static_cast<float>(kDownStartDurationMs) / 1000.0f,
                true);
            BeginPhase(
                AnimationPhase::DiveExit,
                kDownStartDurationMs);
        }

        if (g_state.phase == AnimationPhase::DiveStart)
        {
            PlayAnimation(unit, model, kAnimDownStart);
            if (now < g_state.phaseUntilMs)
            {
                return;
            }
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimDown,
                1.0f, 0.0f, true);
            BeginPhase(AnimationPhase::Dive, 0);
            return;
        }

        if (g_state.phase == AnimationPhase::Dive)
        {
            PlayAnimation(unit, model, kAnimDown);
            return;
        }

        if (g_state.phase == AnimationPhase::DiveExit)
        {
            PlayAnimation(
                unit, model, kAnimDownStart, -1.0f,
                static_cast<float>(kDownStartDurationMs) / 1000.0f);
            if (now < g_state.phaseUntilMs)
                return;
            g_state.inDive = false;
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimForwardGlide,
                1.0f, 0.0f, true);
            BeginPhase(AnimationPhase::Cruise, 0);
            return;
        }

        PlayAnimation(
            unit, model, CruiseAnimation(), 1.0f, 0.0f);
        if (g_state.phase != AnimationPhase::Cruise)
            BeginPhase(AnimationPhase::Cruise, 0);
    }
    void ResetState(bool keepConfiguration = true)
    {
        const bool classicVertical = g_state.classicVertical;
        const float turnRate = g_state.turnRate;
        ReleaseForcedForward();
        ReleaseAnimation();
        g_state = {};deferredModeOffAt=0;
        if (keepConfiguration)
        {
            g_state.classicVertical = classicVertical;
            g_state.turnRate = turnRate;
        }
        g_state.flightRate = 1.0f;
        g_state.coastYardsPerSec = 2.5f * kBaseFlightYards;
        g_state.lastBrakeSent = -1;
    }
    void SetMode(bool active)
    {
        const bool transientAbilityDrop = AbilityPhase();
        if (!active && g_state.active &&
            (Now() < g_state.takeoffGraceUntilMs ||
             transientAbilityDrop))
        {
            CGUnit_C* unit = ActivePlayer();
            if (unit && OnAdvFlyMount(unit))
            {
                // KnockbackFrom briefly makes the server's mount candidate
                // test false during Skyward and Whirling Surge. MODE 1
                // follows in the same movement transition; tearing down here
                // interrupts the active flap/roll and restarts glide ownership.
                
                return;
            }
        }

        if (g_state.active == active)
        {
            if (!active)
            {
                g_state.grounded = false;
                g_state.groundLockUntilMs = 0;
            }
            return;
        }

        const bool classicVertical = g_state.classicVertical;
        const float turnRate = g_state.turnRate;
        ResetState(false);
        g_state.classicVertical = classicVertical;
        g_state.turnRate = turnRate;
        g_state.active = active;
        if (active)
        {
            CGUnit_C* unit = ActivePlayer();
            g_state.previousFlying = IsFlying(unit);
            g_state.previousFalling = IsFalling(unit);
        }
        
    }
    bool SendCommand(std::string_view body)
    {
        if (body.empty() || body.size() > kMaxCommandBytes)
            return false;
        std::vector<uint8_t> payload(sizeof(uint32_t) + body.size());
        const uint32_t length = static_cast<uint32_t>(body.size());
        std::memcpy(payload.data(), &length, sizeof(length));
        std::memcpy(
            payload.data() + sizeof(length), body.data(), body.size());
        return network::Send(opcodes::CmsgSkyriding, payload);
    }
    struct Tokens
    {
        std::array<std::string_view, 8> values{};
        size_t count = 0;
    };

    Tokens Split(std::string_view body)
    {
        Tokens result;
        size_t begin = 0;
        while (result.count < result.values.size())
        {
            const size_t end = body.find('\t', begin);
            result.values[result.count++] = body.substr(
                begin, end == std::string_view::npos
                    ? body.size() - begin : end - begin);
            if (end == std::string_view::npos) break;
            begin = end + 1;
        }
        return result;
    }

    float ParseFloat(std::string_view value, float fallback = 0.0f)
    {
        if (value.empty() || value.size() > 32) return fallback;
        char buffer[33]{};
        std::memcpy(buffer, value.data(), value.size());
        char* end = nullptr;
        const float parsed = std::strtof(buffer, &end);
        return end && *end == '\0' && std::isfinite(parsed)
            ? parsed : fallback;
    }

    uint32_t ParseUnsigned(
        std::string_view value, uint32_t fallback = 0)
    {
        if (value.empty() || value.size() > 10) return fallback;
        char buffer[11]{};
        std::memcpy(buffer, value.data(), value.size());
        char* end = nullptr;
        const unsigned long parsed = std::strtoul(buffer, &end, 10);
        return end && *end == '\0'
            ? static_cast<uint32_t>(parsed) : fallback;
    }

    void ApplyGroundLockMessage(uint32_t durationMs)
    {
        if (!durationMs)
        {
            g_state.grounded = false;
            g_state.groundLockUntilMs = 0;
            g_state.takeoffGraceUntilMs =
                Now() + kTakeoffGraceMs;
            g_state.haveLastPosition = false;
            g_state.settleFrames = 0;
            g_state.diveLandFrames = 0;
            return;
        }

        ForceGroundFromServer();
        g_state.groundLockUntilMs =
            Now() + std::max(
                durationMs, uint32_t{kDefaultGroundLockMs});
    }

    void HandleCommand(std::string_view body)
    {
        const size_t commandBytes = std::min(
            body.size(), g_state.lastCommand.size() - 1);
        std::memcpy(
            g_state.lastCommand.data(), body.data(), commandBytes);
        g_state.lastCommand[commandBytes] = '\0';

        const Tokens tokens = Split(body);
        if (!tokens.count || tokens.values[0].empty()) return;
        const std::string_view command = tokens.values[0];

        if (command == "MODE" && tokens.count >= 2)
            {
            const bool value=ParseUnsigned(tokens.values[1])!=0;
            SetMode(value);
            // Preserve donor transient MODE0 grace, but apply an unpaired
            // authoritative revocation after its current transition finishes.
            deferredModeOffAt=(!value&&g_state.active)?Now()+(std::max)(kTakeoffGraceMs,g_state.phaseUntilMs>Now()?g_state.phaseUntilMs-Now():0u):0;
        }
        else if (command == "VERT" && tokens.count >= 2)
            g_state.classicVertical =
                ParseUnsigned(tokens.values[1]) != 0;
        else if (command == "TURN" && tokens.count >= 2)
            g_state.turnRate = std::clamp(
                ParseFloat(tokens.values[1], 0.75f), 0.05f, 1.0f);
        else if (command == "RATE" && tokens.count >= 2)
        {
            g_state.flightRate = ParseFloat(tokens.values[1]);
            if (g_state.grounded)
            {
                g_state.flightRate = 0.0f;
                g_state.stalled = false;
            }
            else if (g_state.flightRate <= -1.0f)
            {
                g_state.aerialHaltUntilMs = Now() + 4000;
                g_state.stalled = false;
            }
            else
                g_state.stalled = g_state.flightRate < 0.0f;
        }
        else if (command == "SPD" && tokens.count >= 5)
        {
            const float speed = ParseFloat(tokens.values[4]);
            if (speed > 0.0f && !g_state.grounded)
                g_state.coastYardsPerSec =
                    std::clamp(speed, 2.0f, 120.0f);
            g_state.stalled =
                tokens.values[3] == "stall";
        }
        else if (command == "ANIM" && tokens.count >= 2)
        {
            if (!g_state.active) SetMode(true);
            g_state.grounded = false;
            g_state.haveLastPosition = false;
            g_state.settleFrames = 0;
            g_state.diveLandFrames = 0;
            BeginFlap(static_cast<int>(
                ParseUnsigned(tokens.values[1])));
        }
        else if (command == "LAND")
            ForceGroundFromServer();
        else if (command == "GLOCK" && tokens.count >= 2)
            ApplyGroundLockMessage(
                ParseUnsigned(tokens.values[1]));

        if (command != "RATE" && command != "SPD" &&
            command != "CHG" && command != "AUX")
        {
            
        }
    }

    void __cdecl OnPacket(const uint8_t* data, uint32_t size, void*)
    {
        const std::span<const uint8_t> payload(data, size);
        if (payload.size() < sizeof(uint32_t) ||
            payload.size() > sizeof(uint32_t) + kMaxCommandBytes)
        {
            
            return;
        }
        uint32_t length = 0;
        std::memcpy(&length, payload.data(), sizeof(length));
        if (!length || length > kMaxCommandBytes ||
            payload.size() != sizeof(length) + length)
        {
            
            return;
        }
        HandleCommand(std::string_view(
            reinterpret_cast<const char*>(
                payload.data() + sizeof(length)), length));
    }

    struct MoveAddImpulsePacket
    {
        uint64_t moverGuid = 0;
        uint32_t sequence = 0;
        std::array<float, 3> direction{};
    };
static_assert(sizeof(MoveAddImpulsePacket)==24);
    bool IsSkywardImpulse(
        const MoveAddImpulsePacket& packet) noexcept
    {
        const float horizontal = std::hypot(
            packet.direction[0], packet.direction[1]);
        const bool skywardPhase =
            g_state.phase == AnimationPhase::SkywardFirst ||
            g_state.phase == AnimationPhase::SkywardSecond;
        // The phase is the primary discriminator. The constrained fallback
        // keeps compatibility with a server that emitted the impulse just
        // before ANIM without mistaking Surge/Whirling for Skyward.
        return packet.direction[2] > 0.0f &&
            (skywardPhase ||
             (packet.direction[2] >= 20.0f && horizontal <= 16.0f));
    }

void OnMoveAddImpulse(const uint8_t* data,uint32_t size,void*) {
 if(size!=sizeof(MoveAddImpulsePacket))return;MoveAddImpulsePacket p;std::memcpy(&p,data,sizeof(p));
 auto* unit=ActivePlayer();
 if(!unit||!p.moverGuid||p.moverGuid!=ClntObjMgrGetActivePlayer().m_guid||!OnAdvFlyMount(unit)||
    !std::ranges::all_of(p.direction,[](float x){return std::isfinite(x);}))return;
 g_state.active=true;g_state.grounded=false;g_state.groundLockUntilMs=0;g_state.takeoffGraceUntilMs=Now()+kTakeoffGraceMs;
 g_state.impulseSequence=p.sequence;g_state.impulseStartedMs=Now();g_state.impulseActive=true;g_state.impulseVelocity=p.direction;
 if(IsSkywardImpulse(p)&&AuthorizedFlight(unit)) {
  g_state.impulseActive=false;g_state.launchActive=true;g_state.launchStartedMs=Now();g_state.launchDurationMs=kSkywardLaunchDurationMs;
  const float length=std::hypot(p.direction[0],p.direction[1]);
  g_state.launchHeading=length>0.1f?std::array<float,2>{p.direction[0]/length,p.direction[1]/length}:std::array<float,2>{std::cos(float(Facing(unit))),std::sin(float(Facing(unit)))};
  g_state.launchPitchStart=std::clamp((std::max)(float(Pitch(unit)),kSkywardMinPitch),kSkywardMinPitch,kSkywardMaxPitch);
  g_state.skywardPitchLocked=false; // Native impulse easing owns pitch until launch completion.
 }
 // ACK is exactly moverGuid:uint64 + sequence:uint32. No movement packet is fabricated.
 std::array<uint8_t,12> ack{};std::memcpy(ack.data(),&p.moverGuid,8);std::memcpy(ack.data()+8,&p.sequence,4);
 network::Send(opcodes::CmsgMoveAddImpulseAck,ack);
}
void ApplyMoveImpulse(CGUnit_C* unit,float) {
 if(g_state.impulseActive&&Now()-g_state.impulseStartedMs>kImpulseStateDurationMs){g_state.impulseActive=false;g_state.impulseVelocity={};}
 if(!g_state.launchActive||!AuthorizedFlight(unit))return;
 const float t=std::clamp(float(Now()-g_state.launchStartedMs)/float((std::max)(1u,g_state.launchDurationMs)),0.0f,1.0f);
 const float p=t<=0.28f?1.0f:1.0f-std::clamp((t-0.28f)/0.72f,0.0f,1.0f);
 Facing(unit)=std::atan2(g_state.launchHeading[1],g_state.launchHeading[0]);
 Pitch(unit)=g_state.launchPitchStart*p*p*(3.0f-2.0f*p);g_state.launchProgress=t;
 if(t>=1.0f){g_state.launchActive=false;g_state.haveFacing=false;}
}
void ApplyCoast(CGUnit_C* unit){
 if(!AuthorizedFlight(unit)){g_state.braking=false;ReleaseForcedForward();return;}
 g_state.braking=keys[KEY_S]||keys[KEY_DOWN]||(MovementFlags(unit)&MOVEMENTFLAG_BACKWARD);
 if(g_state.braking){ReleaseForcedForward();return;}
 if(!keys[KEY_W]&&!keys[KEY_UP]&&!(MovementFlags(unit)&MOVEMENTFLAG_FORWARD)){SetForwardControl(true);g_state.forcedForward=true;}
 if(keys[KEY_W]||keys[KEY_UP])g_state.forcedForward=false;
}
void ApplyMovementGate(CGUnit_C* unit){if(g_state.active&&!g_state.classicVertical&&AuthorizedFlight(unit))unit->m_movementData.OnAscendDescendStopLocal(Now());}
    void TickOwner(float dt)
    {
        CGUnit_C* unit = ActivePlayer();
        if (!unit)
        {
            ReleaseForcedForward();
            ReleaseAnimation();
            g_state.haveLastCollisionPosition = false;
            return;
        }


        if (!g_state.active || !OnAdvFlyMount(unit))
        {
            ReleaseForcedForward();
            ReleaseAnimation();
            g_state.haveFacing = false;
            g_state.haveLastCollisionPosition = false;
            g_state.lastPhysicsMs = 0;
            return;
        }

        if(g_state.grounded&&g_state.groundLockUntilMs&&int32_t(Now()-g_state.groundLockUntilMs)<0){ReleaseForcedForward();ReleaseAnimation();return;}
        const bool flying = IsFlying(unit);
        const bool falling = IsFalling(unit);
        const uint32_t now = Now();
        if (falling)
            g_state.lastFallingMs = now;
        const bool recentFall = g_state.lastFallingMs &&
            now - g_state.lastFallingMs < kRecentFlyMs;
        const bool holdGround = g_state.grounded &&
            g_state.groundLockUntilMs &&
            now < g_state.groundLockUntilMs;

        if (flying && !g_state.previousFlying)
        {
            g_state.grounded = false;
            g_state.groundLockUntilMs = 0;
        }

        if (!flying)
            ReleaseForcedForward();

        if (holdGround && !flying && !falling && !recentFall)
        {
            ReleaseAnimation();
            g_state.previousFlying = false;
            g_state.previousFalling = false;
            return;
        }
        if (flying || falling || recentFall)
        {
            g_state.grounded = false;
            g_state.groundLockUntilMs = 0;
        }

        if (!flying && !falling)
        {
            g_state.haveFacing = false;
            g_state.haveLastCollisionPosition = false;
            g_state.lastPhysicsMs = 0;
            ReleaseAnimation();
            g_state.previousFlying = false;
            g_state.previousFalling = false;
            return;
        }

        if (flying)
        {

            ApplyCoast(unit);

            ApplyMoveImpulse(unit, dt);
            ApplyTurnInertia(unit);
        }

        CM2Model* model = AnimationModel(unit);
        if (!model)
        {
            g_state.previousFlying = flying;
            g_state.previousFalling = falling;
            return;
        }

        float pitch = 0.0f;
        {
            pitch = Pitch(unit);
        }

        if (flying && !g_state.previousFlying &&
            !AbilityPhase() &&
            (falling || recentFall || pitch < kDiveEnterPitch))
        {
            g_state.inDive = true;
            g_state.boneAnimation = -1;
            PlayAnimation(
                unit, model, kAnimDownStart,
                1.0f, 0.0f, true);
            BeginPhase(
                AnimationPhase::DiveStart,
                kDownStartDurationMs);
        }

        if (flying || (falling && AbilityPhase()))
            TickAnimation(unit, model);
        else if (falling && recentFall)
        {
            if (!g_state.inDive)
            {
                g_state.inDive = true;
                g_state.boneAnimation = -1;
                PlayAnimation(
                    unit, model, kAnimDownStart,
                    1.0f, 0.0f, true);
                BeginPhase(
                    AnimationPhase::DiveStart,
                    kDownStartDurationMs);
            }
            TickAnimation(unit, model);
        }
        else
            ReleaseAnimation();

        g_state.previousFlying = flying;
        g_state.previousFalling = falling;
    }

void ResetQueue(void*) noexcept {accepting.store(false,std::memory_order_release);try{std::lock_guard lock(queueMutex);pending.clear();resetPending=true;}catch(...){}}
void Receive(uint16_t opcode,const uint8_t* data,uint32_t size,uint64_t epoch,void*) noexcept {
 if(!accepting.load(std::memory_order_acquire)||!epoch||!data||((opcode==opcodes::SmsgSkyriding)&&(size<5||size>132))||((opcode==opcodes::SmsgMoveAddImpulse)&&(size!=24)))return;
 try{std::lock_guard lock(queueMutex);if(pending.size()>=64){pending.clear();resetPending=true;return;}pending.push_back({opcode,epoch,capturedMap.load(std::memory_order_acquire),std::vector<uint8_t>(data,data+size)});}catch(...){ResetQueue(nullptr);}
}
int LuaState(lua_State* L){const auto d=GetDiagnostics();lua_createtable(L,0,10);
 lua_pushboolean(L,d.active);lua_setfield(L,-2,"active");lua_pushboolean(L,d.flying);lua_setfield(L,-2,"flying");lua_pushboolean(L,d.braking);lua_setfield(L,-2,"braking");lua_pushboolean(L,d.stalled);lua_setfield(L,-2,"stalled");lua_pushstring(L,d.phase);lua_setfield(L,-2,"phase");lua_pushnumber(L,d.pitch);lua_setfield(L,-2,"pitch");lua_pushnumber(L,d.flightRate);lua_setfield(L,-2,"flightRate");lua_pushnumber(L,d.impulseSequence);lua_setfield(L,-2,"impulseSequence");return 1;}
int LuaCommand(lua_State* L){size_t size=0;const char* text=lua_type(L,1)==LUA_TSTRING?lua_tolstring(L,1,&size):nullptr;const std::string_view command=text?std::string_view(text,size):std::string_view{};
 const bool allowed=command=="RESYNC\t1"||command=="TAKEOFF"||command=="BRK\t0"||command=="BRK\t1";
 const bool context=Enabled()&&protocol::SessionGeneration(L)==session&&ActivePlayer()&&OnAdvFlyMount(ActivePlayer());
 lua_pushboolean(L,allowed&&context&&SendCommand(command));return 1;}
}
void Register(){enabled=CVar::Register("wxlSkyriding","WXL native server-authorized Skyriding controller",0,WXL_ENABLE_SKYRIDING?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept{return WXL_ENABLE_SKYRIDING&&NativeEnabled()&&ProtocolEnabled()&&ClientExtensionsEnabled()&&enabled&&enabled->m_intValue;}
bool Install(){
 if(!installed){const protocol::NativeDeclaration declarations[]={{opcodes::CmsgSkyriding,true},{opcodes::SmsgSkyriding,false,&Receive,&ResetQueue},{opcodes::CmsgMoveAddImpulseAck,true},{opcodes::SmsgMoveAddImpulse,false,&Receive,&ResetQueue}};installed=protocol::RegisterNativeDispatch(declarations,4);}
 if(installed){ResetState();session=0;FrameScript_RegisterFunction("WXL_GetSkyridingState",&LuaState);FrameScript_RegisterFunction("WXL_SkyridingCommand",&LuaCommand);}return installed;
}
void Destroy() noexcept {ResetState();ResetQueue(nullptr);session=playerGuid=0;mountedModel=nullptr;mountHandle=0;keys.fill(false);}
void Tick(float dt){
 const auto currentSession=Enabled()&&installed?protocol::SessionGeneration(FrameScript_GetContext()):0;
 auto* unit=ActivePlayer();const auto guid=unit?ClntObjMgrGetActivePlayer().m_guid:0;const auto currentMap=unit?ClntObjMgrGetMapID():0;
 capturedMap.store(currentMap,std::memory_order_release);accepting.store(currentSession&&unit,std::memory_order_release);
 std::deque<Packet> packets;bool reset=false;{std::lock_guard lock(queueMutex);packets.swap(pending);reset=resetPending;resetPending=false;}
 if(!currentSession||!unit){if(session||g_state.active)ResetState();session=0;playerGuid=0;mountedModel=nullptr;mountHandle=0;keys.fill(false);return;}
 if(reset||session!=currentSession||playerGuid!=guid||map!=currentMap){ResetState();session=currentSession;playerGuid=guid;map=currentMap;keys.fill(false);mountedModel=nullptr;mountHandle=0;SendCommand("RESYNC\t1");}
 auto* model=AnimationModel(unit);const auto handle=model?model->m_handle:0;
 if(mountedModel&&(mountedModel!=model||mountHandle!=handle)){ResetState();SendCommand("RESYNC\t1");}
 mountedModel=model;mountHandle=handle;
 for(auto& p:packets)if(p.session==session&&p.map==map){if(p.opcode==opcodes::SmsgSkyriding)OnPacket(p.data.data(),uint32_t(p.data.size()),nullptr);else OnMoveAddImpulse(p.data.data(),uint32_t(p.data.size()),nullptr);}
 if(deferredModeOffAt&&int32_t(Now()-deferredModeOffAt)>=0){ResetState();deferredModeOffAt=0;}
 TickOwner(std::isfinite(dt)?std::clamp(dt,0.0f,0.1f):0.016f);
 if(g_state.active){const int braking=!g_state.grounded&&g_state.braking?1:0;if(braking!=g_state.lastBrakeSent&&SendCommand(braking?"BRK\t1":"BRK\t0"))g_state.lastBrakeSent=braking;}
}
bool KeyDown(uint32_t key,bool repeat){
 if(key<keys.size())keys[key]=true;
 if(!Enabled()||!session||!g_state.active||g_state.classicVertical||(key!=KEY_SPACE&&key!=KEY_X))return false;
 auto* unit=ActivePlayer();if(!unit||(!OnAdvFlyMount(unit)&&!IsFlying(unit)))return false;
 if(key==KEY_SPACE&&!repeat){const auto now=Now();if(g_state.lastSpaceMs&&now-g_state.lastSpaceMs<=kDoubleJumpWindowMs){g_state.lastSpaceMs=0;SendCommand("TAKEOFF");return true;}g_state.lastSpaceMs=now;}
 ApplyMovementGate(unit);return true;
}
void KeyUp(uint32_t key) noexcept {if(key<keys.size())keys[key]=false;}
uint32_t ResolveAnimation(CGUnit_C* unit,CM2Model* model,uint32_t resolved) noexcept {
 if(!Enabled()||!g_state.active||g_state.forcedAnimation<=0||unit!=ActivePlayer()||unit!=g_state.forcedUnit||model!=g_state.forcedModel||model!=mountedModel||!model||model->m_handle!=mountHandle||!ModelSupportsAdvFly(model))return resolved;
 if(!IsFlying(unit)&&!IsFalling(unit))return resolved;
 return uint32_t(g_state.forcedAnimation);
}
bool OwnsAnimation(CGUnit_C* unit,CM2Model* model) noexcept{return ResolveAnimation(unit,model,0xffffffffu)!=0xffffffffu;}
Diagnostics GetDiagnostics() noexcept
    {
        Diagnostics diagnostics;
        diagnostics.active = g_state.active;
        diagnostics.grounded = g_state.grounded;
        diagnostics.braking = g_state.braking;
        diagnostics.stalled = g_state.stalled;
        diagnostics.forcedForward = g_state.forcedForward;
        diagnostics.impulseActive = g_state.impulseActive;
        diagnostics.launchActive = g_state.launchActive;
        diagnostics.impulseSequence = g_state.impulseSequence;
        diagnostics.launchDurationMs = g_state.launchDurationMs;
        diagnostics.flightRate = g_state.flightRate;
        diagnostics.coastYardsPerSec = g_state.coastYardsPerSec;
        diagnostics.launchProgress = g_state.launchProgress;
        diagnostics.launchForwardSpeed =
            g_state.launchForwardSpeed;
        diagnostics.launchForwardYards = g_state.launchForwardYards;
        diagnostics.launchRiseYards = g_state.launchRiseYards;
        diagnostics.impulseVelocity = g_state.impulseVelocity;
        diagnostics.launchHeading = g_state.launchHeading;
        diagnostics.forcedAnimation = g_state.forcedAnimation;
        diagnostics.boneAnimation = g_state.boneAnimation;
        diagnostics.cruiseAnimation = g_state.cruiseAnimation;
        diagnostics.phase = PhaseName(g_state.phase);
        diagnostics.lastCommand = g_state.lastCommand.data();
        if (g_state.launchActive)
            diagnostics.launchElapsedMs =
                Now() - g_state.launchStartedMs;

        CGUnit_C* unit = ActivePlayer();
        diagnostics.playerAvailable = unit != nullptr;
        if (!unit)
            return diagnostics;

        diagnostics.flying = IsFlying(unit);
        diagnostics.falling = IsFalling(unit);
        {
            diagnostics.movementFlags = MovementFlags(unit);
            diagnostics.facing = Facing(unit);
            diagnostics.pitch = Pitch(unit);
            std::copy_n(
                UnitPosition(unit), diagnostics.position.size(),
                diagnostics.position.begin());
        }
        return diagnostics;
    }
}
