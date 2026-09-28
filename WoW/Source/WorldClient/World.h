#ifndef WOW_SOURCE_WORLDCLIENT_WORLD_H
#define WOW_SOURCE_WORLDCLIENT_WORLD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CBoundingBox.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "_Triage/CWFrustum.h"

class CAaBox;
class C44Matrix;
class CiRect;
class GxuScene;

namespace Liquid {
    class CRenderer;
}
class CImVector;
class CM2Scene;
class CMapEntity;
class CTexture;
class Particulate;
class Weather;

class CMapBaseObj;
class CMapChunk;
class CMapDoodadDef;
class CMapDoodadDefMapChunkLink;

class CMapDoodadDefMapChunkLink {
    public:
    /* 0000 */ uint32_t m_objectIndex;
    /* 0004 */ CMapDoodadDef* m_owner;
    /* 0008 */ CMapBaseObj* m_ref;
    /* 000C */ TSLink<CMapDoodadDefMapChunkLink> m_refLink;
    /* 0014 */ TSLink<CMapDoodadDefMapChunkLink> m_ownerLink;
};

class CSortEntry {
    public:
    /* 0000 */ TSExplicitList<CMapChunk> m_mapChunkList;
    /* 000C */ TSList<void> m_exteriorGroupList;
    /* 0018 */ TSList<void> m_entityList;
    /* 0024 */ TSList<void> m_doodadDefList;
    /* 0030 */ TSList<void> m_liquidList;
    /* 003C */ TSList<void> m_occluderList;
    /* 0048 */ TSList<void> m_unkList7;
    /* 0054 */ TSList<void> m_holedChunkList;
    /* 0060 */ TSList<void> m_unkList9;
};

struct OcclusionVolumeData {
    /* 0000 */ C3Vector m_min;
    /* 000C */ C3Vector m_max;
    /* 0018 */ C3Vector* m_positions;
    /* 001C */ uint32_t m_positionCount;
};

struct OcclusionVolume {
    /* 0000 */ int32_t m_mapID;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ OcclusionVolumeData m_data;
};

struct SPortalExt {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ CRect m_rect;
    /* 0014 */ uint32_t m_stamp;
    /* 0018 */ uint32_t m_pad;
};

struct WorldStatus {
    /* 0000 */ uint32_t m_prepareAll;
    /* 0004 */ uint32_t m_chunksChanged;
};

struct SWModelFadeout {
    /* 0000 */ CMapEntity* m_entity;
    /* 0004 */ uint8_t unk_0004[84];
    /* 0058 */ TSLink<SWModelFadeout> m_link;
};

extern TSExplicitList<SWModelFadeout, offsetof(SWModelFadeout, m_link)> s_fadeoutList;
extern TSExplicitList<SWModelFadeout, offsetof(SWModelFadeout, m_link)> s_fadeoutDataPool;

class CWorldScene {
    public:
    static Liquid::CRenderer* s_unk_CD8610;
    static TSExplicitList<CWFrustum, offsetof(CWFrustum, m_link)> frustumFreeList;
    static GxuScene s_unk_B2EB68;
    static CTexture* s_defaultBlendTexture;
    static CTexture* s_defaultTexture;
    static float s_unk_CD877C;
    static CMapEntity* camTargetEntity;
    static float s_fadeDist[5];
    static float s_fadeDistDefault[5];
    static float s_fadeDistMin[5];
    static float s_fadeDistMinSqr[5];
    static float s_fadeRangeDefault[5];
    static float s_fadeSizeDefault[5];
    static int32_t s_unk_CD87B0;

    static void ClearObjectFadeoutList();
    static void Destroy();
    static void Initialize();
    static void ScaleFadeDist(float scale);

    /* 0000 */ uint8_t unk_0000[16];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[24];
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[28];
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[32];
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[4];
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint8_t unk_0080[8];
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint8_t unk_008C[36];
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint8_t unk_00B4[64];
    /* 00F4 */ uint32_t unk_00F4;
    /* 00F8 */ uint8_t unk_00F8[36];
    /* 011C */ uint32_t unk_011C;
    /* 0120 */ uint8_t unk_0120[16];
    /* 0130 */ uint32_t unk_0130;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ uint8_t unk_0138[52];
    /* 016C */ uint32_t unk_016C;
    /* 0170 */ uint8_t unk_0170[84];
    /* 01C4 */ uint32_t unk_01C4;
    /* 01C8 */ uint8_t unk_01C8[28];
    /* 01E4 */ uint32_t unk_01E4;
};

namespace World {
extern CM2Scene* s_m2Scene;
extern Weather* s_weather;
extern float groundEffectDistValueSqr;
extern C4Vector texVect[8];
extern int32_t lodMax;
extern int32_t lodMin;
extern C44Matrix idMat;
extern float s_farClip;
extern float s_horizonFarClipScale;
extern float s_horizonNearClipScale;
extern int32_t s_detailDoodadDensity;
extern float s_nearClip;
extern float s_prevFarClip;
extern int32_t s_texCacheDirty;
extern uint32_t s_texCacheOverride;
extern int32_t s_waterLOD;

int32_t FrustumCull(const CAaBox& box);
int32_t ProjectPositionCallback(const C3Vector& position, float& height, void* param);
void ProjectTextureCallback(const CAaBox& box, const CImVector& color, int32_t index, void* param, int32_t flags);
bool MapIsDungeon();
void ObjectDestroy(CMapEntity* entity);
void SetDetailDoodadDensity(int32_t density);
void SetDetailDoodadDist(float dist);
void SetFarClip(float farClip);
void SetHorizonFarClipScale(float scale);
void SetHorizonNearClipScale(float scale);
void SetNearClip(float nearClip);
void SetTexLodBias(float bias);
float ValidateFarClip(float farClip, int32_t mapID);
}

class CWorld {
    public:
    static int32_t bShowSimpleDoodads;
    static int32_t bLoadSimpleDoodads;
    static Particulate* particulate;
    static int32_t shadowMipLevel;
    static int32_t unk_CD7658;
    static uint32_t maxLights;
    static int32_t terrainAlphaBitDepth;
    static int32_t enableLayerCnt;
    static float farFog;
    static int32_t detailDoodadAlphaRef;
    static int32_t detailDoodadTest;
    static WorldStatus status;
    static int32_t chunkCnt;
    static int32_t frameCnt;
    static int32_t s_elapsedTicks;
    static float s_elapsedTimeSec;
    static uint32_t enables;
    static uint32_t enables2;
    static uint32_t m_M2PassMask;
    static CAaBox s_groupAreaOfInterest;
    static CAaBox s_objectAreaOfInterest;
    static CiRect chunkRect;
    static CiRect gbChunkRect;
    static CiRect gbChunkRectOld;
    static uint32_t shadowColor;
    static float detailDoodadDist;
    static CTexture* shadowModTexture;

    static int32_t ConsoleCommand_CharacterAmbient(const char* command, const char* arguments);
    static int32_t ConsoleCommand_DetailDoodadAlpha(const char* command, const char* arguments);
    static int32_t ConsoleCommand_MaxLOD(const char* command, const char* arguments);
    static int32_t ConsoleCommand_SetShadow(const char* command, const char* arguments);
    static int32_t ConsoleCommand_ShowCull(const char* command, const char* arguments);
    static int32_t ConsoleCommand_ShowDetailDoodads(const char* command, const char* arguments);
    static int32_t ConsoleCommand_ShowLowDetail(const char* command, const char* arguments);
    static int32_t ConsoleCommand_ShowShadow(const char* command, const char* arguments);
    static int32_t ConsoleCommand_ShowSimpleDoodads(const char* command, const char* arguments);
    static int32_t ConsoleCommand_WaterParticulates(const char* command, const char* arguments);
    static int32_t ConsoleCommand_WaterRipples(const char* command, const char* arguments);
    static void Destroy();
    static void Initialize();
};

#endif
