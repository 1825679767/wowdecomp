#include <cstdint>
#include <cstring>
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Storm/H/storm.h"

typedef IDirect3D9* (WINAPI* Direct3DCreate9Proc)(UINT sdkVersion);

// STATIC: 0x00A2E4E4
static const D3DQUERYTYPE s_d3dQueryTypes[1] = { D3DQUERYTYPE_OCCLUSION };

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
