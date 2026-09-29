#include "MapLoad.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

// VERIFY: 1ce54c55 manual
// OFFSET: 0x00780CD0
void CMapStaticEntity::ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    CMapStaticEntity* entity = reinterpret_cast<CMapStaticEntity*>(userArg);

    lighting->m_flags |= 0x10;
    auto dayNight = DayNight::GetInfo();
    if (entity) {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
        entity->SelectLights(lighting);
        entity->SelectUnderwater(lighting);
        if ((entity->m_flags & MAPOBJ_FLAG_INTERIOR) != 0) {
            lighting->m_flags |= 8;
            return;
        }
    } else {
        lighting->AddAmbient(C3Vector(dayNight->m_light1.m_ambient));
        lighting->AddDiffuse(C3Vector(dayNight->m_light1.m_diffuse), dayNight->m_light1.m_dir);
    }
    lighting->m_flags &= ~8u;
}

// VERIFY: f8dfe4b0 manual
// OFFSET: 0x007C10C0
void CMapStaticEntity::SelectUnderwater(CM2Lighting* lighting) {
    if (!(this->unk_007C & 0x20)) {
        lighting->m_flags = (lighting->m_flags & ~0x60) | 0x20;
    } else if (!(this->unk_007C & 0x40)) {
        lighting->m_flags = (lighting->m_flags & ~0x60) | 0x40;
    } else {
        float d = -this->m_liquidHeight;
        lighting->m_flags |= 0x60;
        lighting->m_liquidPlane.n.x = 0.0f;
        lighting->m_liquidPlane.n.y = 0.0f;
        lighting->m_liquidPlane.n.z = 1.0f;
        lighting->m_liquidPlane.d = d;
    }
}

// VERIFY: 7043a8be manual
// OFFSET: 0x007C1AD0
void CMapStaticEntity::AdjustLightmap(const CImVector* color, CImVector* diffuse, CImVector limit, CImVector* ambient, uint8_t maxBrightness) {
    uint8_t brightest;

    if ((color->r > color->g ? color->r : color->g) > color->b) {
        brightest = color->r > color->g ? color->r : color->g;
    } else {
        brightest = color->b;
    }

    if (brightest < 1) {
        brightest = 1;
    }

    *diffuse = *color;

    if (brightest < limit.b) {
        C3Vector rgb(diffuse->r * 0.0039215689f, diffuse->g * 0.0039215689f, color->b * 0.0039215689f);
        C3Vector hsv(0.0f, 0.0f, 0.0f);
        RGBtoHSV(rgb, hsv);
        hsv.z = static_cast<float>(limit.b) / static_cast<float>(brightest) * hsv.z;
        HSVtoRGB(hsv, rgb);
        limit = rgb;
        *diffuse = limit;
    }

    *ambient = *color;

    if (brightest > maxBrightness) {
        int32_t scale = CMath::fint_(static_cast<float>(maxBrightness) * 255.0f / static_cast<float>(brightest) - 0.5f);
        uint8_t r = static_cast<uint8_t>((ambient->r * scale + 255) >> 8);
        uint8_t g = static_cast<uint8_t>((ambient->g * scale + 255) >> 8);
        uint8_t b = static_cast<uint8_t>((ambient->b * scale + 255) >> 8);
        ambient->r = r;
        ambient->g = g;
        ambient->b = b;
    }
}
