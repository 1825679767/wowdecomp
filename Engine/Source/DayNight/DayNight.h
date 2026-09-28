#ifndef ENGINE_SOURCE_DAYNIGHT_DAYNIGHT_H
#define ENGINE_SOURCE_DAYNIGHT_DAYNIGHT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"

class LightFloatBandRec;
class LightParamsRec;

class C3Vector;
class CImVector;
class CM2Model;
class CM2Scene;
class CTexture;
namespace DayNight { struct DNPlanet; }
class LightRec;

struct AreaLightQueueEntry {
    /* 0000 */ float m_key;
    /* 0004 */ LightRec** m_light;
};

struct DNKeyframe {
    /* 0000 */ float m_time;
    /* 0004 */ float m_value;
};

class DNLightBands {
    public:
    /* 0000 */ CImVector m_ambient;
    /* 0004 */ CImVector m_diffuse;
    /* 0008 */ CImVector m_shadowOpacity;
    /* 000C */ CImVector m_sky0;
    /* 0010 */ CImVector m_sky1;
    /* 0014 */ CImVector m_sky2;
    /* 0018 */ CImVector m_sky3;
    /* 001C */ CImVector m_sky4;
    /* 0020 */ CImVector m_skyFog;
    /* 0024 */ CImVector m_sunColor;
    /* 0028 */ CImVector m_cloudColor0;
    /* 002C */ CImVector m_cloudColor1;
    /* 0030 */ CImVector m_cloudColor2;
    /* 0034 */ CImVector m_band13;
    /* 0038 */ CImVector m_band14;
    /* 003C */ CImVector m_band15;
    /* 0040 */ CImVector m_band16;
    /* 0044 */ CImVector m_band17;
    /* 0048 */ float m_fogEnd;
    /* 004C */ float m_fogStartMul;
    /* 0050 */ float m_fogRate;
    /* 0054 */ float m_highlightSky;
    /* 0058 */ float m_glow;
    /* 005C */ float m_floatBand2;
    /* 0060 */ float m_cloudDensity;
    /* 0064 */ float m_floatBand4;
    /* 0068 */ float m_floatBand5;
    /* 006C */ float m_waterShallowAlpha;
    /* 0070 */ float m_waterDeepAlpha;
    /* 0074 */ float m_oceanShallowAlpha;
    /* 0078 */ float m_oceanDeepAlpha;
    /* 007C */ uint32_t m_skyboxId0;
    /* 0080 */ float m_skyboxWeight0;
    /* 0084 */ uint32_t m_skyboxId1;
    /* 0088 */ float m_skyboxWeight1;
    /* 008C */ uint32_t m_skyboxId2;
    /* 0090 */ float m_skyboxWeight2;
    /* 0094 */ uint32_t m_cloudTypeId;
    /* 0098 */ float m_cloudWeight;

    DNLightBands();
    void Copy(const DNLightBands* src);
};

struct DNOverrideSky {
    /* 0000 */ uint32_t m_hashval;
    /* 0004 */ uint8_t m_hashInternals [16];
    /* 0014 */ char* m_name;
    /* 0018 */ CM2Model* m_model;
    /* 001C */ uint32_t m_animLengthMs;
    /* 0020 */ uint32_t m_lastTimeOfDay;
    /* 0024 */ uint32_t m_flags;
};

namespace DayNight {
struct DNClouds {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t m_density;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ TSFixedArray<C3Vector> unk_0054;
    /* 0060 */ TSFixedArray<uint8_t> unk_0060;
    /* 006C */ TSFixedArray<uint8_t> unk_006C;
    /* 0078 */ TSFixedArray<uint8_t> unk_0078;
    /* 0084 */ uint16_t unk_0084;
    /* 0086 */ uint16_t unk_0086;
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
};
}

namespace DayNight {
struct DNGlare {
    /* 0000 */ void* m_vtbl;
    /* 0004 */ uint32_t m_occlusionQuery;
    /* 0008 */ uint32_t m_enable;
    /* 000C */ C3Vector m_position;
    /* 0018 */ CImVector m_color;
    /* 001C */ CTexture* m_texture;
    /* 0020 */ float m_baseScale;
    /* 0024 */ float m_scale;
    /* 0028 */ float m_fadeInRate;
    /* 002C */ float m_fadeOutRate;
    /* 0030 */ float m_fade;
    /* 0034 */ float m_fadeTarget;
    /* 0038 */ uint32_t m_skipOcclusionOnce;
    /* 003C */ uint8_t unk_003C [48];
    /* 006C */ uint32_t m_occlusionPixels;
    /* 0070 */ DNKeyframe m_alphaCurve [4];
    /* 0090 */ float m_scaleMin;
    /* 0094 */ float m_scaleMax;
    /* 0098 */ float m_minDot;
    /* 009C */ float m_alphaMin;
    /* 00A0 */ float m_alphaMax;
    /* 00A4 */ float m_screenGlow;
    /* 00A8 */ DayNight::DNPlanet* m_planet;
};
}

namespace DayNight {
struct DNPlanet {
    /* 0000 */ C3Vector m_position;
    /* 000C */ CImVector m_color;
    /* 0010 */ CTexture* m_texture;
    /* 0014 */ float m_scale;
    /* 0018 */ float m_baseScale;
    /* 001C */ float m_period;

    void Initialize(const char* fileName);
    void Render();
};
}

namespace DayNight {
struct DNPlanets {
    /* 0000 */ DayNight::DNPlanet m_sun;
    /* 0020 */ DayNight::DNPlanet m_moon;
    /* 0040 */ DayNight::DNPlanet m_moon2;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
};
}

namespace DayNight {
struct DNSky {
    /* 0000 */ TSFixedArray<uint8_t> m_geoVerts;
    /* 000C */ TSFixedArray<CImVector> m_clrVerts;
    /* 0018 */ TSFixedArray<uint16_t> m_indices;
    /* 0024 */ uint32_t m_sphThetaTess;
    /* 0028 */ uint16_t m_nVerts;
    /* 002A */ uint16_t m_nIndices;
    /* 002C */ float m_sphRadius;

    void Render();
    void GenSphere(float sphRadius);
    void SetColors();
};
}

namespace DayNight {
struct DNStars {
    /* 0000 */ CM2Scene* m_scene;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ CImVector m_color;
    /* 000C */ C3Vector m_position;
    /* 0018 */ uint32_t m_lastTimeMs;
};
}

namespace DayNight {
    float InterpFloatBand(const LightFloatBandRec* rec, int32_t now, int32_t bandIndex);
    float CalcIndividualColorFromDBRec(int32_t bandTime, const LightParamsRec* params, int32_t bandIndex);
    void LoadLightingBands(int32_t bandTime, DNLightBands* bands, LightParamsRec* params);
    float CalcFogRate(float fogStart, float fogEnd);
    void CalcColors(int32_t bandTime, DNLightBands* bands, LightParamsRec* params);
    void BlendColors(DNLightBands* dst, const DNLightBands* src, float t);
    LightParamsRec* GetLightParams(const LightRec* light, int32_t slot);
    void SetLightColors();
    DNOverrideSky* GetOverrideSky(const char* path, uint32_t flags);
    void LoadMap(int32_t zoneID);
    void LightLoad(TSGrowableArray<LightRec*>* lightArray, uint32_t a2);
    void ColorLerpBytes(CImVector* out, const CImVector* a, const CImVector* b, float t);
    void BlendOutputs(DNLightBands* dst, const DNLightBands* src, float t);
    void BlendAreaLightColors(DNLightBands* dst, const LightRec* light, int32_t clearSlot, int32_t stormSlot);
    void BlendAreaLight(DNLightBands* dst, float maxFade, const LightRec* light, int32_t clearSlot, int32_t stormSlot);
    void BlendZoneLight(DNLightBands* dst, float maxFade, const LightRec* light, float distPct, int32_t clearSlot, int32_t stormSlot);
    void DoAreaLights(DNLightBands* light, int32_t underwater);
    void UpdateFog();
    void SetDirection();
    void UpdateLighting();
    void Update(int32_t reset, const C3Vector* cameraPos);
    void SetPlanets();
    CImVector DarkenColor(CImVector colour, float scale);
}

struct SMOFog {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ float m_smallerRadius;
    /* 0014 */ float m_largerRadius;
    /* 0018 */ float m_fogEnd;
    /* 001C */ float m_fogStartScalar;
    /* 0020 */ CImVector m_fogColor;
    /* 0024 */ float m_uwFogEnd;
    /* 0028 */ float m_uwFogStartScalar;
    /* 002C */ CImVector m_uwFogColor;
};

namespace DayNight {

    class LightQE {
        public:
        float m_key;        // +0x00
        LightRec** m_light; // +0x04

        static int32_t s_localSort;

        static bool HasHigherPriority(const LightQE& a, const LightQE& b);
    };

}

namespace DayNight {

    class LightQueue {
        public:
        void* m_vtable;
        void* m_alloc;
        uint32_t m_allocBytes;
        LightQE* m_data;
        uint32_t m_dataBytes;
        uint32_t m_chunk;
        uint32_t m_capacity;
        uint32_t m_count;

        LightQueue(uint32_t initial, uint32_t chunk);
        ~LightQueue();

        bool Resize(uint32_t bytes, bool noClear);
        void Insert(float key, LightRec** light);
        bool Erase(uint32_t index, uint32_t count);
        void Pop(LightQE* out);
    };

}


#endif
