#include "Internal.h"
#include "Engine/Source/Gx/Gx.h"
#include <cstdio>
#include <limits>

using namespace gx11;

CGxDeviceD3d11::CGxDeviceD3d11():m(std::make_unique<Impl>(this)){
    m_api=GxApi_D3d11;m_caps.m_colorFormat=GxCF_argb;
    DeviceCreatePools();DeviceCreateStreamBufs();
}
CGxDeviceD3d11::~CGxDeviceD3d11(){DeviceDestroy();}
CGxDevice* CreateCGxDeviceD3d11(){return new CGxDeviceD3d11();}

bool CGxDeviceD3d11::Available(){
    HMODULE lib=LoadLibraryExW(L"d3d11.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!lib)return false;const bool found=GetProcAddress(lib,"D3D11CreateDevice")!=nullptr;FreeLibrary(lib);return found;
}
bool CGxDeviceD3d11::Initialize(bool warp){
    const D3D_FEATURE_LEVEL levels[]={D3D_FEATURE_LEVEL_11_0};m->warp=warp;
    if(!Check(D3D11CreateDevice(nullptr,warp?D3D_DRIVER_TYPE_WARP:D3D_DRIVER_TYPE_HARDWARE,nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,levels,1,D3D11_SDK_VERSION,&m->device,&m->featureLevel,&m->context),"CreateDevice"))return false;
    if(m->window){
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;ComPtr<IDXGIFactory> factory;
        if(!Check(m->device.As(&dxgi),"DXGI device")||!Check(dxgi->GetAdapter(&adapter),"adapter")||
           !Check(adapter->GetParent(IID_PPV_ARGS(&factory)),"factory"))return false;
        DXGI_SWAP_CHAIN_DESC sd{};sd.BufferDesc.Width=m->width;sd.BufferDesc.Height=m->height;
        sd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;sd.SampleDesc.Count=1;
        sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;sd.BufferCount=std::max(1u,m_format.m_backBufferCount);
        sd.OutputWindow=m->window;sd.Windowed=TRUE;sd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
        if(!Check(factory->CreateSwapChain(m->device.Get(),&sd,&m->swapchain),"swapchain"))return false;
        factory->MakeWindowAssociation(m->window,DXGI_MWA_NO_ALT_ENTER|DXGI_MWA_NO_WINDOW_CHANGES);
        if(!m_format.m_window&&!Check(m->swapchain->SetFullscreenState(TRUE,nullptr),"fullscreen"))return false;
    }
    memset(&m_caps,0,sizeof(m_caps));m_caps.m_numTmus=16;m_caps.m_numStreams=16;m_caps.m_streamOffset=1;
    m_caps.m_colorFormat=GxCF_argb;m_caps.m_pixelCenterOnEdge=0;m_caps.m_texelCenterOnEdge=0;
    m_caps.unk_0018=1;m_caps.m_maxIndex=65535;m_caps.m_generateMipMaps=1;
    m_caps.m_shaderTargets[GxSh_Vertex]=3;m_caps.m_shaderTargets[GxSh_Pixel]=4;
    m_caps.m_shaderConstants[GxSh_Vertex]=256;m_caps.m_shaderConstants[GxSh_Pixel]=224;
    m_caps.m_texFilterTrilinear=1;m_caps.m_texFilterAnisotropic=1;m_caps.m_maxTexAnisotropy=16;
    m_caps.m_depthBias=1;m_caps.m_colorWrite=1;m_caps.m_maxClipPlanes=6;
    m_caps.m_occlusionQuery=1;m_caps.m_blendFactor=1;m_caps.m_shaderAlphaRef=1;
    for(uint32_t i=1;i<GxTexFormats_Last;++i){UINT support=0;
        if(SUCCEEDED(m->device->CheckFormatSupport(Format(EGxTexFormat(i)),&support))){
            m_caps.m_texFmt[i]=(support&D3D11_FORMAT_SUPPORT_TEXTURE2D)!=0;
            m_caps.m_rttFormat[i]=(support&(D3D11_FORMAT_SUPPORT_RENDER_TARGET|D3D11_FORMAT_SUPPORT_DEPTH_STENCIL))!=0;
        }
    }
    m_caps.m_texFmt[GxTex_D24X8]=1;m_caps.m_rttFormat[GxTex_D24X8]=1;
    for(uint32_t i=0;i<GxTexTargets_Last;++i){m_caps.m_texTarget[i]=1;m_caps.m_texMaxSize[i]=D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;}
    m_caps.m_texNonPow2=1;
    for(uint32_t stage=0;stage<2;++stage)for(uint32_t buffer=0;buffer<4;++buffer){
        D3D11_BUFFER_DESC d{};d.ByteWidth=buffer==0?4096:(buffer==1?256:(buffer==2?64:256));
        d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        if(!Check(m->device->CreateBuffer(&d,nullptr,&m->constants[stage][buffer]),"constant buffer"))return false;
    }
    return Resize(m->width,m->height);
}

bool CGxDeviceD3d11::CreateOffscreen(uint32_t width,uint32_t height,bool warp){
    if(!width||!height||width>16384||height>16384||m->device)return false;
    m->offscreen=true;m->width=width;m->height=height;m_format.m_size.x=width;m_format.m_size.y=height;m_format.m_window=1;
    if(!Initialize(warp)){DeviceDestroy();return false;}return true;
}

bool CGxDeviceD3d11::Resize(uint32_t width,uint32_t height){
    if(!m->device||!width||!height||width>16384||height>16384)return false;
    m->UnbindViews();m->savedDepth.Reset();m->colorView.Reset();m->depthView.Reset();m->depthSrv.Reset();
    m->backbuffer.Reset();m->color.Reset();m->depth.Reset();
    m->width=width;m->height=height;
    if(m->swapchain){
        if(!Check(m->swapchain->ResizeBuffers(0,width,height,DXGI_FORMAT_UNKNOWN,0),"ResizeBuffers")||
            !Check(m->swapchain->GetBuffer(0,IID_PPV_ARGS(&m->backbuffer)),"backbuffer"))return false;
    }
    m->samples=std::max(1u,m_format.m_multisampleCount);UINT quality=0;
    if(m->samples>1&&(FAILED(m->device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,m->samples,&quality))||!quality))return false;
    D3D11_TEXTURE2D_DESC d{};d.Width=width;d.Height=height;d.MipLevels=1;d.ArraySize=1;
    d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.SampleDesc.Count=m->samples;d.Usage=D3D11_USAGE_DEFAULT;
    d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
    if(m->backbuffer&&m->samples==1)m->color=m->backbuffer;
    else if(!Check(m->device->CreateTexture2D(&d,nullptr,&m->color),"color target"))return false;
    if(!Check(m->device->CreateRenderTargetView(m->color.Get(),nullptr,&m->colorView),"color view"))return false;
    d.Format=DXGI_FORMAT_R24G8_TYPELESS;d.BindFlags=D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
    if(!Check(m->device->CreateTexture2D(&d,nullptr,&m->depth),"depth target"))return false;
    D3D11_DEPTH_STENCIL_VIEW_DESC dd{};dd.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
    dd.ViewDimension=m->samples>1?D3D11_DSV_DIMENSION_TEXTURE2DMS:D3D11_DSV_DIMENSION_TEXTURE2D;
    if(!Check(m->device->CreateDepthStencilView(m->depth.Get(),&dd,&m->depthView),"depth view"))return false;
    D3D11_SHADER_RESOURCE_VIEW_DESC sr{};sr.Format=DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    sr.ViewDimension=m->samples>1?D3D11_SRV_DIMENSION_TEXTURE2DMS:D3D11_SRV_DIMENSION_TEXTURE2D;sr.Texture2D.MipLevels=1;
    if(!Check(m->device->CreateShaderResourceView(m->depth.Get(),&sr,&m->depthSrv),"depth sample view"))return false;
    CRect rect{};rect.maxX=float(width);rect.maxY=float(height);DeviceSetDefWindow(rect);DeviceSetCurWindow(rect);
    m_format.m_size.x=width;m_format.m_size.y=height;m_context=1;m_windowVisible=1;m_windowFocus=1;
    IRsForceUpdate();m_viewportDirty=1;m_scissorRectDirty=1;return BindTargets();
}

LRESULT CALLBACK CGxDeviceD3d11::WindowProc(HWND window,UINT msg,WPARAM wp,LPARAM lp){
    auto* self=reinterpret_cast<CGxDeviceD3d11*>(GetWindowLongPtrW(window,GWLP_USERDATA));
    if(msg==WM_NCCREATE){self=static_cast<CGxDeviceD3d11*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(self));}
    if(self){
        if(msg==WM_SIZE){CRect rect{};rect.maxX=LOWORD(lp);rect.maxY=HIWORD(lp);self->DeviceWM(GxWM_Size,reinterpret_cast<uintptr_t>(&rect),wp==SIZE_MINIMIZED?1:0);}
        else if(msg==WM_SETFOCUS)self->DeviceWM(GxWM_SetFocus,0,0);
        else if(msg==WM_KILLFOCUS)self->DeviceWM(GxWM_KillFocus,0,0);
        else if(msg==WM_DESTROY)self->DeviceWM(GxWM_Destroy,0,0);
        else if(msg==WM_ERASEBKGND)return 1;
        else if(msg==WM_SETCURSOR&&LOWORD(lp)==HTCLIENT){SetCursor(self->m_cursorVisible?LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)):nullptr);return TRUE;}
        if(self->m_windowProc)return self->m_windowProc(window,msg,wp,lp);
    }
    return DefWindowProcW(window,msg,wp,lp);
}
int32_t CGxDeviceD3d11::DeviceCreate(GxWindowProc proc,const CGxFormat& format){
    m_windowProc=proc;m_format=format;m->ownWindow=true;
    const auto instance=GetModuleHandleW(nullptr);WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=instance;
    wc.lpfnWndProc=WindowProc;wc.lpszClassName=L"WxlNativeGxD3d11";wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
    if(!RegisterClassExW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return 0;
    DWORD style=format.m_window?WS_OVERLAPPEDWINDOW:WS_POPUP;
    RECT rect{0,0,format.m_size.x,format.m_size.y};AdjustWindowRectEx(&rect,style,FALSE,0);
    m->window=CreateWindowExW(0,wc.lpszClassName,L"World of Warcraft",style,CW_USEDEFAULT,CW_USEDEFAULT,
        rect.right-rect.left,rect.bottom-rect.top,nullptr,nullptr,instance,this);
    if(!m->window)return 0;m->width=format.m_size.x;m->height=format.m_size.y;
    char value[4]{};const bool warp=GetEnvironmentVariableA("WXL_DX11_WARP",value,4)&&value[0]=='1';
    if(!Initialize(warp)){DeviceDestroy();return 0;}
    ShowWindow(m->window,format.m_maximize?SW_MAXIMIZE:SW_SHOW);return 1;
}
int32_t CGxDeviceD3d11::DeviceCreate(void* window,const CGxFormat& format){
    if(!window)return 0;m->window=static_cast<HWND>(window);m->ownWindow=false;m_format=format;
    m->width=format.m_size.x;m->height=format.m_size.y;if(!Initialize(false)){DeviceDestroy();return 0;}return 1;
}
int32_t CGxDeviceD3d11::DeviceSetFormat(const CGxFormat& format){
    CGxDevice::DeviceSetFormat(format);if(!m->device)return 0;
    if(m->swapchain&&!Check(m->swapchain->SetFullscreenState(!format.m_window,nullptr),"fullscreen change"))return 0;
    const bool result=Resize(format.m_size.x,format.m_size.y);if(result)NotifyOnDeviceRestored();return result;
}
void CGxDeviceD3d11::DeviceDestroy(){
    if(!m)return;m_context=0;
    if(m->context){m->context->ClearState();m->context->Flush();}
    for(auto& [t,v]:m->textures){t->m_apiSpecificData=nullptr;t->m_apiSpecificData2=nullptr;t->m_needsCreation=1;t->m_needsUpdate=1;}
    for(auto& [p,v]:m->pools)p->m_apiSpecific=nullptr;
    for(uint32_t stage=0;stage<6;++stage)for(auto* s=m_shaderList[stage].Head();s;s=m_shaderList[stage].Next(s)){
        s->m_apiSpecific=nullptr;s->m_loaded=0;s->m_valid=0;
    }
    for(auto* q=m_queryList.Head();q;q=m_queryList.Next(q))if(q->m_apiSpecific){static_cast<IUnknown*>(q->m_apiSpecific)->Release();q->m_apiSpecific=nullptr;}
    HWND window=m->window;const bool own=m->ownWindow;
    if(m->swapchain)m->swapchain->SetFullscreenState(FALSE,nullptr);
    m.reset(new Impl(this));if(window&&own)DestroyWindow(window);CGxDevice::DeviceDestroy();
}
void* CGxDeviceD3d11::DeviceWindow(){return m->window;}
int32_t CGxDeviceD3d11::DeviceWM(EGxWM wm,uintptr_t a,uintptr_t b){
    if(wm==GxWM_SetFocus)m_windowFocus=1;
    else if(wm==GxWM_KillFocus||wm==GxWM_Destroy)m_windowFocus=0;
    else if(wm==GxWM_Size||wm==GxWM_DisplayChange){
        if(b==1||b==2)m_windowVisible=0;
        else if(a&&m->device){const auto& r=*reinterpret_cast<CRect*>(a);
            const uint32_t w=uint32_t(std::max(0.f,r.maxX-r.minX)),h=uint32_t(std::max(0.f,r.maxY-r.minY));
            if(w&&h&&(w!=m->width||h!=m->height)){if(!Resize(w,h)){m_context=0;return 0;}NotifyOnDeviceRestored();}m_windowVisible=1;
        }
    }return 1;
}
void CGxDeviceD3d11::CapsWindowSize(CRect& rect){rect=m_defWindowRect;}
void CGxDeviceD3d11::CapsWindowSizeInScreenCoords(CRect& rect){
    rect=m_defWindowRect;if(m->window){POINT a{0,0};ClientToScreen(m->window,&a);rect.minX+=a.x;rect.maxX+=a.x;rect.minY+=a.y;rect.maxY+=a.y;}
}
void CGxDeviceD3d11::LogCrashInfo(char* out,uint32_t n){if(out&&n)std::snprintf(out,n,"Native DX11 FL=%x WARP=%d %ux%u MSAA=%u",m->featureLevel,m->warp,m->width,m->height,m->samples);}
void CGxDeviceD3d11::ScenePresent(){
    if(!m_context)return;
    if(m_scrShotClick){CiRect r(0,0,m->height,m->width);DeviceReadPixels(r,m_scrShotPixels);m_scrShotWidth=m->width;m_scrShotHeight=m->height;}
    ICursorDraw();
    if(m->swapchain){if(m->samples>1)m->context->ResolveSubresource(m->backbuffer.Get(),0,m->color.Get(),0,DXGI_FORMAT_R8G8B8A8_UNORM);
        const HRESULT hr=m->swapchain->Present(m_format.m_vsync?1:0,0);
        if(FAILED(hr)){Check(hr,"Present");m_context=0;}
    }
    CGxDevice::ScenePresent();WaitForFPSCap();
}
void CGxDeviceD3d11::SceneClear(uint32_t mask,CImVector color){
    if(!m_context||!BindTargets())return;
    if(mask&1){const float c[]={color.r/255.f,color.g/255.f,color.b/255.f,color.a/255.f};m->context->ClearRenderTargetView(m->CurrentColor(),c);}
    if(mask&6){auto* d=m->CurrentDepth();if(d)m->context->ClearDepthStencilView(d,((mask&2)?D3D11_CLEAR_DEPTH:0)|((mask&4)?D3D11_CLEAR_STENCIL:0),1,0);}
}
void CGxDeviceD3d11::XformSetProjection(const C44Matrix& matrix){
    CGxDevice::XformSetProjection(matrix);C44Matrix p=matrix;
    if(std::abs(p.c3-1.f)>0.00000023841858f&&std::abs(p.c3)>0.00000023841858f)p*=1.f/p.c3;
    if(p.d3==0){const float nearZ=-p.d2/(p.c2+1),farZ=-p.d2/(p.c2-1);p.c2=farZ/(farZ-nearZ);p.d2=farZ*nearZ/(nearZ-farZ);}
    else {const float nearZ=(-1-p.d2)/p.c2,farZ=(1-p.d2)/p.c2;p.c2=1/(farZ-nearZ);p.d2=nearZ/(nearZ-farZ);}
    if(!(m_appMasterEnables&(1<<GxMasterEnable_NormalProjection))&&p.d3!=1){C44Matrix scale;scale.a0=.2f;scale.b1=.2f;scale.c2=.2f;p=p*scale;}
    m_projNative=p;m_xforms[GxXform_Projection].m_dirty=1;
}
void CGxDeviceD3d11::DeviceEvictResources(){if(m->context){m->context->ClearState();m->context->Flush();IRsForceUpdate();}}
void CGxDeviceD3d11::CursorUnlock(uint32_t x,uint32_t y){CGxDevice::CursorUnlock(x,y);}
void CGxDeviceD3d11::CursorSetVisible(int32_t v){CGxDevice::CursorSetVisible(v);}
void CGxDeviceD3d11::ICursorDraw(){CGxDevice::ICursorDraw();}
void CGxDeviceD3d11::StereoSetConvergence(float){} float CGxDeviceD3d11::StereoGetConvergence(){return 1;}
void CGxDeviceD3d11::StereoSetSeparation(float){} float CGxDeviceD3d11::StereoGetSeparation(){return 0;}
bool CGxDeviceD3d11::StereoEnabled(){return false;}
void CGxDeviceD3d11::DeviceUnk81(uint32_t,uint32_t){}void CGxDeviceD3d11::DeviceUnk82(){}void CGxDeviceD3d11::DeviceUnk83(uint32_t,uint32_t){}

int32_t CGxDeviceD3d11::AdapterFormats(TSGrowableArray<CGxFormat>& formats){
    ComPtr<IDXGIFactory> factory;if(FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))return 0;
    ComPtr<IDXGIAdapter> adapter;if(FAILED(factory->EnumAdapters(0,&adapter)))return 0;
    ComPtr<IDXGIOutput> output;if(FAILED(adapter->EnumOutputs(0,&output)))return 0;UINT count=0;
    if(FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,0,&count,nullptr))||!count)return 0;
    std::vector<DXGI_MODE_DESC> modes(count);if(FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,0,&count,modes.data())))return 0;
    for(const auto& mode:modes){CGxFormat f;f.m_size.x=mode.Width;f.m_size.y=mode.Height;f.m_colorFormat=CGxFormat::Fmt_Argb8888;
        f.m_depthFormat=CGxFormat::Fmt_Ds248;f.m_refreshRate=mode.RefreshRate.Denominator?mode.RefreshRate.Numerator/mode.RefreshRate.Denominator:0;
        f.m_hwTnL=true;f.m_multisampleCount=1;*formats.New()=f;
    }return formats.Count()!=0;
}
