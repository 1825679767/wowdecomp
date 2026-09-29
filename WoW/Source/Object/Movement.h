#ifndef WOW_SOURCE_OBJECT_MOVEMENT_H
#define WOW_SOURCE_OBJECT_MOVEMENT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/CPlayerMoveEvent.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/Object/Types.h"

class CAaBox;
class C4Plane;
class CGUnit_C;
class CClientMoveUpdate;
class CClientObjCreate;

struct ClipPolygon {
    /* 0000 */ C3Vector m_v [15];
    /* 00B4 */ int32_t m_flags [15];
    /* 00F0 */ int32_t m_count;

    ClipPolygon(const ClipPolygon& source);
};

struct MoveState {
    /* 0000 */ C3Vector m_anchorPos;
    /* 000C */ float m_anchorFacing;
    /* 0010 */ float m_anchorPitch;
    /* 0014 */ uint32_t m_anchorElapsedMs;
    /* 0018 */ int32_t m_fallTimeMs;
    /* 001C */ C3Vector m_moveDir;
    /* 0028 */ C2Vector m_moveDir2D;
    /* 0030 */ uint32_t m_flags;
    /* 0034 */ float m_currentSpeed;
};

class CPlayerMoveQueue : public TSList<CPlayerMoveEvent, TSGetExplicitLink<CPlayerMoveEvent> > {
    public:
    static TSList<CPlayerMoveEvent, TSGetExplicitLink<CPlayerMoveEvent> > s_freeList;

    static CPlayerMoveEvent* AllocEvent(uint32_t eventTime, MoveEventId eventId);

    void Enqueue(CPlayerMoveEvent* event);
    void FreeAll();
    void FreeAllExceptTimeSync();
    int32_t HasEventBetween(MoveEventId first, MoveEventId last);
    void UnlinkEventById(MoveEventId eventId);
};

class CMovement_C : public CMovementShared {
    public:
    static void WrapFacingToRange(float& facing, float min, float max);

    /* 00C4 */ uint32_t m_gameTime;
    /* 00C8 */ float m_collisionRadius;
    /* 00CC */ float m_collisionHeight;
    /* 00D0 */ float m_stepUpHeight;
    /* 00D4 */ C3Vector m_interpolationPos;
    /* 00E0 */ float m_interpolationFacing;
    /* 00E4 */ float m_interpolationPitch;
    /* 00E8 */ int16_t m_timeSkewSamples [32];
    /* 0128 */ uint32_t m_timeSkewIndex;
    /* 012C */ uint32_t m_timeSkew;
    /* 0130 */ uint32_t m_interpolation;
    /* 0134 */ uint32_t m_moveStartDelay;
    /* 0138 */ CPlayerMoveQueue m_moveQueue;
    /* 0144 */ CGUnit_C* m_unit;

    CMovement_C(const C3Vector& position, float facing, const WOWGUID& ownerGuid, CGUnit_C* unit);
    void SetUpdateInfo(uint32_t time, const CClientMoveUpdate* update, int32_t activePlayer);
    void AddPlayerChangeSeatsEvent(uint32_t eventTime, MoveEventId eventId, bool needAck, WOWGUID transport, uint8_t seat);
    void AddPlayerMoveEvent(uint32_t eventTime, MoveEventId eventId, bool needAck, uint32_t ackCounter, float facing, float pitch, uint16_t extraFlags);
    int32_t AddTimeSkewSample(uint32_t localTime, uint32_t serverTime);
    void BuildCollisionBox(const C3Vector& position, CAaBox& box) const;
    void BuildCollisionVolume(const C3Vector& position, float height, C4Plane* planes, C3Vector* points, uint8_t* faces) const;
    void BuildCollisionVolumePlanes(const C3Vector& position, float height, float radius, C4Plane* planes) const;
    C3Vector CalcRunWalkBlockingObstaclePush(const C3Vector& move, float dt, float maxDistance, const C3Vector& normal) const;
    C3Vector CalcRunWalkWalkableObstaclePush(const C3Vector& move, float* scale, C3Vector normal, int32_t useAltNormal, const C3Vector& altNormal) const;
    int32_t CheckFallImpactThreshold(float timeFallen, float elapsed, const C3Vector& move, int32_t force) const;
    int32_t ComputeSteepSurfacePushNormalImpl(const C4Plane& plane) const;
    void FixupQueuedSetFacingEvents(float delta);
    uint32_t GetMoveStartTime(uint32_t elapsed);
    void GetMoveStatus(NETMESSAGE msgId, uint32_t gameTime, CMovementStatus* status) const;
    float GetTimeJustFallen(float elapsed, float timeFallen, C3Vector* move, float* distance, int32_t force, float speed);
    uint32_t GetFacetQueryFlags() const;
    float GetRemainingStepUpHeight() const;
    float GetStepUpHeight() const;
    int32_t HandlePendingActions();
    int32_t Interpolate(uint32_t now, uint32_t elapsed, C3Vector* position, float* facing, float* pitch);
    int32_t IsFacetOverhead(uint32_t facet) const;
    int32_t IsFacetWall(uint32_t facet) const;
    int32_t IsJumping() const;
    int32_t IsValidPosition() const;
    void Launch(const C2Vector& direction, float speed, float verticalSpeed);
    void OnAscendDescendStartLocal(uint32_t eventTime, int32_t ascend);
    void OnAscendDescendStopLocal(uint32_t eventTime);
    void OnCanFlyLocal(uint32_t eventTime, uint32_t ackCounter, int32_t canFly);
    void OnCanTransitionBetweenSwimAndFlyLocal(uint32_t eventTime, uint32_t ackCounter, int32_t canTransition);
    void OnChangeSeatsOnControlledVehicleLocal(uint32_t eventTime, WOWGUID transport, uint8_t seat);
    void OnDisableGravityLocal(uint32_t eventTime, uint32_t ackCounter, int32_t disable);
    void OnDismissControlledVehicleLocal(uint32_t eventTime);
    void OnFallLocal(uint32_t eventTime);
    void OnFeatherFall(int32_t enable);
    void OnFeatherFallLocal(uint32_t eventTime, uint32_t ackCounter, int32_t featherFall);
    int32_t OnFlightBackSpeedChange(float value);
    void OnFlightBackSpeedChange(uint32_t eventTime, float value);
    void OnFlightBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnFlightLocal(uint32_t eventTime, int32_t flying);
    int32_t OnFlightSpeedChange(float value);
    void OnFlightSpeedChange(uint32_t eventTime, float value);
    void OnFlightSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnHoverLocal(uint32_t eventTime, uint32_t ackCounter, int32_t hover);
    void OnJumpLocal(uint32_t eventTime);
    void OnLaunchLocal(uint32_t eventTime, uint32_t ackCounter, const C2Vector& direction, float speed, float verticalSpeed);
    void OnMoveStartLocal(uint32_t eventTime, int32_t forward);
    void OnMoveStopLocal(uint32_t eventTime);
    int32_t OnPitchRateChange(float value);
    void OnPitchRateChange(uint32_t eventTime, float value);
    void OnPitchRateChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnPitchStopLocal(uint32_t eventTime);
    void OnPitchToAngleLocal(uint32_t eventTime, float pitch);
    void OnRootLocal(uint32_t eventTime, uint32_t ackCounter, int32_t root);
    int32_t OnRunBackSpeedChange(float value);
    void OnRunBackSpeedChange(uint32_t eventTime, float value);
    void OnRunBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    int32_t OnRunSpeedChange(float value);
    void OnRunSpeedChange(uint32_t eventTime, float value);
    void OnRunSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnSetCollisionHeight(uint32_t eventTime, float value);
    void OnSetCollisionHeightLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnSetPitchLocal(uint32_t eventTime, float pitch);
    void OnSetRunMode(int32_t run);
    void OnSetRunModeLocal(uint32_t eventTime, int32_t run);
    void OnSkipTime(uint32_t time);
    void OnStrafeStartLocal(uint32_t eventTime, int32_t left);
    void OnStrafeStopLocal(uint32_t eventTime);
    int32_t OnSwimBackSpeedChange(float value);
    void OnSwimBackSpeedChange(uint32_t eventTime, float value);
    void OnSwimBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    int32_t OnSwimSpeedChange(float value);
    void OnSwimSpeedChange(uint32_t eventTime, float value);
    void OnSwimSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnSwimStart(uint32_t eventTime);
    void OnSwimStop(uint32_t eventTime);
    void OnTimeSyncRequest(uint32_t eventTime, uint32_t counter);
    int32_t OnTurnRateChange(float value);
    void OnTurnRateChange(uint32_t eventTime, float value);
    void OnTurnRateChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnTurnStopLocal(uint32_t eventTime);
    int32_t OnWalkSpeedChange(float value);
    void OnWalkSpeedChange(uint32_t eventTime, float value);
    void OnWalkSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value);
    void OnWaterWalk(int32_t enable);
    void OnWaterWalkLocal(uint32_t eventTime, uint32_t ackCounter, int32_t waterWalk);
    int32_t PlaneIntersectsVolume(const C4Plane& plane) const;
    C3Vector PushOffObstacleEdge(const C3Vector& point, const C3Vector& direction, uint32_t facet) const;
    void QueueFallEvent(uint32_t eventTime);
    void RestoreMoveState(const MoveState& state);
    void SaveMoveState(MoveState& state) const;
    void SendTimeSyncResp(uint32_t clientTime, uint32_t counter);
    void SetCollisionBox(float width, float height, float scale, float referenceScale, int32_t force);
    void SetInterpolation(uint32_t time);
    void SetSplineFaceAngle(float facing);
    void SetSplineFacePoint(const C3Vector& point);
    void SetSplineFaceTarget(const WOWGUID& target);
    void SplineToggleEnableFlying(int32_t enable);
    void SnapToSpline(const C3Vector& position, int32_t force);
    void UpdateHeartbeatTimerA(uint32_t time);
    void ValidateWorldPosition(const C3Vector& position) const;
    void UpdateTransportStatus(const CClientObjCreate& create);
    void UpdateHoverState(int32_t hover, int32_t a3);
    void SnapToGroundIfCloseEnough();
    void Disable();
};

#endif
