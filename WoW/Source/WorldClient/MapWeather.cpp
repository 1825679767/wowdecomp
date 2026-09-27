#include "MapWeather.h"
#include <cfloat>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"

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

// VERIFY: da99d6fa manual
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
    this->unk_0034[0] = 0xFF;
    this->unk_0034[1] = 0xFF;
    this->unk_0034[2] = 0xFF;
    this->unk_0034[3] = 0xFF;
    this->unk_013C = 0;
    this->unk_0140 = 0;
    this->unk_0144 = 0;
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
