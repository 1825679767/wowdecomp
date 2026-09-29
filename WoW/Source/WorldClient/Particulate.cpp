#include "Particulate.h"
#include <cmath>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CRandom.h"

// VERIFY: 464f8a1f auto
// OFFSET: 0x0079B360
void Particulate::SetScale(float scale) {
    this->m_particleSize = scale;
}

// VERIFY: a93d2c97 manual
// OFFSET: 0x0079E100
Particulate::Particulate(float particleSize, float spawnAreaSize, const char* textureName) {
    this->m_unkByte = 0;
    this->m_texture = nullptr;
    this->m_particleSize = particleSize;
    this->m_maxParticles = sizeof(this->m_particles) / sizeof(this->m_particles[0]);
    this->m_spawnAreaSize = spawnAreaSize;
    this->SetTexture(textureName);
    this->InitParticles(2);
    this->InitMovement();
}

// VERIFY: 1e6bbe9a auto
// OFFSET: 0x0079B340
Particulate::~Particulate() {
    if (this->m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_texture));
    }
}

// VERIFY: 04a44b4f manual
// OFFSET: 0x0079B8E0
void Particulate::InitParticles(uint32_t count) {
    float minSize = this->m_particleSize * 0.5f;
    float sizeRange = this->m_particleSize * 1.5f - minSize;
    float halfArea = 0.5f * this->m_spawnAreaSize;

    for (uint32_t i = 0; i < this->m_maxParticles; i++) {
        this->m_particles[i].m_position = C3Vector(CRandom::ufloat_(g_rndSeed) * this->m_spawnAreaSize - halfArea, CRandom::ufloat_(g_rndSeed) * this->m_spawnAreaSize - halfArea, CRandom::ufloat_(g_rndSeed) * this->m_spawnAreaSize - halfArea);
        this->m_particles[i].m_size = CRandom::ufloat_(g_rndSeed) * sizeRange + minSize;
    }

    this->m_unkValue = count;
}

// VERIFY: d68e0ec8 manual
// OFFSET: 0x0079BCC0
void Particulate::InitMovement() {
    float theta = CRandom::sfloat_(g_rndSeed) * 3.1415927f;
    float phi = CRandom::sfloat_(g_rndSeed) * 3.1415927f;
    float sinTheta = sin(theta);

    this->m_direction.y = sin(phi) * sinTheta;
    this->m_direction.x = sinTheta * cos(phi);
    this->m_direction.z = cos(theta) * 0.25f;

    if (this->m_direction.z < 0.0f) {
        this->m_direction.z = -this->m_direction.z;
    }

    float invLength = 1.0f / sqrt(this->m_direction.x * this->m_direction.x + this->m_direction.y * this->m_direction.y + this->m_direction.z * this->m_direction.z);
    this->m_direction.x = this->m_direction.x * invLength;
    this->m_direction.y = this->m_direction.y * invLength;
    this->m_direction.z = this->m_direction.z * invLength;

    this->m_timer = 0.0f;
    this->m_speed = (CRandom::ufloat_(g_rndSeed) + 1.0f) * 0.0125f;
    this->m_rotationSpeed = (CRandom::ufloat_(g_rndSeed) + 1.0f) * 0.005f;
}

// VERIFY: d40a0168 manual
// OFFSET: 0x0079DFF0
void Particulate::SetTexture(const char* textureName) {
    if (this->m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_texture));
    }

    CStatus status;
    this->m_texture = TextureCreate(textureName, CGxTexFlags(GxTex_LinearMipNearest, 0, 0, 0, 0, 0, 1), &status, 0);
    SysMsgAdd(&status);
}
