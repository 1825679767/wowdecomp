#include "Passenger.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"

// VERIFY: 185d3db3 auto
// OFFSET: 0x004F48E0
CPassenger::CPassenger(const WOWGUID& transportGuid, const C3Vector& position, const WOWGUID& ownerGuid) : m_transportGuid(transportGuid), m_position(position) {
    this->m_facing = 0.0f;
    this->m_pitch = 0.0f;
    this->m_ownerGuid = &ownerGuid;
    this->unk_002C = 0;
}

// VERIFY: 3f84735d auto
// OFFSET: 0x004F4230
void CPassenger::PostInit(const C3Vector& position) {
    if (this->m_transportGuid) {
        if (!MovementAddToTransport(this, this->m_transportGuid, 1)) {
            this->m_transportGuid = WOWGUID();
            this->m_position = position;
        }
    }
}

// VERIFY: 871c8fad manual
// OFFSET: 0x004F4280
void CPassenger::Reenable() {
    WOWGUID transport = this->m_transportGuid;

    if (transport) {
        MovementAddToTransport(this, transport, 1);
    }
}

// VERIFY: 2668c115 auto
// OFFSET: 0x004F42A0
float CPassenger::GetFacing(float facing) const {
    if (!this->m_transportGuid) {
        return facing;
    }

    return CMath::normalizeangle0to2pi_(MovementGetTransportFacing(this->m_transportGuid) + facing);
}

// VERIFY: dcd90e3b auto
// OFFSET: 0x004F42E0
float CPassenger::GetRawFacing(float facing) const {
    if (!this->m_transportGuid) {
        return facing;
    }

    return CMath::normalizeangle0to2pi_(facing - MovementGetTransportFacing(this->m_transportGuid));
}

// VERIFY: eeaa5f5d auto
// OFFSET: 0x004F4460
C3Vector CPassenger::GetPosition(const C3Vector& position) const {
    WOWGUID transport = this->m_transportGuid;

    if (!transport) {
        return position;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);
    return position * matrix;
}

// VERIFY: d6a74e0d manual
// OFFSET: 0x004F4500
C3Vector CPassenger::GetRawPosition(const C3Vector& position) const {
    WOWGUID transport = this->m_transportGuid;

    if (!transport) {
        return position;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);
    return position * matrix.AffineInverse();
}
