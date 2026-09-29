#include "Missile_C.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"

// STATIC: 0x00CA0B58
CMissile* CMissile::s_inFlightMissileList;

// STATIC: 0x00CA0B60
uint32_t* CMissile::s_missileHeap;

// VERIFY: fd41554e auto
// OFFSET: 0x006FF210
void CMissile::AdjustAllMissileTrajectoryDurations(uint8_t castID, uint32_t duration) {
    for (CMissile* missile = s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_castID == castID) {
            missile->AdjustTrajectoryDuration(duration);
        }
    }
}

// VERIFY: 05173280 manual
// OFFSET: 0x00703BE0
void CMissile::Destroy() {
    CSimpleScriptManager::UnregisterCallback(CMissile::ScriptManagerCallback);
    delete s_missileHeap;
    s_missileHeap = nullptr;
}

// VERIFY: 00facf3f manual
// OFFSET: 0x00703B80
void CMissile::Initialize() {
    s_missileHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMissile), 64, "CMissile", true));
    CSimpleScriptManager::RegisterCallback(CMissile::ScriptManagerCallback);
}

// VERIFY: ffb5de8d auto
// OFFSET: 0x006FF2C0
void CMissile::OnRemoteMissileTrajectoryCollision(const MissileCollisionData& data) {
    for (CMissile* missile = s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == data.m_casterGuid && missile->m_castID == data.m_castID) {
            missile->m_flags |= 0x400000;
            missile->m_remoteCollisionPos = data.m_position;
        }
    }
}

// VERIFY: eb1164f7 auto
// OFFSET: 0x006FF250
void CMissile::StartFizzling(WOWGUID caster, uint8_t castID, uint32_t spellID) {
    for (CMissile* missile = s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == caster && missile->m_castID == castID && missile->m_spellID == spellID && !missile->m_fizzleTime) {
            missile->m_fizzleTime = OsGetAsyncTimeMs() + 500;

            if (!missile->m_fizzleTime) {
                missile->m_fizzleTime = 1;
            }
        }
    }
}

// VERIFY: a6a16617 manual
// OFFSET: 0x006FF130
void CMissile::AdjustTrajectoryDuration(uint32_t duration) {
    if (!(this->m_flags & 0x10000)) {
        return;
    }

    this->m_flags |= 0x40000;
    uint32_t now = OsGetAsyncTimeMs();
    float remaining = (float)(this->m_arrivalTime - now);
    this->m_adjustTime = now;

    if (!(0.0f < remaining)) {
        this->m_adjustedDuration = 1;
        return;
    }

    if (!duration) {
        duration = 1;
    }

    float ratio = duration / remaining;

    if (ratio < 0.5f) {
        ratio = 0.5f;
    } else if (ratio >= 2.0f) {
        ratio = 2.0f;
    }

    this->m_adjustedDuration = (uint32_t)(int64_t)(remaining * ratio);

    if (!this->m_adjustedDuration) {
        this->m_adjustedDuration = 1;
    }
}

// VERIFY: 4674cb82 manual
// OFFSET: 0x006FEB20
void CMissile::ApplyMotionScriptFunctionRotationAndScale(const SimpleScriptIO& io, C44Matrix& matrix) {
    if (0.0 != io.m_numbers.m_data[5]) {
        matrix.RotateAroundZ(io.m_numbers.m_data[5] * 0.017453292f);
    }

    if (0.0 != io.m_numbers.m_data[6]) {
        matrix.RotateAroundY(io.m_numbers.m_data[6] * 0.017453292f);
    }

    if (0.0 != io.m_numbers.m_data[7]) {
        matrix.RotateAroundX(io.m_numbers.m_data[7] * 0.017453292f);
    }

    const double* numbers = io.m_numbers.m_data;

    if (numbers[11] > 0.0) {
        double scale = numbers[11];

        if (0.0099999998f < scale) {
            matrix.Scale((float)scale);
        } else {
            matrix.Scale(0.0099999998f);
        }
    }
}

// VERIFY: 4f254f8a manual
// OFFSET: 0x006FF030
uint32_t CMissile::GetHitResult() const {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400)) {
        return this->m_hitResult;
    }

    if (this->m_currentTarget >= this->m_targetHitResults.m_count) {
        return 0;
    }

    return this->m_targetHitResults.m_data[this->m_currentTarget];
}

// VERIFY: 55dc76e0 manual
// OFFSET: 0x006FF070
uint32_t CMissile::GetMissReason() const {
    if (!(this->m_flags & 0x200)) {
        return this->m_missReason;
    }

    if (this->m_currentTarget >= this->m_targetMissReasons.m_count) {
        return 0;
    }

    return this->m_targetMissReasons.m_data[this->m_currentTarget];
}

// VERIFY: 4bf82194 manual
// OFFSET: 0x006FEFB0
int32_t CMissile::IsHit() const {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400)) {
        return this->m_flags & 1;
    }

    return this->m_currentTarget < this->m_targetHitResults.m_count && !this->m_targetHitResults.m_data[this->m_currentTarget];
}

// VERIFY: 95e36e53 manual
// OFFSET: 0x006FEFF0
void CMissile::SetHitResult(uint32_t result) {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400) || this->m_currentTarget >= this->m_targetHitResults.m_count) {
        this->m_hitResult = result;
    } else {
        this->m_targetHitResults.m_data[this->m_currentTarget] = result;
    }
}

// VERIFY: c4a5d905 auto
// OFFSET: 0x006FEBE0
void CMissile::SetRemoteCollisionPos(const C3Vector& position) {
    this->m_flags |= 0x400000;
    this->m_remoteCollisionPos = position;
}

// VERIFY: d87dbd12 manual
// OFFSET: 0x006FFBB0
void CMissile::DetermineMissType() {
    if (this->IsHit()) {
        return;
    }

    switch (this->GetHitResult()) {
        case 1:
        case 3:
        case 6: {
            this->m_flags |= 0x8;
            break;
        }

        case 2:
        case 7:
        case 8: {
            this->m_flags |= 0x20;
            break;
        }

        case 4: {
            this->m_flags |= 0x40;
            this->SetHitResult(9);
            break;
        }

        case 5: {
            this->m_flags |= 0x10;
            break;
        }

        case 11: {
            if (this->m_flags & 0x8000000) {
                this->m_flags = (this->m_flags & ~0x8000040) | 0x40;
            } else {
                this->m_flags |= 0x80;
            }

            break;
        }

        default: {
            this->m_flags |= 0x40;
            this->m_hitResult = 9;
            break;
        }
    }
}

// VERIFY: d3123a0c manual
// OFFSET: 0x006FF320
C3Vector CMissile::TransformPositionByTransport() {
    if (!this->m_transportGuid) {
        return this->m_impactPos;
    }

    C44Matrix matrix;

    if (MovementGetTransportMtxSafe(this->m_transportGuid, &matrix)) {
        this->m_worldImpactPos = this->m_impactPos * matrix;
        return this->m_worldImpactPos;
    }

    this->m_impactPos = this->m_worldImpactPos;
    this->m_transportGuid = WOWGUID();
    return this->m_worldImpactPos;
}

// VERIFY: e84e0383 manual
// OFFSET: 0x006FFED0
void CMissile::UpdateProgress(float distance) {
    C3Vector impact = this->TransformPositionByTransport();
    float dx = impact.x - this->m_position.x;
    float dy = impact.y - this->m_position.y;
    float dz = impact.z - this->m_position.z;
    float dySq = dy * dy;
    float dxSq = dx * dx;
    float remaining = sqrt(dxSq + dz * dz + dySq);
    float progress = 1.0f;

    if (!(remaining <= 1.0e-6f)) {
        progress = 1.0f - distance / remaining;
    }

    if (progress < this->m_progress) {
        progress = this->m_progress;
    }

    this->m_progress = progress;

    impact = this->TransformPositionByTransport();
    float hx = impact.x - this->m_launchPos.x;
    float hy = impact.y - this->m_launchPos.y;
    float total = sqrt(dxSq + dySq);
    float horizontalProgress = 1.0f;

    if (!(total <= 1.0e-6f)) {
        horizontalProgress = 1.0f - sqrt(hx * hx + hy * hy) / total;
    }

    if (horizontalProgress < this->m_horizontalProgress) {
        horizontalProgress = this->m_horizontalProgress;
    }

    this->m_horizontalProgress = horizontalProgress;
}

// VERIFY: 07b406e6 manual
// OFFSET: 0x00700550
int32_t CMissile::ApplyMotionScriptFunctionTranslation(const SimpleScriptIO& io, C44Matrix& matrix) {
    C3Vector impact = this->TransformPositionByTransport();
    C3Vector dir(impact.x - this->m_position.x, impact.y - this->m_position.y, 0.0f);
    float lengthSq = dir.y * dir.y + dir.x * dir.x;

    if (lengthSq > 2.3841858e-7f) {
        float scale = 1.0f / sqrt(lengthSq);
        dir.x = dir.x * scale;
        dir.y = dir.y * scale;
        dir.z = 0.0f * scale;
    }

    C3Vector side(-dir.y, dir.x, dir.z);
    C3Vector start(matrix.d0, matrix.d1, matrix.d2);
    const double* numbers = io.m_numbers.m_data;

    if (0.0 != numbers[1]) {
        double angle = numbers[0] * 0.017453292f;
        double s = sin(angle);
        matrix.d0 = s * side.x * numbers[1] + matrix.d0;
        matrix.d1 = s * side.y * numbers[1] + matrix.d1;
        matrix.d2 = cos(angle) * numbers[1] + matrix.d2;
    }

    matrix.d0 = side.x * numbers[2] + matrix.d0;
    matrix.d1 = numbers[2] * side.y + matrix.d1;
    matrix.d2 = numbers[2] * side.z + matrix.d2;

    matrix.d0 = dir.x * numbers[3] + matrix.d0;
    matrix.d1 = dir.y * numbers[3] + matrix.d1;
    matrix.d2 = numbers[3] * dir.z + matrix.d2;

    matrix.d2 = numbers[4] + matrix.d2;

    float dx = matrix.d0 - start.x;
    float dy = matrix.d1 - start.y;
    float dz = matrix.d2 - start.z;
    float moved = dx * dx + (dy * dy + dz * dz);

    if (moved > 0.2f) {
        this->m_flags |= 0x8000;
    } else {
        this->m_flags &= ~0x8000;
    }

    return moved > 0.2f;
}

// VERIFY: 88c0c9d0 manual
// OFFSET: 0x00700350
int32_t CMissile::CallMotionScriptFunction(SimpleScriptIO& io) {
    io.m_numbers.m_count = 12;
    io.m_numbers.m_data[0] = this->m_progress;
    io.m_numbers.m_data[1] = (double)(int32_t)(OsGetAsyncTimeMs() - this->unk_0180) * 0.001f;
    io.m_numbers.m_data[2] = this->unk_0110;
    io.m_numbers.m_data[3] = this->unk_0114;

    float dx = this->m_position.x - this->m_launchPos.x;
    float dy = this->m_position.y - this->m_launchPos.y;
    float dz = this->m_position.z - this->m_launchPos.z;
    io.m_numbers.m_data[4] = sqrt(dz * dz + dy * dy + dx * dx);

    C3Vector impact = this->TransformPositionByTransport();
    dx = impact.x - this->m_launchPos.x;
    dy = impact.y - this->m_launchPos.y;
    dz = impact.z - this->m_launchPos.z;
    io.m_numbers.m_data[5] = sqrt(dz * dz + dy * dy + dx * dx);
    io.m_numbers.m_data[6] = this->unk_0184;

    impact = this->TransformPositionByTransport();
    dx = impact.x - this->m_position.x;
    dy = impact.y - this->m_position.y;
    dz = impact.z - this->m_position.z;
    io.m_numbers.m_data[7] = sqrt(dz * dz + dy * dy + dx * dx);
    io.m_numbers.m_data[8] = this->unk_0188;
    io.m_numbers.m_data[9] = this->unk_018C;
    io.m_numbers.m_data[10] = this->unk_0190;
    io.m_numbers.m_data[11] = this->m_spellID;

    if (!CSimpleScriptManager::s_instance->CallFunction(this->unk_017C, io, io)) {
        return 0;
    }

    for (int32_t i = 0; i < 12; i++) {
        if (!_finite(io.m_numbers.m_data[i])) {
            io.m_numbers.m_data[i] = 0.0;
        }
    }

    if (io.m_numbers.m_data[8] > 0.0) {
        this->m_flags |= 0x1000000;
        this->unk_0024 = io.m_numbers.m_data[8];
    }

    if (io.m_numbers.m_data[9] > 0.0) {
        this->m_flags |= 0x1000000;
        this->unk_0024 = io.m_numbers.m_data[9] * this->unk_0028;
    }

    if (0.0 != io.m_numbers.m_data[10]) {
        float scale = io.m_numbers.m_data[10] + this->unk_0028;
        this->unk_0024 = scale;
        this->m_flags |= 0x1000000;
        this->unk_0024 = 0.0099999998f < scale ? scale : 0.0099999998f;
    }

    return 1;
}
