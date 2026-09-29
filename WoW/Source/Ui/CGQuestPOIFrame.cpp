#include "CGQuestPOIFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00ACF17C
const char* CGQuestPOIFrame::s_objectTypeName = "QuestPOIFrame";

// STATIC: 0x00ACF1F0
int32_t CGQuestPOIFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00C0D7C4
int32_t CGQuestPOIFrame::s_objectType;

// STATIC: 0x00C0D7C8
CDataAllocator CGQuestPOIFrame::s_questPOIHeap(sizeof(CGQuestPOIFrame), 5);

// INLINED
void CGQuestPOIFrame::operator delete(void* ptr) {
    CGQuestPOIFrame::s_questPOIHeap.PutData(ptr, nullptr, 0);
}

// INLINED
int32_t CGQuestPOIFrame::GetObjectType() {
    if (!CGQuestPOIFrame::s_objectType) {
        CGQuestPOIFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGQuestPOIFrame::s_objectType;
}

// VERIFY: f4d221d4 auto
// OFFSET: 0x00514260
CSimpleFrame* CGQuestPOIFrame::Create(CSimpleFrame* parent) {
    if (CGQuestPOIFrame::s_questPOIHeap.m_bytesPerData < sizeof(CGQuestPOIFrame)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGQuestPOIFrame::s_questPOIHeap.GetData(0, typeid(CGQuestPOIFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGQuestPOIFrame(parent) : nullptr;
}

// VERIFY: 766b947d auto
// OFFSET: 0x0058ECF0
CGQuestPOIFrame::QuestPOIRenderData::~QuestPOIRenderData() {
}

// VERIFY: b799b5fe manual
// OFFSET: [0x0058F9D0, 0x0058FAB0, 0x00590060]
CGQuestPOIFrame::~CGQuestPOIFrame() {
    if (this->m_fillTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_fillTexture));
        this->m_fillTexture = nullptr;
    }

    if (this->m_borderTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_borderTexture));
        this->m_borderTexture = nullptr;
    }
}

// VERIFY: 4905382c auto
// OFFSET: 0x0058FA90
const char* CGQuestPOIFrame::GetObjectTypeName() {
    return CGQuestPOIFrame::s_objectTypeName;
}

// VERIFY: d10ef621 auto
// OFFSET: 0x0058FAA0
int32_t CGQuestPOIFrame::GetScriptMetaTable() {
    return CGQuestPOIFrame::s_metatable;
}

// VERIFY: 50cca9d9 auto
// OFFSET: 0x0058FAC0
bool CGQuestPOIFrame::IsA(int32_t type) {
    return type == CGQuestPOIFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 4f27cf4a auto
// OFFSET: 0x0058FB00
bool CGQuestPOIFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGQuestPOIFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// VERIFY: 2f4eb365 manual
// OFFSET: 0x0058FD30
void CGQuestPOIFrame::SetFillTexture(const char* fileName) {
    if (this->m_fillTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_fillTexture));
        this->m_fillTexture = nullptr;
    }

    CStatus status;
    this->m_fillTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 0);
}

// VERIFY: ca0d2a19 manual
// OFFSET: 0x0058FDC0
void CGQuestPOIFrame::SetBorderTexture(const char* fileName) {
    if (this->m_borderTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_borderTexture));
        this->m_borderTexture = nullptr;
    }

    CStatus status;
    this->m_borderTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 0);
}

// VERIFY: e49239e2 manual
// OFFSET: 0x0058FF50
CGQuestPOIFrame::CGQuestPOIFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->SetProtected();
    this->DisableEvent(SIMPLE_EVENT_MOUSE);
    this->m_borderScalar = 1.0f;
    this->m_fillAlpha = 0xFF;
    this->m_borderAlpha = 0xFF;
    this->m_mergeThreshold = 0.25f;
    this->m_fillTexture = nullptr;
    this->m_borderTexture = nullptr;
    this->unk_02B8 = 0;
    this->unk_02BC = 0;
    this->m_smoothing = 1;
    this->m_merging = 1;
    this->m_numSplinePoints = 20;

    for (uint32_t i = 0; i < sizeof(this->m_groups) / sizeof(this->m_groups[0]); i++) {
        this->m_groups[i].m_count = 0;
    }
}

// VERIFY: f41da0ed auto
// OFFSET: 0x00590090
void CGQuestPOIFrame::LoadXML(XMLNode* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    const char* fillTexture = node->GetAttributeByName("filltexture");

    if (fillTexture && *fillTexture) {
        this->SetFillTexture(fillTexture);
    }

    const char* borderTexture = node->GetAttributeByName("bordertexture");

    if (borderTexture && *borderTexture) {
        this->SetBorderTexture(borderTexture);
    }
}

// VERIFY: d36106ae auto
// OFFSET: 0x005905D0
void CGQuestPOIFrame::RenderCallback(void* param) {
    if (param) {
        static_cast<CGQuestPOIFrame*>(param)->Render();
    }
}

// VERIFY: 4d0a681d auto
// OFFSET: 0x00590660
void CGQuestPOIFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 3) {
        batch->QueueCallback(CGQuestPOIFrame::RenderCallback, this);
    }
}
