#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/GfxSingletonManager.h"

// STATIC: 0x00DCE680
uint32_t CParticleEmitter2::s_initCount;

// STATIC: 0x00DCE684
CGxBuf* CParticleEmitter2::s_indexBuf;

// STATIC: 0x00DCE688
CGxPool* CParticleEmitter2::s_indexPool;

// STATIC: 0x00DCE890
uint8_t CParticleEmitter2::s_noIndexBuffer;

// STATIC: 0x00DCE8C4
void* ParticleSystemManager::sm_projectParam;

// STATIC: 0x00DCE8C8
int32_t (*ParticleSystemManager::sm_projectCallback)(const C3Vector&, float&, void*);

// VERIFY: c8cd89a9 manual
// OFFSET: 0x00979170
void CParticleEmitter2::Init() {
    if (!CParticleEmitter2::s_initCount) {
        if (CParticleEmitter2::s_noIndexBuffer) {
            CParticleEmitter2::s_initCount++;
            CParticleEmitter2::s_indexPool = nullptr;
            CParticleEmitter2::s_indexBuf = nullptr;
            return;
        }

        CParticleEmitter2::s_indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Static, sizeof(uint16_t) * 0x1FFF8, GxPoolHintBit_Unk2, "CParticleEmitter2_idx");
        CParticleEmitter2::s_indexBuf = g_theGxDevicePtr->BufCreate(CParticleEmitter2::s_indexPool, sizeof(uint16_t), 0x1FFF8, 0);
    }

    CParticleEmitter2::s_initCount++;
}
