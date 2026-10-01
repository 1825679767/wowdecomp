#ifndef WOW_SOURCE_UI_MINIMAPFRAME_H
#define WOW_SOURCE_UI_MINIMAPFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class CRenderBatch;
class CScriptRegion;
class CSimpleTexture;
class CStatus;
class XMLNode;
struct lua_State;

class AreaPOIRec;

struct MINIMAPMD5NAME : public TSHashObject<MINIMAPMD5NAME, HASHKEY_STRI> {
    /* 0018 */ char m_md5Name[40];
};

struct TrackingTypeInfo {
    /* 0000 */ int32_t m_type;
    /* 0004 */ uint32_t m_npcFlags;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_texture;
    /* 0010 */ uint32_t m_classMask;
};

void DisplayQuestPoint(float x, float y, int32_t a3, const char* name);

class CGMinimapFrame : public CSimpleFrame {
    public:
    static void EnterWorld();
    static CDataAllocator s_minimapHeap;
    static void SetPingPosition(const WOWGUID& guid, const C2Vector& position);
    static void MinimapTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void SetTrackingSpell(int32_t spellId);
    static void LoadTrackingInfo(int32_t a1);
    static int32_t s_metatable;
    static int32_t s_objectType;

    static bool IsTrackingTrivialQuests();
    static const char* s_objectTypeName;
    static CGMinimapFrame* s_minimapFrame;
    static int32_t s_rotateMinimap;
    static float s_facing;
    static int32_t s_scaleChanged;
    static float s_blipSize1;
    static float s_blipHalfSize1;
    static float s_blipSize2;
    static float s_blipHalfSize2;
    static float s_blipSize3;
    static float s_blipHalfSize3;
    static float s_blipSize4;
    static C3Vector s_blipQuads[3][4];
    static const TrackingTypeInfo* s_trackingInfo;
    static const TrackingTypeInfo s_trackingTypes[15];
    static int32_t CountAvailableTrackingTypes();
    static const TrackingTypeInfo* GetTrackingTypeByIndex(int32_t index);
    static int32_t s_trackingSpell;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void InitializeGame();
    static void RegisterScriptMethods(lua_State* L);
    static void RenderCallback(void* param);

    /* 029C */ int32_t m_blipFrameLevel;
    /* 02A0 */ CSimpleTexture* m_playerTexture;
    /* 02A4 */ float m_facing;
    /* 02A8 */ CScriptRegion* m_compassTexture;
    /* 02AC */ uint32_t unk_02AC;

    ~CGMinimapFrame() override;
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void PostLoadXML(XMLNode* node, CStatus* status) override;
    void OnLayerUpdate(float elapsedSec) override;
    int32_t OnLayerTrackUpdate(const CMouseEvent& evt) override;
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    void OnLayerCursorExit(int32_t a2, int32_t a3) override;
    bool UpdateScale(bool force) override;

    CGMinimapFrame(CSimpleFrame* parent);
    void Render();
    void UpdateRotation(float facing);
};

int32_t Script_SetTracking(lua_State* L);
void MinimapRegisterScriptFunctions();

void MinimapInitialize(int32_t mapId);
float MinimapGetViewRadius();
AreaPOIRec* MinimapGetGossipPoi();
void UpdatePointsOfInterest();
void MinimapSetCorpsePOI(float x, float y);
void MinimapSetSpiritHealerPOI(float x, float y);

#endif
