#include "MapWeather.h"
#include <cfloat>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/Sand.h"
#include "WoW/Source/WorldClient/Rain.h"
#include <cmath>
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/DB/Rec/WeatherRec.h"

// STATIC: 0x00ADF1F8
float s_weatherDensity = 0.66f;

// STATIC: 0x00CD8528
uint8_t s_weatherTypeForced;

// VERIFY: 63454e2e manual
// OFFSET: 0x00783B30
int32_t Weather::TimeToFixed(float seconds) {
    float ticks = seconds * 1024.0f;
    return CMath::fint_(ticks - 0.5f);
}

// INLINED
WeatherGridCell::WeatherGridCell() {
    this->unk_1000 = -9999;
    this->unk_1004 = -9999;

    for (uint32_t i = 0; i < sizeof(this->m_heights) / sizeof(this->m_heights[0]); i++) {
        this->m_heights[i] = FLT_MAX;
    }
}

// VERIFY: b61c5f42 manual
// OFFSET: 0x00783FB0
WeatherGridBase::~WeatherGridBase() {
}

// VERIFY: 1c1b6708 manual
// OFFSET: 0x00784AB0
WeatherGrid::WeatherGrid() {
    this->unk_0004 = 0.0f;
    this->unk_0008 = 0.0f;
    this->unk_000C = 0.0f;
    this->unk_0010[0] = 0;
    this->unk_0010[1] = 0;
    this->unk_0010[2] = 0;
    this->unk_0010[3] = 0;
    this->unk_0010[4] = 0;
    this->unk_0010[5] = 0;

    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            this->m_cells[y][x] = new (SMemAlloc(sizeof(WeatherGridCell), __FILE__, __LINE__, 0)) WeatherGridCell();
        }
    }
}

// VERIFY: 6499eaa9 auto
// OFFSET: 0x00784B80
WeatherGrid::~WeatherGrid() {
    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            if (this->m_cells[y][x]) {
                SMemFree(this->m_cells[y][x], "delete", SLOG_FUNCTION, 0);
            }
        }
    }
}

// VERIFY: 9b569e0a manual
// OFFSET: 0x00787780
Weather::Weather() {
    this->unk_0000 = 0.0f;
    this->unk_0004 = 0.0f;
    this->unk_0008 = 0.0f;
    this->unk_000C = 0.0f;
    this->unk_0010 = 0.0f;
    this->unk_0014 = 0.0f;
    this->unk_0018 = 0;
    this->unk_001C = 0;
    this->unk_0020 = -1;
    this->m_densityChanged = 0;
    this->unk_0025 = 0;
    this->unk_0028 = 1.0f;
    this->unk_002C = 1.0f;
    this->unk_0030 = 1.0f;
    this->m_color.b = 0xFF;
    this->m_color.g = 0xFF;
    this->m_color.r = 0xFF;
    this->m_color.a = 0xFF;
    this->m_rain = nullptr;
    this->m_snow = nullptr;
    this->m_sand = nullptr;
    this->m_grid = nullptr;
    this->unk_0164 = 0.0f;
    this->unk_0168 = 0.0f;
    this->unk_016C = 0.0f;
    this->unk_0170 = 0.0f;
    this->unk_0174 = 0.0f;
    this->unk_0178 = 0.0f;
    this->unk_017C = 0.0f;
    this->unk_0180 = 0.0f;
    this->unk_0184 = 0.0f;
    this->unk_0188 = 0;
    this->unk_018C = 0.0f;
    this->unk_0190 = 0.0f;
    this->unk_0194 = 0;

    this->m_grid = new (SMemAlloc(sizeof(WeatherGrid), __FILE__, __LINE__, 0)) WeatherGrid();

    CVar::Register("weatherDensity", nullptr, 0, "2", CVarHandler_weatherDensity, CATEGORY_DEFAULT, false, nullptr, false);
    s_useShaders = CVar::Register("useWeatherShaders", nullptr, 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);

    uint32_t forcedType = 0;

    if (SRegLoadValue("Internal", "force-weather-type-on", 0, &forcedType) && forcedType > 0 && forcedType < 4) {
        this->SetType(forcedType, 1.0f, nullptr, false, 1.0f);
        s_weatherTypeForced = 1;
    }

    this->m_precipTexture[0] = '\0';
}

// VERIFY: b0352c89 manual
// OFFSET: 0x0078D540
Weather::~Weather() {
    this->Clear();

    delete this->m_grid;
    this->m_grid = nullptr;

    this->unk_014C.Clear();
    this->unk_0158.Clear();
}

// VERIFY: f3eaf113 auto
// OFFSET: 0x0078D0B0
void Weather::Clear() {
    if (this->m_rain) {
        Rain* rain = this->m_rain;
        rain->~Rain();
        SMemFree(rain, "delete", SLOG_FUNCTION, 0);
    }

    this->m_rain = nullptr;

    if (this->m_snow) {
        Snow* snow = this->m_snow;
        snow->~Snow();
        SMemFree(snow, "delete", SLOG_FUNCTION, 0);
    }

    this->m_snow = nullptr;

    if (this->m_sand) {
        Sand* sand = this->m_sand;
        sand->~Sand();
        SMemFree(sand, "delete", SLOG_FUNCTION, 0);
    }

    this->m_sand = nullptr;
}

// VERIFY: dbf9517f auto
// OFFSET: 0x0078D130
void Weather::Reset() {
    if (s_weatherTypeForced) {
        return;
    }

    this->Clear();
    this->unk_0004 = 0.0f;
    this->unk_0020 = -1;
    this->unk_0000 = 0.0f;
    this->m_precipTexture[0] = '\0';
    this->unk_0008 = 0.0f;
    this->unk_0010 = 0.0f;
    this->unk_000C = 0.0f;
    this->unk_0014 = 0.0f;
    this->unk_0030 = 1.0f;
    this->unk_002C = 1.0f;
    this->unk_0028 = 1.0f;
}

// VERIFY: 2089fdca manual
// OFFSET: 0x00784A30
bool Weather::GetActiveEffectBounds(CAaBox* bounds) {
    if (this->m_rain) {
        *bounds = this->m_rain->m_bbox;
        return true;
    }

    if (this->m_snow) {
        *bounds = this->m_snow->m_bbox;
        return true;
    }

    if (this->m_sand) {
        *bounds = this->m_sand->GetBoundingBox();
        return true;
    }

    return false;
}

// OFFSET: 0x0078CA50
void Weather::Render() {
    if (!this->unk_0194) {
        return;
    }

    if (this->m_rain) {
        this->m_rain->Render();
    }

    if (this->m_snow) {
        this->m_snow->Render();
    }

    if (this->m_sand) {
        this->m_sand->Render();
    }
}

// VERIFY: 496365da manual
// OFFSET: 0x0078D170
void Weather::Update() {
    this->UpdatePosDeltas();
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (activePlayer) {
        CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            C3Vector delta(this->unk_0164, this->unk_0168, 0.0f);

            if (sqrtf(delta.y * delta.y + delta.x * delta.x) < 1.0f || (player->m_unit->m_unitFieldFlags & 0x100000)) {
                this->unk_018C = player->GetFacing();
            } else {
                delta.Normalize();
                float angle = atan2f(delta.y, delta.x);
                this->unk_018C = angle;

                if (angle < 0.0f) {
                    this->unk_018C = angle + 6.2831855f;
                }
            }

            this->unk_0188 = (player->m_unit->m_unitFieldFlags >> 20) & 1;
            C3Vector position = player->GetPosition();
            this->unk_017C = position.x;
            this->unk_0180 = position.y;
            this->unk_0184 = position.z;

            Weather* weather = World::s_weather;

            if (weather->unk_0170 * weather->unk_0170 + weather->unk_0174 * weather->unk_0174 + weather->unk_0178 * weather->unk_0178 <= 2.0f) {
                this->unk_0190 = player->m_movement->m_currentSpeed;
            } else {
                this->unk_0190 = sqrtf(this->unk_0170 * this->unk_0170 + this->unk_0174 * this->unk_0174);
            }
        }
    }

    bool stormUpdated = false;

    if (this->unk_0020 != -1) {
        int32_t active = 0;

        if (this->m_rain) {
            active = this->m_rain->CountActivePackets();
        }

        if (this->m_snow) {
            active += this->m_snow->CountActivePackets();
        }

        if (this->m_sand) {
            active += this->m_sand->CountActivePackets();
        }

        if (active > 0 && this->unk_0025) {
            this->SetStormIntensity(this->unk_0004, this->unk_0000 < 0.25f ? this->unk_0000 : 0.25f);
            stormUpdated = true;

            if (this->m_snow) {
                this->m_snow->m_fadeOut = 1;
                this->m_snow->m_mists.unk_003C = 1;
            }

            if (this->m_rain) {
                this->m_rain->m_fadeOut = 1;
                this->m_rain->m_mists.unk_003C = 1;
            }

            if (this->m_sand) {
                this->m_sand->m_fadeOut = 1;
                this->m_sand->m_mists.unk_003C = 1;
            }
        } else {
            this->Clear();

            switch (this->unk_0020) {
                case 1: {
                    void* mem = SMemAlloc(sizeof(Rain), __FILE__, __LINE__, 0);
                    this->m_rain = mem ? new (mem) Rain(130.0f, 75.0f) : nullptr;
                    break;
                }

                case 2: {
                    void* mem = SMemAlloc(sizeof(Snow), __FILE__, __LINE__, 0);
                    this->m_snow = mem ? new (mem) Snow(90.0f, 60.0f) : nullptr;
                    break;
                }

                case 3: {
                    void* mem = SMemAlloc(sizeof(Sand), __FILE__, __LINE__, 0);
                    this->m_sand = mem ? new (mem) Sand(40.0f, 25.0f) : nullptr;
                    break;
                }
            }

            this->unk_0018 = OsGetAsyncTimeMs();
            this->unk_0004 = this->unk_0008;
            this->unk_0020 = -1;
        }
    }

    if (this->m_rain || this->m_snow || this->m_sand) {
        this->m_grid->UpdateOriginCell(reinterpret_cast<float*>(&CWorldScene::s_activeWorldView));
    }

    bool changed = fabsf(this->unk_0008 - this->unk_0000) >= 0.00000023841858f || fabsf(this->unk_0014 - this->unk_000C) >= 0.00000023841858f || fabsf(this->unk_0028 - this->unk_0030) >= 0.00000023841858f;

    if ((changed && !stormUpdated) || this->m_densityChanged) {
        this->SetStormIntensity(this->unk_0004, this->unk_0000);
    }

    this->unk_0194 = 1;
}

// VERIFY: f7fa9fb7 manual
// OFFSET: 0x00783B60
uint32_t Weather::GetType() const {
    if (this->m_rain) {
        return 1;
    }

    if (this->m_snow) {
        return 2;
    }

    if (this->m_sand) {
        return 3;
    }

    return 0;
}

// VERIFY: fe5a540b manual
// OFFSET: 0x007846A0
void Weather::SetType(uint32_t type, float intensity, const WeatherRec* rec, bool immediate, float transitionTime) {
    if (s_weatherTypeForced || type > 3) {
        return;
    }

    this->unk_0025 = immediate;
    bool unchanged = true;

    if (this->GetType() != type) {
        this->m_precipTexture[0] = '\0';
        unchanged = false;
    }

    bool copied = false;

    if (rec) {
        this->m_color.Set(1.0f, rec->m_effectColor[0], rec->m_effectColor[1], rec->m_effectColor[2]);

        if (SStrLen(rec->m_effectTexture) > 0 && SStrCmpI(this->m_precipTexture, rec->m_effectTexture, STORM_MAX_STR)) {
            unchanged = false;
            SStrCopy(this->m_precipTexture, rec->m_effectTexture, STORM_MAX_STR);
            copied = true;
        }
    }

    if (SStrLen(this->m_precipTexture) <= 0 && !copied) {
        this->FillInDefaultPrecipTexturePath(type);
        unchanged = false;
    }

    if (this->GetType() != type || !unchanged) {
        this->unk_0020 = type;
    }

    float density = intensity < 0.0f ? 0.0f : (intensity >= 1.0f ? 1.0f : intensity);
    this->unk_0004 = this->unk_0000;
    this->unk_0000 = density;

    float time;

    if (this->GetType() == 0) {
        this->unk_0030 = transitionTime;
        time = transitionTime;
    } else if (type == 0) {
        time = this->unk_0030;
    } else {
        time = transitionTime;
    }

    this->unk_002C = this->unk_0030;
    this->unk_0030 = time;

    if (!immediate) {
        this->m_densityChanged = 1;
        this->unk_0004 = density;
        this->unk_0008 = density;
        this->unk_002C = time;
        this->unk_0028 = time;
    }

    this->unk_000C = density < 0.0f ? 0.0f : (density >= 0.25f ? 0.25f : density);
    this->unk_0010 = this->unk_0004 < 0.0f ? 0.0f : (this->unk_0004 >= 0.25f ? 0.25f : this->unk_0004);

    uint32_t now = OsGetAsyncTimeMs();
    this->unk_0018 = now;
    this->unk_001C = now;
}

// VERIFY: 383f409b manual
// OFFSET: 0x0078C500
void Weather::UpdatePosDeltas() {
    uint32_t total = 0;

    for (PosDelta* delta = this->unk_014C.Head(); delta;) {
        total += delta->m_time;
        PosDelta* next = this->unk_014C.Next(delta);

        if (total >= 150) {
            this->unk_014C.DeleteNode(this->unk_014C.Head());
        }

        delta = next;
    }

    total = 0;

    for (PosDelta* delta = this->unk_0158.Head(); delta;) {
        total += delta->m_time;
        PosDelta* next = this->unk_0158.Next(delta);

        if (total >= 150) {
            this->unk_0158.DeleteNode(this->unk_0158.Head());
        }

        delta = next;
    }

    C3Vector playerPos;
    C3Vector playerVelocity;
    C3Vector transportPos;
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (activePlayer) {
        CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            playerPos = player->GetPosition();
            playerVelocity = player->m_movement->m_moveDir * player->m_movement->m_currentSpeed;

            if (player->GetTransportGUID()) {
                CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ player->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);

                if (transport) {
                    transportPos = transport->GetPosition();
                }
            }
        }
    }

    uint32_t now = OsGetAsyncTimeMs();
    static C3Vector s_lastTransportPos = transportPos;
    static uint32_t s_lastTime = now;

    if (now != s_lastTime) {
        C3Vector delta = transportPos - s_lastTransportPos;
        PosDelta* node = this->unk_0158.NewNode(2, 0, 0);
        node->m_delta = delta;
        node->m_time = now - s_lastTime;
    }

    s_lastTransportPos = transportPos;

    if (now - s_lastTime) {
        C3Vector delta = playerPos - C3Vector(this->unk_017C, this->unk_0180, this->unk_0184);
        PosDelta* node = this->unk_014C.NewNode(2, 0, 0);
        node->m_delta = delta;
        node->m_time = now - s_lastTime;
    }

    s_lastTime = now;

    C3Vector playerSum;
    total = 0;

    for (PosDelta* delta = this->unk_014C.Head(); delta; delta = this->unk_014C.Next(delta)) {
        total += delta->m_time;
        playerSum.x += delta->m_delta.x;
        playerSum.y += delta->m_delta.y;
        playerSum.z += delta->m_delta.z;
    }

    float playerScale = 1.0f / (static_cast<float>(total + 1) * 0.001f);
    this->unk_0164 = playerSum.x * playerScale;
    this->unk_0168 = playerSum.y * playerScale;
    this->unk_016C = playerSum.z * playerScale;

    C3Vector transportSum;
    total = 0;

    for (PosDelta* delta = this->unk_0158.Head(); delta; delta = this->unk_0158.Next(delta)) {
        total += delta->m_time;
        transportSum.x += delta->m_delta.x;
        transportSum.y += delta->m_delta.y;
        transportSum.z += delta->m_delta.z;
    }

    float transportScale = 1.0f / (static_cast<float>(total + 1) * 0.001f);
    this->unk_0170 = transportSum.x * transportScale;
    this->unk_0174 = transportSum.y * transportScale;
    this->unk_0178 = transportSum.z * transportScale;

    if (fabs(playerVelocity.x) < 2.3841858e-07f && fabs(playerVelocity.y) < 2.3841858e-07f && fabs(playerVelocity.z) < 2.3841858e-07f && this->unk_0170 * this->unk_0170 + this->unk_0174 * this->unk_0174 + this->unk_0178 * this->unk_0178 <= 2.0f) {
        this->unk_0164 = 0.0f;
        this->unk_0168 = 0.0f;
        this->unk_016C = 0.0f;
    }
}
