#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00A2D57C
static const uint32_t s_texelSize[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 4, 4, 2, 4, 4, 4 };

// STATIC: 0x00A2D5B0
static const int32_t s_vertexAttribOffset[GxVertexBufferFormats_Last][GxVAs_Last] = {
    { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, 24, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, 28, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, 24, 32, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, 28, 36, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, 16, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, 16, 24, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, -1, -1, 12, 20, -1, -1, -1, -1, -1, -1 },
    { 0, 12, 16, 20, -1, -1, 32, 40, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, 28, 32, 40, -1, -1, -1, -1, -1, -1 }
};

// STATIC: 0x00AD8AF8
static BlitFormat s_blitFormats[GxTexFormats_Last] = {
    BlitFormat_Unknown, BlitFormat_Abgr8888, BlitFormat_Argb8888, BlitFormat_Argb4444, BlitFormat_Argb1555,
    BlitFormat_Rgb565, BlitFormat_Dxt1, BlitFormat_Dxt3, BlitFormat_Dxt5, BlitFormat_Uv88, BlitFormat_Gr1616F,
    BlitFormat_R32F, BlitFormat_D24X8
};

// STATIC: 0x00AD8AC0
static uint32_t s_vertexSize[GxVertexBufferFormats_Last] = { 12, 24, 28, 32, 36, 40, 44, 16, 24, 32, 20, 28, 48, 48 };

// STATIC: 0x00C5DF74
static uint32_t s_maxFPSBk;

// STATIC: 0x00C5DF78
static EGxPrim s_lockPrimType;

// STATIC: 0x00C5DF7C
static uint32_t s_maxFPS;

// STATIC: 0x00C5DF80
static uint32_t s_lockIndexCount;

// STATIC: 0x00C5DF84
static uint32_t s_lockVertexCount;

// VERIFY: cef2b995 auto
// OFFSET: 0x006811D0
int32_t GxApiSupported(EGxApi api) {
    switch (api) {
        case GxApi_OpenGl:
        case GxApi_D3d9:
        case GxApi_D3d9Ex:
            return 1;

        default:
            return 0;
    }
}

// VERIFY: 619cba02 auto
// OFFSET: 0x00681230
uint32_t GxVertexSize(EGxVertexBufferFormat format) {
    return s_vertexSize[format];
}

// VERIFY: d108cfe2 auto
// OFFSET: 0x00681240
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib) {
    return s_vertexAttribOffset[format][attrib];
}

// VERIFY: 930fa88b manual
// OFFSET: 0x006815F0
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width) {
    if (format < GxTex_Dxt1 || format > GxTex_Dxt5) {
        return s_texelSize[format] * width;
    }

    return s_texelSize[format] * ((width + 3) & ~3u);
}

// VERIFY: 1ff77454 manual
// OFFSET: 0x00408030
void GxXformSetProjection(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetProjection(matrix);
}

// VERIFY: d15ab073 manual
// OFFSET: 0x00408050
void GxXformSetView(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetView(matrix);
}

// VERIFY: fb2b6e6c auto
// OFFSET: 0x00408070
void GxXformViewport(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
    minX = g_theGxDevicePtr->m_viewport.m_x.m_l;
    maxX = g_theGxDevicePtr->m_viewport.m_x.m_h;
    minY = g_theGxDevicePtr->m_viewport.m_y.m_l;
    maxY = g_theGxDevicePtr->m_viewport.m_y.m_h;
    minZ = g_theGxDevicePtr->m_viewport.m_z.m_l;
    maxZ = g_theGxDevicePtr->m_viewport.m_z.m_h;
}

// VERIFY: 346b4596 auto
// OFFSET: 0x004080E0
void GxXformView(C44Matrix& matrix) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[GxXform_View];
    matrix = stack.m_mtx[stack.m_level];
}

// VERIFY: 30d43d0d manual
// OFFSET: 0x00408110
void GxXformViewProjNativeTranspose(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    C44Matrix proj;

    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    proj = device->m_projNative;

    if (device->DeviceApi() == GxApi_OpenGl) {
        proj.c0 = proj.c0 * -1.0f;
        proj.c1 = proj.c1 * -1.0f;
        proj.c2 = proj.c2 * -1.0f;
        proj.c3 = -1.0f * proj.c3;
    }

    matrix = (view * proj).Transpose();
}

// VERIFY: 45fd1052 manual
// OFFSET: 0x00408210
void GxShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    if (constants) {
        g_theGxDevicePtr->ShaderConstantsSet(target, index, constants, count);
    }
}

// VERIFY: 2d7b5b76 manual
// OFFSET: 0x004829D0
char* GxBufLock(CGxBuf* buf) {
    return g_theGxDevicePtr->BufLock(buf);
}

// VERIFY: d2aedfb6 manual
// OFFSET: 0x004829F0
void GxBufUnlock(CGxBuf* buf, uint32_t size) {
    g_theGxDevicePtr->BufUnlock(buf, size);
    buf->unk_001C = 1;
}

// VERIFY: 13b86081 manual
// OFFSET: 0x00482A40
void GxDraw(CGxBatch* batch, int32_t count) {
    g_theGxDevicePtr->Draw(batch, count);
}

// VERIFY: 24d4e729 manual
// OFFSET: 0x00493BF0
void GxCapsWindowSizeInScreenCoords(CRect& rect) {
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(rect);
}

// VERIFY: d8579ed1 auto
// OFFSET: 0x004BEC70
void GxXformProjection(C44Matrix& matrix) {
    matrix = g_theGxDevicePtr->m_projection;
}

// VERIFY: 8bf3caad auto
// OFFSET: 0x0057C400
void GxXformPush(EGxXform xf) {
    g_theGxDevicePtr->XformPush(xf);
}

// VERIFY: 42304661 manual
// OFFSET: 0x0057C420
void GxXformPop(EGxXform xf) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;

    if (stack.m_level) {
        stack.m_level--;
    }
}

// VERIFY: 25db6ccb manual
// OFFSET: 0x0057C4A0
void GxXformTranslate(EGxXform xf, const C3Vector& move) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level].Translate(move);
}

// VERIFY: b3c95c9e manual
// OFFSET: 0x0057C4F0
void GxXformScale(EGxXform xf, const C3Vector& scale) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level].Scale(scale);
}

// VERIFY: cd7c4ef1 manual
// OFFSET: 0x00408010
int32_t GxHasContext() {
    return g_theGxDevicePtr ? g_theGxDevicePtr->CapsHasContext(-1) : 0;
}

// VERIFY: bd3d99bd manual
// OFFSET: 0x00407FF0
int32_t GxIsWindowVisible() {
    return g_theGxDevicePtr ? g_theGxDevicePtr->CapsIsWindowVisible(-1) : 0;
}

// VERIFY: 4ed7a557 auto
// OFFSET: 0x00616AF0
int32_t GxMasterEnable(EGxMasterEnables state) {
    return g_theGxDevicePtr->MasterEnable(state);
}

// VERIFY: 480127f6 manual
// OFFSET: 0x006813B0
void GxSceneClear(uint32_t mask, CImVector color) {
    g_theGxDevicePtr->SceneClear(mask, color);
}

// VERIFY: 061b1844 manual
// OFFSET: 0x00681470
void GxTexDestroy(CGxTex* texId) {
    g_theGxDevicePtr->TexDestroy(texId);
}

// VERIFY: 4926e588 manual
// OFFSET: 0x006814D0
int32_t GxTexCopy(CGxTex* dest, CGxTex* source, const C2iVector& pos, const C2iVector& size, uint32_t level, uint32_t plane) {
    if (dest && static_cast<int32_t>(dest->m_flags.m_fFlags) < 0 && (dest->m_target != GxTex_CubeMap || plane < 6)) {
        return g_theGxDevicePtr->TexCopy(dest, source, pos, size, level, plane);
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    return 0;
}

// VERIFY: 43084cdc auto
// OFFSET: 0x006815C0
int32_t GxTexReusable(CGxTexParms& parms) {
    return parms.m_depth == 0 && (parms.m_flags.m_fFlags & 0xC0) == 0 && parms.m_target == GxTex_2d && parms.m_format <= GxTex_Dxt5;
}

// VERIFY: 01ea0eb4 manual
// OFFSET: 0x00681630
void GxQueryCreate(CGxQuery*& query, EGxQueryType type) {
    if (type < 1) {
        g_theGxDevicePtr->QueryCreate(query, type);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }
}

// VERIFY: be57cc7e manual
// OFFSET: 0x00681660
void GxQueryDestroy(CGxQuery*& query) {
    if (query) {
        g_theGxDevicePtr->QueryDestroy(query);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }
}

// VERIFY: 955d4344 auto
// OFFSET: 0x00681770
void GxSetMaxFPS(uint32_t maxFPS) {
    s_maxFPS = maxFPS;
}

// VERIFY: b5658956 auto
// OFFSET: 0x00681780
uint32_t GxGetMaxFPS() {
    return s_maxFPS;
}

// VERIFY: 07048e1b auto
// OFFSET: 0x00681790
void GxSetMaxFPSBk(uint32_t maxFPSBk) {
    s_maxFPSBk = maxFPSBk;
}

// VERIFY: 0d2f3abb auto
// OFFSET: 0x006817A0
uint32_t GxGetMaxFPSBk() {
    return s_maxFPSBk;
}

// VERIFY: 80231ea8 auto
// OFFSET: 0x006817B0
BlitFormat GxGetBlitFormat(EGxTexFormat format) {
    return s_blitFormats[format];
}

// VERIFY: 08ab74c6 manual
// OFFSET: 0x00681A20
void GxBufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
    if (!size) {
        size = buf->m_itemSize * buf->m_itemCount;
    }

    g_theGxDevicePtr->BufData(buf, data, size, offset);
    buf->unk_001C = 1;
}

// VERIFY: 9c35593e manual
// OFFSET: 0x00681A60
void GxDrawLockedElements() {
    if (!s_lockVertexCount) {
        return;
    }

    CGxBatch batch;
    batch.m_primType = s_lockPrimType;
    batch.m_start = 0;
    batch.m_count = s_lockIndexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(s_lockVertexCount - 1);
    g_theGxDevicePtr->Draw(&batch, 1);
}

// VERIFY: 2b10a3f5 manual
// OFFSET: 0x00682130
void GxXformViewProj(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    C44Matrix proj;

    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    proj = device->m_projection;
    matrix = view * proj;
}
