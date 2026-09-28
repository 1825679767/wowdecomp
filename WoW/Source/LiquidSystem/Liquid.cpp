#include "Liquid.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D43B34
CDataAllocator Liquid::s_unk_D43B34(0x6C, 0x20);

// STATIC: 0x00B23F64
float Liquid::s_unk_B23F64 = 1.0f;

// STATIC: 0x00B23F68
int32_t Liquid::s_unk_B23F68 = 1;

// STATIC: 0x00D439E8
IDatabase<LiquidMaterialRec>* Liquid::s_liquidMaterialDB;

// STATIC: 0x00D439EC
IDatabase<LiquidTypeRec>* Liquid::s_liquidTypeDB;

// STATIC: 0x00D439F0
char Liquid::s_unk_D439F0[256];

// STATIC: 0x00D43AF0
int32_t Liquid::s_unk_D43AF0;

// STATIC: 0x00D43AF4
int32_t Liquid::s_unk_D43AF4;

// STATIC: 0x00D43AF8
int32_t Liquid::s_unk_D43AF8;

// STATIC: 0x00D43AFC
int32_t Liquid::s_unk_D43AFC;

// VERIFY: fb13c55c auto
// OFFSET: 0x008A1770
int32_t Liquid::Initialize(int32_t a1, float a2, int32_t a3, const char* a4, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB) {
    SStrCopy(Liquid::s_unk_D439F0, a4, sizeof(Liquid::s_unk_D439F0));
    Liquid::s_unk_B23F64 = a2;
    Liquid::s_unk_D43AF8++;
    Liquid::s_unk_D43AFC++;
    Liquid::s_unk_D43AF4 = a1;
    Liquid::s_unk_D43AF0 = a3;
    Liquid::s_unk_B23F68 = 1;
    Liquid::s_liquidTypeDB = liquidTypeDB;
    Liquid::s_liquidMaterialDB = liquidMaterialDB;
    return 1;
}

// VERIFY: c3031eda manual
// OFFSET: 0x008A2400
void Liquid::Shutdown() {
    int32_t count = Liquid::s_unk_D43AFC--;
    Liquid::s_liquidTypeDB = nullptr;
    Liquid::s_liquidMaterialDB = nullptr;

    if (count == 1) {
        Liquid::CMaterialBank::Destroy();
        Liquid::s_unk_D43AF0 = 0;
    }

    Liquid::CSettingsBank::Destroy();
    Liquid::ClearMaterials();
    Liquid::s_unk_D43B34.Clear(".?AVCInstance@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: 58e8bcb0 auto
// OFFSET: 0x008A1700
void Liquid::CInstance::AddRef() {
    this->m_refCount++;
}

// VERIFY: 01aeb3b0 manual
// OFFSET: 0x008A1710
void Liquid::CInstance::Purge() {
    this->m_geomFactory->Purge();
}

// VERIFY: e803d76a auto
// OFFSET: 0x008A1740
int32_t Liquid::UseCameraRelativeCoords() {
    return Liquid::s_unk_D43AF4;
}

// VERIFY: d02f3d04 auto
// OFFSET: 0x008A1750
float Liquid::WorldCoordScale() {
    return Liquid::s_unk_B23F64;
}

// VERIFY: ecbae4e7 auto
// OFFSET: 0x008A1760
const char* Liquid::GetShaderSuffix() {
    return Liquid::s_unk_D439F0;
}

// VERIFY: 6413183d manual
// OFFSET: 0x008A1A40
Liquid::CInstance::CInstance() {
    this->m_material = nullptr;
    this->m_settings = nullptr;
    this->m_geomFactory = nullptr;
    this->m_waveManager = nullptr;
    this->m_environment = nullptr;
    this->m_bounds.r = 0.0f;
    this->m_queued = 0;
    this->m_refCount = 1;
}

// VERIFY: d1684687 manual
// OFFSET: 0x008A1AC0
void Liquid::CInstance::Release() {
    if (this->m_refCount-- == 1) {
        this->m_geomFactory->Release();
        this->m_environment->Release();

        if (this->m_waveManager) {
            this->m_waveManager->Release();
        }

        Liquid::s_unk_D43B34.PutData(this, nullptr, 0);
    }
}

// VERIFY: 7f5df1db auto
// OFFSET: 0x008A1B00
Liquid::CInstance* Liquid::CInstance::Create() {
    void* mem = Liquid::s_unk_D43B34.GetData(0, ".?AVCInstance@Liquid@@", SLOG_OBJECT);
    return mem ? new (mem) Liquid::CInstance() : nullptr;
}
