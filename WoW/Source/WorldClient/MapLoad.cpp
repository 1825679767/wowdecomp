#include "MapLoad.h"
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
