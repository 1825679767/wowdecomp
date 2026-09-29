#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLECAMERA_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLECAMERA_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CGObject_C;
class CGUnit_C;
class VehicleSeatRec;

class CVehicleCamera_C;

class CVehicleCameraHeap {
    public:
    /* 0000 */ uint32_t m_heapId;

    CVehicleCamera_C* New();
};

class CVehicleCamera_C {
    public:
    static CVehicleCameraHeap* s_cameraPool;
    static uint32_t s_cameraCount;

    static void InitSystem();
    static void UninitSystem();
    static int32_t ConvertFromRawToWorld(float& facing, CGObject_C* relativeTo);
    static int32_t ConvertFromRawOrOffsetToWorld(C3Vector& position, CGObject_C* relativeTo);
    static int32_t ConvertFromWorldToRawOrOffset(C3Vector& position, CGObject_C* relativeTo);
    static int32_t ConvertSmoothFacingFromRawToWorld(float& facing, CGObject_C* relativeTo);
    static int32_t ConvertSmoothFacingFromWorldToRaw(float& facing, CGObject_C* relativeTo);

    /* 0000 */ uint32_t m_memHandle;
    /* 0004 */ CGUnit_C* m_unit;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ VehicleSeatRec* m_seatRec;
    /* 0014 */ C3Vector unk_0014;
    /* 0020 */ C3Vector unk_0020;
    /* 002C */ C3Vector unk_002C;
    /* 0038 */ C3Vector unk_0038;
    /* 0044 */ uint32_t unk_0044 [2];
    /* 004C */ C3Vector unk_004C;
    /* 0058 */ C3Vector unk_0058;
    /* 0064 */ C3Vector unk_0064;
    /* 0070 */ C3Vector unk_0070;
    /* 007C */ uint32_t unk_007C [10];
    /* 00A4 */ uint32_t m_state;
    /* 00A8 */ uint32_t m_transitionStartTime;
    /* 00AC */ uint32_t m_transitionDuration;
    /* 00B0 */ WOWGUID m_targetGuid;
    /* 00B8 */ WOWGUID m_nextCameraGuid;
    /* 00C0 */ WOWGUID m_relativeToGuid;

    CVehicleCamera_C();
    uint64_t GetRelativeTo() const;
    int32_t WouldAddingNextCameraCreateLoop(const CVehicleCamera_C* next) const;
    void Destroy();
    int32_t IsHierarchyChasingFacing() const;
    int32_t UpdateTransitionFlags();
    float GetTransitionProgress(uint32_t time) const;
    int32_t AttachToHierarchy();
    void Unk759A60();
    void BeginTransition(uint32_t time);
};

#endif
