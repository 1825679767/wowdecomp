#ifndef ENGINE_SOURCE_GX_CGXDEVICED3D_CGXDEVICED3D_H
#define ENGINE_SOURCE_GX_CGXDEVICED3D_CGXDEVICED3D_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include <d3d9.h>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxDeviceD3d : public CGxDevice {
    public:
    struct GxVertexDecl {
        /* 0000 */ uint32_t m_count;
        /* 0004 */ D3DVERTEXELEMENT9 m_elements [15];
        /* 007C */ IDirect3DVertexDeclaration9* m_d3dDecl;
    };

    /* 3968 */ HWND m_hwnd;
    /* 396C */ ATOM m_hwndClass;
    /* 396E */ uint16_t unk_396E;
    /* 3970 */ int32_t m_ownhwnd;
    /* 3974 */ HINSTANCE m_d3dLib;
    /* 3978 */ LPDIRECT3D9 m_d3d;
    /* 397C */ LPDIRECT3DDEVICE9 m_d3dDevice;
    /* 3980 */ D3DCAPS9 m_d3dCaps;
    /* 3AB0 */ int32_t m_d3dIsHwDevice;
    /* 3AB4 */ int32_t m_d3dNVAPI;
    /* 3AB8 */ uint32_t m_d3dStereoEnabled;
    /* 3ABC */ uint32_t m_d3dStereoRestore;
    /* 3AC0 */ uint32_t m_d3dStereoHandle;
    /* 3AC4 */ float m_d3dStereoConvergence;
    /* 3AC8 */ float m_d3dStereoSeparation;
    /* 3ACC */ int32_t m_d3dStereoDirty;
    /* 3AD0 */ TSGrowableArray<CGxDeviceD3d::GxVertexDecl> m_gxVertexDecl;
    /* 3AE0 */ LPDIRECT3DVERTEXDECLARATION9 m_d3dVertexDecl [14];
    /* 3B18 */ D3DDISPLAYMODE m_desktopDisplayMode;
    /* 3B28 */ int32_t m_inScene;
    /* 3B2C */ int32_t m_ambientOnlyMode;
    /* 3B30 */ D3DFORMAT m_devDepthFormat;
    /* 3B34 */ D3DFORMAT m_devAdapterFormat;
    /* 3B38 */ LPDIRECT3DSURFACE9 m_defDepthStencilSurface;
    /* 3B3C */ LPDIRECT3DSURFACE9 m_defColorSurface;
    /* 3B40 */ LPDIRECT3DSURFACE9 m_defDepthSurface;
    /* 3B44 */ LPDIRECT3DSURFACE9 m_surface3B44;
    /* 3B48 */ LPDIRECT3DQUERY9 m_eventQuery;
    /* 3B4C */ int32_t m_hwCursorDirty;
    /* 3B50 */ LPDIRECT3DTEXTURE9 m_hwCursorTexture;
    /* 3B54 */ LPDIRECT3DSURFACE9 m_hwCursorBitmap;
    /* 3B58 */ CGxTex* m_texture3B58;
    /* 3B5C */ LPDIRECT3DVERTEXDECLARATION9 m_d3dCurrentVertexDecl;
    /* 3B60 */ LPDIRECT3DINDEXBUFFER9 m_d3dCurrentIndexBuf;
    /* 3B64 */ LPDIRECT3DVERTEXBUFFER9 m_d3dVertexStreamBuf [8];
    /* 3B84 */ uint32_t m_d3dVertexStreamOfs [8];
    /* 3BA4 */ uint32_t m_d3dVertexStreamStride [8];
    /* 3BC4 */ uint32_t m_deviceStates [182];
    /* 3E9C */ bool m_texEnabled [8];

    virtual void ITexMarkAsUpdated(CGxTex* texId);
    virtual void IRsSendToHw(EGxRenderState which);
    virtual void ICursorCreate(const CGxFormat& format);
    virtual void ICursorDestroy();
    virtual void ICursorDraw();
    virtual int32_t DeviceCreate(GxWindowProc windowProc, const CGxFormat& format);
    virtual int32_t DeviceCreate(void* window, const CGxFormat& format);
    virtual void DeviceDestroy();
    virtual void DeviceEvictResources();
    virtual int32_t DeviceSetFormat(const CGxFormat& format);
    virtual void DeviceSetBaseMipLevel(uint32_t level);
    virtual void DeviceSetGamma(float gamma);
    virtual void DeviceSetGamma(const CGxGammaRamp& ramp);
    virtual void* DeviceWindow();
    virtual void DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels);
    virtual void DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths);
    virtual int32_t DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2);
    virtual void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
    virtual void DeviceSaveDepthSurface();
    virtual void DeviceRestoreDepthSurface();
    virtual void DeviceResolveDepthBuffer(CGxTex* texture);
    virtual void DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane);
    virtual void DeviceOverride(EGxOverride override, uint32_t value);
    virtual void CapsWindowSize(CRect& rect);
    virtual void CapsWindowSizeInScreenCoords(CRect& rect);
    virtual void LogCrashInfo(char* buffer, uint32_t size);
    virtual void ScenePresent();
    virtual void SceneClear(uint32_t mask, CImVector color);
    virtual void XformSetProjection(const C44Matrix& matrix);
    virtual void XformSetView(const C44Matrix& matrix);
    virtual void Draw(CGxBatch* batch, int32_t count);
    virtual void PoolSizeSet(CGxPool* pool, uint32_t size);
    virtual void PoolDestroy(CGxPool* pool);
    virtual char* BufLock(CGxBuf* buf);
    virtual int32_t BufUnlock(CGxBuf* buf, uint32_t size);
    virtual void BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
    virtual void TexDestroy(CGxTex* texId);
    virtual int32_t TexCopy(CGxTex* dest, CGxTex* source, const C2iVector& pos, const C2iVector& size, uint32_t level, uint32_t plane);
    virtual int32_t TexStretch(CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face);
    virtual void QueryCreate(CGxQuery*& query, EGxQueryType type);
    virtual void QueryDestroy(CGxQuery*& query);
    virtual bool QueryBegin(CGxQuery* query);
    virtual bool QueryEnd(CGxQuery* query);
    virtual bool QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t& data);
    virtual bool QueryGetData(CGxQuery* query, uint32_t& data);
    virtual void ShaderCreate(CGxShader** shaders, EGxShTarget target, const char* path, const char* name, uint32_t permutations);
    virtual void ShaderDestroy(CGxShader*& shader);
    virtual void IShaderReload(CGxShader* shader, const char* path, const char* name);
    virtual void IShaderCreate(CGxShader* shader);
    virtual void CursorSetVisible(int32_t visible);
    virtual void CursorUnlock(uint32_t x, uint32_t y);
    virtual void StereoSetConvergence(float convergence);
    virtual float StereoGetConvergence();
    virtual void StereoSetSeparation(float separation);
    virtual float StereoGetSeparation();
    virtual bool StereoEnabled();
    virtual void DeviceUnk81(uint32_t a, uint32_t b);
    virtual void DeviceUnk82();
    virtual void DeviceUnk83(uint32_t a, uint32_t b);

    static int32_t ILoadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d);
    static void IUnloadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d);

    CGxDeviceD3d();
    LPDIRECT3DINDEXBUFFER9 ICreateD3dIB(EGxPoolUsage usage, uint32_t size);
    LPDIRECT3DVERTEXBUFFER9 ICreateD3dVB(EGxPoolUsage usage, uint32_t size);
    void IReleaseD3dPool(CGxPool* pool);
    void ISceneEnd();
    void ISetWindowFocus(int32_t focus);
    void IShaderCreatePixel(CGxShader* shader);
    void IShaderCreateVertex(CGxShader* shader);
};

CGxDevice* CreateCGxDeviceD3d();

#endif
