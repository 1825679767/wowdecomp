#include "MapObjDef.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/WorldClient/MapLoad.h"

// VERIFY: 18be6c93 auto
// OFFSET: 0x007B3990
bool CMapObjDef::TestAABox(C3Vector& start, C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end);
}

// VERIFY: 62f4837e auto
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
    this->m_subzoneName = nullptr;
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

// VERIFY: 3113f3c8 manual
// OFFSET: 0x007B39B0
int32_t CMapObjDef::GetGroundType(uint32_t groupNum, uint16_t faceIndex) {
    CMapObjGroup* group = this->m_owner->GetGroup(groupNum, 0);

    if (group && faceIndex < group->m_polyListSize) {
        SMOPoly* poly = &group->m_polyList[faceIndex];

        if (poly) {
            return this->m_owner->m_materialList[poly->m_materialId].m_groundType;
        }
    }

    return -1;
}

// VERIFY: 14be0e9a manual
// OFFSET: 0x007B66E0
void CMapObjDef::UpdatePos(CMapObjDef* mapObjDef, const C3Vector* position, float facing, float a4, float a5) {
    mapObjDef->m_position = *position;
    mapObjDef->m_mat = C44Matrix();
    mapObjDef->m_mat.d0 = position->x;
    mapObjDef->m_mat.d1 = position->y;
    mapObjDef->m_mat.d2 = position->z;
    mapObjDef->m_mat.RotateAroundZ(facing);
    mapObjDef->m_mat.RotateAroundY(a4);
    mapObjDef->m_mat.RotateAroundX(a5);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();
    CMapObjDef::UpdateMoved(mapObjDef);
}

// VERIFY: 70a9844c manual
// OFFSET: 0x007B41F0
void CMapObjDefGroup::SetSequenceFinishedCallback(M2SequenceCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_doodadDefLinkList.Head(); link; link = this->m_doodadDefLinkList.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadSet && !doodadDef->unk_016C) {
            break;
        }

        if (doodadDef->m_model && doodadDef->unk_016C == doodadSet) {
            doodadDef->m_model->SetSequenceCallback(reinterpret_cast<void*>(callback), param);
        }
    }
}

// VERIFY: 5ecf6841 manual
// OFFSET: 0x007B4640
void CMapObjDef::SetSequenceFinishedCallback(M2SequenceCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_mapObjDefGroupLinkList.Head(); link; link = this->m_mapObjDefGroupLinkList.Next(link)) {
        static_cast<CMapObjDefGroup*>(link->m_owner)->SetSequenceFinishedCallback(callback, param, doodadSet);
    }
}

// VERIFY: a5601f23 manual
// OFFSET: 0x007B67B0
void CMapObjDef::UpdateMatrix(CMapObjDef* mapObjDef, const C44Matrix& matrix) {
    mapObjDef->m_position.x = matrix.d0;
    mapObjDef->m_position.y = matrix.d1;
    mapObjDef->m_position.z = matrix.d2;
    mapObjDef->m_mat = matrix;
    mapObjDef->m_invMat = matrix.AffineInverse();
    CMapObjDef::UpdateMoved(mapObjDef);
}

// VERIFY: 04ab0ef9 manual
// OFFSET: 0x007B42F0
bool CMapObjDefGroup::IsAllLoaded() {
    if (!(this->m_flags & MAPOBJ_FLAG_REFS_CREATED)) {
        return false;
    }

    for (CMapBaseObjLink* link = this->m_doodadDefLinkList.Head(); link; link = this->m_doodadDefLinkList.Next(link)) {
        CMapStaticEntity* doodadDef = static_cast<CMapStaticEntity*>(link->m_owner);

        if (doodadDef->m_model && !(static_cast<uint32_t>(doodadDef->m_flags) & 0x80000000)) {
            if (SFile::IsStreamingMode()) {
                doodadDef->m_model->LinkToAsyncHead(1);
            }

            return false;
        }
    }

    return true;
}

// VERIFY: c4f26801 manual
// OFFSET: 0x007B4760
bool CMapObjDef::IsAllLoaded() {
    if (!(this->m_flags & MAPOBJ_FLAG_PREPARED)) {
        return false;
    }

    if (!(this->m_flags & 0x4000)) {
        for (CMapBaseObjLink* link = this->m_mapObjDefGroupLinkList.Head(); link; link = this->m_mapObjDefGroupLinkList.Next(link)) {
            if (!static_cast<CMapObjDefGroup*>(link->m_owner)->IsAllLoaded()) {
                return false;
            }
        }

        this->m_flags = static_cast<MapObjFlag>(this->m_flags | 0x4000);
    }

    return true;
}
