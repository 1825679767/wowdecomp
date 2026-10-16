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
    HMODULE signer=LoadLibraryExW(L"dxbcSigner.dll",nullptr,LOAD_LIBRARY_SEARCH_APPLICATION_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    const bool signedShaders=signer&&GetProcAddress(signer,"SignDxbc");if(signer)FreeLibrary(signer);
    if(!signedShaders){Log("DX11 requires the matching AMD64 dxbcSigner.dll next to this executable");return false;}
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
    // Preserve Wrath's legacy pixel/texel contract. The converter applies the
    // D3D9-to-D3D11 half-pixel adjustment after native UI/font projection setup.
    m_caps.m_colorFormat=GxCF_argb;m_caps.m_pixelCenterOnEdge=0;m_caps.m_texelCenterOnEdge=1;
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
    if(!Resize(m->width,m->height))return false;
    if(!m->offscreen){
        if(!s_uiVertexShader[0])ShaderCreate(s_uiVertexShader,GxSh_Vertex,"Shaders\\Vertex","UI",2);
        if(!s_uiPixelShader)ShaderCreate(&s_uiPixelShader,GxSh_Pixel,"Shaders\\Pixel","UI",1);
        if(!s_uiVertexShader[0]||!s_uiVertexShader[1]||!s_uiPixelShader){Log("DX11 requires the complete mounted native UI shader family");return false;}
        ICursorCreate(m_format);
    }
    Log("DX11 native device initialized FL=%x WARP=%u size=%ux%u samples=%u",m->featureLevel,m->warp,m->width,m->height,m->samples);
    if(m_log)SLogFlush(m_log);
    return true;
}

bool CGxDeviceD3d11::CreateOffscreen(uint32_t width,uint32_t height,bool warp){
    if(!width||!height||width>16384||height>16384||m->device)return false;
    m->offscreen=true;m->width=width;m->height=height;C2iVector size;size.x=width;size.y=height;
    m_format=CGxFormat(true,size,CGxFormat::Fmt_Argb8888,CGxFormat::Fmt_Ds248,0,0,true,false,false,true,0);
    if(!Initialize(warp)){DeviceDestroy();return false;}return true;
}

bool CGxDeviceD3d11::Resize(uint32_t width,uint32_t height){
    if(!m->device||!width||!height||width>16384||height>16384)return false;
    m_context=0;
    m->UnbindViews();m->savedDepth.Reset();m->depthOverride.Reset();m->colorView.Reset();m->depthView.Reset();m->depthSrv.Reset();
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
    m->depthStorage=DXGI_FORMAT_R24G8_TYPELESS;m->depthTargetFormat=DXGI_FORMAT_D24_UNORM_S8_UINT;m->depthSampleFormat=DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    if(m_format.m_depthFormat==CGxFormat::Fmt_Ds160){m->depthStorage=DXGI_FORMAT_R16_TYPELESS;m->depthTargetFormat=DXGI_FORMAT_D16_UNORM;m->depthSampleFormat=DXGI_FORMAT_R16_UNORM;}
    else if(m_format.m_depthFormat==CGxFormat::Fmt_Ds320){m->depthStorage=DXGI_FORMAT_R32_TYPELESS;m->depthTargetFormat=DXGI_FORMAT_D32_FLOAT;m->depthSampleFormat=DXGI_FORMAT_R32_FLOAT;}
    d.Format=m->depthStorage;d.BindFlags=D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
    if(!Check(m->device->CreateTexture2D(&d,nullptr,&m->depth),"depth target"))return false;
    D3D11_DEPTH_STENCIL_VIEW_DESC dd{};dd.Format=m->depthTargetFormat;
    dd.ViewDimension=m->samples>1?D3D11_DSV_DIMENSION_TEXTURE2DMS:D3D11_DSV_DIMENSION_TEXTURE2D;
    if(!Check(m->device->CreateDepthStencilView(m->depth.Get(),&dd,&m->depthView),"depth view"))return false;
    D3D11_SHADER_RESOURCE_VIEW_DESC sr{};sr.Format=m->depthSampleFormat;
    sr.ViewDimension=m->samples>1?D3D11_SRV_DIMENSION_TEXTURE2DMS:D3D11_SRV_DIMENSION_TEXTURE2D;sr.Texture2D.MipLevels=1;
    if(!Check(m->device->CreateShaderResourceView(m->depth.Get(),&sr,&m->depthSrv),"depth sample view"))return false;
    CRect rect{};rect.maxX=float(width);rect.maxY=float(height);DeviceSetDefWindow(rect);DeviceSetCurWindow(rect);
    if(auto* target=m_textureTarget[GxBuffers_Color].m_texture){rect.maxX=float(target->m_width);rect.maxY=float(target->m_height);DeviceSetCurWindow(rect);}
    m_format.m_size.x=width;m_format.m_size.y=height;m_context=1;m_windowVisible=1;m_windowFocus=1;
    IRsForceUpdate();m_viewportDirty=1;m_scissorRectDirty=1;return BindTargets();
}
bool CGxDeviceD3d11::RecoverDevice(){
    if(!m->device)return false;m_context=0;
    const HWND window=m->window;const bool own=m->ownWindow,offscreen=m->offscreen,warp=m->warp;
    const uint32_t width=m->width,height=m->height;
    const auto focused=m_windowFocus,visible=m_windowVisible;
    m->RestoreGamma();m->context->ClearState();m->context->Flush();
    for(auto& [texture,data]:m->textures){texture->m_apiSpecificData=nullptr;texture->m_apiSpecificData2=nullptr;texture->m_needsCreation=1;texture->m_needsUpdate=1;}
    auto pools=std::move(m->pools);for(auto& [pool,data]:pools){pool->m_apiSpecific=nullptr;data.gpu.Reset();data.dirtyBegin=0;data.dirtyEnd=uint32_t(data.cpu.size());}
    for(uint32_t stage=0;stage<6;++stage)for(auto* s=m_shaderList[stage].Head();s;s=m_shaderList[stage].Next(s)){
        s->m_apiSpecific=nullptr;s->m_loaded=0;s->m_valid=0;
    }
    for(auto* q=m_queryList.Head();q;q=m_queryList.Next(q))if(q->m_apiSpecific){static_cast<IUnknown*>(q->m_apiSpecific)->Release();q->m_apiSpecific=nullptr;}
    auto sources=std::move(m->shaderSources);
    if(m->swapchain)m->swapchain->SetFullscreenState(FALSE,nullptr);
    m.reset(new Impl(this));m->window=window;m->ownWindow=own;m->offscreen=offscreen;m->warp=warp;m->width=width;m->height=height;
    m->pools=std::move(pools);m->shaderSources=std::move(sources);
    if(!Initialize(warp)){Log("DX11 device recovery failed; rendering remains unavailable");m_context=0;return false;}
    m_windowFocus=focused;m_windowVisible=visible;
    m->ApplyGamma();NotifyOnDeviceRestored();NotifyOnTextureRecreation();Log("DX11 device recovered with native owners preserved");return true;
}

LRESULT CALLBACK CGxDeviceD3d11::WindowProc(HWND window,UINT msg,WPARAM wp,LPARAM lp){
    auto* self=reinterpret_cast<CGxDeviceD3d11*>(GetWindowLongPtrW(window,GWLP_USERDATA));
    if(msg==WM_NCCREATE){self=static_cast<CGxDeviceD3d11*>(reinterpret_cast<CREATESTRUCTA*>(lp)->lpCreateParams);SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(self));return DefWindowProcA(window,msg,wp,lp);}
    if(self){
        if(msg==WM_CREATE)return 0;
        if(msg==WM_SIZE){CRect rect{};rect.maxX=LOWORD(lp);rect.maxY=HIWORD(lp);self->DeviceWM(GxWM_Size,reinterpret_cast<uintptr_t>(&rect),wp==SIZE_MINIMIZED?1:0);}
        else if(msg==WM_SETFOCUS){self->DeviceWM(GxWM_SetFocus,0,0);return 0;}
        else if(msg==WM_KILLFOCUS){self->DeviceWM(GxWM_KillFocus,0,0);return 0;}
        else if(msg==WM_DESTROY){self->DeviceWM(GxWM_Destroy,0,0);return 0;}
        else if(msg==WM_PAINT){PAINTSTRUCT paint{};BeginPaint(window,&paint);EndPaint(window,&paint);return 0;}
        else if(msg==WM_ERASEBKGND)return 1;
        else if(msg==WM_SETCURSOR&&LOWORD(lp)==HTCLIENT){SetCursor(nullptr);return TRUE;} // Native software cursor owns the client area.
        if(self->m_windowProc)return self->m_windowProc(window,msg,wp,lp);
    }
    return DefWindowProcA(window,msg,wp,lp);
}
int32_t CGxDeviceD3d11::DeviceCreate(GxWindowProc proc,const CGxFormat& format){
    m_windowProc=proc;m_format=format;m->ownWindow=true;
    // The native GxWindowProc contract and its DefWindowProc use ANSI messages.
    const auto instance=GetModuleHandleA(nullptr);WNDCLASSEXA wc{};wc.cbSize=sizeof(wc);wc.hInstance=instance;
    wc.lpfnWndProc=WindowProc;wc.lpszClassName="WxlNativeGxD3d11";wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
    if(!RegisterClassExA(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return 0;
    DWORD style=format.m_window?WS_OVERLAPPEDWINDOW:WS_POPUP;
    RECT rect{0,0,format.m_size.x,format.m_size.y};AdjustWindowRectEx(&rect,style,FALSE,0);
    m->window=CreateWindowExA(0,wc.lpszClassName,"World of Warcraft",style,CW_USEDEFAULT,CW_USEDEFAULT,
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
    m->RestoreGamma();
    CGxDevice::DeviceSetFormat(format);if(!m->device)return 0;
    m->changingFormat=true;
    if(m->window&&m->ownWindow){
        if(m->swapchain)m->swapchain->SetFullscreenState(FALSE,nullptr);
        const DWORD style=format.m_window?WS_OVERLAPPEDWINDOW:WS_POPUP;
        SetWindowLongPtrW(m->window,GWL_STYLE,style);RECT rect{0,0,format.m_size.x,format.m_size.y};
        AdjustWindowRectEx(&rect,style,FALSE,0);
        SetWindowPos(m->window,nullptr,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_NOMOVE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOACTIVATE);
    }
    m->changingFormat=false;
    if(m->swapchain&&!format.m_window){
        DXGI_MODE_DESC mode{};mode.Width=format.m_size.x;mode.Height=format.m_size.y;
        mode.Format=DXGI_FORMAT_R8G8B8A8_UNORM;mode.RefreshRate.Numerator=format.m_refreshRate;mode.RefreshRate.Denominator=1;
        if(!Check(m->swapchain->ResizeTarget(&mode),"fullscreen display mode"))return 0;
    }
    if(m->swapchain&&!Check(m->swapchain->SetFullscreenState(!format.m_window,nullptr),"fullscreen change"))return 0;
    const bool result=Resize(format.m_size.x,format.m_size.y);if(result){m->ApplyGamma();NotifyOnDeviceRestored();}return result;
}
void CGxDeviceD3d11::DeviceDestroy(){
    if(!m)return;
    if(!m->offscreen){
        for(auto& shader:s_uiVertexShader)if(shader)ShaderDestroy(shader);
        if(s_uiPixelShader)ShaderDestroy(s_uiPixelShader);
        if(m_cursorTexture){auto* cursor=m_cursorTexture;m_cursorTexture=nullptr;TexDestroy(cursor);}
    }
    m_context=0;
    m->RestoreGamma();
    if(m->context){m->context->ClearState();m->context->Flush();}
    for(auto& [t,v]:m->textures){t->m_apiSpecificData=nullptr;t->m_apiSpecificData2=nullptr;t->m_needsCreation=1;t->m_needsUpdate=1;}
    for(auto& [p,v]:m->pools)p->m_apiSpecific=nullptr;
    for(uint32_t stage=0;stage<6;++stage)for(auto* s=m_shaderList[stage].Head();s;s=m_shaderList[stage].Next(s)){
        s->m_apiSpecific=nullptr;s->m_loaded=0;s->m_valid=0;
    }
    for(auto* q=m_queryList.Head();q;q=m_queryList.Next(q))if(q->m_apiSpecific){static_cast<IUnknown*>(q->m_apiSpecific)->Release();q->m_apiSpecific=nullptr;}
    HWND window=m->window;const bool own=m->ownWindow;
    if(m->swapchain)m->swapchain->SetFullscreenState(FALSE,nullptr);
    // The engine destroys its input/IME owners before ConsoleDeviceDestroy.
    // DestroyWindow can synchronously send IME/focus messages; detach the dead
    // native input callback before releasing our owned window.
    m_windowProc=nullptr;
    m.reset(new Impl(this));if(window&&own){SetWindowLongPtrA(window,GWLP_USERDATA,0);DestroyWindow(window);}CGxDevice::DeviceDestroy();
}
void CGxDeviceD3d11::DeviceSetBaseMipLevel(uint32_t level){
    if(level==m_baseMipLevel)return;CGxDevice::DeviceSetBaseMipLevel(level);
    if(m->context)m->UnbindViews();
    for(auto i=m->textures.begin();i!=m->textures.end();){auto* t=i->first;
        if(!t->m_flags.m_renderTarget){t->m_apiSpecificData=nullptr;t->m_apiSpecificData2=nullptr;t->m_needsCreation=1;t->m_needsUpdate=1;i=m->textures.erase(i);}else ++i;
    }
    NotifyOnTextureRecreation();if(m_context)BindTargets();
}
void* CGxDeviceD3d11::DeviceWindow(){return m->window;}
int32_t CGxDeviceD3d11::DeviceWM(EGxWM wm,uintptr_t a,uintptr_t b){
    if(wm==GxWM_SetFocus){m_windowFocus=1;m->ApplyGamma();}
    else if(wm==GxWM_KillFocus||wm==GxWM_Destroy){m_windowFocus=0;m->RestoreGamma();}
    else if(wm==GxWM_Size||wm==GxWM_DisplayChange){
        if(m->changingFormat)return 1;
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
void CGxDeviceD3d11::LogCrashInfo(char* out,uint32_t n){if(out&&n)std::snprintf(out,n,"Native DX11 FL=%x WARP=%d %ux%u MSAA=%u frames=%llu draws=%llu variants=%zu",m->featureLevel,m->warp,m->width,m->height,m->samples,m->presentedFrames,m->drawCalls,m->variants.size());}
void CGxDeviceD3d11::ScenePresent(){
    if(!m_context)return;
    if(m_scrShotClick){CiRect r(0,0,m->height,m->width);DeviceReadPixels(r,m_scrShotPixels);m_scrShotWidth=m->width;m_scrShotHeight=m->height;}
    ICursorDraw();
    if(m_format.m_fixLag){
        if(!m->frameFence){D3D11_QUERY_DESC d{D3D11_QUERY_EVENT,0};Check(m->device->CreateQuery(&d,&m->frameFence),"frame fence");}
        if(m->frameFence){m->context->End(m->frameFence.Get());m->context->Flush();const ULONGLONG begin=GetTickCount64();
            while(m->context->GetData(m->frameFence.Get(),nullptr,0,0)==S_FALSE&&GetTickCount64()-begin<5000)Sleep(0);}
    }
    if(m->swapchain){if(m->samples>1)m->context->ResolveSubresource(m->backbuffer.Get(),0,m->color.Get(),0,DXGI_FORMAT_R8G8B8A8_UNORM);
        const HRESULT hr=m->swapchain->Present(m_format.m_vsync?1:0,0);
        if(SUCCEEDED(hr)&&++m->presentedFrames==1){Log("DX11 first native Present succeeded; draws=%llu variants=%zu",m->drawCalls,m->variants.size());if(m_log)SLogFlush(m_log);}
        if(FAILED(hr)){Check(hr,"Present");m_context=0;if(hr==DXGI_ERROR_DEVICE_REMOVED||hr==DXGI_ERROR_DEVICE_RESET)RecoverDevice();}
    }
    CGxDevice::ScenePresent();WaitForFPSCap();
}
void CGxDeviceD3d11::SceneClear(uint32_t mask,CImVector color){
    if(!m_context||!BindTargets())return;
    if(mask&1){const float c[]={color.r/255.f,color.g/255.f,color.b/255.f,color.a/255.f};m->context->ClearRenderTargetView(m->CurrentColor(),c);
        if(auto* t=m_textureTarget[GxBuffers_Color].m_texture)m->textures[t].mipGenerationDirty=true;}
    if(mask&6){auto* d=m->CurrentDepth();if(d){D3D11_DEPTH_STENCIL_VIEW_DESC view{};d->GetDesc(&view);
        const UINT flags=((mask&2)?D3D11_CLEAR_DEPTH:0)|((mask&4)&&view.Format==DXGI_FORMAT_D24_UNORM_S8_UINT?D3D11_CLEAR_STENCIL:0);
        if(flags)m->context->ClearDepthStencilView(d,flags,1,0);}}
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
void CGxDeviceD3d11::Impl::RestoreGamma(){
    if(gammaCaptured&&gammaOutput)Check(gammaOutput->SetGammaControl(&originalGamma),"restore fullscreen gamma");
    gammaCaptured=false;gammaOutput.Reset();
}
void CGxDeviceD3d11::Impl::ApplyGamma(){
    if(!swapchain||self->m_format.m_window||!self->m_windowFocus)return;
    if(!gammaOutput&&!Check(swapchain->GetContainingOutput(&gammaOutput),"gamma output"))return;
    DXGI_GAMMA_CONTROL_CAPABILITIES caps{};
    if(!Check(gammaOutput->GetGammaControlCapabilities(&caps),"gamma capabilities")||!caps.NumGammaControlPoints||caps.NumGammaControlPoints>1025)return;
    if(!gammaCaptured){if(!Check(gammaOutput->GetGammaControl(&originalGamma),"capture fullscreen gamma"))return;gammaCaptured=true;}
    DXGI_GAMMA_CONTROL gamma{};gamma.Scale={1,1,1};gamma.Offset={0,0,0};
    for(UINT i=0;i<caps.NumGammaControlPoints;++i){const float p=std::max(0.f,std::min(caps.ControlPointPositions[i],1.f))*255;
        const UINT at=std::min(UINT(p),255u),next=std::min(at+1,255u);const float fraction=p-at;
        auto sample=[&](const uint16_t* channel){return (channel[at]+(channel[next]-float(channel[at]))*fraction)/65535.f;};
        gamma.GammaCurve[i]={sample(self->m_gammaRamp.m_red),sample(self->m_gammaRamp.m_green),sample(self->m_gammaRamp.m_blue)};
    }
    Check(gammaOutput->SetGammaControl(&gamma),"fullscreen gamma");
}
void CGxDeviceD3d11::DeviceSetGamma(float gamma){if(!std::isfinite(gamma)||gamma<=0)return;CGxDevice::DeviceSetGamma(gamma);m->ApplyGamma();}
void CGxDeviceD3d11::DeviceSetGamma(const CGxGammaRamp& ramp){CGxDevice::DeviceSetGamma(ramp);m->ApplyGamma();}
void CGxDeviceD3d11::DeviceOverride(EGxOverride setting,uint32_t value){
    CGxDevice::DeviceOverride(setting,value);
    if(setting==GxOverride_PixelShader&&value<=4)m_caps.m_shaderTargets[GxSh_Pixel]=int32_t(value);
    else if(setting==GxOverride_Unk7)m_caps.m_texNonPow2=value!=0;
}
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
    ComPtr<ID3D11Device> device;D3D_FEATURE_LEVEL obtained;const D3D_FEATURE_LEVEL required[]={D3D_FEATURE_LEVEL_11_0};
    if(FAILED(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,required,1,D3D11_SDK_VERSION,&device,&obtained,nullptr)))return 0;
    const CGxFormat::Format depths[]={CGxFormat::Fmt_Ds160,CGxFormat::Fmt_Ds24X,CGxFormat::Fmt_Ds248,CGxFormat::Fmt_Ds320};
    for(const auto& mode:modes)for(auto depth:depths)for(UINT samples:{1u,2u,4u,8u,16u}){
        const DXGI_FORMAT depthFormat=depth==CGxFormat::Fmt_Ds160?DXGI_FORMAT_D16_UNORM:(depth==CGxFormat::Fmt_Ds320?DXGI_FORMAT_D32_FLOAT:DXGI_FORMAT_D24_UNORM_S8_UINT);
        UINT colorQuality=0,depthQuality=0;
        if(FAILED(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,samples,&colorQuality))||!colorQuality||
           FAILED(device->CheckMultisampleQualityLevels(depthFormat,samples,&depthQuality))||!depthQuality)continue;
        C2iVector size;size.x=mode.Width;size.y=mode.Height;
        CGxFormat f(false,size,CGxFormat::Fmt_Argb8888,depth,
            mode.RefreshRate.Denominator?mode.RefreshRate.Numerator/mode.RefreshRate.Denominator:0,0,true,false,false,true,0);
        f.m_multisampleCount=samples;*formats.New()=f;
    }return formats.Count()!=0;
}
