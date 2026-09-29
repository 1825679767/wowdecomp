#include "MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include <cmath>
#include <cstring>
#include <typeinfo>
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"

// STATIC: 0x00ADEEBC
float CMapStaticEntity::s_characterAmbient = 1.0f;

// STATIC: 0x00AEEDF0
C3Vector CMapStaticEntity::s_interiorSunDir = C3Vector(-0.30822f, -0.30822f, -0.9f);

// VERIFY: cc3d4a96 manual
// OFFSET: 0x007A0B50
CMapStaticEntity::CMapStaticEntity() {
    this->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_sphere.r = 0.0f;
    this->m_scale = 0.0f;
    this->unk_007C &= 0xFFFE4000;
    this->m_distanceToCamera = 0.0f;
    this->m_liquidHeight = 0.0f;
    this->m_model = nullptr;
    this->m_diffuseLightScale = 0.0f;
    this->m_unkCounter = 0;
    this->unk_0028 = 0;
    this->unk_0029 = 0;
    this->unk_002A = 0;
    this->unk_002B = 0;
    this->m_fadeLevel = 0;
    this->m_vec2 = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_culled = 2;
}

// VERIFY: cd4cc195 auto
// OFFSET: 0x007A0C70
void FogQueue::Erase(uint32_t index, uint32_t count) {
    if (index >= this->m_count) {
        return;
    }

    if (index + count > this->m_count) {
        count = this->m_count - index;
    }

    uint32_t tail = this->m_count - index - count;

    if (tail) {
        memmove(this->m_data + sizeof(FogQ) * index, this->m_data + sizeof(FogQ) * (index + count), sizeof(FogQ) * tail);
    }

    this->m_count -= count;
}

// VERIFY: 67998dc9 manual
// OFFSET: 0x007A0CD0
static void LerpInteriorFog(SMOFog::Fog* dst, const SMOFog::Fog* src, float t) {
    for (int32_t i = 0; i < 2; i++) {
        dst[i].m_end = (src[i].m_end - dst[i].m_end) * t + dst[i].m_end;
        dst[i].m_startScalar = t * (src[i].m_startScalar - dst[i].m_startScalar) + dst[i].m_startScalar;
        dst[i].m_color = DayNight::InterpColor(dst[i].m_color, src[i].m_color, t);
    }
}

// VERIFY: a41b0f80 manual
// OFFSET: 0x007A0ED0
FogQueue::FogQueue(const uint32_t* params) : CMemBlock(sizeof(FogQ) * params[0], 0, typeid(FogQ).raw_name(), SLOG_OBJECT) {
    uint32_t count = params[0];
    uint32_t chunk = params[1];

    this->m_chunk = chunk;
    this->m_capacity = count;
    this->m_count = 0;

    if (count) {
        this->m_count++;
        return;
    }

    if (!chunk) {
        return;
    }

    uint32_t grow = chunk;

    if (grow <= 1 - count) {
        grow = 1 - count;
    }

    if (this->Resize(sizeof(FogQ) * (grow + count), true)) {
        this->m_capacity += grow;
        this->m_count++;
    }
}

// VERIFY: 307e1c8c manual
// OFFSET: 0x007A0F50
void FogQueue::Push(float distance, uint32_t fogIndex) {
    if (this->m_count + 1 <= this->m_capacity) {
        this->m_count++;
    } else if (this->m_chunk) {
        uint32_t grow = this->m_chunk;

        if (grow <= this->m_count - this->m_capacity + 1) {
            grow = this->m_count - this->m_capacity + 1;
        }

        if (this->Resize(sizeof(FogQ) * (grow + this->m_capacity), true)) {
            this->m_capacity += grow;
            this->m_count++;
        }
    }

    FogQ* data = reinterpret_cast<FogQ*>(this->m_data);
    uint32_t index = this->m_count - 1;

    if (this->m_count != 1 && index != 1) {
        uint32_t parent;

        do {
            data = reinterpret_cast<FogQ*>(this->m_data);
            parent = index >> 1;

            if (distance < data[parent].m_distance) {
                break;
            }

            data[index] = data[parent];
            index >>= 1;
        } while (parent > 1);
    }

    data = reinterpret_cast<FogQ*>(this->m_data);
    data[index].m_distance = distance;
    data[index].m_fogIndex = fogIndex;
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

// VERIFY: 515c7062 manual
// OFFSET: 0x007A1150
int32_t CMapEntity::QueryCameraFog(SMOFog* fog, CMapObjDef** mapObjDef, uint8_t* found, TSGrowableArray<uint32_t>** groups, float* distance) {
    if (!CWorldScene::s_viewerMapObjDef) {
        return 0;
    }

    CMapObj* owner = CWorldScene::s_viewerMapObjDef->m_owner;

    if (!owner || owner->m_fogsCount == 1) {
        return 0;
    }

    C3Vector localPos = CWorldScene::s_activeWorldView * CWorldScene::s_viewerMapObjDef->m_invMat;
    *found = 0;

    for (uint32_t i = 0; i < CWorldScene::s_viewerMapObjGroups.m_count; i++) {
        CMapObjGroup* group = owner->GetGroup(CWorldScene::s_viewerMapObjGroups.m_data[i], 0);

        if (group && !(group->m_flags & 0x48)) {
            *found = 1;
            float portalDistance = owner->DistFromClosestExtPortal(group, localPos, 1);

            if (portalDistance < *distance) {
                *distance = portalDistance;
            }
        }
    }

    if (*found) {
        *mapObjDef = CWorldScene::s_viewerMapObjDef;
        *groups = &CWorldScene::s_viewerMapObjGroups;
    } else {
        *distance = 0.0f;
    }

    *fog = owner->m_fogList[0];

    uint32_t params[2] = { 32, 32 };
    static FogQueue s_fogQueue(params);

    CMapObjGroup* group = owner->GetGroup(CWorldScene::s_viewerMapObjGroups.m_data[0], 0);

    if (group) {
        for (uint32_t i = 0; i < 4; i++) {
            uint8_t fogIndex = group->m_fogs[i];

            if (!fogIndex) {
                continue;
            }

            const SMOFog& groupFog = owner->m_fogList[fogIndex];
            float dx = groupFog.m_position.x - localPos.x;
            float dy = groupFog.m_position.y - localPos.y;
            float dz = groupFog.m_position.z - localPos.z;
            float fogDistance = sqrt(dx * dx + dy * dy + dz * dz);

            if (fogDistance < groupFog.m_largerRadius && !(groupFog.m_flags & 1)) {
                s_fogQueue.Push(fogDistance, fogIndex);
            }
        }
    }

    while (s_fogQueue.m_count > 1) {
        FogQ entry = s_fogQueue.Pop();
        const SMOFog& groupFog = owner->m_fogList[entry.m_fogIndex];
        float clamped = entry.m_distance;

        if (clamped < 0.0f) {
            clamped = 0.0f;
        } else if (groupFog.m_largerRadius <= clamped) {
            clamped = groupFog.m_largerRadius;
        }

        float weight;

        if (clamped >= groupFog.m_smallerRadius) {
            weight = 1.0f - (clamped - groupFog.m_smallerRadius) / (groupFog.m_largerRadius - groupFog.m_smallerRadius);
        } else {
            weight = 1.0f;
        }

        LerpInteriorFog(&fog->m_fog, &groupFog.m_fog, weight);

        if (s_fogQueue.m_count == 1) {
            fog->m_flags = groupFog.m_flags;
        }
    }

    return 1;
}

// VERIFY: 3d0d7fa9 manual
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

            ambient = C3Vector(this->m_m2AmbientColor);
        } else {
            diffuse = C3Vector(this->m_m2DiffuseColor);
            diffuse.x *= this->m_diffuseLightScale;
            diffuse.y *= this->m_diffuseLightScale;
            diffuse.z *= this->m_diffuseLightScale;

            ambient = C3Vector(this->m_m2AmbientColor);

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

// VERIFY: feea4d8f manual
// OFFSET: 0x0097E080
FogQ FogQueue::Pop() {
    FogQ* data = reinterpret_cast<FogQ*>(this->m_data);
    FogQ top = data[1];
    FogQ last = data[this->m_count - 1];

    if (this->m_count) {
        this->Erase(this->m_count - 1, 1);
    }

    if (this->m_count < 2) {
        return top;
    }

    uint32_t limit = this->m_count - 1;
    uint32_t half = limit >> 1;
    uint32_t index = 1;

    if (half) {
        while (1) {
            uint32_t child = 2 * index;
            data = reinterpret_cast<FogQ*>(this->m_data);

            if (child < limit && data[child].m_distance <= data[child + 1].m_distance) {
                child++;
            }

            if (last.m_distance >= data[child].m_distance) {
                break;
            }

            data[index] = data[child];
            index = child;

            if (child > half) {
                break;
            }
        }
    }

    reinterpret_cast<FogQ*>(this->m_data)[index] = last;

    return top;
}
