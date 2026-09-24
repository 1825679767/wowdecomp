#include <cstdint>
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"

// STATIC: 0x00AD8B4C
static uint32_t s_primVertsPerPrim[GxPrims_Last] = { 1, 2, 1, 3, 1, 1 };

// STATIC: 0x00AD8B64
static uint32_t s_primVertsExtra[GxPrims_Last] = { 0, 0, 1, 0, 2, 2 };

// STATIC: 0x00AD8BC0
static C2iVector kWinMinSize = { 320, 240 };

// STATIC: 0x00C5DF88
CGxDevice* g_theGxDevicePtr;

// STATIC: 0x00C5FFF0
static float kWinAspect;

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
