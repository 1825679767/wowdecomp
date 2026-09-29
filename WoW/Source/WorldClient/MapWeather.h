#ifndef WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H
#define WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/Rain.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CVar;
class WeatherRec;

class CGxShader;
class CTexture;
struct SnowFlakeParticleBuffer;

class Snow {
    public:
    /* 0000 */ int32_t m_useWeatherShaders;
    /* 0004 */ CAaBox m_bbox;
    /* 001C */ Mists m_mists;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ void* unk_0074;
    /* 0078 */ void* unk_0078;
    /* 007C */ int32_t m_activePacketsCount;
    /* 0080 */ int32_t m_freePacketsCount;
    /* 0084 */ int32_t unk_0084;
    /* 0088 */ void* unk_0088;
    /* 008C */ void* unk_008C;
    /* 0090 */ Packet<SnowFlakeParticleBuffer>* m_snowFlakePacketPtr;
    /* 0094 */ float unk_0094;
    /* 0098 */ CTexture* m_snowFlakeTex;
    /* 009C */ CGxShader* m_shaderPtr;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ int32_t unk_00A4;

    ~Snow();
};

struct SnowFlakeParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_velocity;
    /* 0018 */ float m_lifetime;
    /* 001C */ float m_size;
};

struct SnowFlakeParticleBuffer {
    /* 0000 */ uint32_t m_particleCount;
    /* 0004 */ SnowFlakeParticle m_particles [6144];
};

struct WeatherGridCell {
    /* 0000 */ float m_heights[1024];
    /* 1000 */ int32_t unk_1000;
    /* 1004 */ int32_t unk_1004;

    WeatherGridCell();
};

class WeatherGridBase {
    public:
    virtual void* unk_0000(void* a2) = 0;
    virtual int32_t UpdateOriginCell(float* position) = 0;
    virtual bool ComputeCellBounds(float* a2, float* a3, float* a4) = 0;
    virtual void GetCellAtPosition(float* position) = 0;
    virtual void Unk10(float* a1, float* a2) = 0;
    virtual ~WeatherGridBase();
};

class WeatherGrid : public WeatherGridBase {
    public:
    /* 0004 */ float unk_0004;
    /* 0008 */ float unk_0008;
    /* 000C */ float unk_000C;
    /* 0010 */ int32_t unk_0010[6];
    /* 0028 */ WeatherGridCell* m_cells[5][5];

    virtual void* unk_0000(void* a2);
    virtual int32_t UpdateOriginCell(float* position);
    virtual bool ComputeCellBounds(float* a2, float* a3, float* a4);
    virtual void GetCellAtPosition(float* position);
    virtual void Unk10(float* a1, float* a2);
    virtual ~WeatherGrid();

    WeatherGrid();
};

struct PosDelta : public TSLinkedNode<PosDelta> {
};

class Sand;

class Weather {
    public:
    static int32_t TimeToFixed(float seconds);

    /* 0000 */ float unk_0000;
    /* 0004 */ float unk_0004;
    /* 0008 */ float unk_0008;
    /* 000C */ float unk_000C;
    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ uint8_t m_densityChanged;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026[2];
    /* 0028 */ float unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ float unk_0030;
    /* 0034 */ uint8_t unk_0034[4];
    /* 0038 */ char m_precipTexture[260];
    /* 013C */ Rain* m_rain;
    /* 0140 */ Snow* m_snow;
    /* 0144 */ Sand* m_sand;
    /* 0148 */ WeatherGrid* m_grid;
    /* 014C */ TSList<PosDelta, TSGetLink<PosDelta> > unk_014C;
    /* 0158 */ TSList<PosDelta, TSGetLink<PosDelta> > unk_0158;
    /* 0164 */ float unk_0164;
    /* 0168 */ float unk_0168;
    /* 016C */ float unk_016C;
    /* 0170 */ float unk_0170;
    /* 0174 */ float unk_0174;
    /* 0178 */ float unk_0178;
    /* 017C */ float unk_017C;
    /* 0180 */ float unk_0180;
    /* 0184 */ float unk_0184;
    /* 0188 */ uint8_t unk_0188;
    /* 0189 */ uint8_t unk_0189[3];
    /* 018C */ float unk_018C;
    /* 0190 */ float unk_0190;
    /* 0194 */ uint8_t unk_0194;
    /* 0195 */ uint8_t unk_0195[3];

    Weather();
    bool GetActiveEffectBounds(CAaBox* bounds);
    void Update();
    ~Weather();
    void Clear();
    void Reset();
    void SetType(uint32_t type, float intensity, const WeatherRec* rec, bool immediate, float transitionTime);
};

extern float s_weatherDensity;
extern uint8_t s_weatherTypeForced;
extern CVar* s_useShaders;

bool CVarHandler_weatherDensity(CVar* var, const char* oldValue, const char* value, void* arg);

class MapWeather {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026[2];
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint8_t unk_0034[172];
    /* 00E0 */ uint8_t unk_00E0;
    /* 00E1 */ uint8_t unk_00E1[91];
    /* 013C */ uint32_t unk_013C;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint32_t unk_0144;
    /* 0148 */ uint32_t unk_0148;
    /* 014C */ uint8_t unk_014C[24];
    /* 0164 */ uint32_t unk_0164;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint8_t unk_016C[4];
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint8_t unk_0178[4];
    /* 017C */ uint32_t unk_017C;
    /* 0180 */ uint32_t unk_0180;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ uint8_t unk_0188;
    /* 0189 */ uint8_t unk_0189[3];
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint32_t unk_0190;
    /* 0194 */ uint8_t unk_0194;
    /* 0195 */ uint8_t unk_0195[3];
};

#endif
