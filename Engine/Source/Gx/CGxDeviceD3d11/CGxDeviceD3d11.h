#pragma once
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include <memory>
#include <windows.h>

// Native WotLK device contract. Does not inherit from a D3D9 device or import
// Cataclysm's shared Gx layouts. All API resources belong to this device.
class CGxDeviceD3d11 final : public CGxDevice {
public:
    CGxDeviceD3d11();
    ~CGxDeviceD3d11() override;
    int32_t DeviceCreate(GxWindowProc proc,const CGxFormat& format) override;
    int32_t DeviceCreate(void* window,const CGxFormat& format) override;
    void DeviceDestroy() override;
    int32_t DeviceSetFormat(const CGxFormat& format) override;
    void DeviceSetBaseMipLevel(uint32_t level) override;
    void DeviceSetGamma(float gamma) override;
    void DeviceSetGamma(const CGxGammaRamp& ramp) override;
    void DeviceOverride(EGxOverride setting,uint32_t value) override;
    void DeviceEvictResources() override;
    void* DeviceWindow() override;
    int32_t DeviceWM(EGxWM wm,uintptr_t a,uintptr_t b) override;
    void CapsWindowSize(CRect& rect) override;
    void CapsWindowSizeInScreenCoords(CRect& rect) override;
    void LogCrashInfo(char* buffer,uint32_t size) override;
    void ScenePresent() override;
    void SceneClear(uint32_t mask,CImVector color) override;
    void XformSetProjection(const C44Matrix& matrix) override;
    void Draw(CGxBatch* batches,int32_t count) override;
    void PoolSizeSet(CGxPool* pool,uint32_t size) override;
    void PoolDestroy(CGxPool* pool) override;
    char* BufLock(CGxBuf* buf) override;
    int32_t BufUnlock(CGxBuf* buf,uint32_t size) override;
    void BufData(CGxBuf* buf,const void* data,uint32_t size,uint32_t offset) override;
    void TexDestroy(CGxTex* texture) override;
    void ITexMarkAsUpdated(CGxTex* texture) override;
    int32_t TexCopy(CGxTex* dest,const C2iVector& destPos,const C2iVector& srcPos,const C2iVector& size,uint32_t level,uint32_t plane) override;
    int32_t TexStretch(CGxTex* dest,const CiRect* destRect,const CiRect* srcRect,uint32_t level,uint32_t face) override;
    void DeviceSetRenderTarget(EGxBuffer target,CGxTex* texture,uint32_t plane) override;
    void DeviceSaveDepthSurface() override;
    void DeviceRestoreDepthSurface() override;
    void DeviceResolveDepthBuffer(CGxTex* texture) override;
    void DeviceCopyTex(CGxTex* src,uint32_t srcPlane,CGxTex* dest,uint32_t destPlane) override;
    void DeviceReadPixels(CiRect& rect,TSGrowableArray<CImVector>& pixels) override;
    void DeviceReadDepths(CiRect& rect,TSGrowableArray<float>& depths) override;
    void QueryDestroy(CGxQuery*& query) override;
    bool QueryBegin(CGxQuery* query) override;
    bool QueryEnd(CGxQuery* query) override;
    bool QueryGetParam(CGxQuery* query,EGxQueryParam param,uint32_t& data) override;
    bool QueryGetData(CGxQuery* query,uint32_t& data) override;
    void ShaderCreate(CGxShader** shaders,EGxShTarget target,const char* path,const char* name,uint32_t permutations) override;
    void IShaderReload(CGxShader* shader,const char* path,const char* name) override;
    void IShaderCreate(CGxShader* shader) override;
    void ShaderDestroy(CGxShader*& shader) override;
    void IRsSendToHw(EGxRenderState which) override;
    void CursorUnlock(uint32_t x,uint32_t y) override;
    void CursorSetVisible(int32_t visible) override;
    void ICursorDraw() override;
    void StereoSetConvergence(float value) override;
    float StereoGetConvergence() override;
    void StereoSetSeparation(float value) override;
    float StereoGetSeparation() override;
    bool StereoEnabled() override;
    // The three unimplemented native D3D9 extension slots remain inert here.
    void DeviceUnk81(uint32_t,uint32_t) override;
    void DeviceUnk82() override;
    void DeviceUnk83(uint32_t,uint32_t) override;

    // Diagnostic creation exercises the exact backend without a game/window.
    bool CreateOffscreen(uint32_t width,uint32_t height,bool warp);
    void ReadTargetPixels(CiRect& rect,TSGrowableArray<CImVector>& pixels);
    bool RecoverDevice();
    // Draw-local donor sampler policy; leaves shared CGxTex flags untouched.
    struct SamplerOverride { bool enabled=false,mipLinear=false; };
    SamplerOverride ExchangeSamplerOverride(uint32_t stage,SamplerOverride value);
    // Capture active RT0 (including MSAA), preserving native target ownership.
    CGxTex* CaptureCurrentColor();
    uint64_t SubmittedDraws() const;
    // WXL silhouette policy stays in its module; GPU resources stay device-owned.
    void BeginUnitOutlineFrame();
    bool StampUnitOutline(const CGxBatch& batch,bool indexed,const float* rgba,bool depthTest);
    bool EndUnitOutlineFrame();
    static bool Available();
    static int32_t AdapterFormats(TSGrowableArray<CGxFormat>& formats);
private:
    struct Impl;
    std::unique_ptr<Impl> m;
    bool Initialize(bool warp);
    bool Resize(uint32_t width,uint32_t height);
    bool BindTargets();
    void ReadColorPixels(CiRect&,TSGrowableArray<CImVector>&,bool backbuffer);
    bool TextureReady(CGxTex* texture);
    bool SyncDraw(EGxPrim primitive);
    bool PoolReady(CGxPool* pool);
    static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
};
CGxDevice* CreateCGxDeviceD3d11();
