#include "MinimapFrame.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"

// STATIC: 0x00ACEBAC
int32_t CGMinimapFrame::s_scaleChanged = 1;

// STATIC: 0x00ACEBB0
const char* CGMinimapFrame::s_objectTypeName = "Minimap";

// STATIC: 0x00ACEBB4
int32_t CGMinimapFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00BEBA44
float CGMinimapFrame::s_blipSize4;

// STATIC: 0x00BEBA48
float CGMinimapFrame::s_blipHalfSize3;

// STATIC: 0x00BEBA4C
float CGMinimapFrame::s_blipSize3;

// STATIC: 0x00BEBA50
float CGMinimapFrame::s_blipHalfSize2;

// STATIC: 0x00BEBA54
float CGMinimapFrame::s_blipSize2;

// STATIC: 0x00BEBA58
float CGMinimapFrame::s_blipHalfSize1;

// STATIC: 0x00BEBA5C
float CGMinimapFrame::s_blipSize1;

// STATIC: 0x00BEBA60
int32_t CGMinimapFrame::s_objectType;

// STATIC: 0x00BEBA6C
int32_t CGMinimapFrame::s_rotateMinimap;

// STATIC: 0x00BEBA70
float CGMinimapFrame::s_facing;

// STATIC: 0x00BEBA88
CGMinimapFrame* CGMinimapFrame::s_minimapFrame;

// STATIC: 0x00BEBA9C
CDataAllocator CGMinimapFrame::s_minimapHeap(sizeof(CGMinimapFrame), 1);

// STATIC: 0x00BF7FC8
C3Vector CGMinimapFrame::s_blipQuads[3][4];

// STATIC: 0x00BEBA64
const TrackingTypeInfo* CGMinimapFrame::s_trackingInfo;

// STATIC: 0x00BEBA68
int32_t CGMinimapFrame::s_trackingSpell;

// STATIC: 0x00BEBA04
static CTexture* s_vehicleTextures[7];

// STATIC: 0x00BEBA20
static CTexture* s_groupArrowTexture;

// STATIC: 0x00BEBA24
static CTexture* s_staticPOIArrowTexture;

// STATIC: 0x00BEBA28
static CTexture* s_corpseArrowTexture;

// STATIC: 0x00BEBA2C
static CTexture* s_poiArrowTexture;

// STATIC: 0x00BEBA30
static CTexture* s_maskTexture;

// STATIC: 0x00BEBA34
static CTexture* s_classBlipTexture;

// STATIC: 0x00BEBA38
static CTexture* s_blipTexture;

// STATIC: 0x00BEBA3C
static CTexture* s_iconTexture;

// STATIC: 0x00BF6148
static C2Vector s_iconTexCoords[196][4];

// STATIC: 0x00BF79C8
static C2Vector s_classBlipTexCoords[16][4];

// STATIC: 0x00BF7BC8
static C2Vector s_blipTexCoords[32][4];

// STATIC: 0x00BEBA40
static int32_t s_unkBEBA40;

// STATIC: 0x00BF5F1C
static float s_unkBF5F1C;

// STATIC: 0x00BF5F14
static CTexture* s_renderTarget;

// STATIC: 0x00BEB9A8
static uint32_t s_unkBEB9A8[23];

// STATIC: 0x00A11C50
const TrackingTypeInfo CGMinimapFrame::s_trackingTypes[15] = {
    { 1, 0x1000, "MINIMAP_TRACKING_REPAIR", "Repair", 0x0 },
    { 1, 0x200, "MINIMAP_TRACKING_VENDOR_FOOD", "Food", 0x0 },
    { 1, 0x400, "MINIMAP_TRACKING_VENDOR_POISON", "Poisons", 0x10 },
    { 1, 0x100, "MINIMAP_TRACKING_VENDOR_AMMO", "Ammunition", 0x1A },
    { 1, 0x800, "MINIMAP_TRACKING_VENDOR_REAGENT", "Reagents", 0x0 },
    { 1, 0x10000, "MINIMAP_TRACKING_INNKEEPER", "Innkeeper", 0x0 },
    { 1, 0x2000, "MINIMAP_TRACKING_FLIGHTMASTER", "FlightMaster", 0x0 },
    { 1, 0x400000, "MINIMAP_TRACKING_STABLEMASTER", "StableMaster", 0x8 },
    { 1, 0x100000, "MINIMAP_TRACKING_BATTLEMASTER", "BattleMaster", 0x0 },
    { 1, 0x20, "MINIMAP_TRACKING_TRAINER_CLASS", "Class", 0x0 },
    { 1, 0x40, "MINIMAP_TRACKING_TRAINER_PROFESSION", "Profession", 0x0 },
    { 1, 0x200000, "MINIMAP_TRACKING_AUCTIONEER", "Auctioneer", 0x0 },
    { 1, 0x20000, "MINIMAP_TRACKING_BANKER", "Banker", 0x0 },
    { 2, 0x13, "MINIMAP_TRACKING_MAILBOX", "Mailbox", 0x0 },
    { 3, 0x0, "MINIMAP_TRACKING_TRIVIAL_QUESTS", "TrivialQuests", 0x0 },
};

// STATIC: 0x00A41E04
static const int32_t s_outdoorZoomTiles[6] = { 14, 12, 10, 8, 6, 4 };

// STATIC: 0x00A41E1C
static const float s_indoorZoomRadius[6] = { 150.0f, 120.0f, 90.0f, 60.0f, 40.0f, 25.0f };

// STATIC: 0x00AF4E48
static int32_t s_mapId = -1;

// STATIC: 0x00AF4E4C
static int32_t s_outdoorZoom = 3;

// STATIC: 0x00AF4E50
static int32_t s_indoorZoom = 3;

// STATIC: 0x00AF4E58
static const char* s_minimapTexturePath = "Textures\\Minimap";

// STATIC: 0x00D39210
static float s_closestPOIFacing[3];

// STATIC: 0x00D3921C
static int32_t s_closestPOIs[3];

// STATIC: 0x00D39228
static int32_t s_poisChanged;

// STATIC: 0x00D39230
static char s_dynamicPOINames[4][64];

// STATIC: 0x00D39434
static uint8_t s_isIndoors;

// STATIC: 0x00D39444
static uint32_t s_numPOIs;

// STATIC: 0x00D39448
static uint32_t s_numClosestPOIs;

// STATIC: 0x00D3944C
static int32_t s_closestPOIsChanged;

// STATIC: 0x00D39454
static C3Vector s_playerPosition;

// STATIC: 0x00D39490
static AreaPOIRec s_dynamicPOIs[4];

// STATIC: 0x00D395F0
static TSFixedArray<AreaPOIRec*> s_pois;

// STATIC: 0x00D395FC
static TSFixedArray<int32_t> s_poiVisible;

// STATIC: 0x00D39608
static TSFixedArray<int32_t> s_poiOnEdge;

// STATIC: 0x00D39614
static TSGrowableArray<AreaPOIRec*> s_visiblePOIs;

// STATIC: 0x00D39624
static TSHashTable<MINIMAPMD5NAME, HASHKEY_STRI> s_md5Names;

// INLINED
void CGMinimapFrame::operator delete(void* ptr) {
    CGMinimapFrame::s_minimapHeap.PutData(ptr, nullptr, 0);
}

// INLINED
int32_t CGMinimapFrame::GetObjectType() {
    if (!CGMinimapFrame::s_objectType) {
        CGMinimapFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGMinimapFrame::s_objectType;
}

// VERIFY: 185b6554 auto
// OFFSET: 0x00514230
CSimpleFrame* CGMinimapFrame::Create(CSimpleFrame* parent) {
    void* data = CGMinimapFrame::s_minimapHeap.GetData(0, typeid(CGMinimapFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGMinimapFrame(parent) : nullptr;
}

// VERIFY: 9c7a5302 auto
// OFFSET: 0x0057BC90
bool CGMinimapFrame::UpdateScale(bool force) {
    if (!CSimpleFrame::UpdateScale(force)) {
        return false;
    }

    CGMinimapFrame::s_scaleChanged = 1;
    return true;
}

// VERIFY: e963994e auto
// OFFSET: 0x0057BCC0
void CGMinimapFrame::UpdateRotation(float facing) {
    this->m_facing = facing;
    CGMinimapFrame::s_facing = facing;

    if (this->m_playerTexture) {
        if (g_rotateMinimapCVar->m_intValue) {
            facing = 0.0f;
        }

        this->m_playerTexture->SetRotation(facing, 0.5f, 0.5f);
    }
}

// VERIFY: 2d4505b3 auto
// OFFSET: 0x0057BE80
const char* CGMinimapFrame::GetObjectTypeName() {
    return CGMinimapFrame::s_objectTypeName;
}

// VERIFY: 54d9ac95 auto
// OFFSET: 0x0057BE90
int32_t CGMinimapFrame::GetScriptMetaTable() {
    return CGMinimapFrame::s_metatable;
}

// VERIFY: b0c85a44 auto
// OFFSET: 0x0057BEA0
void CGMinimapFrame::PostLoadXML(XMLNode* node, CStatus* status) {
    CSimpleFrame::PostLoadXML(node, status);

    const char* playerTexture = node->GetAttributeByName("minimapPlayerTexture");

    if (!playerTexture) {
        playerTexture = "Interface\\Minimap\\MinimapArrow.tga";
    }

    if (!this->m_playerTexture->SetTexture(playerTexture, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        status->Add(STATUS_WARNING, "Invalid minimapPlayerTexture in Minimap.xml");
    }

    CLayoutFrame* compass = this->GetLayoutFrameByName("MinimapCompassTexture");
    this->m_compassTexture = compass ? static_cast<CScriptRegion*>(compass) : nullptr;
}

// VERIFY: c9d382fd auto
// OFFSET: 0x0057C540
bool CGMinimapFrame::IsA(int32_t type) {
    return type == CGMinimapFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: af38ff6d auto
// OFFSET: 0x0057C580
bool CGMinimapFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGMinimapFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// VERIFY: 812aa182 manual
// OFFSET: 0x0057C6A0
void CGMinimapFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        this->UpdateRotation(target->GetFacing());
    } else {
        this->UpdateRotation(camera->m_yaw);
    }
}

// VERIFY: 44ce878d auto
// OFFSET: 0x0057D580
void CGMinimapFrame::OnLayerCursorExit(int32_t a2, int32_t a3) {
    CGGameUI::m_gameTooltip->Hide();
    CSimpleFrame::OnLayerCursorExit(a2, a3);
}

// VERIFY: b41af30f auto
// OFFSET: 0x0057DCA0
CGMinimapFrame::CGMinimapFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_facing = -10000.0f;
    this->unk_02AC = 0;
    CGMinimapFrame::s_rotateMinimap = g_rotateMinimapCVar->m_intValue;
    this->m_playerTexture = nullptr;

    if (!CGMinimapFrame::s_minimapFrame) {
        CGMinimapFrame::s_minimapFrame = this;
    }

    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);

    float scale = NDCToDDCHeight(1.0f) * 1.6666666f;
    CGMinimapFrame::s_blipSize1 = 0.0125f * scale;
    CGMinimapFrame::s_blipHalfSize1 = CGMinimapFrame::s_blipSize1 * 0.5f;
    CGMinimapFrame::s_blipSize2 = scale * 0.0375f;
    CGMinimapFrame::s_blipHalfSize2 = CGMinimapFrame::s_blipSize2 * 0.5f;
    CGMinimapFrame::s_blipSize3 = scale * 0.045f;
    CGMinimapFrame::s_blipHalfSize3 = 0.5f * CGMinimapFrame::s_blipSize3;
    CGMinimapFrame::s_blipSize4 = scale * 0.055f;

    float halfSizes[3] = { CGMinimapFrame::s_blipHalfSize1, CGMinimapFrame::s_blipHalfSize2, CGMinimapFrame::s_blipHalfSize3 };

    for (uint32_t i = 0; i < 3; i++) {
        float half = halfSizes[i];
        CGMinimapFrame::s_blipQuads[i][0] = C3Vector(-half, -half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][1] = C3Vector(half, -half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][2] = C3Vector(-half, half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][3] = C3Vector(half, half, 0.0f);
    }

    this->m_blipFrameLevel = this->m_level + 3;

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(this, 2, 1) : nullptr;
    this->m_playerTexture = texture;
    texture->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, 0.001f, 0.001f, 1);
    this->m_compassTexture = nullptr;
}

// VERIFY: eb33fe04 auto
// OFFSET: [0x0057C250, 0x0057DFD0]
CGMinimapFrame::~CGMinimapFrame() {
    if (CGMinimapFrame::s_minimapFrame == this) {
        CGMinimapFrame::s_minimapFrame = nullptr;
    }
}

// VERIFY: 6329f6cb auto
// OFFSET: 0x005832A0
void CGMinimapFrame::RenderCallback(void* param) {
    if (param) {
        static_cast<CGMinimapFrame*>(param)->Render();
    }
}

// VERIFY: a761a756 auto
// OFFSET: 0x005832C0
void CGMinimapFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (!layer) {
        batch->QueueCallback(CGMinimapFrame::RenderCallback, this);
    }
}

// VERIFY: 30278d2d auto
// OFFSET: 0x005831C0
void CGMinimapFrame::EnterWorld() {
    s_unkBEBA40 = 1;
    MinimapInitialize(CGPlayer_C::GetNewContinentID());
    s_unkBF5F1C = 10.0f;
    EGxTexFormat format = GxTex_Unknown;

    if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Argb8888]) {
        format = GxTex_Argb8888;
    } else if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Abgr8888]) {
        format = GxTex_Abgr8888;
    } else if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Rgb565]) {
        format = GxTex_Rgb565;
    }

    CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 1, 1, 0, 0, 0);
    s_renderTarget = TextureCreate(256, 256, format, format, flags, nullptr, CGMinimapFrame::MinimapTextureCallback, "MinimapRenderTarget", 0);
    memset(s_unkBEB9A8, 0, sizeof(s_unkBEB9A8));
}

// VERIFY: 7d4e4155 manual
// OFFSET: 0x005832F0
void CGMinimapFrame::InitializeGame() {
    CStatus status;

    s_maskTexture = TextureCreate("Textures\\MinimapMask", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_iconTexture = TextureCreate("Interface\\Minimap\\POIIcons", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_blipTexture = TextureCreate("Interface\\Minimap\\ObjectIcons", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_classBlipTexture = TextureCreate("Interface\\Minimap\\PartyRaidBlips", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[0] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Unoccupied", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[1] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Occupied", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[2] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Unoccupied", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[3] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Occupied", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[4] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Unoccupied", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[5] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Horde", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_vehicleTextures[6] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Alliance", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_staticPOIArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapArrow", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_poiArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapGuideArrow", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_corpseArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapCorpseArrow", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);
    s_groupArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapGroupArrow", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 1);

    uint32_t width = 256;
    uint32_t height = 256;
    TextureGetDimensions(s_iconTexture, &width, &height, 1);

    uint32_t columns = CMath::sqrt_(196);
    float texel = 1.0f / width;
    float size = texel * 18.0f;

    for (uint32_t i = 0; i < 196; i++) {
        float left = (i % columns) * size + texel;
        float right = left + size - texel;
        float top = (i / columns) * size + texel;
        float bottom = top + size - texel;

        s_iconTexCoords[i][0].x = left;
        s_iconTexCoords[i][0].y = bottom;
        s_iconTexCoords[i][1].x = right;
        s_iconTexCoords[i][1].y = bottom;
        s_iconTexCoords[i][2].x = left;
        s_iconTexCoords[i][2].y = top;
        s_iconTexCoords[i][3].x = right;
        s_iconTexCoords[i][3].y = top;
    }

    for (uint32_t i = 0; i < 16; i++) {
        float left = (i & 7) * 0.125f;
        float bottom = ((i >> 3) + 1) * 0.5f;
        float right = ((i & 7) + 1) * 0.125f;
        float top = (i >> 3) * 0.5f;

        s_classBlipTexCoords[i][0].x = left;
        s_classBlipTexCoords[i][0].y = bottom;
        s_classBlipTexCoords[i][1].x = right;
        s_classBlipTexCoords[i][1].y = bottom;
        s_classBlipTexCoords[i][2].x = left;
        s_classBlipTexCoords[i][2].y = top;
        s_classBlipTexCoords[i][3].x = right;
        s_classBlipTexCoords[i][3].y = top;
    }

    for (uint32_t i = 0; i < 32; i++) {
        float left = (i & 7) * 0.125f;
        float bottom = ((i >> 3) + 1) * 0.25f;
        float right = ((i & 7) + 1) * 0.125f;
        float top = (i >> 3) * 0.25f;

        s_blipTexCoords[i][0].x = left;
        s_blipTexCoords[i][0].y = bottom;
        s_blipTexCoords[i][1].x = right;
        s_blipTexCoords[i][1].y = bottom;
        s_blipTexCoords[i][2].x = left;
        s_blipTexCoords[i][2].y = top;
        s_blipTexCoords[i][3].x = right;
        s_blipTexCoords[i][3].y = top;
    }

    CGMinimapFrame::s_trackingSpell = 0;
    CGMinimapFrame::s_trackingInfo = nullptr;
}

// VERIFY: 78b3bb60 manual
// OFFSET: 0x0057E980
int32_t CGMinimapFrame::CountAvailableTrackingTypes() {
    int32_t count = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t classMask = player ? 1 << player->m_unit->m_unitFieldBytes0[1] : 0;

    for (uint32_t i = 0; i < sizeof(CGMinimapFrame::s_trackingTypes) / sizeof(CGMinimapFrame::s_trackingTypes[0]); i++) {
        uint32_t mask = CGMinimapFrame::s_trackingTypes[i].m_classMask;

        if (!mask || (mask & classMask)) {
            count++;
        }
    }

    return count;
}

// VERIFY: 9e30c5ed manual
// OFFSET: 0x0057EB00
const TrackingTypeInfo* CGMinimapFrame::GetTrackingTypeByIndex(int32_t index) {
    int32_t count = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t classMask = player ? 1 << player->m_unit->m_unitFieldBytes0[1] : 0;

    for (uint32_t i = 0; i < sizeof(CGMinimapFrame::s_trackingTypes) / sizeof(CGMinimapFrame::s_trackingTypes[0]); i++) {
        uint32_t mask = CGMinimapFrame::s_trackingTypes[i].m_classMask;

        if (mask && !(mask & classMask)) {
            continue;
        }

        if (count == index) {
            return &CGMinimapFrame::s_trackingTypes[i];
        }

        count++;
    }

    return nullptr;
}

// VERIFY: aba21c39 auto
// OFFSET: 0x007F3BE0
AreaPOIRec* MinimapGetGossipPoi() {
    return &s_dynamicPOIs[1];
}

// VERIFY: 23614ed9 manual
// OFFSET: 0x0057EA30
void CGMinimapFrame::SetTrackingSpell(int32_t spellId) {
    CGMinimapFrame::s_trackingSpell = spellId;

    if (spellId) {
        CGMinimapFrame::LoadTrackingInfo(0);
    } else {
        Script_SendUniqueSignal(SCRIPT_EVENT_MINIMAP_UPDATE_TRACKING);
    }
}

// VERIFY: 08401ea1 auto
// OFFSET: 0x007F3B90
float MinimapGetViewRadius() {
    if (s_isIndoors) {
        return s_indoorZoomRadius[s_indoorZoom];
    }

    return s_outdoorZoomTiles[s_outdoorZoom] * 0.5f * 33.333332f;
}

// VERIFY: 10a7b5c4 manual
// OFFSET: 0x007F44A0
void UpdatePointsOfInterest() {
    if (s_mapId == -1) {
        return;
    }

    s_visiblePOIs.SetCount(0);
    float viewRadius = MinimapGetViewRadius();

    for (uint32_t i = 0; i < s_numPOIs; i++) {
        AreaPOIRec* poi = s_pois.m_data[i];

        if ((poi->m_pos[0] == 0.0f && poi->m_pos[1] == 0.0f) || (poi->m_worldStateID && !GetWorldState(poi->m_worldStateID))) {
            if (!s_poiVisible.m_data[i] && !s_poiOnEdge.m_data[i]) {
                continue;
            }

            s_poiVisible.m_data[i] = 0;
            s_poiOnEdge.m_data[i] = 0;
            s_poisChanged = 1;
            continue;
        }

        float dy = s_playerPosition.y - poi->m_pos[1];
        float dx = s_playerPosition.x - poi->m_pos[0];

        if (sqrt(dx * dx + dy * dy) / viewRadius > 0.8f) {
            if (!s_poiVisible.m_data[i] && s_poiOnEdge.m_data[i]) {
                continue;
            }

            s_poiVisible.m_data[i] = 0;
            s_poiOnEdge.m_data[i] = 1;
            s_poisChanged = 1;
            continue;
        }

        if (!(poi->m_flags & 2)) {
            if (!s_poiVisible.m_data[i] && !s_poiOnEdge.m_data[i]) {
                continue;
            }

            s_poiVisible.m_data[i] = 0;
            s_poiOnEdge.m_data[i] = 0;
            s_poisChanged = 1;
            continue;
        }

        *s_visiblePOIs.New() = poi;

        if (s_poiVisible.m_data[i] && !s_poiOnEdge.m_data[i]) {
            continue;
        }

        s_poiVisible.m_data[i] = 1;
        s_poiOnEdge.m_data[i] = 0;
        s_poisChanged = 1;
    }

    int32_t worst = -1;
    uint32_t count = 0;
    float distances[3] = { 694.44446f, 694.44446f, 694.44446f };
    int32_t indices[3] = { -1, -1, -1 };
    int32_t importance[3];

    for (uint32_t i = 0; i < s_numPOIs; i++) {
        if (!s_poiOnEdge.m_data[i]) {
            continue;
        }

        AreaPOIRec* poi = s_pois.m_data[i];
        float dy = poi->m_pos[1] - s_playerPosition.y;
        float dx = poi->m_pos[0] - s_playerPosition.x;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 694.44446f && poi != &s_dynamicPOIs[2]) {
            continue;
        }

        if (worst == -1) {
            distances[count] = distance;
            indices[count] = i;
            importance[count] = poi->m_importance;
            count++;
        } else if (poi->m_importance < importance[worst] || (poi->m_importance == importance[worst] && distance < distances[worst])) {
            distances[worst] = distance;
            indices[worst] = i;
            importance[worst] = poi->m_importance;
        }

        if (count == 3) {
            worst = 0;

            for (int32_t j = 1; j < 3; j++) {
                if (importance[j] > importance[worst] || (importance[j] == importance[worst] && distances[worst] < distances[j])) {
                    worst = j;
                }
            }
        }
    }

    if (s_numClosestPOIs != count) {
        s_numClosestPOIs = count;
        s_closestPOIsChanged = 1;
    } else {
        for (uint32_t j = 0; j < count; j++) {
            if (indices[j] != s_closestPOIs[j]) {
                s_closestPOIsChanged = 1;
                break;
            }
        }
    }

    for (uint32_t j = 0; j < count; j++) {
        s_closestPOIs[j] = indices[j];
        AreaPOIRec* poi = s_pois.m_data[indices[j]];
        C3Vector position(poi->m_pos[0], poi->m_pos[1], s_playerPosition.z);
        s_closestPOIFacing[j] = CalculateFacingTo(&s_playerPosition, &position);
    }
}

// VERIFY: 3a8b6454 auto
// OFFSET: 0x007F4990
void MinimapSetCorpsePOI(float x, float y) {
    const char* name = FrameScript_GetText("CORPSE_RED", -1, GENDER_NOT_APPLICABLE);
    s_dynamicPOIs[2].m_pos[0] = x;
    s_dynamicPOIs[2].m_pos[1] = y;
    s_dynamicPOIs[2].m_flags = 2;
    s_dynamicPOIs[2].m_continentID = s_mapId;
    s_dynamicPOIs[2].m_icon[0] = 8;
    s_dynamicPOIs[2].m_importance = -1;
    SStrCopy(s_dynamicPOINames[2], name, sizeof(s_dynamicPOINames[2]));
    UpdatePointsOfInterest();
    s_closestPOIsChanged = 1;
}

// VERIFY: 7ee85ac8 auto
// OFFSET: 0x007F4A00
void MinimapSetSpiritHealerPOI(float x, float y) {
    const char* name = FrameScript_GetText("SPIRIT_HEALER_RELEASE_RED", -1, GENDER_NOT_APPLICABLE);
    s_dynamicPOIs[3].m_pos[0] = x;
    s_dynamicPOIs[3].m_pos[1] = y;
    s_dynamicPOIs[3].m_flags = 2;
    s_dynamicPOIs[3].m_continentID = s_mapId;
    s_dynamicPOIs[3].m_icon[0] = 8;
    s_dynamicPOIs[3].m_importance = -1;
    SStrCopy(s_dynamicPOINames[3], name, sizeof(s_dynamicPOINames[3]));
    UpdatePointsOfInterest();
    s_closestPOIsChanged = 1;
}

// VERIFY: 0eda32e5 manual
// OFFSET: 0x007F6540
static void LoadMD5Names() {
    char path[260];
    SStrPrintf(path, sizeof(path), "%s\\md5translate.trs", s_minimapTexturePath);
    void* data;

    if (!SFile::LoadFile(path, &data, nullptr, 1, nullptr)) {
        return;
    }

    const char* cursor = static_cast<const char*>(data);
    char line[260];
    memset(line, 0, sizeof(line));

    do {
        SStrTokenize(&cursor, line, sizeof(line), "\r\n", nullptr);

        if (!*cursor || !line[0]) {
            break;
        }

        if (!SStrCmp(line, "dir:", SStrLen("dir:"))) {
            continue;
        }

        char* tab = SStrChr(line, '\t');

        if (!tab) {
            continue;
        }

        *tab = '\0';
        MINIMAPMD5NAME* entry = s_md5Names.Ptr(line);

        if (!entry) {
            entry = s_md5Names.New(line, 0, 0);
        }

        SStrCopy(entry->m_md5Name, tab + 1, sizeof(entry->m_md5Name));
    } while (line[0] && *cursor);

    SFile::Unload(data);
}

// VERIFY: 885c45ba manual
// OFFSET: 0x007F6730
void MinimapInitialize(int32_t mapId) {
    s_numPOIs = 0;

    for (int32_t pass = 0; pass < 2; pass++) {
        if (pass > 0) {
            s_pois.SetCount(s_numPOIs);
            s_poiVisible.SetCount(s_numPOIs);
            s_poiOnEdge.SetCount(s_numPOIs);
            memset(s_poiVisible.m_data, 0, sizeof(int32_t) * s_poiVisible.m_count);
            memset(s_poiOnEdge.m_data, 0, sizeof(int32_t) * s_poiOnEdge.m_count);
        }

        uint32_t count = 0;

        for (int32_t i = g_areaPOIDB.m_numRecords - 1; i >= 0; i--) {
            AreaPOIRec* rec = g_areaPOIDB.GetRecordByIndex(i);

            if (rec->m_continentID == mapId && (rec->m_flags & 1)) {
                if (pass > 0) {
                    s_pois.m_data[count] = rec;
                }

                count++;
            }
        }

        s_numPOIs = count + 4;
    }

    for (uint32_t i = 0; i < 4; i++) {
        s_pois.m_data[s_numPOIs - 4 + i] = &s_dynamicPOIs[i];
        memset(&s_dynamicPOIs[i], 0, sizeof(s_dynamicPOIs[i]));
        s_dynamicPOIs[i].m_name = s_dynamicPOINames[i];
    }

    s_closestPOIFacing[0] = -1.0f;
    s_closestPOIFacing[1] = -1.0f;
    s_closestPOIFacing[2] = -1.0f;
    s_closestPOIs[0] = -1;
    s_closestPOIs[1] = -1;
    s_closestPOIs[2] = -1;
    s_outdoorZoom = g_minimapZoomCVar->m_intValue;
    s_indoorZoom = g_minimapInsideZoomCVar->m_intValue;
    FrameScript_SignalEvent(SCRIPT_EVENT_MINIMAP_UPDATE_ZOOM, nullptr);
    LoadMD5Names();
}
