#include "CSimpleCamera.h"

// VERIFY: fbf6cda8 auto
// OFFSET: 0x005FEE70
float CSimpleCamera::FOV() const {
    return this->m_fov;
}

// VERIFY: 98278b28 auto
// OFFSET: 0x005FEE80
C3Vector CSimpleCamera::Forward() const {
    return C3Vector(this->m_facing.a0, this->m_facing.a1, this->m_facing.a2);
}

// VERIFY: 8b1f5f49 auto
// OFFSET: 0x005FEEA0
C3Vector CSimpleCamera::Right() const {
    return C3Vector(this->m_facing.b0, this->m_facing.b1, this->m_facing.b2);
}

// VERIFY: 8ec84a54 auto
// OFFSET: 0x005FEEC0
C3Vector CSimpleCamera::Up() const {
    return C3Vector(this->m_facing.c0, this->m_facing.c1, this->m_facing.c2);
}
