#include <cfloat>
#include <cstdint>
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"

// STATIC: 0x00AD8B4C
static uint32_t s_primVertsPerPrim[GxPrims_Last] = { 1, 2, 1, 3, 1, 1 };

// STATIC: 0x00AD8B64
static uint32_t s_primVertsExtra[GxPrims_Last] = { 0, 0, 1, 0, 2, 2 };

// STATIC: 0x00AD8BC0
static C2iVector kWinMinSize = { 320, 240 };

// STATIC: 0x00C5DF88
CGxDevice* g_theGxDevicePtr;

// STATIC: 0x00C5DFE0
static ShadowConstants s_shadowConstants[2];

// STATIC: 0x00C5FFF0
static float kWinAspect;

// STATIC: 0x00C60300
static C2iVector s_clientAdjust;

void CGxDevice::DeviceEvictResources() {
}

void CGxDevice::DeviceSaveDepthSurface() {
}

void CGxDevice::DeviceRestoreDepthSurface() {
}

void CGxDevice::DeviceResolveDepthBuffer(CGxTex* texture) {
}

void CGxDevice::DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane) {
}

void CGxDevice::LogCrashInfo(char* buffer, uint32_t size) {
}

void CGxDevice::SceneClear(uint32_t mask, CImVector color) {
}

void CGxDevice::Draw(CGxBatch* batch, int32_t count) {
}

void CGxDevice::PrimPointSize(float size) {
}

void CGxDevice::PrimLineWidth(float width) {
}

void CGxDevice::TexSetCacheSize(uint32_t size) {
}

// VERIFY: c758af65 manual
// OFFSET: 0x00682CB0
int32_t CGxDevice::DeviceCreate(GxWindowProc windowProc, const CGxFormat& format) {
    this->m_windowProc = windowProc;
    return this->DeviceSetFormat(format);
}

// VERIFY: c06d856e auto
// OFFSET: 0x00682CD0
int32_t CGxDevice::DeviceCreate(void* window, const CGxFormat& format) {
    this->m_format = format;
    return 1;
}

// VERIFY: e7b62b2b auto
// OFFSET: 0x00682CF0
void CGxDevice::DeviceDestroy() {
    this->m_windowProc = nullptr;
}

// VERIFY: 25e504a6 auto
// OFFSET: 0x00682D00
void CGxDevice::DeviceSetBaseMipLevel(uint32_t level) {
    this->m_baseMipLevel = level;
}

// VERIFY: 524ed3c5 auto
// OFFSET: 0x00682D30
void CGxDevice::DeviceTakeScreenShot() {
    this->m_scrShotClick = 1;
}

// VERIFY: 4da47b00 auto
// OFFSET: 0x00682E50
void CGxDevice::ScenePresent() {
    this->m_frameCount++;
    this->m_scrShotClick = 0;
    this->m_intF5C = 0;
}

// VERIFY: 7c26f822 auto
// OFFSET: 0x00682F70
void CGxDevice::PrimNormal(const C3Vector& n) {
    this->m_primNormal = n;
    this->m_primMask |= 0x200;
}

// VERIFY: c4ffca2f auto
// OFFSET: 0x00682FA0
void CGxDevice::PrimTexCoord(uint32_t tmu, const C2Vector& t) {
    this->m_primTexCoord[tmu] = t;
    this->m_primMask |= 2 << tmu;
}

// VERIFY: c119ac3a manual
// OFFSET: 0x00683130
void CGxDevice::BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
    buf->unk_001E = 1;
    buf->unk_001F = 0;
}

// VERIFY: 0d3647c3 manual
// OFFSET: 0x00683150
char* CGxDevice::BufLock(CGxBuf* buf) {
    buf->unk_001E = 1;
    buf->unk_001F = 0;
    this->m_bufLocked[buf->m_pool->m_target] = buf;
    return nullptr;
}

// VERIFY: da5b4d82 auto
// OFFSET: 0x00683180
int32_t CGxDevice::BufUnlock(CGxBuf* buf, uint32_t size) {
    this->m_bufLocked[buf->m_pool->m_target] = nullptr;
    return 1;
}

// VERIFY: 51c4b8e7 auto
// OFFSET: 0x00683640
void CGxDevice::CursorSetVisible(int32_t visible) {
    this->m_cursorVisible = visible;
}

// VERIFY: dfe1cbff auto
// OFFSET: 0x00683650
uint32_t* CGxDevice::CursorLock() {
    return this->m_cursor;
}

// VERIFY: fd4d7197 manual
// OFFSET: 0x006840F0
int32_t CGxDevice::DeviceSetFormat(const CGxFormat& format) {
    this->m_format = format;

    if (this->m_format.m_size.x < kWinMinSize.x && this->m_format.m_size.y < kWinMinSize.y) {
        this->m_format.m_size = kWinMinSize;
    }

    if (this->m_format.m_aspect) {
        kWinAspect = static_cast<float>(this->m_format.m_size.x) / static_cast<float>(this->m_format.m_size.y);
    } else {
        kWinAspect = 0.0f;
    }

    return 1;
}

// VERIFY: 142cf1ae auto
// OFFSET: 0x00684170
void CGxDevice::DeviceSetGamma(float gamma) {
    this->m_gammaRamp.Set(gamma);
}

// VERIFY: 9e53fc0c auto
// OFFSET: 0x00684190
void CGxDevice::DeviceSetGamma(const CGxGammaRamp& ramp) {
    this->m_gammaRamp = ramp;
}

// VERIFY: d081145f auto
// OFFSET: 0x00684260
void CGxDevice::DeviceReadScreenShot(uint32_t& width, uint32_t& height, const CImVector*& pixels) {
    width = this->m_scrShotWidth;
    height = this->m_scrShotHeight;
    pixels = this->m_scrShotPixels.Count() ? this->m_scrShotPixels.Ptr() : nullptr;
}

// VERIFY: 29912845 manual
// OFFSET: 0x006842A0
void CGxDevice::DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    CGxDevice::TextureTarget& target = this->m_textureTarget[buffer];
    target.m_plane = plane;
    target.m_texture = texture;

    if (texture && (this->m_textureTarget[GxBuffers_Color].m_texture || this->m_textureTarget[GxBuffers_Depth].m_texture)) {
        this->m_curWindowRect.minY = 0.0f;
        this->m_curWindowRect.minX = 0.0f;
        this->m_curWindowRect.maxY = static_cast<float>(texture->m_height);
        this->m_curWindowRect.maxX = static_cast<float>(texture->m_width);
    } else {
        this->m_curWindowRect = this->m_defWindowRect;
    }
}

// VERIFY: 48d0bcc7 auto
// OFFSET: 0x006843B0
void CGxDevice::NotifyOnDeviceRestored() {
    for (uint32_t i = 0; i < this->m_deviceRestoredCallbacks.Count(); i++) {
        this->m_deviceRestoredCallbacks[i]();
    }
}

// VERIFY: 27328acc auto
// OFFSET: 0x006843E0
void CGxDevice::NotifyOnTextureRecreation() {
    for (uint32_t i = 0; i < this->m_textureRecreationCallbacks.Count(); i++) {
        this->m_textureRecreationCallbacks[i]();
    }
}

// VERIFY: 1bc77089 auto
// OFFSET: 0x00684410
void CGxDevice::NotifyOnStereoChanged() {
    for (uint32_t i = 0; i < this->m_stereoChangedCallbacks.Count(); i++) {
        this->m_stereoChangedCallbacks[i]();
    }
}

// VERIFY: e5049b8a auto
// OFFSET: 0x00684590
void CGxDevice::PrimColor(const CImVector& c) {
    this->m_primColor = c;
    this->m_primMask |= 0x400;
}

// VERIFY: df433744 auto
// OFFSET: 0x00684900
void CGxDevice::ITexMarkAsUpdated(CGxTex* texId) {
    if (texId->m_needsUpdate) {
        texId->m_updateRect.minY = texId->m_height;
        texId->m_updateRect.minX = texId->m_width;
        texId->m_updateRect.maxY = 0;
        texId->m_updateRect.maxX = 0;
        texId->m_needsUpdate = 0;
    }
}

// VERIFY: 62bd795e manual
// OFFSET: 0x006853B0
void CGxDevice::AddDeviceRestoredCallback(GxDeviceCallback callback) {
    this->m_deviceRestoredCallbacks.New(callback);
}

// VERIFY: 657b2662 manual
// OFFSET: 0x006853D0
void CGxDevice::RemoveDeviceRestoredCallback(GxDeviceCallback callback) {
    uint32_t count = this->m_deviceRestoredCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_deviceRestoredCallbacks[i] == callback) {
            this->m_deviceRestoredCallbacks[i] = this->m_deviceRestoredCallbacks[count - 1];
            this->m_deviceRestoredCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// VERIFY: 8ea3cd66 manual
// OFFSET: 0x00685460
void CGxDevice::AddTextureRecreationCallback(GxDeviceCallback callback) {
    this->m_textureRecreationCallbacks.New(callback);
}

// VERIFY: 50ad83fc manual
// OFFSET: 0x00685480
void CGxDevice::RemoveTextureRecreationCallback(GxDeviceCallback callback) {
    uint32_t count = this->m_textureRecreationCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_textureRecreationCallbacks[i] == callback) {
            this->m_textureRecreationCallbacks[i] = this->m_textureRecreationCallbacks[count - 1];
            this->m_textureRecreationCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// VERIFY: 3c7c9e71 manual
// OFFSET: 0x00685510
void CGxDevice::AddStereoChangedCallback(GxDeviceCallback callback) {
    this->m_stereoChangedCallbacks.New(callback);
}

// VERIFY: 94dac580 manual
// OFFSET: 0x00685530
void CGxDevice::RemoveStereoChangedCallback(GxDeviceCallback callback) {
    uint32_t count = this->m_stereoChangedCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_stereoChangedCallbacks[i] == callback) {
            this->m_stereoChangedCallbacks[i] = this->m_stereoChangedCallbacks[count - 1];
            this->m_stereoChangedCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// VERIFY: df04dabc manual
// OFFSET: 0x006855C0
void CGxDevice::PrimBegin(EGxPrim primType) {
    this->m_primType = primType;
    this->m_inBeginEnd = 1;
    this->m_primMask = 0;
    this->m_primIndexArray.SetCount(0);
    this->m_primVertexArray.SetCount(0);

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_caps.m_numTmus); i++) {
        this->m_primTexCoordArray[i].SetCount(0);
    }

    this->m_primNormalArray.SetCount(0);
    this->m_primColorArray.SetCount(0);
}

// VERIFY: b1a5b43a manual
// OFFSET: 0x00685640
void CGxDevice::PrimVertex(const C3Vector& v) {
    this->m_primVertex = v;
    this->m_primMask |= 0x1;

    uint16_t index = static_cast<uint16_t>(this->m_primVertexArray.Count());
    *this->m_primIndexArray.New() = index;
    *this->m_primVertexArray.New() = this->m_primVertex;

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_caps.m_numTmus); i++) {
        *this->m_primTexCoordArray[i].New() = this->m_primTexCoord[i];
    }

    *this->m_primNormalArray.New() = this->m_primNormal;
    *this->m_primColorArray.New() = this->m_primColor;

    uint32_t perPrim = s_primVertsPerPrim[this->m_primType];
    uint32_t indexCount = this->m_primIndexArray.Count();
    int32_t prims = (perPrim == 1 ? indexCount : indexCount / perPrim) - s_primVertsExtra[this->m_primType];
    uint32_t maxVerts = perPrim == 1 ? 0x20000 : 0x20000 / perPrim;

    if (prims == static_cast<int32_t>(maxVerts - s_primVertsExtra[this->m_primType])) {
        uint32_t mask = this->m_primMask;
        this->PrimDrawElements();
        this->PrimBegin(this->m_primType);
        this->m_primMask = mask;
    }
}

// VERIFY: 37074a54 manual
// OFFSET: 0x006877C0
void CGxDevice::QueryCreate(CGxQuery*& query, EGxQueryType type) {
    query = new (SMemAlloc(sizeof(CGxQuery), __FILE__, __LINE__, 0)) CGxQuery(type);
    this->m_queryList.LinkToTail(query);
}

// VERIFY: d0147a38 manual
// OFFSET: 0x006879F0
void CGxDevice::QueryDestroy(CGxQuery*& query) {
    query->m_link.Unlink();
    delete query;
    query = nullptr;
}

// VERIFY: d0a23aa3 auto
// OFFSET: 0x00689040
void CGxDevice::XformSetProjection(const C44Matrix& matrix) {
    this->m_projection = matrix;
}

// VERIFY: 1e7393f5 manual
// OFFSET: 0x0057C3A0
void CGxDevice::XformPush(EGxXform xf) {
    CGxMatrixStack& stack = this->m_xforms[xf];
    stack.m_level++;

    if (stack.m_level >= 3) {
        stack.m_level = 3;
    }

    stack.m_mtx[stack.m_level] = stack.m_mtx[stack.m_level - 1];
    stack.m_flags[stack.m_level] = stack.m_flags[stack.m_level - 1];
    stack.m_dirty = 1;
}

// VERIFY: a5c0394a auto
// OFFSET: 0x00682D20
EGxApi CGxDevice::DeviceApi() {
    return this->m_api;
}

// VERIFY: 8d0aae56 manual
// OFFSET: 0x00682DC0
int32_t CGxDevice::CapsIsWindowVisible(int32_t visible) {
    if (visible < 0) {
        return this->m_windowVisible;
    }

    this->m_windowVisible = visible > 0;
    return visible > 0;
}

// VERIFY: be5adc5f manual
// OFFSET: 0x00682E20
int32_t CGxDevice::CapsHasContext(int32_t context) {
    if (context < 0) {
        return this->m_context;
    }

    this->m_context = context > 0;
    return context > 0;
}

// VERIFY: 2c348324 auto
// OFFSET: 0x00683100
int32_t CGxDevice::MasterEnable(EGxMasterEnables state) {
    return ((1 << state) & this->m_appMasterEnables) != 0;
}

// VERIFY: c1bb2ca3 auto
// OFFSET: 0x00685370
void CGxDevice::DeviceClearScreenShot() {
    this->m_scrShotHeight = 0;
    this->m_scrShotWidth = 0;
    this->m_scrShotPixels.Clear();
}

// VERIFY: dfe43898 auto
// OFFSET: 0x00532AF0
CGxCaps& CGxDevice::Caps() {
    return this->m_caps;
}

// VERIFY: 6ef0a158 manual
// OFFSET: 0x00616A30
void CGxDevice::XformPushAndSet(EGxXform xf, const C44Matrix& matrix) {
    this->XformPush(xf);

    CGxMatrixStack& stack = this->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;
}

// VERIFY: 522bd08f manual
// OFFSET: 0x00682C50
void CGxDevice::InitLights() {
    for (uint32_t i = 0; i < 4; i++) {
        this->m_lights[i].m_flags = 0xFF00FF;
        this->m_lights[i].m_enable = 0;
    }
}

// VERIFY: dfbdc17f auto
// OFFSET: 0x00682D10
uint32_t CGxDevice::DeviceBaseMipLevel() {
    return this->m_baseMipLevel;
}

// VERIFY: 9260b546 auto
// OFFSET: 0x00682D40
int32_t CGxDevice::IDevIsWindowed() {
    return this->m_format.m_window;
}

// VERIFY: f81d278e auto
// OFFSET: 0x00682D70
const CRect& CGxDevice::DeviceCurWindow() {
    return this->m_curWindowRect;
}

// VERIFY: a019ace1 auto
// OFFSET: 0x00682D80
const CRect& CGxDevice::DeviceDefWindow() {
    return this->m_defWindowRect;
}

// VERIFY: e58361ab auto
// OFFSET: 0x00682D90
void CGxDevice::DeviceSetCurWindow(const CRect& rect) {
    this->m_curWindowRect = rect;
}

// VERIFY: a202f01f manual
// OFFSET: 0x00682DF0
int32_t CGxDevice::CapsWindowHasFocus(int32_t focus) {
    if (focus < 0) {
        return this->m_windowFocus;
    }

    this->m_windowFocus = focus > 0;
    return focus > 0;
}

// VERIFY: efe49605 auto
// OFFSET: 0x00682E70
void CGxDevice::SetScissor(const CRect& rect) {
    this->m_scissorRectDirty = 1;
    this->m_scissorRect = rect;
}

// VERIFY: 712d3c39 auto
// OFFSET: 0x00682EB0
void CGxDevice::PrimVertexMask(uint32_t mask) {
    this->m_primVertexDirty |= mask ^ this->m_primVertexMask;
    this->m_primVertexMask = mask;
    this->m_primVertexFormat = GxVertexBufferFormats_Last;
}

// VERIFY: a26b1a58 auto
// OFFSET: 0x00682F10
void CGxDevice::PrimIndexPtr(CGxBuf* buf) {
    if (buf->unk_001E || this->m_primIndexBuf != buf) {
        buf->unk_001E = 0;
        this->m_primIndexDirty = 1;
        this->m_primIndexBuf = buf;
    }
}

// VERIFY: 31489fa9 manual
// OFFSET: 0x00682F40
uint32_t CGxDevice::PrimCalcCount(EGxPrim primType, uint32_t indexCount) {
    uint32_t perPrim = s_primVertsPerPrim[primType];
    return (perPrim == 1 ? indexCount : indexCount / perPrim) - s_primVertsExtra[primType];
}

// VERIFY: 341ce950 manual
// OFFSET: 0x00683080
void CGxDevice::LightEnable(uint32_t index, int32_t enable) {
    CGxApiLight& light = this->m_lights[index];

    if (light.m_enable != enable) {
        light.m_flags |= 0x1;
        light.m_enable = enable;
    }
}

// VERIFY: 45b10c41 manual
// OFFSET: 0x006830B0
int32_t CGxDevice::NeedsUpdate(uint32_t app, uint32_t hw, uint32_t appDisables, uint32_t hwDisables, uint32_t flagPos, int32_t& enable) {
    enable = (app & (1 << flagPos) & ~appDisables) != 0;
    return enable != ((hw & (1 << flagPos) & ~hwDisables) != 0);
}

// VERIFY: 30703cd8 auto
// OFFSET: 0x006831A0
void CGxDevice::BufSizeSet(CGxBuf* buf, uint32_t itemSize, uint32_t itemCount) {
    buf->m_itemSize = itemSize;
    buf->m_itemCount = itemCount;
    buf->m_size = itemCount * itemSize;
    buf->unk_001C = 0;
}

// VERIFY: fdced826 auto
// OFFSET: 0x006831C0
int32_t CGxDevice::TexIsReadyNow(CGxTex* texId) {
    return texId->m_apiSpecificData && !texId->m_needsUpdate && !texId->m_needsCreation;
}

// VERIFY: 9e848655 auto
// OFFSET: 0x006831F0
int32_t CGxDevice::TexCanUpdate(CGxTex* texId) {
    return texId->m_userFunc != nullptr;
}

// VERIFY: 6ff3bc84 auto
// OFFSET: 0x00683320
void CGxDevice::TexSetCannotUpdate(CGxTex* texId) {
    texId->m_userFunc = nullptr;
    texId->m_userArg = nullptr;
}

// VERIFY: bec86cc4 auto
// OFFSET: 0x00683340
void CGxDevice::TexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat) {
    texId->m_dataFormat = dataFormat;
}

// VERIFY: 8d0ba344 auto
// OFFSET: 0x00683350
void CGxDevice::TexParameters(const CGxTex* texId, CGxTexParms& parms) {
    parms.m_target = texId->m_target;
    parms.m_width = texId->m_width;
    parms.m_height = texId->m_height;
    parms.m_depth = texId->m_depth;
    parms.m_format = texId->m_format;
    parms.m_dataFormat = texId->m_dataFormat;
    parms.m_flags = texId->m_flags;
    parms.m_userArg = texId->m_userArg;
    parms.m_userFunc = texId->m_userFunc;
}

// VERIFY: 48e3bca3 auto
// OFFSET: 0x006833A0
void CGxDevice::ShaderConstantsClear() {
    for (uint32_t i = 0; i < 256 * 4; i++) {
        s_shadowConstants[0].constants[i] = FLT_MAX;
    }

    for (uint32_t i = 0; i < 256 * 4; i++) {
        s_shadowConstants[1].constants[i] = FLT_MAX;
    }

    s_shadowConstants[1].dirtyMin = 255;
    s_shadowConstants[1].dirtyMax = 0;
    s_shadowConstants[0].dirtyMin = 255;
    s_shadowConstants[0].dirtyMax = 0;
}

// VERIFY: 4ce72d69 manual
// OFFSET: 0x006833E0
void CGxDevice::ShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    ShadowConstants& shadow = s_shadowConstants[target ? 0 : 1];
    float* dest = &shadow.constants[index * 4];

    for (uint32_t i = index; i < index + count; i++) {
        int32_t changed = 0;

        for (uint32_t c = 0; c < 4; c++) {
            if (constants[c] != dest[c]) {
                dest[c] = constants[c];
                changed = 1;
            }
        }

        if (changed) {
            if (shadow.dirtyMin > i) {
                shadow.dirtyMin = i;
            }

            if (shadow.dirtyMax < i) {
                shadow.dirtyMax = i;
            }
        }

        dest += 4;
        constants += 4;
    }
}

// VERIFY: 713bcd86 manual
// OFFSET: 0x00683560
float* CGxDevice::ShaderConstantsLock(EGxShTarget target) {
    return s_shadowConstants[target ? 0 : 1].constants;
}

// VERIFY: 636e6d3d manual
// OFFSET: 0x00683580
void CGxDevice::ShaderConstantsUnlock(EGxShTarget target, uint32_t index, uint32_t count) {
    ShadowConstants& shadow = s_shadowConstants[target ? 0 : 1];

    if (shadow.dirtyMin > index) {
        shadow.dirtyMin = index;
    }

    if (shadow.dirtyMax < index + count - 1) {
        shadow.dirtyMax = index + count - 1;
    }
}

// VERIFY: ab58eaf0 auto
// OFFSET: 0x006835E0
void CGxDevice::ICursorDestroy() {
    if (this->m_cursorTexture) {
        GxTexDestroy(this->m_cursorTexture);
        this->m_cursorTexture = nullptr;
    }
}

// VERIFY: 63ceb0b1 auto
// OFFSET: 0x00683610
void CGxDevice::ICursorUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch) {
        texelStrideInBytes = 32 * sizeof(uint32_t);
        texels = static_cast<CGxDevice*>(userArg)->m_cursor;
    }
}

// VERIFY: 81324e32 auto
// OFFSET: 0x00683660
void CGxDevice::CursorSetDepth(float depth) {
    this->m_cursorDepth = depth;
}

// VERIFY: c79f0867 manual
// OFFSET: 0x00683EA0
void CGxDevice::ClampRectLeft(CiRect& windowRect) {
    if (windowRect.maxX - windowRect.minX - s_clientAdjust.x < kWinMinSize.x) {
        windowRect.minX = windowRect.maxX - s_clientAdjust.x - kWinMinSize.x;
    }
}

// VERIFY: c52fe6ce auto
// OFFSET: 0x00683ED0
void CGxDevice::ClampRectRight(CiRect& windowRect) {
    if (windowRect.maxX - windowRect.minX - s_clientAdjust.x < kWinMinSize.x) {
        windowRect.maxX = kWinMinSize.x + s_clientAdjust.x + windowRect.minX;
    }
}

// VERIFY: 641be1e3 manual
// OFFSET: 0x00683F00
void CGxDevice::ClampRectTop(CiRect& windowRect) {
    if (windowRect.maxY - windowRect.minY - s_clientAdjust.y < kWinMinSize.y) {
        windowRect.minY = windowRect.maxY - s_clientAdjust.y - kWinMinSize.y;
    }
}

// VERIFY: 4d3cc8da auto
// OFFSET: 0x00683F30
void CGxDevice::ClampRectBottom(CiRect& windowRect) {
    if (windowRect.maxY - windowRect.minY - s_clientAdjust.y < kWinMinSize.y) {
        windowRect.maxY = kWinMinSize.y + s_clientAdjust.y + windowRect.minY;
    }
}

// VERIFY: 0b022779 auto
// OFFSET: 0x006841B0
void CGxDevice::DeviceGamma(CGxGammaRamp& ramp) {
    ramp = this->m_systemGammaRamp;
}

// VERIFY: 17f58e3d manual
// OFFSET: 0x006841D0
void CGxDevice::DeviceScreenShot() {
    this->m_scrShotWidth = static_cast<uint32_t>(this->m_curWindowRect.maxX);
    this->m_scrShotHeight = static_cast<uint32_t>(this->m_curWindowRect.maxY);

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = this->m_scrShotHeight;
    rect.maxX = this->m_scrShotWidth;
    this->DeviceReadPixels(rect, this->m_scrShotPixels);
}

// VERIFY: 5f171551 auto
// OFFSET: 0x00684360
void CGxDevice::DeviceSetDefWindow(const CRect& rect) {
    this->m_defWindowRect = rect;
    this->m_curWindowRect = rect;
}

// VERIFY: f4bada04 manual
// OFFSET: 0x00684440
void CGxDevice::ClipPlaneSet(uint32_t index, const C4Plane& plane) {
    C4Plane& current = this->m_clipPlane[index];

    if (plane.n.x != current.n.x || plane.n.y != current.n.y || plane.n.z != current.n.z || plane.d != current.d) {
        this->m_clipPlaneMask |= 1 << index;
        current.n.x = plane.n.x;
        current.n.y = plane.n.y;
        current.n.z = plane.n.z;
        current.d = plane.d;
    }
}

// VERIFY: dba46566 auto
// OFFSET: 0x006844C0
void CGxDevice::PrimVertexFormat(CGxBuf* buf, const CGxVertexAttrib* attribs, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        const CGxVertexAttrib& attrib = attribs[i];
        CGxVertexAttrib& current = this->m_primVertexFormatAttrib[attrib.m_attrib];

        if (buf->unk_001E || this->m_primVertexFormatBuf[attrib.m_attrib] != buf || ((current.m_offset - attrib.m_offset) | (current.m_type - attrib.m_type) | (current.m_bufSize - attrib.m_bufSize))) {
            this->m_primVertexDirty |= 1 << attrib.m_attrib;
        }

        this->m_primVertexFormatBuf[attrib.m_attrib] = buf;
        current = attrib;
    }

    buf->unk_001E = 0;
    this->m_primVertexFormat = GxVertexBufferFormats_Last;
}

// VERIFY: 73552709 manual
// OFFSET: 0x006847D0
void CGxDevice::LightSet(uint32_t index, const CGxLight& light, const C3Vector& pos) {
    CGxApiLight& apiLight = this->m_lights[index];
    apiLight = light;

    if ((light.m_flags & 0x2) && (pos.x != 0.0f || pos.y != 0.0f || pos.z != 0.0f)) {
        apiLight.m_flags |= 0x2;
        apiLight.m_dir.x = apiLight.m_dir.x - pos.x;
        apiLight.m_dir.y = apiLight.m_dir.y - pos.y;
        apiLight.m_dir.z = apiLight.m_dir.z - pos.z;
    }
}

// VERIFY: 97a9c89d manual
// OFFSET: 0x00684850
CGxBuf* CGxDevice::BufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount) {
    CGxBuf* buf = this->m_streamBufs[target];
    CGxPool* pool = buf->m_pool;

    if (pool && static_cast<uint32_t>(pool->m_size) < itemCount * itemSize) {
        this->PoolSizeSet(pool, itemCount * itemSize);
    }

    buf->m_itemSize = itemSize;
    buf->m_size = itemCount * itemSize;
    buf->m_itemCount = itemCount;
    buf->unk_001C = 0;
    return buf;
}

// VERIFY: 06465d73 auto
// OFFSET: 0x006848A0
void CGxDevice::TexMarkForUpdate(CGxTex* texId, const CiRect& updateRect, int32_t immediate) {
    texId->m_needsUpdate = 1;

    if (updateRect.minY >= updateRect.maxY || updateRect.minX >= updateRect.maxX) {
        texId->m_updateRect.minY = 0;
        texId->m_updateRect.minX = 0;
        texId->m_updateRect.maxY = texId->m_height;
        texId->m_updateRect.maxX = texId->m_width;
    } else {
        texId->m_updateRect = updateRect;
    }

    if (immediate) {
        this->ITexMarkAsUpdated(texId);
    }
}

// VERIFY: a4937c7c auto
// OFFSET: 0x00684930
void CGxDevice::TexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg) {
    texId->m_userFunc = userFunc;
    texId->m_userArg = userArg;
    texId->m_updateRect.minY = 0;
    texId->m_updateRect.minX = 0;
    texId->m_updateRect.maxY = texId->m_height;
    texId->m_updateRect.maxX = texId->m_width;
    texId->m_needsUpdate = 1;
}

// VERIFY: 6017c596 manual
// OFFSET: 0x00684AD0
void CGxDevice::ICursorCreate(const CGxFormat& format) {
    this->m_hwCursor = format.m_hwCursor && this->m_caps.m_hwCursor;

    if (!this->m_hwCursor && !this->m_cursorTexture) {
        CGxTexFlags flags;
        flags.m_filter = GxTex_Linear;
        flags.m_wrapU = 0;
        flags.m_wrapV = 0;
        flags.m_forceMipTracking = 0;
        flags.m_generateMipMaps = 0;
        flags.m_renderTarget = 0;
        flags.m_maxAnisotropy = 1;
        GxTexCreate(32, 32, GxTex_Argb8888, flags, this, CGxDevice::ICursorUpdate, this->m_cursorTexture);
    }
}

// VERIFY: 4216d1f6 auto
// OFFSET: 0x00684B50
void CGxDevice::CursorUnlock(uint32_t x, uint32_t y) {
    this->m_cursorHotspotY = y;
    this->m_cursorHotspotX = x;

    if (this->m_cursorTexture) {
        CiRect rect;
        rect.maxY = 32;
        rect.maxX = 32;
        GxTexUpdate(this->m_cursorTexture, rect, 0);
    }
}

// VERIFY: efed35aa manual
// OFFSET: 0x00685C60
int32_t CGxDevice::TexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId) {
    texId = new (SMemAlloc(sizeof(CGxTex), __FILE__, __LINE__, 0)) CGxTex(target, width, height, depth, format, dataFormat, flags, userArg, userFunc, name);
    this->m_textures.LinkToHead(texId);
    return 1;
}

// VERIFY: ff832600 manual
// OFFSET: 0x00687980
void CGxDevice::TexDestroy(CGxTex* texId) {
    texId->m_link.Unlink();
    delete texId;
}

// VERIFY: 87fe3127 manual
// OFFSET: 0x00688290
void CGxDevice::BufDestroy(CGxBuf*& buf) {
    if (!buf) {
        return;
    }

    if (buf->m_pool) {
        buf->m_pool->UnlinkBuf(buf);
    }

    buf->m_link.Unlink();
    delete buf;
    buf = nullptr;
}

// VERIFY: e4c51956 auto
// OFFSET: 0x00682EE0
void CGxDevice::PrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format) {
    this->m_primVertexFormat = format;
    this->m_primVertexBuf = buf;
    this->m_primVertexSize = Buffer::s_vertexBufDesc[format].size;
}
