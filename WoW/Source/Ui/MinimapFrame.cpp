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

// STATIC: 0x00BEBA40
static int32_t s_unkBEBA40;

// STATIC: 0x00BF5F1C
static float s_unkBF5F1C;

// STATIC: 0x00BF5F14
static CTexture* s_renderTarget;

// STATIC: 0x00BEB9A8
static uint32_t s_unkBEB9A8[23];

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
