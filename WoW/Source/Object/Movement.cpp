#include "Movement.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/CMovementGlobals.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "_Triage/CMoveSpline.h"
#include "_Triage/CMovementStatus.h"

// STATIC: 0x00ADA370
TSList<CPlayerMoveEvent, TSGetExplicitLink<CPlayerMoveEvent> > CPlayerMoveQueue::s_freeList;

// STATIC: 0x00A37F3C
static const uint8_t s_volumeEdges[32] = { 0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2, 3, 3, 4, 4, 1, 1, 5, 2, 6, 3, 7, 4, 8, 5, 6, 6, 7, 7, 8, 8, 5 };

// STATIC: 0x00ADBA34
static World::FacetData s_moveFacets;

// VERIFY: 9482b603 manual
// OFFSET: 0x006EBC70
CPlayerMoveEvent* CPlayerMoveQueue::AllocEvent(uint32_t eventTime, MoveEventId eventId) {
    CPlayerMoveEvent* event = s_freeList.Head();

    if (event) {
        event->m_link.Unlink();
    } else {
        event = s_freeList.NewNode(0, 0, 0);
    }

    event->m_eventTime = eventTime;
    event->m_eventId = eventId;
    event->unk_0050 = 0;
    return event;
}

// VERIFY: 08a36d29 manual
// OFFSET: 0x006EC090
void CPlayerMoveQueue::Enqueue(CPlayerMoveEvent* event) {
    for (CPlayerMoveEvent* node = this->Head(); node; node = node->m_link.Next()) {
        if ((int32_t)(event->m_eventTime - node->m_eventTime) < 0) {
            this->LinkNode(event, 2, node);
            return;
        }
    }

    this->LinkToTail(event);
}

// VERIFY: 9b07340c manual
// OFFSET: 0x006EC1F0
void CPlayerMoveQueue::FreeAll() {
    if (MovementGetGlobals()) {
        s_freeList.Combine(this, 2, nullptr);
    } else {
        this->Clear();
    }
}

// VERIFY: ee0cfb58 manual
// OFFSET: 0x006EB4E0
void CPlayerMoveQueue::FreeAllExceptTimeSync() {
    CPlayerMoveEvent* event = this->Head();

    while (event) {
        CPlayerMoveEvent* next = event->m_link.Next();

        if (event->m_eventId != MOVEEVENT_TIME_SYNC) {
            event->m_link.Unlink();
            s_freeList.LinkToTail(event);
        }

        event = next;
    }
}

// VERIFY: e65bbdf5 manual
// OFFSET: 0x006EAC00
int32_t CPlayerMoveQueue::HasEventBetween(MoveEventId first, MoveEventId last) {
    for (CPlayerMoveEvent* event = this->Head(); event; event = event->m_link.Next()) {
        if (event->m_eventId >= first && event->m_eventId <= last) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 7f5da03e auto
// OFFSET: 0x006EB590
void CPlayerMoveQueue::UnlinkEventById(MoveEventId eventId) {
    for (CPlayerMoveEvent* event = this->Head(); event; event = event->m_link.Next()) {
        if (event->m_eventId == eventId) {
            event->m_link.Unlink();
            s_freeList.LinkToTail(event);
            return;
        }
    }
}

// VERIFY: 216c2ed6 manual
// OFFSET: 0x0075B610
ClipPolygon::ClipPolygon(const ClipPolygon& source) {
    this->m_count = source.m_count;
    memcpy(this->m_v, source.m_v, sizeof(C3Vector) * source.m_count);
    memcpy(this->m_flags, source.m_flags, sizeof(int32_t) * source.m_count);
}

// VERIFY: 6a25e158 auto
// OFFSET: 0x006E9290
void CMovement_C::WrapFacingToRange(float& facing, float min, float max) {
    while (min > facing) {
        facing = facing + 6.2831855f;
    }

    if (max > facing) {
        return;
    }

    float previous;
    float wrapped = facing;

    do {
        previous = wrapped;
        wrapped = previous - 6.2831855f;
    } while (wrapped > max);

    facing = wrapped;

    if (wrapped <= min && previous - max < min - wrapped) {
        facing = previous;
    }
}

// VERIFY: 29f6438b auto
// OFFSET: 0x006E97D0
int32_t CMovement_C::AddTimeSkewSample(uint32_t localTime, uint32_t serverTime) {
    int32_t skew = serverTime + this->m_timeSkew - localTime;

    if ((uint32_t)(skew + 0x8000) > 0xFFFF) {
        skew = ~((skew + 0x8000) >> 31) - 0x8000;
    }

    this->m_timeSkewSamples[this->m_timeSkewIndex] = skew;
    this->m_timeSkewIndex = (this->m_timeSkewIndex + 1) & 31;

    for (int32_t i = 0; i < 32; i++) {
        if (this->m_timeSkewSamples[i] > skew) {
            skew = this->m_timeSkewSamples[i];
        }
    }

    return skew;
}

// VERIFY: 366620da auto
// OFFSET: 0x0075CD00
void CMovement_C::BuildCollisionBox(const C3Vector& position, CAaBox& box) const {
    box.b = position;
    box.t = position;
    box.b.x = box.b.x - this->m_collisionRadius;
    box.b.y = box.b.y - this->m_collisionRadius;
    box.t.x = box.t.x + this->m_collisionRadius;
    box.t.y = box.t.y + this->m_collisionRadius;
    box.t.z = this->m_collisionHeight + box.t.z;
}

// VERIFY: 5ec7444f manual
// OFFSET: 0x0075C8F0
void CMovement_C::BuildCollisionVolumePlanes(const C3Vector& position, float height, float radius, C4Plane* planes) const {
    planes[0].n.x = -1.0f;
    planes[0].n.y = 0.0f;
    planes[0].n.z = 0.0f;
    planes[0].d = position.x - radius;

    planes[1].n.x = 1.0f;
    planes[1].n.y = 0.0f;
    planes[1].n.z = 0.0f;
    planes[1].d = -position.x - radius;

    planes[2].n.x = 0.0f;
    planes[2].n.y = 1.0f;
    planes[2].n.z = 0.0f;
    planes[2].d = -position.y - radius;

    planes[3].n.x = 0.0f;
    planes[3].n.y = -1.0f;
    planes[3].n.z = 0.0f;
    planes[3].d = position.y - radius;

    planes[4].n.x = 0.0f;
    planes[4].n.y = 0.0f;
    planes[4].n.z = 1.0f;
    planes[4].d = -position.z - height;

    planes[5].n = C3Vector(-0.87964189f, 0.0f, -0.4756366f);
    planes[5].d = position.z * 0.4756366f + position.x * 0.87964189f + position.y * 0.0f;

    planes[6].n = C3Vector(0.87964189f, 0.0f, -0.4756366f);
    planes[6].d = position.z * 0.4756366f - position.x * 0.87964189f + position.y * 0.0f;

    planes[7].n = C3Vector(0.0f, 0.87964189f, -0.4756366f);
    planes[7].d = position.z * 0.4756366f + position.x * 0.0f - position.y * 0.87964189f;

    planes[8].n = C3Vector(0.0f, -0.87964189f, -0.4756366f);
    planes[8].d = position.z * 0.4756366f + position.x * 0.0f + position.y * 0.87964189f;
}

// VERIFY: 5e433e2e manual
// OFFSET: 0x0075E250
C3Vector CMovement_C::CalcRunWalkBlockingObstaclePush(const C3Vector& move, float dt, float maxDistance, const C3Vector& normal) const {
    if (normal.z < 0.0f && -normal.z > 0.64278764f) {
        return C3Vector(0.0f, 0.0f, 0.0f);
    }

    C2Vector direction(normal);
    float lengthSq = direction.y * direction.y + direction.x * direction.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        direction.x = direction.x * scale;
        direction.y = direction.y * scale;
    }

    float push = (-normal.x * move.x + -normal.y * move.y + -normal.z * move.z) * dt;
    float denominator = normal.y * direction.y + normal.x * direction.x + normal.z * 0.0f;
    float distance = fabs(denominator) < 0.00000023841858f ? push : push / denominator;
    float resultX = direction.x * (distance + 0.001f);
    float resultY = direction.y * (distance + 0.001f);

    C2Vector move2D(move);
    float moveX = move2D.x * dt;
    float moveY = dt * move2D.y;
    float totalX = moveX + resultX;
    float totalY = moveY + resultY;
    float totalSq = totalY * totalY + totalX * totalX;

    if (maxDistance * maxDistance < totalSq) {
        float scale = maxDistance / sqrt(totalSq);
        resultX = totalX * scale - moveX;
        resultY = totalY * scale - moveY;
    }

    return C3Vector(resultX, resultY, 0.0f);
}

// VERIFY: 2f98d9fc manual
// OFFSET: 0x0075D740
int32_t CMovement_C::ComputeSteepSurfacePushNormalImpl(const C4Plane& plane) const {
    float top = this->m_collisionRadius * 1.849399f;
    C3Vector points[5];
    points[0] = this->m_position;
    points[1] = C3Vector(this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[2] = C3Vector(this->m_position.x - this->m_collisionRadius, this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[3] = C3Vector(this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[4] = C3Vector(-this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, top + this->m_position.z);

    for (uint32_t i = 0; i < 5; i++) {
        if (fabs(fabs(points[i].x * plane.n.x + points[i].z * plane.n.z + points[i].y * plane.n.y + plane.d)) < 0.0013888889f) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 08eb7639 manual
// OFFSET: 0x0075D890
int32_t CMovement_C::PlaneIntersectsVolume(const C4Plane& plane) const {
    float top = this->m_collisionRadius * 1.849399f;
    C3Vector points[9];
    points[0] = this->m_position;
    points[1] = C3Vector(this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[2] = C3Vector(-this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[3] = C3Vector(this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, this->m_position.z + top);
    points[4] = C3Vector(-this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, top + this->m_position.z);
    points[5] = C3Vector(this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z);
    points[6] = C3Vector(this->m_position.x - this->m_collisionRadius, this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z);
    points[7] = C3Vector(this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z);
    points[8] = C3Vector(-this->m_collisionRadius + this->m_position.x, -this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z);

    for (uint32_t i = 0; i < 32; i += 2) {
        const C3Vector& a = points[s_volumeEdges[i]];
        const C3Vector& b = points[s_volumeEdges[i + 1]];
        float distanceA = fabs(a.y * plane.n.y + a.z * plane.n.z + a.x * plane.n.x + plane.d);
        float distanceB = fabs(b.y * plane.n.y + b.z * plane.n.z + b.x * plane.n.x + plane.d);

        if (distanceA * distanceB < 0.0f) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 5e0b6fee manual
// OFFSET: 0x0075DB00
C3Vector CMovement_C::PushOffObstacleEdge(const C3Vector& point, const C3Vector& direction, uint32_t facet) const {
    const CFacet& data = s_moveFacets.facets.m_data[facet];

    C3Vector edge = data.v[1] - data.v[0];
    float lengthSq = edge.y * edge.y + edge.z * edge.z + edge.x * edge.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        edge.x = edge.x * scale;
        edge.y = edge.y * scale;
        edge.z = edge.z * scale;
    }

    float t = (point.z - data.v[0].z) * edge.z + (point.y - data.v[0].y) * edge.y + (point.x - data.v[0].x) * edge.x;
    float dz = point.z - (t * edge.z + data.v[0].z);
    float dy = point.y - (edge.y * t + data.v[0].y);
    float dx = point.x - (t * edge.x + data.v[0].x);
    float best = dx * dx + dy * dy + dz * dz;

    C3Vector edge1 = data.v[2] - data.v[1];
    lengthSq = edge1.z * edge1.z + edge1.y * edge1.y + edge1.x * edge1.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        edge1.x = edge1.x * scale;
        edge1.y = edge1.y * scale;
        edge1.z = scale * edge1.z;
    }

    t = (point.z - data.v[1].z) * edge1.z + (point.y - data.v[1].y) * edge1.y + (point.x - data.v[1].x) * edge1.x;
    dy = point.y - (edge1.y * t + data.v[1].y);
    dz = point.z - (edge1.z * t + data.v[1].z);
    dx = point.x - (edge1.x * t + data.v[1].x);
    float distance = dx * dx + (dz * dz + dy * dy);

    if (best > distance) {
        best = distance;
        edge = edge1;
    }

    C3Vector edge2 = data.v[0] - data.v[2];
    lengthSq = edge2.y * edge2.y + edge2.z * edge2.z + edge2.x * edge2.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        edge2.x = edge2.x * scale;
        edge2.y = edge2.y * scale;
        edge2.z = scale * edge2.z;
    }

    t = (point.z - data.v[2].z) * edge2.z + (point.y - data.v[2].y) * edge2.y + (point.x - data.v[2].x) * edge2.x;
    dx = point.x - (edge2.x * t + data.v[2].x);
    dy = point.y - (edge2.y * t + data.v[2].y);
    dz = point.z - (edge2.z * t + data.v[2].z);

    if (dx * dx + dz * dz + dy * dy < best) {
        edge = edge2;
    }

    C3Vector push(data.plane.n.z * edge.y - data.plane.n.y * edge.z, edge.z * data.plane.n.x - data.plane.n.z * edge.x, edge.x * data.plane.n.y - edge.y * data.plane.n.x);

    if (direction.z * push.z + direction.y * push.y + push.x * direction.x < 0.0f) {
        push = C3Vector(-push.x, -push.y, -push.z);
    }

    return push;
}

// VERIFY: ddb278fe manual
// OFFSET: 0x0075CFE0
int32_t CMovement_C::IsFacetOverhead(uint32_t facet) const {
    float left = this->m_position.x - this->m_collisionRadius;
    float right = this->m_collisionRadius + this->m_position.x;
    float front = this->m_position.y - this->m_collisionRadius;
    float back = this->m_collisionRadius + this->m_position.y;
    float top = this->m_collisionHeight + this->m_position.z;
    C3Vector corners[4] = { C3Vector(left, front, top), C3Vector(right, front, top), C3Vector(left, back, top), C3Vector(right, back, top) };
    const CFacet& data = s_moveFacets.facets.m_data[facet];

    for (uint32_t i = 0; i < 4; i++) {
        if (fabs(corners[i].z * data.plane.n.z + corners[i].x * data.plane.n.x + corners[i].y * data.plane.n.y + data.plane.d) < 0.0013888889f) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: c4ba2cd0 manual
// OFFSET: 0x0075D2D0
int32_t CMovement_C::IsFacetWall(uint32_t facet) const {
    const CFacet& data = s_moveFacets.facets.m_data[facet];

    if (data.plane.n.z > 0.64278764f) {
        return 0;
    }

    return data.plane.n.z >= 0.0f || -data.plane.n.z <= 0.64278764f;
}

// VERIFY: be7db241 manual
// OFFSET: 0x006EC3B0
void CMovement_C::FixupQueuedSetFacingEvents(float delta) {
    for (CPlayerMoveEvent* event = this->m_moveQueue.Head(); event; event = event->m_link.Next()) {
        if (event->m_eventId == MOVEEVENT_SET_FACING || event->unk_0050) {
            event->m_facing = CMath::normalizeangle0to2pi_(delta + event->m_facing);
        }
    }
}

// VERIFY: 7291cd32 manual
// OFFSET: 0x006E9520
float CMovement_C::GetRemainingStepUpHeight() const {
    float height = this->m_unit->IsClientControlled() ? this->m_stepUpHeight : 2.0f;

    if (this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        height -= this->m_position.z - this->m_stepUpStartZ;
    }

    if (height <= 0.0f) {
        return 0.0f;
    }

    return height;
}

// VERIFY: fe524558 auto
// OFFSET: 0x006E8FC0
float CMovement_C::GetStepUpHeight() const {
    if (!this->m_unit->IsClientControlled()) {
        return 2.0f;
    }

    return this->m_stepUpHeight;
}

// VERIFY: 7f04ce16 manual
// OFFSET: 0x006E9F50
uint32_t CMovement_C::GetMoveStartTime(uint32_t elapsed) {
    if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
        return this->m_anchorElapsedMs;
    }

    if (!(this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING))) {
        return this->m_anchorElapsedMs;
    }

    this->m_moveStartDelay += elapsed;
    uint32_t delay = this->m_moveStartDelay;

    if (delay <= 500) {
        return this->m_anchorElapsedMs;
    }

    int32_t lag = delay - 1500;

    if (lag < 0) {
        lag = 0;
    }

    int32_t ramp = delay - 500;

    if (ramp > 1000) {
        ramp = 1000;
    }

    int32_t time = this->m_anchorElapsedMs - lag;

    if (time < 0) {
        time = 0;
    }

    int32_t result = time - ramp * ramp / 2000;

    if (result < 0) {
        return 0;
    }

    return result;
}

// VERIFY: 27d22089 manual
// OFFSET: 0x006EA7E0
int32_t CMovement_C::Interpolate(uint32_t now, uint32_t elapsed, C3Vector* position, float* facing, float* pitch) {
    CPlayerMoveEvent* next = this->m_moveQueue.Head();
    int32_t delta;

    if (!next || (delta = next->m_eventTime - now) < 0) {
        this->m_flags2 &= ~(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT | MOVEMENTFLAG2_INTERPOLATED_TURNING | MOVEMENTFLAG2_INTERPOLATED_PITCHING);
        return 0;
    }

    float t = (float)delta / (float)this->m_interpolation;
    float inverse = 1.0f - t;

    if (this->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT) {
        C3Vector interpolated;
        interpolated.x = (next->m_position.x - this->m_interpolationPos.x * t) * inverse + position->x * t;
        interpolated.y = (next->m_position.y - this->m_interpolationPos.y * t) * inverse + position->y * t;
        interpolated.z = (next->m_position.z - this->m_interpolationPos.z * t) * inverse + position->z * t;

        float dx = interpolated.x - this->m_position.x;
        float dy = interpolated.y - this->m_position.y;
        float maxDistance = elapsed * 0.001f * 60.0f;

        if (dx * dx + dy * dy + 0.00000095367432f <= maxDistance * maxDistance) {
            *position = interpolated;
        } else {
            this->m_flags2 &= ~MOVEMENTFLAG2_INTERPOLATED_MOVEMENT;
        }
    }

    if (this->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_TURNING) {
        float target = CMath::normalizeangle0to2pi_(next->m_facing - t * this->m_interpolationFacing);
        *facing = CMath::normalizeangle0to2pi_(CMath::normalizeAngleNegPiToPi_(target - *facing) * inverse + *facing);
    }

    if (this->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_PITCHING) {
        float target = CMath::normalizeangle0to2pi_(next->m_pitch - t * this->m_interpolationPitch);
        *pitch = CMath::normalizeangle0to2pi_(CMath::normalizeAngleNegPiToPi_(target - *pitch) * inverse + *pitch);
    }

    return this->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT;
}

// VERIFY: 14204d2d auto
// OFFSET: 0x005FEDE0
int32_t CMovement_C::IsJumping() const {
    return (this->m_flags & MOVEMENTFLAG_FALLING) && 0.0f != this->m_fallVelocity;
}

// VERIFY: 47ecbc25 auto
// OFFSET: 0x006E9440
int32_t CMovement_C::IsValidPosition() const {
    return World::IsValidPosition(this->m_position.x, this->m_position.y, this->m_position.z, 2.0f) != 0;
}

// VERIFY: 28844f7c auto
// OFFSET: 0x006E90D0
void CMovement_C::OnSetRunMode(int32_t run) {
    this->SetRunMode(run);
}

// VERIFY: f0dec4a3 auto
// OFFSET: 0x006E9280
void CMovement_C::OnSkipTime(uint32_t time) {
    this->m_gameTime += time;
}

// VERIFY: 15cccdda auto
// OFFSET: 0x006E9240
void CMovement_C::OnWaterWalk(int32_t enable) {
    this->WalkOnWater(enable);
}

// VERIFY: 061b2da3 auto
// OFFSET: 0x0075B4F0
void CMovement_C::RestoreMoveState(const MoveState& state) {
    this->m_anchorPos = state.m_anchorPos;
    this->m_anchorFacing = state.m_anchorFacing;
    this->m_anchorPitch = state.m_anchorPitch;
    this->m_anchorElapsedMs = state.m_anchorElapsedMs;
    this->m_fallTimeMs = state.m_fallTimeMs;
    this->m_moveDir = state.m_moveDir;
    this->m_moveDir2D = state.m_moveDir2D;
    this->m_flags = state.m_flags;
    this->m_currentSpeed = state.m_currentSpeed;
}

// VERIFY: 2e29bfd8 auto
// OFFSET: 0x0075B480
void CMovement_C::SaveMoveState(MoveState& state) const {
    state.m_anchorPos = this->m_anchorPos;
    state.m_anchorFacing = this->m_anchorFacing;
    state.m_anchorPitch = this->m_anchorPitch;
    state.m_anchorElapsedMs = this->m_anchorElapsedMs;
    state.m_fallTimeMs = this->m_fallTimeMs;
    state.m_moveDir = this->m_moveDir;
    state.m_moveDir2D = this->m_moveDir2D;
    state.m_flags = this->m_flags;
    state.m_currentSpeed = this->m_currentSpeed;
}

// VERIFY: 20aefe0e manual
// OFFSET: 0x006E9BB0
void CMovement_C::SendTimeSyncResp(uint32_t clientTime, uint32_t counter) {
    CDataStore msg;
    msg.Put((uint32_t)CMSG_TIME_SYNC_RESPONSE);
    msg.Put(counter);
    msg.Put(clientTime);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: dea84484 manual
// OFFSET: 0x006EA6A0
void CMovement_C::SetInterpolation(uint32_t time) {
    this->m_flags2 &= ~(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT | MOVEMENTFLAG2_INTERPOLATED_TURNING | MOVEMENTFLAG2_INTERPOLATED_PITCHING);
    this->m_interpolation = 0;

    CPlayerMoveEvent* next = this->m_moveQueue.Head();

    if (!next || !next->unk_0050 || next->m_eventId == MOVEEVENT_TELEPORT) {
        return;
    }

    if (this->m_transportGuid != next->m_transportGuid) {
        return;
    }

    uint32_t interpolation = next->m_eventTime - time;

    if (!interpolation) {
        return;
    }

    if (this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) {
        this->m_flags2 |= MOVEMENTFLAG2_INTERPOLATED_MOVEMENT;
    }

    if (CMath::fnotequal(this->m_facing, next->m_facing)) {
        this->m_flags2 |= MOVEMENTFLAG2_INTERPOLATED_TURNING;
    }

    if ((this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && CMath::fnotequal(this->m_pitch, next->m_pitch)) {
        this->m_flags2 |= MOVEMENTFLAG2_INTERPOLATED_PITCHING;
    }

    this->m_interpolationPos = next->m_position - this->m_position;
    this->m_interpolationFacing = CMath::normalizeAngleNegPiToPi_(next->m_facing - this->m_facing);
    this->m_interpolationPitch = CMath::normalizeAngleNegPiToPi_(next->m_pitch - this->m_pitch);
    this->m_interpolation = interpolation;
}

// VERIFY: 0eac7dea auto
// OFFSET: 0x006E96A0
void CMovement_C::SetSplineFaceAngle(float facing) {
    this->m_spline->m_flags |= SPLINEFLAG_FINAL_ANGLE;
    this->m_spline->m_face.m_angle = facing;
}

// VERIFY: 8b3481cb manual
// OFFSET: 0x006E9640
void CMovement_C::SetSplineFacePoint(const C3Vector& point) {
    this->m_spline->m_flags |= SPLINEFLAG_FINAL_POINT;
    CMoveSplineFaceData& face = this->m_spline->m_face;
    face.m_point[0] = point.x;
    face.m_point[1] = point.y;
    face.m_point[2] = point.z;
}

// VERIFY: 42b61ef1 auto
// OFFSET: 0x006E9670
void CMovement_C::SetSplineFaceTarget(const WOWGUID& target) {
    this->m_spline->m_flags |= SPLINEFLAG_FINAL_TARGET;
    this->m_spline->m_face.m_target = target;
}

// VERIFY: 50aa2f7e auto
// OFFSET: 0x006ECAE0
void CMovement_C::AddPlayerChangeSeatsEvent(uint32_t eventTime, MoveEventId eventId, bool needAck, WOWGUID transport, uint8_t seat) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, eventId);
    event->m_facing = 0.0f;
    event->m_pitch = 0.0f;
    event->m_transportGuid = transport;
    event->m_needAck = needAck;
    event->m_seat = seat;
    event->m_ackCounter = 0;
    event->m_moveExtraFlags = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: ea0758f0 auto
// OFFSET: 0x006EC840
void CMovement_C::AddPlayerMoveEvent(uint32_t eventTime, MoveEventId eventId, bool needAck, uint32_t ackCounter, float facing, float pitch, uint16_t extraFlags) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, eventId);
    event->m_facing = facing;
    event->m_pitch = pitch;
    event->m_needAck = needAck;
    event->m_ackCounter = ackCounter;
    event->m_moveExtraFlags = extraFlags;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: b9e313d9 auto
// OFFSET: 0x006EF2A0
void CMovement_C::OnAscendDescendStartLocal(uint32_t eventTime, int32_t ascend) {
    this->AddPlayerMoveEvent(eventTime, ascend ? MOVEEVENT_START_ASCEND : MOVEEVENT_START_DESCEND, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: d22ae73c auto
// OFFSET: 0x006EF310
void CMovement_C::OnAscendDescendStopLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP_ASCEND, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 76dfcd21 auto
// OFFSET: 0x006EF060
void CMovement_C::OnCanFlyLocal(uint32_t eventTime, uint32_t ackCounter, int32_t canFly) {
    this->AddPlayerMoveEvent(eventTime, canFly ? MOVEEVENT_SET_CAN_FLY : MOVEEVENT_UNSET_CAN_FLY, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 88ede5f4 auto
// OFFSET: 0x006EF150
void CMovement_C::OnCanTransitionBetweenSwimAndFlyLocal(uint32_t eventTime, uint32_t ackCounter, int32_t canTransition) {
    this->AddPlayerMoveEvent(eventTime, canTransition ? MOVEEVENT_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY : MOVEEVENT_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 4f5e403f auto
// OFFSET: 0x006EE690
void CMovement_C::OnDisableGravityLocal(uint32_t eventTime, uint32_t ackCounter, int32_t disable) {
    this->AddPlayerMoveEvent(eventTime, disable ? MOVEEVENT_GRAVITY_DISABLE : MOVEEVENT_GRAVITY_ENABLE, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 21a1bec5 auto
// OFFSET: 0x006EF540
void CMovement_C::OnDismissControlledVehicleLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_DISMISS_VEHICLE, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: efedeeb5 auto
// OFFSET: 0x006ECCF0
void CMovement_C::OnFallLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_START_FALLING, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: f9f6ad58 auto
// OFFSET: 0x006EEE50
void CMovement_C::OnFeatherFallLocal(uint32_t eventTime, uint32_t ackCounter, int32_t featherFall) {
    this->AddPlayerMoveEvent(eventTime, featherFall ? MOVEEVENT_FEATHER_FALL : MOVEEVENT_NORMAL_FALL, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 1b613e65 auto
// OFFSET: 0x006EE120
void CMovement_C::OnFlightBackSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_FLIGHT_BACK_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: d16823ed auto
// OFFSET: 0x006EE0D0
void CMovement_C::OnFlightBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_FLIGHT_BACK_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: b7c435c0 auto
// OFFSET: 0x006EF230
void CMovement_C::OnFlightLocal(uint32_t eventTime, int32_t flying) {
    this->AddPlayerMoveEvent(eventTime, flying ? MOVEEVENT_SET_FLYING : MOVEEVENT_UNSET_FLYING, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 435caaf0 auto
// OFFSET: 0x006EE030
void CMovement_C::OnFlightSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_FLIGHT_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 2e542412 auto
// OFFSET: 0x006EDFE0
void CMovement_C::OnFlightSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_FLIGHT_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: ad9f01bd auto
// OFFSET: 0x006EEEF0
void CMovement_C::OnHoverLocal(uint32_t eventTime, uint32_t ackCounter, int32_t hover) {
    this->AddPlayerMoveEvent(eventTime, hover ? MOVEEVENT_SET_HOVER : MOVEEVENT_UNSET_HOVER, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 814c2c88 auto
// OFFSET: 0x006ECC20
void CMovement_C::OnJumpLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_JUMP, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 25983ce1 auto
// OFFSET: 0x006ECC80
void CMovement_C::OnLaunchLocal(uint32_t eventTime, uint32_t ackCounter, const C2Vector& direction, float speed, float verticalSpeed) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_KNOCK_BACK);
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    event->m_value = direction.x;
    event->m_launchDirY = direction.y;
    event->m_launchSpeed = speed;
    event->m_launchVerticalSpeed = verticalSpeed;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: f22b99fe auto
// OFFSET: 0x006ECB50
void CMovement_C::OnMoveStartLocal(uint32_t eventTime, int32_t forward) {
    this->AddPlayerMoveEvent(eventTime, (MoveEventId)(forward == 0), true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: be31074a auto
// OFFSET: 0x006ECDE0
void CMovement_C::OnMoveStopLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 26308ea1 auto
// OFFSET: 0x006EE300
void CMovement_C::OnPitchRateChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_PITCH_RATE);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 8a5eeb60 auto
// OFFSET: 0x006EE2B0
void CMovement_C::OnPitchRateChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_PITCH_RATE);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 85716caa auto
// OFFSET: 0x006EECA0
void CMovement_C::OnPitchStopLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP_PITCH, true, 0, 0.0f, 0.0f, 0);
    this->m_moveQueue.UnlinkEventById(MOVEEVENT_STOP_PITCH_AT_ANGLE);
}

// VERIFY: d068e179 auto
// OFFSET: 0x006EF490
void CMovement_C::OnPitchToAngleLocal(uint32_t eventTime, float pitch) {
    CVehicle_C* vehicle = this->m_unit->m_vehicle;
    VehicleRec* rec = vehicle ? vehicle->m_vehicleRec : nullptr;

    if (rec && (rec->m_flags & 0x40)) {
        if (pitch < rec->m_pitchMin) {
            pitch = rec->m_pitchMin;
        } else if (rec->m_pitchMax <= pitch) {
            pitch = rec->m_pitchMax;
        }
    }

    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_PITCH_TO_ANGLE, true, 0, 0.0f, pitch, 0);
}

// VERIFY: 721988e2 auto
// OFFSET: 0x006EEFF0
void CMovement_C::OnRootLocal(uint32_t eventTime, uint32_t ackCounter, int32_t root) {
    this->AddPlayerMoveEvent(eventTime, root ? MOVEEVENT_ROOT : MOVEEVENT_UNROOT, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: b5f3f880 auto
// OFFSET: 0x006EDC70
void CMovement_C::OnRunBackSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_RUN_BACK_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 83d31a1b auto
// OFFSET: 0x006EDC20
void CMovement_C::OnRunBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_RUN_BACK_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 7b9ea868 auto
// OFFSET: 0x006EDB80
void CMovement_C::OnRunSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_RUN_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 55f19341 auto
// OFFSET: 0x006EDB30
void CMovement_C::OnRunSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_RUN_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 31a38b65 auto
// OFFSET: 0x006EE7B0
void CMovement_C::OnSetCollisionHeight(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_COLLISION_HEIGHT);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 213e5c5a auto
// OFFSET: 0x006EE760
void CMovement_C::OnSetCollisionHeightLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_COLLISION_HEIGHT);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: b7c54e54 auto
// OFFSET: 0x006EE460
void CMovement_C::OnSetPitchLocal(uint32_t eventTime, float pitch) {
    CVehicle_C* vehicle = this->m_unit->m_vehicle;
    VehicleRec* rec = vehicle ? vehicle->m_vehicleRec : nullptr;

    if (rec && (rec->m_flags & 0x40)) {
        if (pitch < rec->m_pitchMin) {
            pitch = rec->m_pitchMin;
        } else if (rec->m_pitchMax <= pitch) {
            pitch = rec->m_pitchMax;
        }
    }

    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_SET_PITCH, true, 0, 0.0f, pitch, 0);
}

// VERIFY: df4fb107 auto
// OFFSET: 0x006ECF10
void CMovement_C::OnSetRunModeLocal(uint32_t eventTime, int32_t run) {
    this->AddPlayerMoveEvent(eventTime, run ? MOVEEVENT_SET_RUN_MODE : MOVEEVENT_SET_WALK_MODE, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 2facaa06 auto
// OFFSET: 0x006ECBB0
void CMovement_C::OnStrafeStartLocal(uint32_t eventTime, int32_t left) {
    this->AddPlayerMoveEvent(eventTime, left ? MOVEEVENT_START_STRAFE_LEFT : MOVEEVENT_START_STRAFE_RIGHT, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 73637dde auto
// OFFSET: 0x006ECE40
void CMovement_C::OnStrafeStopLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP_STRAFE, true, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 8ba9d836 auto
// OFFSET: 0x006EDF40
void CMovement_C::OnSwimBackSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_SWIM_BACK_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: c6690fa9 auto
// OFFSET: 0x006EDEF0
void CMovement_C::OnSwimBackSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_SWIM_BACK_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: a35422a3 auto
// OFFSET: 0x006EDE50
void CMovement_C::OnSwimSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_SWIM_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 7f0fc534 auto
// OFFSET: 0x006EDE00
void CMovement_C::OnSwimSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_SWIM_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 25306062 auto
// OFFSET: 0x006EE9F0
void CMovement_C::OnSwimStart(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_START_SWIM, false, 0, 0.0f, 0.0f, 0);
}

// VERIFY: e8806cc6 auto
// OFFSET: 0x006EEB20
void CMovement_C::OnSwimStop(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP_SWIM, false, 0, 0.0f, 0.0f, 0);
}

// VERIFY: e8d0639d auto
// OFFSET: 0x006EF370
void CMovement_C::OnTimeSyncRequest(uint32_t eventTime, uint32_t counter) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_TIME_SYNC, true, counter, 0.0f, 0.0f, 0);
}

// VERIFY: b49a921d auto
// OFFSET: 0x006EE210
void CMovement_C::OnTurnRateChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_TURN_RATE);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 003e4758 auto
// OFFSET: 0x006EE1C0
void CMovement_C::OnTurnRateChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_TURN_RATE);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 6df42ae1 auto
// OFFSET: 0x006ECEA0
void CMovement_C::OnTurnStopLocal(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_STOP_TURN, true, 0, 0.0f, 0.0f, 0);
    this->m_moveQueue.UnlinkEventById(MOVEEVENT_STOP_TURN_AT_ANGLE);
}

// VERIFY: 7f375340 auto
// OFFSET: 0x006EDD60
void CMovement_C::OnWalkSpeedChange(uint32_t eventTime, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_WALK_SPEED);
    event->m_value = value;
    event->m_needAck = 0;
    event->m_ackCounter = 0;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: d5ba5c2f auto
// OFFSET: 0x006EDD10
void CMovement_C::OnWalkSpeedChangeLocal(uint32_t eventTime, uint32_t ackCounter, float value) {
    CPlayerMoveEvent* event = CPlayerMoveQueue::AllocEvent(eventTime, MOVEEVENT_SET_WALK_SPEED);
    event->m_value = value;
    event->m_ackCounter = ackCounter;
    event->m_needAck = 1;
    this->m_moveQueue.Enqueue(event);
    if (!this->m_globalUnitsLink.IsLinked()) {
        CMovementGlobals* globals = MovementGetGlobals();

        if (globals) {
            globals->m_movers.LinkToHead(this);
        }
    }
}

// VERIFY: 6af8b4ef auto
// OFFSET: 0x006EED70
void CMovement_C::OnWaterWalkLocal(uint32_t eventTime, uint32_t ackCounter, int32_t waterWalk) {
    this->AddPlayerMoveEvent(eventTime, waterWalk ? MOVEEVENT_WATER_WALK : MOVEEVENT_LAND_WALK, true, ackCounter, 0.0f, 0.0f, 0);
}

// VERIFY: 181a5182 auto
// OFFSET: 0x006ECD50
void CMovement_C::QueueFallEvent(uint32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, MOVEEVENT_START_FALLING, this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover, 0, 0.0f, 0.0f, 0);
}

// VERIFY: 193562a4 manual
// OFFSET: 0x0075E040
int32_t CMovement_C::CheckFallImpactThreshold(float timeFallen, float elapsed, const C3Vector& move, int32_t force) const {
    if (force) {
        return 1;
    }

    if (this->m_fallVelocity < 0.0f) {
        float peak = this->TimeToJumpPeak();

        if (elapsed + timeFallen < peak) {
            return 1;
        }

        if (timeFallen > peak) {
            return 0;
        }

        float distance = (peak - timeFallen) * this->m_currentSpeed;

        if (move.x * move.x + move.y * move.y < distance * distance) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 987977e3 manual
// OFFSET: 0x0075E3D0
uint32_t CMovement_C::GetFacetQueryFlags() const {
    uint32_t flags = 0x100111;

    if (!this->m_unit->IsClientControlled()) {
        flags = 0x102111;
    }

    if (this->m_unit->IsLocalClientControlled()) {
        flags |= 0x80000000;
    }

    uint32_t moveFlags = this->m_flags;

    if ((moveFlags & MOVEMENTFLAG_WATERWALKING) && !(moveFlags & MOVEMENTFLAG_SWIMMING) && (-0.6457718f < this->m_pitch || (this->m_flags2 & MOVEMENTFLAG2_UNK_0200))) {
        flags |= 0x10000;
    }

    if (moveFlags & MOVEMENTFLAG_FLYING) {
        flags |= 0x200;

        if (!(this->m_flags2 & MOVEMENTFLAG2_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY)) {
            flags |= 0x20000;
        }
    }

    CGUnit_C* unit = this->m_unit;

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10)) {
        flags |= 0x8000;
    }

    return flags;
}

// VERIFY: 44c1133a manual
// OFFSET: 0x0075CA80
void CMovement_C::BuildCollisionVolume(const C3Vector& position, float height, C4Plane* planes, C3Vector* points, uint8_t* faces) const {
    this->BuildCollisionVolumePlanes(position, height, this->m_collisionRadius, planes);

    float lowZ = this->m_collisionRadius * 1.849399f;

    points[0] = position;
    points[1] = C3Vector(position.x - this->m_collisionRadius, position.y - this->m_collisionRadius, lowZ + position.z);
    points[2] = C3Vector(position.x - this->m_collisionRadius, this->m_collisionRadius + position.y, lowZ + position.z);
    points[3] = C3Vector(this->m_collisionRadius + position.x, this->m_collisionRadius + position.y, lowZ + position.z);
    points[4] = C3Vector(this->m_collisionRadius + position.x, position.y - this->m_collisionRadius, lowZ + position.z);
    points[5] = C3Vector(position.x - this->m_collisionRadius, position.y - this->m_collisionRadius, height + position.z);
    points[6] = C3Vector(position.x - this->m_collisionRadius, this->m_collisionRadius + position.y, height + position.z);
    points[7] = C3Vector(this->m_collisionRadius + position.x, this->m_collisionRadius + position.y, height + position.z);
    points[8] = C3Vector(this->m_collisionRadius + position.x, position.y - this->m_collisionRadius, height + position.z);

    faces[0] = 1;
    faces[1] = 2;
    faces[2] = 6;
    faces[3] = 5;
    faces[4] = 3;
    faces[5] = 4;
    faces[6] = 8;
    faces[7] = 7;
    faces[8] = 2;
    faces[9] = 3;
    faces[10] = 7;
    faces[11] = 6;
    faces[12] = 4;
    faces[13] = 1;
    faces[14] = 5;
    faces[15] = 8;
    faces[16] = 5;
    faces[17] = 6;
    faces[18] = 7;
    faces[19] = 8;
    faces[20] = 0;
    faces[21] = 1;
    faces[22] = 2;
    faces[23] = 0;
    faces[24] = 3;
    faces[25] = 4;
    faces[26] = 0;
    faces[27] = 2;
    faces[28] = 3;
    faces[29] = 0;
    faces[30] = 4;
    faces[31] = 1;
}

// VERIFY: 2b0fe832 auto
// OFFSET: 0x006E91D0
int32_t CMovement_C::OnFlightBackSpeedChange(float value) {
    return this->ChangeFlightBackSpeed(value);
}

// VERIFY: 8ce26848 auto
// OFFSET: 0x006E91B0
int32_t CMovement_C::OnFlightSpeedChange(float value) {
    return this->ChangeFlightSpeed(value);
}

// VERIFY: e6981de6 auto
// OFFSET: 0x006E9210
int32_t CMovement_C::OnPitchRateChange(float value) {
    return this->ChangePitchRate(value);
}

// VERIFY: ffcdbdf3 auto
// OFFSET: 0x006E9130
int32_t CMovement_C::OnRunBackSpeedChange(float value) {
    return this->ChangeRunBackSpeed(value);
}

// VERIFY: 8076b053 auto
// OFFSET: 0x006E9110
int32_t CMovement_C::OnRunSpeedChange(float value) {
    return this->ChangeRunSpeed(value);
}

// VERIFY: bb3db46c auto
// OFFSET: 0x006E9190
int32_t CMovement_C::OnSwimBackSpeedChange(float value) {
    return this->ChangeSwimBackSpeed(value);
}

// VERIFY: 622e8b42 auto
// OFFSET: 0x006E9170
int32_t CMovement_C::OnSwimSpeedChange(float value) {
    return this->ChangeSwimSpeed(value);
}

// VERIFY: 56a6fec9 auto
// OFFSET: 0x006E91F0
int32_t CMovement_C::OnTurnRateChange(float value) {
    return this->ChangeTurnRate(value);
}

// VERIFY: a98a8ed5 auto
// OFFSET: 0x006E9150
int32_t CMovement_C::OnWalkSpeedChange(float value) {
    return this->ChangeWalkSpeed(value);
}

// VERIFY: b9bcc84b auto
// OFFSET: 0x006E9250
void CMovement_C::OnFeatherFall(int32_t enable) {
    this->FeatherFall(enable);
    this->PostFeatherFall();
}

// VERIFY: 4bc4b2f5 auto
// OFFSET: 0x006EF5A0
void CMovement_C::OnChangeSeatsOnControlledVehicleLocal(uint32_t eventTime, WOWGUID transport, uint8_t seat) {
    this->AddPlayerChangeSeatsEvent(eventTime, MOVEEVENT_CHANGE_SEAT, true, transport, seat);
}

// VERIFY: 047dad0f auto
// OFFSET: 0x006E9570
void CMovement_C::SetCollisionBox(float width, float height, float scale, float referenceScale, int32_t force) {
    float stepUpScale = 1.0f;

    if (!(fabs(referenceScale) < 2.3841858e-7f)) {
        stepUpScale = scale / referenceScale;
    }

    this->m_stepUpHeight = stepUpScale;
    this->m_collisionRadius = width * scale * 0.5f;

    if (force || !this->m_unit->IsClientControlled()) {
        this->m_collisionHeight = scale * height;
    }

    this->ValidateWorldPosition(CPassenger::GetPosition(this->m_position));
}

// VERIFY: 8b6b9a4f manual
// OFFSET: 0x006E9470
void CMovement_C::SnapToSpline(const C3Vector& position, int32_t force) {
    if (!force) {
        float dx = position.x - this->m_position.x;
        float dy = position.y - this->m_position.y;
        float dz = position.z - this->m_position.z;

        if (dx * dx + dy * dy + dz * dz < 9.0f) {
            return;
        }
    }

    this->m_position = position;

    if (this->m_spline->m_flags & SPLINEFLAG_FALLING) {
        this->m_fallTimeMs = this->m_spline->m_start;
        this->UpdateAnchors(0);
    } else {
        this->StopFallingAlwaysAnchor();
    }

    this->ValidateWorldPosition(CPassenger::GetPosition(this->m_position));
}

// VERIFY: bec49cf6 auto
// OFFSET: 0x00632050
void CMovement_C::ValidateWorldPosition(const C3Vector& position) const {
}

// VERIFY: 98dc8a0f auto
// OFFSET: 0x0075EB00
float CMovement_C::GetTimeJustFallen(float elapsed, float timeFallen, C3Vector* move, float* distance, int32_t force, float speed) {
    int32_t impact = this->CheckFallImpactThreshold(timeFallen, elapsed, *move, force);
    float moveTime = 0.0f;

    if (speed > 9.5367432e-7f) {
        C2Vector move2D(*move);
        moveTime = sqrt(move2D.x * move2D.x + move2D.y * move2D.y) / speed * elapsed;
    }

    float fallTime = this->CalcTimeFallen(this->m_fallStartZ - this->m_position.z - move->z, impact);

    if (!(timeFallen < fallTime)) {
        move->x = 0.0f;
        move->y = 0.0f;
        move->z = 0.0f;
        *distance = 0.0f;
        return 0.0f;
    }

    float remaining = fallTime - timeFallen;

    if (elapsed < remaining) {
        return elapsed;
    }

    if (moveTime > remaining) {
        float scale = remaining / moveTime;
        move->x = move->x * scale;
        move->y = scale * move->y;
        *distance = sqrt(move->x * move->x + move->y * move->y + move->z * move->z);
    }

    return remaining;
}

// VERIFY: 927722c0 manual
// OFFSET: 0x006EBD30
CMovement_C::CMovement_C(const C3Vector& position, float facing, const WOWGUID& ownerGuid, CGUnit_C* unit) : CMovementShared(WOWGUID(), position, facing, ownerGuid) {
    this->m_collisionRadius = 0.33333334f;
    this->m_collisionHeight = 2.0277777f;
    this->m_stepUpHeight = 1.0f;
    this->m_interpolationPos.x = 0.0f;
    this->m_interpolationPos.y = 0.0f;
    this->m_interpolationPos.z = 0.0f;
    this->m_timeSkewIndex = 0;
    this->m_interpolation = 0;
    this->m_moveStartDelay = 0;
    this->m_unit = unit;
    this->CalcDirection(0);

    for (uint32_t i = 0; i < 16; i++) {
        this->m_timeSkewSamples[i] = -50;
    }

    for (uint32_t i = 16; i < 32; i++) {
        this->m_timeSkewSamples[i] = 50;
    }

    this->m_timeSkew = 50;
}

// VERIFY: db4836af manual
// OFFSET: 0x0075E0C0
C3Vector CMovement_C::CalcRunWalkWalkableObstaclePush(const C3Vector& move, float* scale, C3Vector normal, int32_t useAltNormal, const C3Vector& altNormal) const {
    if (useAltNormal && normal.z <= 0.64278764f && !(fabs(altNormal.z * altNormal.z + altNormal.y * altNormal.y + altNormal.x * altNormal.x) < 2.3841858e-7f)) {
        normal.x = -altNormal.x;
        normal.y = -altNormal.y;
        normal.z = -altNormal.z;
    }

    float push = (-normal.x * move.x + -normal.y * move.y + -normal.z * move.z) * *scale;
    float height;

    if (fabs(normal.z) < 2.3841858e-7f) {
        height = push >= 0.0f ? 3.4028235e38f : -3.4028235e38f;
    } else {
        height = push / normal.z;
    }

    float result;

    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) && height < 0.0f) {
        *scale = 0.0f;
        result = this->GetRemainingStepUpHeight();
    } else if (this->GetRemainingStepUpHeight() >= height) {
        result = height;

        if (height < -this->GetStepUpHeight()) {
            *scale = -(this->GetStepUpHeight() * (*scale / height));
            result = -this->GetStepUpHeight();
        }
    } else {
        *scale = this->GetRemainingStepUpHeight() * (*scale / height);
        result = this->GetRemainingStepUpHeight();
    }

    return C3Vector(0.0f * result, 0.0f * result, result);
}

// VERIFY: 4914c81c manual
// OFFSET: 0x006E90E0
void CMovement_C::GetMoveStatus(NETMESSAGE msgId, uint32_t gameTime, CMovementStatus* status) const {
    status->m_moveFlags = this->m_flags & ~(MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_SPLINE_ENABLED | MOVEMENTFLAG_STATUS_APPLIED);
    CMovementShared::GetMoveStatus(msgId, gameTime, status);
}

// VERIFY: da8cd466 auto
// OFFSET: 0x006EB3B0
int32_t CMovement_C::HandlePendingActions() {
    uint32_t oldFlags = this->m_flags;

    if (oldFlags & MOVEMENTFLAG_PENDING_STOP) {
        this->ForceStopMove(1);

        if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && !this->m_moveQueue.HasEventBetween(MOVEEVENT_START_FORWARD, MOVEEVENT_START_BACKWARD)) {
            CGInputControl::GetActive()->UpdateMoveStopped();
        }
    }

    if (this->m_flags & MOVEMENTFLAG_PENDING_STRAFE_STOP) {
        this->ForceStopStrafe();
    }

    if (this->m_flags & (MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD)) {
        this->StartMove(this->m_flags & MOVEMENTFLAG_PENDING_FORWARD, 0);
    }

    if (this->m_flags & (MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT)) {
        this->StartStrafe(this->m_flags & MOVEMENTFLAG_PENDING_STRAFE_LEFT);
    }

    this->m_flags &= ~(MOVEMENTFLAG_PENDING_STOP | MOVEMENTFLAG_PENDING_STRAFE_STOP | MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD | MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT | MOVEMENTFLAG_PENDING_ROOT);
    return (oldFlags ^ this->m_flags) & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT);
}

// VERIFY: 29d51d84 manual
// OFFSET: 0x006E9FF0
void CMovement_C::Launch(const C2Vector& direction, float speed, float verticalSpeed) {
    if (this->m_flags & MOVEMENTFLAG_SWIMMING) {
        this->StopSwim();
    } else if (this->m_flags & MOVEMENTFLAG_FLYING) {
        this->StopFlying();
    }

    if (this->m_flags & MOVEMENTFLAG_ROOT) {
        this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_ROOT | MOVEMENTFLAG_PENDING_ROOT)) | MOVEMENTFLAG_PENDING_ROOT;
    }

    this->m_flags &= ~MOVEMENTFLAG_CAN_FLY;
    this->StartFalling(verticalSpeed);
    this->m_moveDir2D = direction;
    this->m_moveDir = C3Vector(direction.x, direction.y, 0.0f);

    WOWGUID transport = *(const WOWGUID*)&this->m_transportGuid;

    if (transport && this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
        C44Matrix matrix;
        MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);
        C44Matrix inverse = matrix.AffineInverse();

        this->m_moveDir = C3Vector(
            inverse.b0 * this->m_moveDir2D.y + this->m_moveDir2D.x * inverse.a0,
            inverse.b1 * this->m_moveDir2D.y + inverse.a1 * this->m_moveDir2D.x,
            inverse.b2 * this->m_moveDir2D.y + inverse.a2 * this->m_moveDir2D.x);
        this->m_moveDir2D = C2Vector(this->m_moveDir);

        float lengthSq = this->m_moveDir2D.y * this->m_moveDir2D.y + this->m_moveDir2D.x * this->m_moveDir2D.x;

        if (lengthSq > 2.3841858e-7f) {
            float scale = 1.0f / sqrt(lengthSq);
            this->m_moveDir2D.x = this->m_moveDir2D.x * scale;
            this->m_moveDir2D.y = scale * this->m_moveDir2D.y;
        }
    }

    this->m_currentSpeed = speed;
    this->m_flags = (this->m_flags & ~MOVEMENTFLAG_BACKWARD) | MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_PENDING_STOP;
}

// VERIFY: 9bb92d2b auto
// OFFSET: 0x006EBE50
void CMovement_C::SplineToggleEnableFlying(int32_t enable) {
    if (enable) {
        if (this->StartFlight()) {
            this->HandlePendingActions();
        }
    } else {
        this->StopFlying();
    }
}

// VERIFY: 6e48f367 auto
// OFFSET: 0x006E9B70
void CMovement_C::UpdateHeartbeatTimerA(uint32_t time) {
    if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
        MovementGetGlobals()->m_timeCounterMaybe = time + 500;
    }
}
