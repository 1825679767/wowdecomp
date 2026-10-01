#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLEPASSENGER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLEPASSENGER_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class VehicleRec;
class CDataStore;
class CM2Model;
class CGUnit_C;
class CVehicle_C;
class VehicleSeatRec;

struct VehiclePassengerRescueTransition {
    /* 0000 */ WOWGUID m_passengerGuid;
    /* 0008 */ WOWGUID m_vehicleGuid;
    /* 0010 */ uint8_t m_seat;
    /* 0011 */ uint8_t m_instant;
};

class CVehiclePassenger_C {
    public:
    static uint32_t* s_passengerPool;
    static TSGrowableArray<CVehiclePassenger_C*>* s_pendingPassengers;
    static uint32_t s_pendingCount;
    static CVehiclePassenger_C* s_headPassenger;
    static CVehiclePassenger_C* s_tailPassenger;
    static TSGrowableArray<VehiclePassengerRescueTransition>* s_rescueTransitions;

    static void ExecutePendingRescueTransitions();
    static void StartAddingPendingRescueTransitions();
    static void InitSystem();
    static void OnReloadUI();
    static void UninitSystem();
    static void PostRenderUpdateAll();
    static void UpdateAll(uint32_t time);
    static void OnVehicleRideLoopAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers);
    static void OnVehicleTransitionAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers);
    static void AddPendingRescueTransition(WOWGUID passengerGuid, WOWGUID vehicleGuid, uint8_t seat, int32_t instant);
    static CVehiclePassenger_C* Alloc(CGUnit_C* unit);
    static void Init(CGUnit_C* unit);

    void StopMovement();
    void SetState(uint32_t state, CGUnit_C* vehicle, uint8_t seat, VehicleSeatRec* seatRec, uint32_t time, WOWGUID vehicleGuid);

    /* 0000 */ uint32_t m_memHandle;
    /* 0004 */ CVehiclePassenger_C* m_prev;
    /* 0008 */ CVehiclePassenger_C* m_next;
    /* 000C */ CGUnit_C* m_unit;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ uint32_t m_state;
    /* 0018 */ uint32_t unk_0018 [2];
    /* 0020 */ WOWGUID m_vehicleGuid;
    /* 0028 */ WOWGUID m_transitionVehicleGuid;
    /* 0030 */ uint8_t m_seat;
    /* 0031 */ uint8_t m_transitionSeat;
    /* 0032 */ uint8_t unk_0032 [2];
    /* 0034 */ uint32_t unk_0034 [3];
    /* 0040 */ WOWGUID m_startRelativeGuid;
    /* 0048 */ float m_facingBlend;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ VehicleRec* m_vehicleRec;
    /* 0054 */ VehicleSeatRec* m_seatRec;
    /* 0058 */ WOWGUID m_rootVehicleGuid;
    /* 0060 */ CDataStore* m_deferredTransitionSpline;
    /* 0064 */ uint32_t m_transitionAnimTimeout;
    /* 0068 */ WOWGUID m_transitionAnimVehicleGuid;
    /* 0070 */ WOWGUID m_boneAnimGuid;
    /* 0078 */ WOWGUID m_transitionAnimGuid;
    /* 0080 */ uint8_t m_transitionAnimSeat;
    /* 0084 */ C3Vector unk_0084;
    /* 0090 */ C3Vector unk_0090;
    /* 009C */ C3Vector m_startPosition;
    /* 00A8 */ C3Vector m_startOffset;
    /* 00B4 */ float m_startFacing;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ float m_smoothFacing;
    /* 00C0 */ float m_targetFacing;
    /* 00C4 */ uint32_t unk_00C4 [2];
    /* 00CC */ C3Vector m_seatOffset;
    /* 00D8 */ float m_lockedWorldFacing;
    /* 00DC */ uint32_t unk_00DC;

    CVehiclePassenger_C();
    void ClearFlag8();
    void DeleteAllDeferredVehicleTransitionAnimSplines();
    int32_t IsWaitingForVehicleTransitionAnimGrabEvent(uint32_t eventId) const;
    int32_t IsWaitingForVehicleTransitionAnimThrowEvent(uint32_t eventId) const;
    void UpdateSmoothFacing();
    void LockWorldFacingNextFrame(float facing);
    CGUnit_C* GetVehicleUnit() const;
    void StartDeferringVehicleTransitionAnimSpline(CDataStore* msg);
    float GetLockedWorldFacing() const;
    void Free();
    CVehicle_C* GetVehicle() const;
    void StoreRootVehicle(WOWGUID guid);
    void StopBoneAnimIfSameBone(WOWGUID guid, const VehicleSeatRec* seat);
    void StartWaitingForVehicleTransitionAnim(WOWGUID guid, uint8_t seat, const VehicleSeatRec* seatRec, int32_t entering, CVehicle_C* vehicle);
    void ComputeSeatOffset(const VehicleSeatRec* seat, CM2Model* model);
    int32_t ChooseSeatExitAnim(uint32_t flags, uint32_t* anim) const;
    int32_t GetLowerAnim(const VehicleSeatRec* seat) const;
    int32_t GetUpperAnim(const VehicleSeatRec* seat) const;
    CGUnit_C* GetStoredRootVehicleUnit() const;
    void StartMirroringVehicleAnims();
    void Uninit();
    void Update(uint32_t time);
    void UpdateBlend(uint32_t time, VehicleSeatRec* seatRec);
    int32_t UpdateState(CGUnit_C* vehicle, VehicleSeatRec* seatRec, uint32_t time);
    C3Vector ComputeStartWorldPosition(const C3Vector& offset, const C3Vector& fallback) const;
    void OnAnimEnd(uint32_t anim);
    int32_t PickHighPriorityAnimation(int32_t a2, uint32_t* anim) const;
    C3Vector ComputeInterpolatedUnitPosition() const;
    int32_t OverridesModelAnimation() const;

    void Unk74A7F0();
    int32_t CheckForVehicleTransitionAnimTimeout();
    void StopWaitingForVehicleTransitionAnim();
    void ExecuteAllDeferredVehicleTransitionAnimSplines();
    static void OnSetActiveMover(CGUnit_C* unit);
};

#endif
