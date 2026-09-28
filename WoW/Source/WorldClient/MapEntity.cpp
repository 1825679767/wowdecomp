#include "MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

// STATIC: 0x00ADEEBC
float CMapStaticEntity::s_characterAmbient = 1.0f;

// STATIC: 0x00AEEDF0
C3Vector CMapStaticEntity::s_interiorSunDir = C3Vector(-0.30822f, -0.30822f, -0.9f);

// VERIFY: b7c5af51 manual
// OFFSET: 0x007A0B50
CMapStaticEntity::CMapStaticEntity() {
    this->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_sphere.r = 0.0f;
    this->m_scale = 0.0f;
    this->unk_007C &= 0xFFFE4000;
    this->m_distanceToCamera = 0.0f;
    this->unk_0080 = 0.0f;
    this->m_model = nullptr;
    this->m_diffuseLightScale = 0.0f;
    this->m_unkCounter = 0;
    this->unk_0028 = 0;
    this->unk_0029 = 0;
    this->unk_002A = 0;
    this->unk_002B = 0;
    this->m_fadeLevel = 0;
    this->m_vec2 = C3Vector(0.0f, 0.0f, 0.0f);
    this->unk_0025 = 2;
}

// VERIFY: eeb1ad46 manual
// OFFSET: 0x007A0FE0
CMapEntity::CMapEntity() {
    this->unk_00A8 = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_func = nullptr;
    this->m_funcParam = nullptr;
    this->m_funcParam64 = 0;
    this->m_funcParam32 = 0;
    this->unk_00B4 = 0;
    this->m_groundType = 0;
    this->unk_00BC = 0;
    this->unk_00BE = 0;
    this->unk_00A4 = 0xFFFFFFFF;
    this->unk_00C0.b = 0xFF;
    this->unk_00C0.g = 0x00;
    this->unk_00C0.r = 0x00;
    this->unk_00C0.a = 0x00;
    this->unk_00C4 = 0.0f;
    this->m_type |= 0x20;
}

// VERIFY: d83b5d3e manual
// OFFSET: 0x007C1730
void CMapStaticEntity::SelectLights(CM2Lighting* lighting) {
    if (CWorld::farFog > this->m_distanceToCamera) {
        if (this->m_flags & MAPOBJ_FLAG_UNPLACED) {
            this->m_flags = static_cast<MapObjFlag>(this->m_flags & ~MAPOBJ_FLAG_UNPLACED);
        }

        C3Vector diffuse = { 0.0f, 0.0f, 0.0f };
        C3Vector dir = { 0.0f, 0.0f, 0.0f };
        C3Vector ambient = { 0.0f, 0.0f, 0.0f };

        if ((this->m_flags & 4) != 0) {
            CM2Light* light = &CMap::s_mapLight->m_light;

            diffuse.x = light->m_dirColor.x * this->m_diffuseLightScale;
            diffuse.y = light->m_dirColor.y * this->m_diffuseLightScale;
            diffuse.z = light->m_dirColor.z * this->m_diffuseLightScale;

            dir = light->m_dir;

            ambient = C3Vector(this->m_m2AmbietColor);
        } else {
            diffuse = C3Vector(this->m_m2DiffuseColor);
            diffuse.x *= this->m_diffuseLightScale;
            diffuse.y *= this->m_diffuseLightScale;
            diffuse.z *= this->m_diffuseLightScale;

            ambient = C3Vector(this->m_m2AmbietColor);

            dir = CMapStaticEntity::s_interiorSunDir;

            if ((this->unk_007C & 0x1000) != 0) {
                DayNight::DNInfo* dayNight = DayNight::GetInfo();
                float blend = this->m_m2DiffuseColor.a / 255.0f;

                dir.x += (dayNight->m_light1.m_dir.x - dir.x) * blend;
                dir.y += (dayNight->m_light1.m_dir.y - dir.y) * blend;
                dir.z += (dayNight->m_light1.m_dir.z - dir.z) * blend;

                dir.Normalize();
            }
        }

        if ((this->unk_007C & 0x8000) != 0) {
            ambient.x *= CMapStaticEntity::s_characterAmbient;
            ambient.y *= CMapStaticEntity::s_characterAmbient;
            ambient.z *= CMapStaticEntity::s_characterAmbient;

            if (ambient.x >= 1.0f) {
                ambient.x = 1.0f;
            }

            if (ambient.y >= 1.0f) {
                ambient.y = 1.0f;
            }

            if (ambient.z >= 1.0f) {
                ambient.z = 1.0f;
            }
        }

        lighting->AddAmbient(ambient);
        lighting->AddDiffuse(diffuse, dir);
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if ((this->m_flags & 0x8000) != 0) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.m_color), dayNight->m_fogInterior.m_start, dayNight->m_fogInterior.m_end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
    }
}
