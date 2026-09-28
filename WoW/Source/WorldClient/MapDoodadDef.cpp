#include "MapDoodadDef.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x007B4FA0
void CMapDoodadDef::ExtendBounds(CMapBaseObj* parent) {
    if (parent->m_type & 0x4) {
        CMapChunk* chunk = static_cast<CMapChunk*>(parent);
        chunk->m_bbox.t.z = this->m_bbox.t.z >= chunk->m_bbox.t.z ? this->m_bbox.t.z : chunk->m_bbox.t.z;
    } else if (parent->m_type & 0x10) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(parent);
        group->m_bbox |= this->m_bbox;

        CMapBaseObjLink* link = group->m_parentLinkList.Head();
        static_cast<CMapObjDef*>(link->m_ref)->m_bbox |= this->m_bbox;
    }
}

// VERIFY: 3ef424d0 manual
// OFFSET: 0x007B55E0
void CMapDoodadDef::ExtendChunkBounds() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        this->ExtendBounds(link->m_ref);
    }
}

// VERIFY: 2d4db79f manual
// OFFSET: 0x007BDB10
void CMapDoodadDef::UpdateBounds() {
    CAaBox box;
    CAaSphere sphere;
    sphere.r = 0.0f;
    CAaBox collisionBox;

    if (this->m_model) {
        box = this->m_model->GetBoundingBox();
        sphere = this->m_model->GetBoundingSphere();

        CM2Model* model = this->m_model;

        if (!(model->m_shared->m_flags & 0x1)) {
            model->WaitForLoad(nullptr);
        }

        collisionBox = model->m_shared->m_data->m_collisionBounds.m_extent;
    }

    this->m_sphere.c = sphere.c * this->m_mat;
    this->m_sphere.r = this->m_scale * sphere.r;

    if (box.b.x <= box.t.x || box.b.y <= box.t.y || box.b.z <= box.t.z) {
        CWorldMath::TransformAABox(this->m_mat, box, this->m_bbox);
    } else {
        this->m_bbox.b = this->m_position;
        this->m_bbox.t = this->m_position;
    }

    CWorldMath::TransformAABox(this->m_mat, collisionBox, this->m_bboxDoodadDef);

    C3Vector center;
    center.x = (collisionBox.t.x + collisionBox.b.x) * 0.5f;
    center.y = (collisionBox.t.y + collisionBox.b.y) * 0.5f;
    center.z = 0.5f * (collisionBox.t.z + collisionBox.b.z);
    this->m_vec2 = center * this->m_mat;

    float size = this->m_bbox.t.x - this->m_bbox.b.x;

    if (this->m_bbox.t.y - this->m_bbox.b.y >= size) {
        size = this->m_bbox.t.y - this->m_bbox.b.y;
    }

    if (this->m_bbox.t.z - this->m_bbox.b.z >= size) {
        size = this->m_bbox.t.z - this->m_bbox.b.z;
    }

    uint8_t fadeLevel;

    for (fadeLevel = 0; fadeLevel < 4; fadeLevel++) {
        if (size <= CWorldView::s_fadeSize[fadeLevel]) {
            break;
        }
    }

    this->m_fadeLevel = fadeLevel;

    if (!(CWorld::enables & Enable_NoAnimation) && fadeLevel < 3) {
        this->m_model->m_fFlags &= ~0x40u;
    }
}

// VERIFY: 6f062999 manual
// OFFSET: 0x007C1150
void CMapDoodadDef::SelectLights(CM2Lighting* lighting) {
    if (this->m_flags & 0x2) {
        lighting->AddAmbient(C3Vector(this->m_m2AmbietColor));
        lighting->AddDiffuse(C3Vector(this->m_m2DiffuseColor), CMapStaticEntity::s_interiorSunDir);
    } else {
        CM2Light* light = &CMap::s_mapLight->m_light;
        lighting->AddAmbient(light->m_ambColor);

        float scale = this->m_diffuseLightScale;
        C3Vector dir = light->m_dir;
        lighting->AddDiffuse(C3Vector(light->m_dirColor.x * scale, light->m_dirColor.y * scale, scale * light->m_dirColor.z), dir);
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if (this->m_flags & 0x8000) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.m_color), dayNight->m_fogInterior.m_start, dayNight->m_fogInterior.m_end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
    }

    if (this->m_flags & 0x2) {
        lighting->m_flags |= 0x8;
    } else {
        lighting->m_flags &= ~0x8;
    }
}
