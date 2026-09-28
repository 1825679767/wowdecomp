#include <cstdint>
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LightIntBandRec.h"
#include "WoW/Source/DB/Rec/LightParamsRec.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/World.h"

// VERIFY: 7c5384b3 manual
// OFFSET: 0x007EAE70
static void GetBandTimeRange(const int32_t* times, int32_t count, int32_t time, int32_t& index, float& t, int32_t& next) {
    next = 0;
    t = 0.0f;
    index = 0;

    if (count <= 0) {
        return;
    }

    do {
        int32_t start = times[index];
        next = (index + 1) % count;
        int32_t end = times[next];

        if (end > start) {
            if (time >= start && time <= end) {
                t = static_cast<float>(time - start) / (end - start);
                return;
            }
        } else if (time <= end || time >= start) {
            end += 2880;

            if (time < start) {
                time += 2880;
            }

            t = static_cast<float>(time - start) / (end - start);
            return;
        }

        index++;
    } while (index < count);
}

// VERIFY: ba90e0db manual
// OFFSET: 0x007EB070
static CImVector InterpolateIntBand(const LightIntBandRec& rec, int32_t time) {
    CImVector color;

    if (!rec.m_num) {
        *reinterpret_cast<uint32_t*>(&color) = 0xFF000000;
        return color;
    }

    int32_t index;
    float t;
    int32_t next;
    GetBandTimeRange(rec.m_time, rec.m_num, time, index, t, next);

    const CImVector& from = *reinterpret_cast<const CImVector*>(&rec.m_data[index]);
    const CImVector& to = *reinterpret_cast<const CImVector*>(&rec.m_data[next]);

    int32_t r = CMath::fint_((to.r - from.r) * t + from.r);
    int32_t g = CMath::fint_((to.g - from.g) * t + from.g);
    int32_t b = CMath::fint_((to.b - from.b) * t + from.b);

    *reinterpret_cast<uint32_t*>(&color) = ((((r & 0xFF) | 0xFFFFFF00) << 8 | (g & 0xFF)) << 8) | (b & 0xFF);
    return color;
}

// VERIFY: d2967a23 manual
// OFFSET: 0x007EAF60
static void TransformMapCoord(LightRec* rec, bool transform) {
    if ((rec->m_gameCoords[0] != 0.0f || rec->m_gameCoords[1] != 0.0f || rec->m_gameCoords[2] != 0.0f) && transform) {
        rec->m_gameCoords[0] *= 1.0f / 36.0f;
        rec->m_gameCoords[1] *= 1.0f / 36.0f;
        rec->m_gameCoords[2] *= 1.0f / 36.0f;
        rec->m_gameFalloffStart *= 1.0f / 36.0f;
        rec->m_gameFalloffEnd *= 1.0f / 36.0f;

        float offsetX = 0.0f;
        float offsetY = 0.0f;

        if (!World::MapIsDungeon()) {
            offsetX = 51200.0f / 3.0f;
            offsetY = 51200.0f / 3.0f;
        }

        float x = -(rec->m_gameCoords[0] - offsetY);
        float z = rec->m_gameCoords[1];
        rec->m_gameCoords[0] = -(rec->m_gameCoords[2] - offsetX);
        rec->m_gameCoords[1] = x;
        rec->m_gameCoords[2] = z;
    }
}

// VERIFY: c7d146ee auto
// OFFSET: 0x007EB010
static void TransformLights(bool transform) {
    for (int32_t i = 0; i < g_lightDB.m_numRecords; i++) {
        LightRec* rec = i >= 0 && i < g_lightDB.m_numRecords ? &g_lightDB.m_records[i] : nullptr;
        TransformMapCoord(rec, transform);
    }
}

// VERIFY: ae55f679 auto
// OFFSET: 0x007EB060
void InitLights(bool transform) {
    TransformLights(transform);
}

// VERIFY: b70039a6 auto
// OFFSET: 0x007EBF30
CImVector DayNight::LoadLightIntBand(int32_t time, const LightParamsRec* params, int32_t band) {
    LightIntBandRec rec;
    g_lightIntBandDB.GetRecord(band + 18 * params->m_id - 17, &rec);
    return InterpolateIntBand(rec, time);
}
