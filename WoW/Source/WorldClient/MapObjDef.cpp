#include "MapObjDef.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

// VERIFY: 18be6c93 auto
// OFFSET: 0x007B3990
bool CMapObjDef::TestAABox(C3Vector& start, C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end);
}

// VERIFY: 93842e5d auto
// OFFSET: 0x007B3DE0
CMapObjDefGroup::CMapObjDefGroup() {
    this->m_sphere.r = 0.0f;
    this->m_ambientColor = 0;
    this->m_doodadDefLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_entityLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_lightLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_unkLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_type |= 0x10;
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_UNPLACED);
    this->m_groupNum = -1;
    this->m_unkFlags = -1;
    this->unk_0058 = 0;
    this->unk_0064 = 0;
    this->unk_0060 = 0;
    this->m_liquidInstance = nullptr;
}

// VERIFY: 7ed1c1b6 manual
// OFFSET: 0x007B3F30
void CMapObjDefGroup::SelectLights(CM2Lighting* lighting) {
    lighting->AddLight(&CMap::s_mapLight->m_light);

    if (CWorld::farFog > this->m_distanceToCamera) {
        if (this->m_flags & MAPOBJ_FLAG_UNPLACED) {
            this->m_flags = static_cast<MapObjFlag>(this->m_flags & ~MAPOBJ_FLAG_UNPLACED);
        }

        for (CMapBaseObjLink* link = this->m_lightLinkList.Head(); link; link = this->m_lightLinkList.Next(link)) {
            CMapLight* light = static_cast<CMapLight*>(link->m_owner);

            if (light->m_light.m_visible && !light->unk_00D1) {
                lighting->AddLight(&light->m_light);
            }
        }
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if (this->m_flags & 0x8000) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.m_color), dayNight->m_fogInterior.m_start, dayNight->m_fogInterior.m_end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
    }
}

// VERIFY: 05a4df33 manual
// OFFSET: 0x007B4350
CMapObjDef::CMapObjDef() {
    this->m_sphere.c = C3Vector();
    this->m_sphere.r = 0.0f;
    this->m_argbColor = 0;
    this->m_nameId = 0;
    this->m_owner = nullptr;
    this->unk_00F8 = 0;
    this->m_type |= 0x8;
    this->m_doodadSet = 0;
    this->m_nameSet = 0;
    this->unk_0108 = 0;
    this->m_objectGuid = WOWGUID();
    this->m_unkFlags = 0xFFFF;
}

// VERIFY: 3855c196 manual
// OFFSET: 0x007B4500
CMapObjDef::~CMapObjDef() {
}
