#include <cstdint>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Storm/H/storm.h"

typedef IDirect3D9* (WINAPI* Direct3DCreate9Proc)(UINT sdkVersion);

// STATIC: 0x00A2E4C8
const uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribSize[7] = { 4, 4, 4, 8, 12, 4, 4 };

// STATIC: 0x00A2E4E4
static const D3DQUERYTYPE s_d3dQueryTypes[1] = { D3DQUERYTYPE_OCCLUSION };

// STATIC: 0x00A2F8EC
const D3DDECLUSAGE CGxDeviceD3d::s_gxAttribToD3dAttribUsage[GxVAs_Last] = { D3DDECLUSAGE_POSITION, D3DDECLUSAGE_BLENDWEIGHT, D3DDECLUSAGE_BLENDINDICES, D3DDECLUSAGE_NORMAL, D3DDECLUSAGE_COLOR, D3DDECLUSAGE_COLOR, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD };

// STATIC: 0x00A2F924
const uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribUsageIndex[GxVAs_Last] = { 0, 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 5, 6, 7 };

// STATIC: 0x00A2F964
const D3DBLEND CGxDeviceD3d::s_srcBlend[12] = { D3DBLEND_ONE, D3DBLEND_ONE, D3DBLEND_SRCALPHA, D3DBLEND_SRCALPHA, D3DBLEND_DESTCOLOR, D3DBLEND_DESTCOLOR, D3DBLEND_DESTCOLOR, D3DBLEND_INVSRCALPHA, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA, D3DBLEND_ONE, D3DBLEND_BLENDFACTOR };

// STATIC: 0x00A2F994
const D3DBLEND CGxDeviceD3d::s_dstBlend[12] = { D3DBLEND_ZERO, D3DBLEND_ZERO, D3DBLEND_INVSRCALPHA, D3DBLEND_ONE, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR, D3DBLEND_ONE, D3DBLEND_ONE, D3DBLEND_ZERO, D3DBLEND_ZERO, D3DBLEND_ONE, D3DBLEND_INVBLENDFACTOR };

// STATIC: 0x00A2F9C4
const D3DTEXTUREADDRESS CGxDeviceD3d::s_wrapModes[2] = { D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP };

// STATIC: 0x00A2F9CC
static const D3DTEXTUREOP s_texOp[6] = { D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_ADD, D3DTOP_SELECTARG2, D3DTOP_SUBTRACT, D3DTOP_BLENDDIFFUSEALPHA };

// STATIC: 0x00A2F9E4
static const uint32_t s_texArgs[6][2] = { { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_CURRENT, D3DTA_TEXTURE }, { D3DTA_TEXTURE, D3DTA_CURRENT } };

// STATIC: 0x00A2FA14
const D3DCMPFUNC CGxDeviceD3d::s_cmpFunc[4] = { D3DCMP_LESSEQUAL, D3DCMP_EQUAL, D3DCMP_GREATEREQUAL, D3DCMP_LESS };

// STATIC: 0x00A2FA24
const D3DCULL CGxDeviceD3d::s_cullMode[3] = { D3DCULL_NONE, D3DCULL_CW, D3DCULL_CCW };

// STATIC: 0x00AD8F24
D3DDECLTYPE CGxDeviceD3d::s_gxAttribToD3dAttribType[7] = { D3DDECLTYPE_D3DCOLOR, D3DDECLTYPE_UBYTE4, D3DDECLTYPE_UBYTE4N, D3DDECLTYPE_FLOAT2, D3DDECLTYPE_FLOAT3, D3DDECLTYPE_SHORT2, D3DDECLTYPE_FLOAT1 };

// STATIC: 0x00AD8F40
D3DTEXTUREFILTERTYPE CGxDeviceD3d::s_filterModes[GxTexFilters_Last][3] = { { D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE }, { D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR } };

// STATIC: 0x00AD8F88
static uint32_t s_normalizeNormals = 5;

// STATIC: 0x00C606B0
static D3DMATERIAL9 s_d3dMaterial;

// STATIC: 0x00C606F8
static D3DLIGHT9 s_d3dLight;

// STATIC: 0x00C60760
static int32_t s_isIdent;

// VERIFY: c3d9f239 auto
// OFFSET: 0x0068FD50
CGxDeviceD3d::CGxDeviceD3d() : m_desktopDisplayMode() {
    this->m_d3dStereoConvergence = 1.0f;
    this->m_d3dStereoSeparation = 25.0f;
    this->m_api = GxApi_D3d9;
    this->m_hwnd = nullptr;
    this->m_d3dLib = nullptr;
    this->m_d3d = nullptr;
    this->m_d3dDevice = nullptr;
    this->m_d3dIsHwDevice = 0;
    this->m_d3dNVAPI = 0;
    this->m_d3dStereoEnabled = 0;
    this->m_d3dStereoRestore = 0;
    this->m_d3dStereoHandle = 0;
    this->m_d3dStereoDirty = 0;
    this->m_caps.m_colorFormat = GxCF_argb;
    this->m_caps.unk_0018 = 1;
    memset(this->m_deviceStates, 0xFF, sizeof(this->m_deviceStates));
    memset(this->m_texEnabled, 0, sizeof(this->m_texEnabled));
    memset(this->m_d3dVertexDecl, 0, sizeof(this->m_d3dVertexDecl));
    this->m_defDepthStencilSurface = nullptr;
    this->m_defColorSurface = nullptr;
    this->m_defDepthSurface = nullptr;
    this->m_surface3B44 = nullptr;
    this->m_eventQuery = nullptr;
    this->m_hwCursorDirty = 1;
    this->m_hwCursorTexture = nullptr;
    this->m_hwCursorBitmap = nullptr;
    this->m_texture3B58 = nullptr;
    this->DeviceCreatePools();
    this->DeviceCreateStreamBufs();
}

// VERIFY: 57303b58 auto
// OFFSET: 0x0068E140
void CGxDeviceD3d::IUnloadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    if (d3d) {
        d3d->Release();
        d3d = nullptr;
    }

    if (d3dLib) {
        FreeLibrary(d3dLib);
        d3dLib = nullptr;
    }
}

// VERIFY: 211a94b3 manual
// OFFSET: 0x0068E180
LPDIRECT3DINDEXBUFFER9 CGxDeviceD3d::ICreateD3dIB(EGxPoolUsage usage, uint32_t size) {
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    DWORD d3dUsage = D3DUSAGE_WRITEONLY | (this->m_d3dIsHwDevice ? 0 : D3DUSAGE_SOFTWAREPROCESSING);

    if (usage == GxPoolUsage_Stream || usage == GxPoolUsage_Dynamic) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DINDEXBUFFER9 indexBuffer = nullptr;
    HRESULT hr = this->m_d3dDevice->CreateIndexBuffer(size, d3dUsage, D3DFMT_INDEX16, d3dPool, &indexBuffer, nullptr);

    return SUCCEEDED(hr) ? indexBuffer : nullptr;
}

// VERIFY: 72eca02b auto
// OFFSET: 0x0068E1F0
void CGxDeviceD3d::IReleaseD3dPool(CGxPool* pool) {
    if (pool->m_usage == GxPoolUsage_Stream) {
        pool->unk_001C = 0;
    }

    pool->Invalidate();

    if (pool->m_apiSpecific) {
        switch (pool->m_target) {
            case GxPoolTarget_Vertex:
                static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific)->Release();
                break;

            case GxPoolTarget_Index:
                static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific)->Release();
                break;

            default:
                break;
        }

        pool->m_apiSpecific = nullptr;
    }
}

// VERIFY: e9919487 manual
// OFFSET: 0x0068E450
void CGxDeviceD3d::DeviceEvictResources() {
    if (!this->m_context || !this->m_d3dDevice) {
        return;
    }

    HRESULT hr = this->m_d3dDevice->EvictManagedResources();

    if (FAILED(hr) && hr == D3DERR_DRIVERINTERNALERROR) {
        SErrDisplayError(STORM_ERROR_FATAL_CONDITION, __FILE__, __LINE__, "Driver internal error on EvictManagedResources().", 0, 1, 0x11111111);
    }
}

// VERIFY: a0f60ba9 auto
// OFFSET: 0x0068E4C0
void CGxDeviceD3d::DeviceSetGamma(float gamma) {
    CGxDevice::DeviceSetGamma(gamma);

    if ((this->m_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !this->IDevIsWindowed()) {
        this->m_d3dDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, reinterpret_cast<const D3DGAMMARAMP*>(&this->m_gammaRamp));
    }
}

// VERIFY: e62cb4c1 auto
// OFFSET: 0x0068E720
void CGxDeviceD3d::PoolDestroy(CGxPool* pool) {
    if (pool) {
        this->IReleaseD3dPool(pool);
        CGxDevice::PoolDestroy(pool);
    }
}

// VERIFY: b07d4055 auto
// OFFSET: 0x0068E750
void CGxDeviceD3d::CursorSetVisible(int32_t visible) {
    CGxDevice::CursorSetVisible(visible);

    if (this->m_hwCursor && this->m_context) {
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(this->m_hwnd, &point);

        RECT rect;
        GetClientRect(this->m_hwnd, &rect);

        if (point.x >= rect.left && point.x < rect.right && point.y >= rect.top && point.y < rect.bottom) {
            this->m_d3dDevice->ShowCursor(this->m_cursorVisible);
        }
    }
}

// VERIFY: 9a0772fe auto
// OFFSET: 0x0068E7E0
void CGxDeviceD3d::CursorUnlock(uint32_t x, uint32_t y) {
    CGxDevice::CursorUnlock(x, y);
    this->m_hwCursorDirty = 1;
}

// VERIFY: 79dfb7f5 auto
// OFFSET: 0x0068E900
void CGxDeviceD3d::ICursorCreate(const CGxFormat& format) {
    CGxDevice::ICursorCreate(format);

    if (this->m_hwCursor && !this->m_hwCursorTexture) {
        this->m_d3dDevice->CreateTexture(32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &this->m_hwCursorTexture, nullptr);

        if (this->m_hwCursorTexture) {
            this->m_hwCursorTexture->GetSurfaceLevel(0, &this->m_hwCursorBitmap);
        }

        this->m_hwCursorDirty = 1;
        this->ICursorDraw();
    }
}

// VERIFY: 8d560b80 auto
// OFFSET: 0x0068E980
void CGxDeviceD3d::StereoSetConvergence(float convergence) {
    this->m_d3dStereoDirty = 1;
    this->m_d3dStereoConvergence = convergence;
}

// VERIFY: ef5be528 auto
// OFFSET: 0x0068E9A0
void CGxDeviceD3d::StereoSetSeparation(float separation) {
    this->m_d3dStereoDirty = 1;
    this->m_d3dStereoSeparation = separation;
}

// VERIFY: 296eb823 auto
// OFFSET: 0x0068E9C0
void CGxDeviceD3d::QueryCreate(CGxQuery*& query, EGxQueryType type) {
    CGxDevice::QueryCreate(query, type);

    CGxQuery* gxQuery = query;
    LPDIRECT3DQUERY9 d3dQuery;

    if (!gxQuery->m_apiSpecific && this->m_d3dDevice->CreateQuery(s_d3dQueryTypes[gxQuery->m_type], &d3dQuery) == D3D_OK) {
        gxQuery->m_apiSpecific = d3dQuery;
    }
}

// VERIFY: 04d8fee3 auto
// OFFSET: 0x0068EA10
bool CGxDeviceD3d::QueryBegin(CGxQuery* query) {
    if (!this->m_context) {
        return false;
    }

    LPDIRECT3DQUERY9 d3dQuery;

    if (!query->m_apiSpecific) {
        if (this->m_d3dDevice->CreateQuery(s_d3dQueryTypes[query->m_type], &d3dQuery) == D3D_OK) {
            query->m_apiSpecific = d3dQuery;
        }

        if (!query->m_apiSpecific) {
            return false;
        }
    }

    HRESULT hr = D3D_OK;

    if (query->m_type == GxQueryType_Occlusion) {
        hr = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->Issue(D3DISSUE_BEGIN);
    }

    return hr == D3D_OK;
}

// VERIFY: d92bc702 auto
// OFFSET: 0x0068EA90
bool CGxDeviceD3d::QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t& data) {
    if (!this->m_context) {
        return false;
    }

    LPDIRECT3DQUERY9 d3dQuery;

    if (!query->m_apiSpecific) {
        if (this->m_d3dDevice->CreateQuery(s_d3dQueryTypes[query->m_type], &d3dQuery) == D3D_OK) {
            query->m_apiSpecific = d3dQuery;
        }

        if (!query->m_apiSpecific) {
            return false;
        }
    }

    switch (param) {
        case GxQueryParam_Unk0:
            data = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->GetDataSize();
            return true;

        case GxQueryParam_Unk1:
            data = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->GetData(nullptr, 0, 0) == D3D_OK;
            return false;

        default:
            return false;
    }
}

// VERIFY: 012b93fa manual
// OFFSET: 0x0068ED80
int32_t CGxDeviceD3d::ILoadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    d3dLib = nullptr;
    d3d = nullptr;
    d3dLib = LoadLibraryA("d3d9.dll");

    if (d3dLib) {
        Direct3DCreate9Proc d3dCreateProc = reinterpret_cast<Direct3DCreate9Proc>(GetProcAddress(d3dLib, "Direct3DCreate9"));

        if (d3dCreateProc) {
            d3d = d3dCreateProc(D3D_SDK_VERSION);

            if (d3d) {
                return 1;
            }

            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to d3dCreateProc()");
        } else {
            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to GetProcAddress()");
        }
    } else {
        CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to LoadLibrary()");
    }

    CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);

    return 0;
}

// VERIFY: b78ec2a8 auto
// OFFSET: 0x0069FE10
void CGxDeviceD3d::ISetWindowFocus(int32_t focus) {
    this->m_windowFocus = focus;

    if (focus) {
        this->m_hwCursorDirty = 1;

        if (!this->m_format.m_window) {
            RECT rect;
            GetWindowRect(this->m_hwnd, &rect);
            ClipCursor(&rect);
        }
    }
}

// VERIFY: d0799ea2 manual
// OFFSET: 0x0069FB00
LPDIRECT3DVERTEXBUFFER9 CGxDeviceD3d::ICreateD3dVB(EGxPoolUsage usage, uint32_t size) {
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    DWORD d3dUsage = D3DUSAGE_WRITEONLY | (this->m_d3dIsHwDevice ? 0 : D3DUSAGE_SOFTWAREPROCESSING);

    if (usage == GxPoolUsage_Stream || usage == GxPoolUsage_Dynamic) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DVERTEXBUFFER9 vertexBuffer = nullptr;
    HRESULT hr = this->m_d3dDevice->CreateVertexBuffer(size, d3dUsage, 0, d3dPool, &vertexBuffer, nullptr);

    return SUCCEEDED(hr) ? vertexBuffer : nullptr;
}

// VERIFY: 2ee2e8ff auto
// OFFSET: 0x0069FE80
void CGxDeviceD3d::DeviceSetGamma(const CGxGammaRamp& ramp) {
    CGxDevice::DeviceSetGamma(ramp);

    if ((this->m_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !this->IDevIsWindowed()) {
        this->m_d3dDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, reinterpret_cast<const D3DGAMMARAMP*>(&this->m_gammaRamp));
    }
}

// VERIFY: 0ec084bf auto
// OFFSET: 0x0069FED0
void* CGxDeviceD3d::DeviceWindow() {
    return this->m_hwnd;
}

// VERIFY: 493ad387 manual
// OFFSET: 0x0069FF40
void CGxDeviceD3d::DeviceOverride(EGxOverride override, uint32_t value) {
    CGxDevice::DeviceOverride(override, value);

    if (override == GxOverride_PixelShader) {
        this->m_caps.m_shaderTargets[GxSh_Pixel] = value;
    } else if (override == GxOverride_Unk7) {
        this->m_caps.m_texNonPow2 = value != 0;
    }
}

// VERIFY: 46e713d8 auto
// OFFSET: 0x0069FF80
void CGxDeviceD3d::PoolSizeSet(CGxPool* pool, uint32_t size) {
    this->IReleaseD3dPool(pool);
    pool->m_size = size;

    switch (pool->m_target) {
        case GxPoolTarget_Vertex:
            pool->m_apiSpecific = this->ICreateD3dVB(pool->m_usage, size);
            break;

        case GxPoolTarget_Index:
            pool->m_apiSpecific = this->ICreateD3dIB(pool->m_usage, size);
            break;

        default:
            break;
    }
}

// VERIFY: 70899344 auto
// OFFSET: 0x006A00C0
void CGxDeviceD3d::ICursorDestroy() {
    CGxDevice::ICursorDestroy();

    if (this->m_hwCursorBitmap) {
        this->m_hwCursorBitmap->Release();
        this->m_hwCursorBitmap = nullptr;
    }

    if (this->m_hwCursorTexture) {
        this->m_hwCursorTexture->Release();
        this->m_hwCursorTexture = nullptr;
    }
}

// VERIFY: ffea8e5a auto
// OFFSET: 0x006A0110
float CGxDeviceD3d::StereoGetConvergence() {
    return this->m_d3dStereoConvergence;
}

// VERIFY: 5cbe2012 auto
// OFFSET: 0x006A0120
float CGxDeviceD3d::StereoGetSeparation() {
    return this->m_d3dStereoSeparation;
}

// VERIFY: b7985cf0 auto
// OFFSET: 0x006A0130
bool CGxDeviceD3d::StereoEnabled() {
    return this->m_d3dStereoEnabled == 1;
}

// VERIFY: 54f891dc auto
// OFFSET: 0x006A0190
void CGxDeviceD3d::QueryDestroy(CGxQuery*& query) {
    CGxQuery* gxQuery = query;

    if (gxQuery->m_apiSpecific) {
        static_cast<LPDIRECT3DQUERY9>(gxQuery->m_apiSpecific)->Release();
    }

    gxQuery->m_apiSpecific = nullptr;
    CGxDevice::QueryDestroy(query);
}

// VERIFY: 57374da8 auto
// OFFSET: 0x006A0240
bool CGxDeviceD3d::QueryEnd(CGxQuery* query) {
    if (!this->m_context || !query->m_apiSpecific) {
        return false;
    }

    HRESULT hr = D3D_OK;

    if (query->m_type == GxQueryType_Occlusion) {
        hr = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->Issue(D3DISSUE_END);
    }

    return hr == D3D_OK;
}

// VERIFY: 5708f36a manual
// OFFSET: 0x006A0310
bool CGxDeviceD3d::QueryGetData(CGxQuery* query, uint32_t& data) {
    if (!this->m_context || !query->m_apiSpecific) {
        return false;
    }

    LPDIRECT3DQUERY9 d3dQuery = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific);
    HRESULT hr;

    do {
        hr = d3dQuery->GetData(&data, d3dQuery->GetDataSize(), D3DGETDATA_FLUSH);
    } while (hr == S_FALSE);

    return hr == D3D_OK;
}

// VERIFY: f57c2b90 auto
// OFFSET: 0x006A3420
void CGxDeviceD3d::ISceneEnd() {
    if (this->m_inScene) {
        this->m_d3dDevice->EndScene();
        this->m_inScene = 0;
    }
}

// VERIFY: d0242deb auto
// OFFSET: 0x006A5A00
void CGxDeviceD3d::CapsWindowSize(CRect& rect) {
    rect = this->DeviceCurWindow();
}

// VERIFY: c58c4596 manual
// OFFSET: 0x006A5E10
void CGxDeviceD3d::IShaderCreate(CGxShader* shader) {
    if (shader->m_target == GxSh_Vertex) {
        this->IShaderCreateVertex(shader);
    } else if (shader->m_target == GxSh_Pixel) {
        this->IShaderCreatePixel(shader);
    }
}

// VERIFY: f717b755 auto
// OFFSET: 0x006A9920
void CGxDeviceD3d::CapsWindowSizeInScreenCoords(CRect& rect) {
    if (this->IDevIsWindowed()) {
        const CRect& window = this->DeviceCurWindow();
        POINT points[2];
        points[0].x = 0;
        points[0].y = 0;
        points[1].x = static_cast<LONG>(window.maxX);
        points[1].y = static_cast<LONG>(window.maxY);
        MapWindowPoints(this->m_hwnd, nullptr, points, 2);
        rect.minY = static_cast<float>(points[0].y);
        rect.minX = static_cast<float>(points[0].x);
        rect.maxY = static_cast<float>(points[1].y);
        rect.maxX = static_cast<float>(points[1].x);
    } else {
        rect = this->DeviceCurWindow();
    }
}

// VERIFY: 146386db manual
// OFFSET: 0x006A9E00
void CGxDeviceD3d::XformSetView(const C44Matrix& matrix) {
    CGxDevice::XformSetView(matrix);
}

// VERIFY: 4be0f29d auto
// OFFSET: 0x006AA070
void CGxDeviceD3d::IShaderCreatePixel(CGxShader* shader) {
    shader->m_valid = 0;

    if (!this->m_context) {
        return;
    }

    shader->m_loaded = 1;

    LPDIRECT3DPIXELSHADER9 pixelShader;

    if (shader->m_code.Count() && this->m_d3dDevice->CreatePixelShader(reinterpret_cast<const DWORD*>(shader->m_code.m_data), &pixelShader) == D3D_OK) {
        shader->m_apiSpecific = pixelShader;
        shader->m_valid = 1;
    }
}

// VERIFY: 42b1d4b6 auto
// OFFSET: 0x006AA0D0
void CGxDeviceD3d::IShaderCreateVertex(CGxShader* shader) {
    shader->m_valid = 0;

    if (!this->m_context) {
        return;
    }

    shader->m_loaded = 1;

    LPDIRECT3DVERTEXSHADER9 vertexShader;

    if (shader->m_code.Count() && this->m_d3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(shader->m_code.m_data), &vertexShader) == D3D_OK) {
        shader->m_apiSpecific = vertexShader;
        shader->m_valid = 1;
    }
}

// VERIFY: 38fa5626 manual
// OFFSET: 0x006AA130
void CGxDeviceD3d::ShaderCreate(CGxShader** shaders, EGxShTarget target, const char* path, const char* name, uint32_t permutations) {
    CGxDevice::ShaderCreate(shaders, target, path, name, permutations);

    if (permutations == 1 && !shaders[0]->m_loaded) {
        if (target == GxSh_Vertex) {
            this->IShaderCreateVertex(shaders[0]);
        } else if (target == GxSh_Pixel) {
            this->IShaderCreatePixel(shaders[0]);
        }
    }
}

// VERIFY: ade44033 manual
// OFFSET: 0x006AA190
void CGxDeviceD3d::ShaderDestroy(CGxShader*& shader) {
    if (shader->m_target == GxSh_Vertex) {
        if (shader->m_refCount == 1) {
            if (shader->m_apiSpecific) {
                static_cast<LPDIRECT3DVERTEXSHADER9>(shader->m_apiSpecific)->Release();
                shader->m_apiSpecific = nullptr;
            }

            shader->m_loaded = 0;
        }
    } else if (shader->m_target == GxSh_Pixel) {
        if (shader->m_refCount == 1) {
            if (shader->m_apiSpecific) {
                static_cast<LPDIRECT3DPIXELSHADER9>(shader->m_apiSpecific)->Release();
                shader->m_apiSpecific = nullptr;
            }

            shader->m_loaded = 0;
        }
    }

    CGxDevice::ShaderDestroy(shader);
}

void CGxDeviceD3d::DeviceUnk81(uint32_t a, uint32_t b) {
}

void CGxDeviceD3d::DeviceUnk82() {
}

void CGxDeviceD3d::DeviceUnk83(uint32_t a, uint32_t b) {
}

// VERIFY: 3b44d011 manual
// OFFSET: 0x006A3720
bool CGxDeviceD3d::GxVertexDecl::Compare(const D3DVERTEXELEMENT9* elements, uint32_t count) {
    if (count != this->m_count) {
        return false;
    }

    for (uint32_t i = 0; i < this->m_count; i++) {
        if (memcmp(&this->m_elements[i], &elements[i], sizeof(D3DVERTEXELEMENT9))) {
            return false;
        }
    }

    return true;
}

// VERIFY: dc0f168f auto
// OFFSET: 0x006A3810
void CGxDeviceD3d::IStateSyncEnables() {
    if (this->m_appMasterEnables == this->m_hwMasterEnables) {
        return;
    }

    int32_t enable;

    if (this->NeedsUpdate(this->m_appMasterEnables, this->m_hwMasterEnables, 0, 0, GxMasterEnable_PolygonFill, enable)) {
        this->m_d3dDevice->SetRenderState(D3DRS_FILLMODE, enable ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
    }

    this->m_hwMasterEnables = this->m_appMasterEnables;
}

// VERIFY: 35d5837f auto
// OFFSET: 0x006A3870
void CGxDeviceD3d::IStateSyncClipPlanes() {
    if (!this->m_clipPlaneMask) {
        return;
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (this->m_clipPlaneMask & (1 << i)) {
            this->m_d3dDevice->SetClipPlane(i, reinterpret_cast<const float*>(&this->m_clipPlane[i]));
        }
    }

    this->m_clipPlaneMask = 0;
}

// VERIFY: 1e83010d manual
// OFFSET: 0x006A38D0
void CGxDeviceD3d::IStateSyncScissorRect() {
    if (!this->m_scissorRectDirty) {
        return;
    }

    const CRect& window = this->DeviceCurWindow();
    RECT rect;

    rect.left = static_cast<LONG>(this->m_scissorRect.minX * window.maxX + 0.5f);
    rect.right = static_cast<LONG>(this->m_scissorRect.maxX * window.maxX + 1.0f);

    if (this->m_textureTarget[0].m_apiSpecific || this->m_textureTarget[1].m_apiSpecific) {
        rect.top = static_cast<LONG>(this->m_scissorRect.minY * window.maxY + 0.5f);
        rect.bottom = static_cast<LONG>(1.0f + this->m_scissorRect.maxY * window.maxY);
    } else {
        rect.top = static_cast<LONG>((1.0f - this->m_scissorRect.maxY) * window.maxY + 0.5f);
        rect.bottom = static_cast<LONG>(1.0f + (1.0f - this->m_scissorRect.minY) * window.maxY);
    }

    if (rect.left < 0) {
        rect.left = 0;
    }

    if (rect.top < 0) {
        rect.top = 0;
    }

    if (rect.right > window.maxX) {
        rect.right = static_cast<LONG>(window.maxX);
    }

    if (rect.bottom > window.maxY) {
        rect.bottom = static_cast<LONG>(window.maxY);
    }

    this->m_d3dDevice->SetScissorRect(&rect);
    this->m_scissorRectDirty = 0;
}

// VERIFY: 1602f0a7 auto
// OFFSET: 0x006A39E0
void CGxDeviceD3d::ISetVertexBuffer(uint32_t stream, LPDIRECT3DVERTEXBUFFER9 buffer, uint32_t offset, uint32_t stride) {
    if (!this->m_caps.unk_0010) {
        offset = 0;
    }

    if (this->m_d3dVertexStreamBuf[stream] != buffer || this->m_d3dVertexStreamOfs[stream] != offset || this->m_d3dVertexStreamStride[stream] != stride) {
        this->m_d3dDevice->SetStreamSource(stream, buffer, offset, stride);
        this->m_d3dVertexStreamBuf[stream] = buffer;
        this->m_d3dVertexStreamOfs[stream] = offset;
        this->m_d3dVertexStreamStride[stream] = stride;
    }
}

// VERIFY: 24c85496 manual
// OFFSET: 0x006A3C40
void CGxDeviceD3d::DsSet(EDeviceState state, uint32_t value) {
    if (this->m_deviceStates[state] == value) {
        return;
    }

    switch (state) {
        case Ds_SrcBlend:
            this->m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, value);
            break;

        case Ds_DstBlend:
            this->m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, value);
            break;

        case Ds_TssMagFilter0:
        case Ds_TssMagFilter1:
        case Ds_TssMagFilter2:
        case Ds_TssMagFilter3:
        case Ds_TssMagFilter4:
        case Ds_TssMagFilter5:
        case Ds_TssMagFilter6:
        case Ds_TssMagFilter7:
        case Ds_TssMagFilter8:
        case Ds_TssMagFilter9:
        case Ds_TssMagFilter10:
        case Ds_TssMagFilter11:
        case Ds_TssMagFilter12:
        case Ds_TssMagFilter13:
        case Ds_TssMagFilter14:
        case Ds_TssMagFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMagFilter0, D3DSAMP_MAGFILTER, value);
            break;

        case Ds_TssMinFilter0:
        case Ds_TssMinFilter1:
        case Ds_TssMinFilter2:
        case Ds_TssMinFilter3:
        case Ds_TssMinFilter4:
        case Ds_TssMinFilter5:
        case Ds_TssMinFilter6:
        case Ds_TssMinFilter7:
        case Ds_TssMinFilter8:
        case Ds_TssMinFilter9:
        case Ds_TssMinFilter10:
        case Ds_TssMinFilter11:
        case Ds_TssMinFilter12:
        case Ds_TssMinFilter13:
        case Ds_TssMinFilter14:
        case Ds_TssMinFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMinFilter0, D3DSAMP_MINFILTER, value);
            break;

        case Ds_TssMipFilter0:
        case Ds_TssMipFilter1:
        case Ds_TssMipFilter2:
        case Ds_TssMipFilter3:
        case Ds_TssMipFilter4:
        case Ds_TssMipFilter5:
        case Ds_TssMipFilter6:
        case Ds_TssMipFilter7:
        case Ds_TssMipFilter8:
        case Ds_TssMipFilter9:
        case Ds_TssMipFilter10:
        case Ds_TssMipFilter11:
        case Ds_TssMipFilter12:
        case Ds_TssMipFilter13:
        case Ds_TssMipFilter14:
        case Ds_TssMipFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMipFilter0, D3DSAMP_MIPFILTER, value);
            break;

        case Ds_TssWrapU0:
        case Ds_TssWrapU1:
        case Ds_TssWrapU2:
        case Ds_TssWrapU3:
        case Ds_TssWrapU4:
        case Ds_TssWrapU5:
        case Ds_TssWrapU6:
        case Ds_TssWrapU7:
        case Ds_TssWrapU8:
        case Ds_TssWrapU9:
        case Ds_TssWrapU10:
        case Ds_TssWrapU11:
        case Ds_TssWrapU12:
        case Ds_TssWrapU13:
        case Ds_TssWrapU14:
        case Ds_TssWrapU15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssWrapU0, D3DSAMP_ADDRESSU, value);
            break;

        case Ds_TssWrapV0:
        case Ds_TssWrapV1:
        case Ds_TssWrapV2:
        case Ds_TssWrapV3:
        case Ds_TssWrapV4:
        case Ds_TssWrapV5:
        case Ds_TssWrapV6:
        case Ds_TssWrapV7:
        case Ds_TssWrapV8:
        case Ds_TssWrapV9:
        case Ds_TssWrapV10:
        case Ds_TssWrapV11:
        case Ds_TssWrapV12:
        case Ds_TssWrapV13:
        case Ds_TssWrapV14:
        case Ds_TssWrapV15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssWrapV0, D3DSAMP_ADDRESSV, value);
            break;

        case Ds_TssTexTransformFlags0:
        case Ds_TssTexTransformFlags1:
        case Ds_TssTexTransformFlags2:
        case Ds_TssTexTransformFlags3:
        case Ds_TssTexTransformFlags4:
        case Ds_TssTexTransformFlags5:
        case Ds_TssTexTransformFlags6:
        case Ds_TssTexTransformFlags7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssTexTransformFlags0, D3DTSS_TEXTURETRANSFORMFLAGS, value);
            break;

        case Ds_TssMaxAnisotropy0:
        case Ds_TssMaxAnisotropy1:
        case Ds_TssMaxAnisotropy2:
        case Ds_TssMaxAnisotropy3:
        case Ds_TssMaxAnisotropy4:
        case Ds_TssMaxAnisotropy5:
        case Ds_TssMaxAnisotropy6:
        case Ds_TssMaxAnisotropy7:
        case Ds_TssMaxAnisotropy8:
        case Ds_TssMaxAnisotropy9:
        case Ds_TssMaxAnisotropy10:
        case Ds_TssMaxAnisotropy11:
        case Ds_TssMaxAnisotropy12:
        case Ds_TssMaxAnisotropy13:
        case Ds_TssMaxAnisotropy14:
        case Ds_TssMaxAnisotropy15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMaxAnisotropy0, D3DSAMP_MAXANISOTROPY, value);
            break;

        case Ds_TssTexCoordIndex0:
        case Ds_TssTexCoordIndex1:
        case Ds_TssTexCoordIndex2:
        case Ds_TssTexCoordIndex3:
        case Ds_TssTexCoordIndex4:
        case Ds_TssTexCoordIndex5:
        case Ds_TssTexCoordIndex6:
        case Ds_TssTexCoordIndex7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssTexCoordIndex0, D3DTSS_TEXCOORDINDEX, value);
            break;

        case Ds_TssColorOp0:
        case Ds_TssColorOp1:
        case Ds_TssColorOp2:
        case Ds_TssColorOp3:
        case Ds_TssColorOp4:
        case Ds_TssColorOp5:
        case Ds_TssColorOp6:
        case Ds_TssColorOp7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorOp0, D3DTSS_COLOROP, value);
            break;

        case Ds_TssAlphaOp0:
        case Ds_TssAlphaOp1:
        case Ds_TssAlphaOp2:
        case Ds_TssAlphaOp3:
        case Ds_TssAlphaOp4:
        case Ds_TssAlphaOp5:
        case Ds_TssAlphaOp6:
        case Ds_TssAlphaOp7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaOp0, D3DTSS_ALPHAOP, value);
            break;

        case Ds_TssColorArg10:
        case Ds_TssColorArg11:
        case Ds_TssColorArg12:
        case Ds_TssColorArg13:
        case Ds_TssColorArg14:
        case Ds_TssColorArg15:
        case Ds_TssColorArg16:
        case Ds_TssColorArg17:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg10, D3DTSS_COLORARG1, value);
            break;

        case Ds_TssColorArg20:
        case Ds_TssColorArg21:
        case Ds_TssColorArg22:
        case Ds_TssColorArg23:
        case Ds_TssColorArg24:
        case Ds_TssColorArg25:
        case Ds_TssColorArg26:
        case Ds_TssColorArg27:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg20, D3DTSS_COLORARG2, value);
            break;

        case Ds_TssAlphaArg10:
        case Ds_TssAlphaArg11:
        case Ds_TssAlphaArg12:
        case Ds_TssAlphaArg13:
        case Ds_TssAlphaArg14:
        case Ds_TssAlphaArg15:
        case Ds_TssAlphaArg16:
        case Ds_TssAlphaArg17:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg10, D3DTSS_ALPHAARG1, value);
            break;

        case Ds_TssAlphaArg20:
        case Ds_TssAlphaArg21:
        case Ds_TssAlphaArg22:
        case Ds_TssAlphaArg23:
        case Ds_TssAlphaArg24:
        case Ds_TssAlphaArg25:
        case Ds_TssAlphaArg26:
        case Ds_TssAlphaArg27:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg20, D3DTSS_ALPHAARG2, value);
            break;

        case Ds_AmbientMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, value);
            break;

        case Ds_DiffuseMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, value);
            break;

        case Ds_SpecularMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, value);
            break;

        case Ds_EmissiveMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, value);
            break;

        case Ds_Ambient:
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENT, value);
            break;

        case Ds_AlphaBlendEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, value);
            break;

        case Ds_AlphaTestEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, value);
            break;

        case Ds_AlphaRef:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHAREF, value);
            break;

        case Ds_FogEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, value);
            break;

        case Ds_ZWriteEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, value);
            break;

        case Ds_ColorWriteEnable: {
            uint32_t d3dValue = (value & 0x1) != 0;

            if (value & 0x4) {
                d3dValue |= D3DCOLORWRITEENABLE_GREEN;
            }

            if (value & 0x2) {
                d3dValue |= D3DCOLORWRITEENABLE_BLUE;
            }

            if (value & 0x8) {
                d3dValue |= D3DCOLORWRITEENABLE_ALPHA;
            }

            this->m_d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, d3dValue);
            break;
        }

        case Ds_Lighting:
            this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, value);
            break;

        case Ds_SpecularEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, value);
            break;

        case Ds_CullMode:
            this->m_d3dDevice->SetRenderState(D3DRS_CULLMODE, value);
            break;

        case Ds_ClipPlaneEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, value);
            break;

        case Ds_ZFunc:
            this->m_d3dDevice->SetRenderState(D3DRS_ZFUNC, value);
            break;

        case Ds_PointScaleA:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_A, value);
            break;

        case Ds_PointScaleB:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_B, value);
            break;

        case Ds_PointScaleC:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_C, value);
            break;

        case Ds_PointScaleEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, value);
            break;

        default:
            break;
    }

    this->m_deviceStates[state] = value;
}

// VERIFY: daa41927 auto
// OFFSET: 0x006A4100
void CGxDeviceD3d::ISetTexCoordIndex(uint32_t tmu, int32_t texGen, uint32_t coord) {
    EDeviceState state = static_cast<EDeviceState>(Ds_TssTexCoordIndex0 + tmu);

    switch (texGen) {
        case GxTexGen_Disabled:
            this->DsSet(state, coord);
            break;

        case GxTexGen_ObjectLinear:
        case GxTexGen_EyeLinear:
        case GxTexGen_Unk3:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACEPOSITION);
            break;

        case GxTexGen_ReflectionMap:
        case GxTexGen_SphereMap:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            break;

        case GxTexGen_Normal:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACENORMAL);
            break;

        default:
            break;
    }
}

// VERIFY: 40ebb823 auto
// OFFSET: 0x006A4190
void CGxDeviceD3d::ISetColorOp(uint32_t tmu, uint32_t op) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + tmu), s_texOp[op]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorArg10 + tmu), s_texArgs[op][0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorArg20 + tmu), s_texArgs[op][1]);
    }
}

// VERIFY: c3e3636b auto
// OFFSET: 0x006A41F0
void CGxDeviceD3d::ISetAlphaOp(uint32_t tmu, uint32_t op) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + tmu), s_texOp[op]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaArg10 + tmu), s_texArgs[op][0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaArg20 + tmu), s_texArgs[op][1]);
    }
}

// VERIFY: c7d0427b auto
// OFFSET: 0x006A4250
void CGxDeviceD3d::ISetMaterial(CImVector diffuse, CImVector emissive, CImVector specular, float specularExp) {
    s_d3dMaterial.Diffuse.r = diffuse.r * (1.0f / 255.0f);
    s_d3dMaterial.Diffuse.g = diffuse.g * (1.0f / 255.0f);
    s_d3dMaterial.Diffuse.b = diffuse.b * (1.0f / 255.0f);
    s_d3dMaterial.Diffuse.a = diffuse.a * (1.0f / 255.0f);
    s_d3dMaterial.Ambient = s_d3dMaterial.Diffuse;
    s_d3dMaterial.Emissive.r = emissive.r * (1.0f / 255.0f);
    s_d3dMaterial.Emissive.g = emissive.g * (1.0f / 255.0f);
    s_d3dMaterial.Emissive.b = emissive.b * (1.0f / 255.0f);
    s_d3dMaterial.Emissive.a = emissive.a * (1.0f / 255.0f);
    s_d3dMaterial.Specular.r = specular.r * (1.0f / 255.0f);
    s_d3dMaterial.Specular.g = specular.g * (1.0f / 255.0f);
    s_d3dMaterial.Specular.b = specular.b * (1.0f / 255.0f);
    s_d3dMaterial.Specular.a = specular.a * (1.0f / 255.0f);
    s_d3dMaterial.Power = specularExp;
    this->m_d3dDevice->SetMaterial(&s_d3dMaterial);
}

// VERIFY: d2ce53e9 manual
// OFFSET: 0x006A43D0
void CGxDeviceD3d::IStateSyncLights() {
    if (this->m_appRenderStates[GxRs_Lighting].m_value.m_data.i[0]) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        this->DsSet(Ds_Lighting, this->m_appMasterEnables & 0x1);
        this->DsSet(Ds_Ambient, 0);

        for (uint32_t i = 0; i < 4; i++) {
            CGxApiLight& light = this->m_lights[i];

            if (!light.m_flags) {
                continue;
            }

            if (light.m_flags & 0x1) {
                if (!light.m_enable) {
                    light.m_flags &= ~0x1;
                    this->m_d3dDevice->LightEnable(i, FALSE);
                    continue;
                }

                this->m_d3dDevice->LightEnable(i, TRUE);
            } else if (!light.m_enable) {
                continue;
            }

            if (light.m_dir.w == 1.0f) {
                s_d3dLight.Type = D3DLIGHT_POINT;
                s_d3dLight.Position.x = light.m_dir.x;
                s_d3dLight.Position.y = light.m_dir.y;
                s_d3dLight.Position.z = light.m_dir.z;
            } else {
                s_d3dLight.Type = D3DLIGHT_DIRECTIONAL;
                s_d3dLight.Direction.x = light.m_dir.x;
                s_d3dLight.Direction.y = light.m_dir.y;
                s_d3dLight.Direction.z = light.m_dir.z;
            }

            s_d3dLight.Diffuse.r = light.m_dirColor.x;
            s_d3dLight.Diffuse.g = light.m_dirColor.y;
            s_d3dLight.Diffuse.b = light.m_dirColor.z;
            s_d3dLight.Diffuse.a = 1.0f;
            s_d3dLight.Ambient.r = light.m_ambColor.x;
            s_d3dLight.Ambient.g = light.m_ambColor.y;
            s_d3dLight.Ambient.b = light.m_ambColor.z;
            s_d3dLight.Ambient.a = 0.0f;
            s_d3dLight.Specular.r = light.m_specColor.x;
            s_d3dLight.Specular.g = light.m_specColor.y;
            s_d3dLight.Specular.b = light.m_specColor.z;
            s_d3dLight.Specular.a = 0.0f;
            s_d3dLight.Range = 10000.0f;
            s_d3dLight.Falloff = 1.0f;
            s_d3dLight.Attenuation0 = light.m_constantAttenuation;
            s_d3dLight.Attenuation1 = light.m_linearAttenuation;
            s_d3dLight.Attenuation2 = light.m_quadraticAttenuation;
            this->m_d3dDevice->SetLight(i, &s_d3dLight);

            light.m_flags &= ~light.m_flags;
        }

        return;
    }

    if ((this->m_primVertexMask & (1 << GxVA_Color0)) || this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0] == 0xFFFFFFFF) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        this->DsSet(Ds_Lighting, 0);
        this->DsSet(Ds_Ambient, 0);
    } else {
        if (!this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 1;

            for (uint32_t i = 0; i < 4; i++) {
                this->m_d3dDevice->LightEnable(i, FALSE);

                if (this->m_lights[i].m_enable) {
                    this->m_lights[i].m_flags |= 0x1;
                } else {
                    this->m_lights[i].m_flags &= ~0x1;
                }
            }
        }

        this->DsSet(Ds_Lighting, 1);
        this->DsSet(Ds_Ambient, 0xFFFFFFFF);
    }
}

// VERIFY: ccb732ea manual
// OFFSET: 0x006A4700
void CGxDeviceD3d::IStateSyncMaterial() {
    uint32_t emissiveSource;

    if (this->m_primVertexMask & (1 << GxVA_Color0)) {
        int32_t colorMaterial = this->m_appRenderStates[GxRs_ColorMaterial].m_value.m_data.i[0];
        this->DsSet(Ds_AmbientMaterialSource, colorMaterial == 0);
        this->DsSet(Ds_DiffuseMaterialSource, colorMaterial == 0);
        this->DsSet(Ds_SpecularMaterialSource, colorMaterial == 1);
        emissiveSource = colorMaterial == 2;
    } else {
        emissiveSource = 0;
        this->DsSet(Ds_AmbientMaterialSource, 0);
        this->DsSet(Ds_DiffuseMaterialSource, 0);
        this->DsSet(Ds_SpecularMaterialSource, 0);
    }

    this->DsSet(Ds_EmissiveMaterialSource, emissiveSource);
}

// VERIFY: 95b0bda6 auto
// OFFSET: 0x006A4850
void CGxDeviceD3d::IStateSyncXforms() {
    if (this->m_xforms[GxXform_Projection].m_dirty) {
        this->m_d3dDevice->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&this->m_projNative));
        this->m_xforms[GxXform_Projection].m_dirty = 0;
    }

    if (this->m_xforms[GxXform_View].m_dirty) {
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        this->m_d3dDevice->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view.m_mtx[view.m_level]));
        view.m_dirty = 0;
    }

    if (this->m_xforms[GxXform_World].m_dirty) {
        this->IXformSetWorld();
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_caps.m_numTmus); i++) {
        if (this->m_xforms[i].m_dirty || this->m_texGen[i].m_dirty) {
            this->IXformSetTex(i);
        }
    }
}

// VERIFY: 8e1dd0ae manual
// OFFSET: 0x006A4900
void CGxDeviceD3d::ISetTexture(uint32_t tmu, CGxTex* texId) {
    if (tmu >= 16) {
        return;
    }

    if (texId) {
        this->ITexMarkAsUpdated(texId);
        this->m_d3dDevice->SetTexture(tmu, static_cast<IDirect3DBaseTexture9*>(texId->m_apiSpecificData));

        const D3DTEXTUREFILTERTYPE* filter = s_filterModes[texId->m_flags.m_filter];
        this->DsSet(static_cast<EDeviceState>(Ds_TssMinFilter0 + tmu), filter[0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMagFilter0 + tmu), filter[1]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMipFilter0 + tmu), filter[2]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapU0 + tmu), s_wrapModes[texId->m_flags.m_wrapU]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapV0 + tmu), s_wrapModes[texId->m_flags.m_wrapV]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMaxAnisotropy0 + tmu), texId->m_flags.m_maxAnisotropy);

        if (tmu < 8) {
            if (!this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.i[0] && !this->m_texEnabled[tmu]) {
                this->ISetColorOp(tmu, this->m_appRenderStates[GxRs_ColorOp0 + tmu].m_value.m_data.i[0]);
                this->ISetAlphaOp(tmu, this->m_appRenderStates[GxRs_AlphaOp0 + tmu].m_value.m_data.i[0]);
                this->m_appRenderStates[GxRs_ColorOp0 + tmu].m_dirty = 0;
                this->m_appRenderStates[GxRs_AlphaOp0 + tmu].m_dirty = 0;
            }

            this->m_texEnabled[tmu] = true;
        }
    } else {
        this->m_d3dDevice->SetTexture(tmu, nullptr);

        if (tmu < 8) {
            if (!this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.i[0] && this->m_texEnabled[tmu]) {
                this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + tmu), D3DTOP_DISABLE);
                this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + tmu), D3DTOP_DISABLE);
            }

            this->m_texEnabled[tmu] = false;
        }
    }
}

// VERIFY: 79183c1d manual
// OFFSET: 0x006A4AC0
void CGxDeviceD3d::ISetTexCoord(uint32_t tmu, uint32_t coord) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->ISetTexCoordIndex(tmu, this->m_appRenderStates[GxRs_TexGen0 + tmu].m_value.m_data.i[0], coord);
    }
}

// VERIFY: 3b2376aa manual
// OFFSET: 0x006A4AF0
void CGxDeviceD3d::ISetTexGen(uint32_t tmu, int32_t texGen) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        return;
    }

    this->ISetTexCoordIndex(tmu, texGen, this->m_appRenderStates[GxRs_TextureCoord0 + tmu].m_value.m_data.i[0]);

    C44Matrix matrix;

    if (texGen == GxTexGen_ObjectLinear || texGen == GxTexGen_EyeLinear) {
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        matrix = view.m_mtx[view.m_level];
        matrix = matrix.AffineInverse();

        if (texGen == GxTexGen_ObjectLinear) {
            CGxMatrixStack& world = this->m_xforms[GxXform_World];
            matrix = matrix * world.m_mtx[world.m_level].Inverse();
        }
    } else if (texGen == GxTexGen_SphereMap) {
        matrix.a0 = 0.5f;
        matrix.b1 = 0.5f;
        matrix.d0 = 0.5f;
        matrix.d1 = 0.5f;
    } else {
        this->m_texGen[tmu].Identity();
        return;
    }

    CGxMatrixStack& stack = this->m_texGen[tmu];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;
}

// VERIFY: 1b818712 manual
// OFFSET: 0x006A4C30
void CGxDeviceD3d::IRsSendToHw(EGxRenderState which) {
    CGxStateBom& value = this->m_appRenderStates[which].m_value;

    switch (which) {
        case GxRs_PolygonOffset:
            if (this->m_caps.m_depthBias) {
                float bias = -value.m_data.f[0];
                this->m_d3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<DWORD*>(&bias));
            }

            break;

        case GxRs_MatDiffuse:
        case GxRs_MatEmissive:
        case GxRs_MatSpecular:
        case GxRs_MatSpecularExp: {
            CImVector diffuse;
            CImVector emissive;
            CImVector specular;
            *reinterpret_cast<uint32_t*>(&diffuse) = this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0];
            *reinterpret_cast<uint32_t*>(&emissive) = this->m_appRenderStates[GxRs_MatEmissive].m_value.m_data.u[0];
            *reinterpret_cast<uint32_t*>(&specular) = this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.u[0];
            this->ISetMaterial(diffuse, emissive, specular, this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0]);

            this->m_appRenderStates[GxRs_MatDiffuse].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatEmissive].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatSpecular].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatSpecularExp].m_dirty = 0;

            uint32_t specularEnable = 0;

            if (this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0] > 0.0f && this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.i[0]) {
                specularEnable = 1;
            }

            this->DsSet(Ds_SpecularEnable, specularEnable);
            break;
        }

        case GxRs_NormalizeNormals:
            if (s_normalizeNormals != value.m_data.u[0]) {
                this->m_d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, value.m_data.u[0]);
                s_normalizeNormals = value.m_data.u[0];
            }

            break;

        case GxRs_BlendingMode: {
            uint32_t mode = value.m_data.u[0];

            if (mode < 2) {
                this->DsSet(Ds_AlphaBlendEnable, 0);
            } else {
                this->DsSet(Ds_AlphaBlendEnable, 1);
                this->DsSet(Ds_SrcBlend, s_srcBlend[mode]);
                this->DsSet(Ds_DstBlend, s_dstBlend[mode]);
            }

            break;
        }

        case GxRs_AlphaRef: {
            int32_t alphaRef = value.m_data.i[0];

            if (alphaRef > 0) {
                this->DsSet(Ds_AlphaRef, alphaRef);
                this->DsSet(Ds_AlphaTestEnable, 1);
            } else {
                this->DsSet(Ds_AlphaTestEnable, 0);
            }

            break;
        }

        case GxRs_FogStart:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGSTART, value.m_data.u[0]);
            break;

        case GxRs_FogEnd:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGEND, value.m_data.u[0]);
            break;

        case GxRs_FogColor:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGCOLOR, value.m_data.u[0]);
            break;

        case GxRs_Lighting: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x1) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_Lighting, enable);
            break;
        }

        case GxRs_Fog: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x2) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_FogEnable, enable);
            break;
        }

        case GxRs_DepthTest:
        case GxRs_DepthFunc: {
            uint32_t func;

            if ((this->m_appMasterEnables & 0x4) && this->m_appRenderStates[GxRs_DepthTest].m_value.m_data.i[0]) {
                func = s_cmpFunc[this->m_appRenderStates[GxRs_DepthFunc].m_value.m_data.i[0]];
            } else {
                func = D3DCMP_ALWAYS;
            }

            this->DsSet(Ds_ZFunc, func);
            this->m_appRenderStates[GxRs_DepthTest].m_dirty = 0;
            this->m_appRenderStates[GxRs_DepthFunc].m_dirty = 0;
            break;
        }

        case GxRs_DepthWrite: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x8) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_ZWriteEnable, enable);
            break;
        }

        case GxRs_ColorWrite: {
            uint32_t mask = 0;

            if (this->m_appMasterEnables & 0x10) {
                mask = value.m_data.u[0];
            }

            this->DsSet(Ds_ColorWriteEnable, mask);
            break;
        }

        case GxRs_Culling: {
            uint32_t cull = 0;

            if (this->m_appMasterEnables & 0x20) {
                cull = value.m_data.i[0];

                if (value.m_data.i[0] > 2) {
                    cull = 2;
                }
            }

            this->DsSet(Ds_CullMode, s_cullMode[cull]);
            break;
        }

        case GxRs_ClipPlaneMask:
            this->DsSet(Ds_ClipPlaneEnable, value.m_data.u[0]);
            break;

        case GxRs_Multisample:
            this->m_d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, value.m_data.i[0] != 0);
            break;

        case GxRs_ScissorTest:
            this->m_d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, value.m_data.i[0] != 0);
            break;

        case GxRs_Texture0:
        case GxRs_Texture1:
        case GxRs_Texture2:
        case GxRs_Texture3:
        case GxRs_Texture4:
        case GxRs_Texture5:
        case GxRs_Texture6:
        case GxRs_Texture7:
        case GxRs_Texture8:
        case GxRs_Texture9:
        case GxRs_Texture10:
        case GxRs_Texture11:
        case GxRs_Texture12:
        case GxRs_Texture13:
        case GxRs_Texture14:
        case GxRs_Texture15:
            this->ISetTexture(which - GxRs_Texture0, static_cast<CGxTex*>(value.m_data.p));
            break;

        case GxRs_ColorOp0:
        case GxRs_ColorOp1:
        case GxRs_ColorOp2:
        case GxRs_ColorOp3:
        case GxRs_ColorOp4:
        case GxRs_ColorOp5:
        case GxRs_ColorOp6:
        case GxRs_ColorOp7:
            this->ISetColorOp(which - GxRs_ColorOp0, value.m_data.u[0]);
            break;

        case GxRs_AlphaOp0:
        case GxRs_AlphaOp1:
        case GxRs_AlphaOp2:
        case GxRs_AlphaOp3:
        case GxRs_AlphaOp4:
        case GxRs_AlphaOp5:
        case GxRs_AlphaOp6:
        case GxRs_AlphaOp7:
            this->ISetAlphaOp(which - GxRs_AlphaOp0, value.m_data.u[0]);
            break;

        case GxRs_TexGen0:
        case GxRs_TexGen1:
        case GxRs_TexGen2:
        case GxRs_TexGen3:
        case GxRs_TexGen4:
        case GxRs_TexGen5:
        case GxRs_TexGen6:
        case GxRs_TexGen7:
            this->ISetTexGen(which - GxRs_TexGen0, value.m_data.i[0]);
            break;

        case GxRs_TextureCoord0:
        case GxRs_TextureCoord1:
        case GxRs_TextureCoord2:
        case GxRs_TextureCoord3:
        case GxRs_TextureCoord4:
        case GxRs_TextureCoord5:
        case GxRs_TextureCoord6:
        case GxRs_TextureCoord7:
            this->ISetTexCoord(which - GxRs_TextureCoord0, value.m_data.u[0]);
            break;

        case GxRs_VertexShader:
            this->IBindVertexShader(static_cast<CGxShader*>(value.m_data.p));
            break;

        case GxRs_PixelShader:
            this->IBindPixelShader(static_cast<CGxShader*>(value.m_data.p));
            break;

        case GxRs_PointScale: {
            float pointScale = value.m_data.f[0];
            const C3Vector& attenuation = *reinterpret_cast<const C3Vector*>(this->m_appRenderStates[GxRs_PointScaleAttenuation].m_value.m_data.f);

            if (attenuation != CGxDevice::s_pointScaleAttenuation) {
                const CRect& window = this->DeviceCurWindow();
                pointScale = pointScale / ((1.0f - this->m_viewport.m_y.m_l) * window.maxY - (1.0f - this->m_viewport.m_y.m_h) * window.maxY);
            }

            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&pointScale));
            break;
        }

        case GxRs_PointScaleAttenuation: {
            const C3Vector& attenuation = *reinterpret_cast<const C3Vector*>(value.m_data.f);
            uint32_t enable = attenuation != CGxDevice::s_pointScaleAttenuation;

            this->DsSet(Ds_PointScaleEnable, enable);

            if (enable) {
                this->DsSet(Ds_PointScaleA, value.m_data.u[0]);
                this->DsSet(Ds_PointScaleB, value.m_data.u[1]);
                this->DsSet(Ds_PointScaleC, value.m_data.u[2]);
            }

            break;
        }

        case GxRs_PointScaleMin:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, value.m_data.u[0]);
            break;

        case GxRs_PointScaleMax:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, value.m_data.u[0]);
            break;

        case GxRs_PointSprite:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, value.m_data.i[0] != 0);
            break;

        case Unk84: {
            uint32_t factor = static_cast<uint32_t>(value.m_data.f[0] * 255.0f);
            this->m_d3dDevice->SetRenderState(D3DRS_BLENDFACTOR, (((((factor << 8) | factor) << 8) | factor) << 8) | factor);
            break;
        }

        default:
            break;
    }
}

// VERIFY: f78906fa manual
// OFFSET: 0x006A5540
LPDIRECT3DVERTEXDECLARATION9 CGxDeviceD3d::ICreateD3dVertexDecl(const D3DVERTEXELEMENT9* elements, uint32_t count) {
    if (this->m_primVertexFormat < GxVertexBufferFormats_Last) {
        if (!this->m_d3dVertexDecl[this->m_primVertexFormat]) {
            this->m_d3dDevice->CreateVertexDeclaration(elements, &this->m_d3dVertexDecl[this->m_primVertexFormat]);
        }

        return this->m_d3dVertexDecl[this->m_primVertexFormat];
    }

    for (uint32_t i = 0; i < this->m_gxVertexDecl.Count(); i++) {
        if (this->m_gxVertexDecl[i].Compare(elements, count)) {
            return this->m_gxVertexDecl[i].m_d3dDecl;
        }
    }

    GxVertexDecl* decl = this->m_gxVertexDecl.New();
    decl->m_count = count;
    memcpy(decl->m_elements, elements, sizeof(D3DVERTEXELEMENT9) * count);
    this->m_d3dDevice->CreateVertexDeclaration(elements, &decl->m_d3dDecl);

    return decl->m_d3dDecl;
}

// VERIFY: d3a984ea manual
// OFFSET: 0x006A5700
void CGxDeviceD3d::IStateSyncVertexPtrs() {
    if (this->m_primVertexFormat < GxVertexBufferFormats_Last && this->m_d3dVertexDecl[this->m_primVertexFormat]) {
        LPDIRECT3DVERTEXDECLARATION9 decl = this->m_d3dVertexDecl[this->m_primVertexFormat];
        CGxBuf* buf = this->m_primVertexBuf;

        if (this->m_d3dCurrentVertexDecl != decl) {
            this->m_d3dDevice->SetVertexDeclaration(decl);
            this->m_d3dCurrentVertexDecl = decl;
        }

        this->ISetVertexBuffer(0, static_cast<LPDIRECT3DVERTEXBUFFER9>(buf->m_pool->m_apiSpecific), buf->m_index, this->m_primVertexSize);

        return;
    }

    CGxBuf** streamBufs = static_cast<CGxBuf**>(_alloca(sizeof(CGxBuf*) * GxVAs_Last));
    uint32_t* streamStrides = static_cast<uint32_t*>(_alloca(sizeof(uint32_t) * GxVAs_Last));
    uint32_t streamCount = 0;

    memset(streamBufs, 0, sizeof(CGxBuf*) * GxVAs_Last);
    memset(streamStrides, 0, sizeof(uint32_t) * GxVAs_Last);

    D3DVERTEXELEMENT9 elements[GxVAs_Last + 1];
    uint32_t elementCount = 0;

    for (uint32_t i = 0; i < GxVAs_Last; i++) {
        if (!(this->m_primVertexMask & (1 << i))) {
            continue;
        }

        uint32_t stream;

        for (stream = 0; stream < streamCount; stream++) {
            if (streamBufs[stream] == this->m_primVertexFormatBuf[i]) {
                break;
            }
        }

        if (stream == streamCount) {
            streamBufs[streamCount++] = this->m_primVertexFormatBuf[i];
        }

        const CGxVertexAttrib& attrib = this->m_primVertexFormatAttrib[i];
        streamStrides[stream] += s_gxAttribToD3dAttribSize[attrib.m_type];

        D3DVERTEXELEMENT9& element = elements[elementCount++];
        element.Stream = static_cast<WORD>(stream);
        element.Offset = static_cast<WORD>(attrib.m_offset);
        element.Type = static_cast<BYTE>(s_gxAttribToD3dAttribType[attrib.m_type]);
        element.Method = D3DDECLMETHOD_DEFAULT;
        element.Usage = static_cast<BYTE>(s_gxAttribToD3dAttribUsage[attrib.m_attrib]);
        element.UsageIndex = static_cast<BYTE>(s_gxAttribToD3dAttribUsageIndex[attrib.m_attrib]);
    }

    D3DVERTEXELEMENT9 end = D3DDECL_END();
    elements[elementCount] = end;

    LPDIRECT3DVERTEXDECLARATION9 decl = this->ICreateD3dVertexDecl(elements, elementCount + 1);

    if (this->m_d3dCurrentVertexDecl != decl) {
        this->m_d3dDevice->SetVertexDeclaration(decl);
        this->m_d3dCurrentVertexDecl = decl;
    }

    for (uint32_t stream = 0; stream < streamCount; stream++) {
        CGxBuf* buf = streamBufs[stream];
        this->ISetVertexBuffer(stream, static_cast<LPDIRECT3DVERTEXBUFFER9>(buf->m_pool->m_apiSpecific), buf->m_index, streamStrides[stream]);
    }
}

// VERIFY: 357545fe manual
// OFFSET: 0x006A5940
void CGxDeviceD3d::IStateSync() {
    int32_t hwVertexShader = this->m_hwRenderStates[GxRs_VertexShader].m_data.i[0];

    this->IShaderConstantsFlush();
    this->IRsSync(0);

    if (!hwVertexShader || !this->m_appRenderStates[GxRs_VertexShader].m_value.m_data.i[0]) {
        this->IStateSyncLights();
        this->IStateSyncMaterial();
        this->IStateSyncXforms();
    }

    this->IStateSyncEnables();
    this->IStateSyncClipPlanes();
    this->IStateSyncScissorRect();
    this->IStateSyncVertexPtrs();

    if (this->m_primIndexDirty) {
        this->m_primIndexDirty = 0;

        LPDIRECT3DINDEXBUFFER9 indexBuffer = static_cast<LPDIRECT3DINDEXBUFFER9>(this->m_primIndexBuf->m_pool->m_apiSpecific);

        if (this->m_d3dCurrentIndexBuf != indexBuffer) {
            this->m_d3dDevice->SetIndices(indexBuffer);
            this->m_d3dCurrentIndexBuf = indexBuffer;
        }
    }

    if (this->m_viewportDirty) {
        this->IXformSetViewport();
    }
}

// VERIFY: 23534957 auto
// OFFSET: 0x006A5A30
void CGxDeviceD3d::IXformSetWorld() {
    CGxMatrixStack& world = this->m_xforms[GxXform_World];

    if (!s_isIdent || !(world.m_flags[world.m_level] & CGxMatrixStack::F_Identity)) {
        this->m_d3dDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&world.m_mtx[world.m_level]));
    }

    int32_t isIdent = world.m_flags[world.m_level] & CGxMatrixStack::F_Identity;
    world.m_dirty = 0;
    s_isIdent = isIdent;
}

// VERIFY: c3372d44 manual
// OFFSET: 0x006A5AA0
void CGxDeviceD3d::IXformSetTex(uint32_t tmu) {
    uint32_t flags = D3DTTFF_DISABLE;
    D3DTRANSFORMSTATETYPE state = static_cast<D3DTRANSFORMSTATETYPE>(D3DTS_TEXTURE0 + tmu);
    CGxMatrixStack& texGen = this->m_texGen[tmu];
    CGxMatrixStack& xform = this->m_xforms[tmu];

    switch (g_theGxDevicePtr->m_appRenderStates[GxRs_TextureShader0 + tmu].m_value.m_data.i[0]) {
        case 0:
            if (texGen.m_flags[texGen.m_level] & CGxMatrixStack::F_Identity) {
                this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&texGen.m_mtx[texGen.m_level]));
            } else {
                this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&texGen.m_mtx[texGen.m_level]));
                flags = D3DTTFF_COUNT3;
            }

            break;

        case 1: {
            C44Matrix product = texGen.m_mtx[texGen.m_level] * xform.m_mtx[xform.m_level];
            C44Matrix matrix = product;

            if (this->m_appRenderStates[GxRs_TexGen0 + tmu].m_value.m_data.i[0]) {
                flags = D3DTTFF_COUNT3;
            } else {
                flags = D3DTTFF_COUNT2;
                matrix.c0 = product.d0;
                matrix.c1 = product.d1;
            }

            this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&matrix));
            break;
        }

        case 2: {
            C44Matrix matrix = texGen.m_mtx[texGen.m_level] * xform.m_mtx[xform.m_level];
            this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&matrix));
            flags = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
            break;
        }

        default:
            break;
    }

    this->DsSet(static_cast<EDeviceState>(Ds_TssTexTransformFlags0 + tmu), flags);
    xform.m_dirty = 0;
    texGen.m_dirty = 0;
}

// VERIFY: e6b411da manual
// OFFSET: 0x006A5C70
void CGxDeviceD3d::IBindPixelShader(CGxShader* shader) {
    if (shader) {
        if (!shader->m_loaded) {
            this->IShaderCreatePixel(shader);
        }

        this->m_d3dDevice->SetPixelShader(static_cast<LPDIRECT3DPIXELSHADER9>(shader->m_apiSpecific));
        return;
    }

    this->m_d3dDevice->SetPixelShader(nullptr);

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_texEnabled[i]) {
            this->ISetColorOp(i, this->m_appRenderStates[GxRs_ColorOp0 + i].m_value.m_data.u[0]);
            this->ISetAlphaOp(i, this->m_appRenderStates[GxRs_AlphaOp0 + i].m_value.m_data.u[0]);
        } else {
            this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + i), D3DTOP_DISABLE);
            this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + i), D3DTOP_DISABLE);
        }

        this->m_appRenderStates[GxRs_ColorOp0 + i].m_dirty = 0;
        this->m_appRenderStates[GxRs_AlphaOp0 + i].m_dirty = 0;
    }
}

// VERIFY: 355e5938 manual
// OFFSET: 0x006A99E0
void CGxDeviceD3d::IXformSetViewport() {
    const CRect& window = this->DeviceCurWindow();
    D3DVIEWPORT9 viewport;

    viewport.X = static_cast<DWORD>(this->m_viewport.m_x.m_l * window.maxX + 0.5f);
    viewport.Y = static_cast<DWORD>((1.0f - this->m_viewport.m_y.m_h) * window.maxY + 0.5f);
    viewport.Width = static_cast<DWORD>(this->m_viewport.m_x.m_h * window.maxX - viewport.X + 0.5f);
    viewport.Height = static_cast<DWORD>((1.0f - this->m_viewport.m_y.m_l) * window.maxY - viewport.Y + 0.5f);
    viewport.MinZ = this->m_viewport.m_z.m_l;
    viewport.MaxZ = this->m_viewport.m_z.m_h;

    if (this->m_textureTarget[0].m_apiSpecific || this->m_textureTarget[1].m_apiSpecific) {
        viewport.Y = static_cast<DWORD>(window.maxY * this->m_viewport.m_y.m_l);
    }

    this->m_d3dDevice->SetViewport(&viewport);
    this->m_viewportDirty = 0;
}

// VERIFY: 2f91f397 manual
// OFFSET: 0x006A9FE0
void CGxDeviceD3d::IShaderConstantsFlush() {
    ShadowConstants& vs = CGxDevice::s_shadowConstants[1];

    if (vs.dirtyMin <= vs.dirtyMax) {
        this->m_d3dDevice->SetVertexShaderConstantF(vs.dirtyMin, &vs.constants[vs.dirtyMin * 4], vs.dirtyMax - vs.dirtyMin + 1);
    }

    vs.dirtyMin = 255;
    vs.dirtyMax = 0;

    ShadowConstants& ps = CGxDevice::s_shadowConstants[0];

    if (ps.dirtyMin <= ps.dirtyMax) {
        this->m_d3dDevice->SetPixelShaderConstantF(ps.dirtyMin, &ps.constants[ps.dirtyMin * 4], ps.dirtyMax - ps.dirtyMin + 1);
    }

    ps.dirtyMax = 0;
    ps.dirtyMin = 255;
}

// VERIFY: 8d590c92 manual
// OFFSET: 0x006AA2F0
void CGxDeviceD3d::IBindVertexShader(CGxShader* shader) {
    if (shader) {
        if (!shader->m_loaded) {
            this->IShaderCreateVertex(shader);
        }

        this->m_d3dDevice->SetVertexShader(static_cast<LPDIRECT3DVERTEXSHADER9>(shader->m_apiSpecific));
    } else {
        this->m_d3dDevice->SetVertexShader(nullptr);
    }
}
