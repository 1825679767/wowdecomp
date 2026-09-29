#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class C44Matrix;
class CM2Model;
struct SimpleScriptIO;

struct MissileCollisionData {
    /* 0000 */ WOWGUID m_casterGuid;
    /* 0008 */ uint8_t m_castID;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ C3Vector m_position;
};

class CMissile {
    public:
    static void UpdateStaticLists();
    static void RemoveMissiles();
    static CMissile* s_inFlightMissileList;
    static uint32_t* s_missileHeap;

    static void AdjustAllMissileTrajectoryDurations(uint8_t castID, uint32_t duration);
    static void Destroy();
    static void Initialize();
    static void OnRemoteMissileTrajectoryCollision(const MissileCollisionData& data);
    static void ScriptManagerCallback(uint32_t event);
    static void StartFizzling(WOWGUID caster, uint8_t castID, uint32_t spellID);
    static void UpdateAttachments(WOWGUID guid);

    /* 0000 */ WOWGUID m_casterGuid;
    /* 0008 */ WOWGUID m_targetGuid;
    /* 0010 */ WOWGUID m_transportGuid;
    /* 0018 */ CM2Model* m_model;
    /* 001C */ uint32_t m_spellID;
    /* 0020 */ uint8_t m_castID;
    /* 0021 */ uint8_t unk_0021[3];
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ C3Vector m_launchPos;
    /* 0038 */ C3Vector m_position;
    /* 0044 */ C3Vector m_impactPos;
    /* 0050 */ C3Vector m_worldImpactPos;
    /* 005C */ float unk_005C;
    /* 0060 */ float unk_0060;
    /* 0064 */ float unk_0064;
    /* 0068 */ uint32_t m_flags;
    /* 006C */ int32_t m_attachmentID;
    /* 0070 */ C3Vector m_attachOffset;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t m_missileSound;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ SSyncObject m_syncObject;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ float unk_0090;
    /* 0094 */ uint32_t m_hitResult;
    /* 0098 */ uint32_t m_missReason;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ TSGrowableArray<C3Vector> m_targetPoints;
    /* 00C0 */ TSGrowableArray<uint32_t> m_targetHitResults;
    /* 00D0 */ TSGrowableArray<uint32_t> m_targetMissReasons;
    /* 00E0 */ uint32_t unk_00E0;
    /* 00E4 */ uint32_t unk_00E4;
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t m_currentTarget;
    /* 00F4 */ float unk_00F4;
    /* 00F8 */ float m_progress;
    /* 00FC */ float m_horizontalProgress;
    /* 0100 */ float unk_0100;
    /* 0104 */ float unk_0104;
    /* 0108 */ float unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ int32_t unk_0110;
    /* 0114 */ int32_t unk_0114;
    /* 0118 */ uint32_t unk_0118;
    /* 011C */ uint32_t m_speed;
    /* 0120 */ C3Vector unk_0120;
    /* 012C */ uint32_t m_travelTime;
    /* 0130 */ float unk_0130;
    /* 0134 */ uint32_t m_arrivalTime;
    /* 0138 */ uint32_t m_adjustTime;
    /* 013C */ uint32_t m_adjustedDuration;
    /* 0140 */ uint32_t m_gravity;
    /* 0144 */ uint32_t m_fizzleTime;
    /* 0148 */ uint32_t m_missileCollision;
    /* 014C */ C3Vector m_remoteCollisionPos;
    /* 0158 */ uint32_t unk_0158;
    /* 015C */ uint32_t unk_015C;
    /* 0160 */ CMissile* m_prev;
    /* 0164 */ CMissile* m_next;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint32_t unk_016C;
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint32_t unk_0178;
    /* 017C */ int32_t unk_017C;
    /* 0180 */ uint32_t unk_0180;
    /* 0184 */ float unk_0184;
    /* 0188 */ float unk_0188;
    /* 018C */ float unk_018C;
    /* 0190 */ float unk_0190;
    /* 0194 */ float unk_0194;
    /* 0198 */ float unk_0198;
    /* 019C */ float unk_019C;
    /* 01A0 */ float unk_01A0;
    /* 01A4 */ float unk_01A4;
    /* 01A8 */ float unk_01A8;
    /* 01AC */ float unk_01AC;
    /* 01B0 */ float unk_01B0;
    /* 01B4 */ float unk_01B4;
    /* 01B8 */ float unk_01B8;
    /* 01BC */ float unk_01BC;
    /* 01C0 */ float unk_01C0;
    /* 01C4 */ uint32_t unk_01C4;

    void AdjustTrajectoryDuration(uint32_t duration);
    void ApplyMotionScriptFunctionRotationAndScale(const SimpleScriptIO& io, C44Matrix& matrix);
    int32_t ApplyMotionScriptFunctionTranslation(const SimpleScriptIO& io, C44Matrix& matrix);
    int32_t CallMotionScriptFunction(SimpleScriptIO& io);
    void DetermineMissType();
    uint32_t GetHitResult() const;
    uint32_t GetMissReason() const;
    int32_t IsHit() const;
    void SetHitResult(uint32_t result);
    void SetRemoteCollisionPos(const C3Vector& position);
    C3Vector TransformPositionByTransport();
    void UpdateProgress(float distance);
    void DeleteSelf();
};

#endif
