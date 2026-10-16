#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Os/TimeManager.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

static uint32_t checks=0;
static bool inputRetired=false;
static uint32_t retiredCallbacks=0;
static intptr_t NativeWindowCallback(void* window,uint32_t message,uintptr_t wparam,intptr_t lparam){
    if(inputRetired)++retiredCallbacks;
    return DefWindowProcA(static_cast<HWND>(window),message,wparam,lparam);
}
static LONG WINAPI TraceCrash(EXCEPTION_POINTERS* info){
    CONTEXT frame=*info->ContextRecord;
    std::fprintf(stderr,"exception=%08lx checks=%u\n",info->ExceptionRecord->ExceptionCode,checks);
    for(uint32_t i=0;i<20&&frame.Rip;++i){
        HMODULE module=nullptr;char name[MAX_PATH]{};
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(frame.Rip),&module);
        if(module)GetModuleFileNameA(module,name,MAX_PATH);
        std::fprintf(stderr,"%s+0x%llx\n",name,frame.Rip-reinterpret_cast<uintptr_t>(module));
        DWORD64 base=0;auto* entry=RtlLookupFunctionEntry(frame.Rip,&base,nullptr);
        if(entry){PVOID handler=nullptr;DWORD64 establisher=0;RtlVirtualUnwind(UNW_FLAG_NHANDLER,base,frame.Rip,entry,&frame,&handler,&establisher,nullptr);}
        else{frame.Rip=*reinterpret_cast<DWORD64*>(frame.Rsp);frame.Rsp+=8;}
    }
    std::fflush(stderr);return EXCEPTION_EXECUTE_HANDLER;
}
static void Check(bool ok,const char* text){++checks;if(!ok)throw std::runtime_error(text);}
static uint32_t Read(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
static std::unique_ptr<CGxShader> Load(const char* path,EGxShTarget target,uint32_t permutation=0){
    const auto size=std::filesystem::file_size(path);Check(size>=12&&size<32*1024*1024,"shader file size");
    std::vector<uint8_t> raw(static_cast<size_t>(size));std::ifstream stream(path,std::ios::binary);
    Check(bool(stream.read(reinterpret_cast<char*>(raw.data()),raw.size())),"shader file read");
    Check(Read(raw.data())==0x47585348&&Read(raw.data()+4)==0x10003&&permutation<Read(raw.data()+8),"legacy container header");
    size_t pos=12;for(uint32_t i=0;i<permutation;++i){Check(pos+16<=raw.size(),"permutation skip header");const auto n=Read(raw.data()+pos+12);pos+=16+((size_t(n)+3)&~size_t(3));}
    Check(pos+16<=raw.size(),"permutation header");const auto n=Read(raw.data()+pos+12);Check(n&&n<=raw.size()-pos-16,"permutation bytecode bounds");
    auto shader=std::make_unique<CGxShader>();shader->m_target=target;shader->m_code.SetCount(n);std::memcpy(shader->m_code.Ptr(),raw.data()+pos+16,n);return shader;
}
struct Pixels {
    uint32_t width=1,height=1;
    std::vector<CImVector> data;
};
static void TextureData(EGxTexCommand cmd,uint32_t,uint32_t,uint32_t,uint32_t,void* arg,uint32_t& stride,const void*& data){
    auto& p=*static_cast<Pixels*>(arg);stride=p.width*4;data=cmd==GxTex_Latch?p.data.data():nullptr;
}
static void CompressedData(EGxTexCommand cmd,uint32_t,uint32_t,uint32_t,uint32_t,void*,uint32_t& stride,const void*& data){
    static const uint8_t redBlock[]={0,248,0,0,0,0,0,0};stride=8;data=cmd==GxTex_Latch?redBlock:nullptr;
}
static CGxTex* Texture(CGxDeviceD3d11& device,Pixels& pixels,bool target=false){
    CGxTexFlags flags(GxTex_Nearest,0,0,0,0,target?1:0);CGxTex* tex=nullptr;
    Check(device.TexCreate(GxTex_2d,pixels.width,pixels.height,1,GxTex_Argb8888,GxTex_Argb8888,flags,&pixels,TextureData,"DX11 test texture",tex)!=0,"create native texture");return tex;
}
struct Vertex {float x,y,z;CImVector color;float u,v;};
static_assert(sizeof(Vertex)==24,"native PCT layout");
static void Triangle(CGxDeviceD3d11& device,CImVector color,float z=.25f,bool indexed=false){
    const Vertex vertices[]={{-1,-1,z,color,.5f,.5f},{-1,3,z,color,.5f,.5f},{3,-1,z,color,.5f,.5f}};
    auto* v=device.BufStream(GxPoolTarget_Vertex,sizeof(Vertex),3);auto* data=device.BufLock(v);
    Check(data!=nullptr,"native vertex lock");std::memcpy(data,vertices,sizeof(vertices));Check(device.BufUnlock(v,0)!=0,"native whole-buffer vertex unlock (zero size)");
    device.PrimVertexPtr(v,GxVBF_PCT);CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_count=3;batch.m_minIndex=0;batch.m_maxIndex=2;
    if(indexed){const uint16_t indices[]={0,1,2};auto* i=device.BufStream(GxPoolTarget_Index,2,3);auto* out=device.BufLock(i);
        Check(out!=nullptr,"native index lock");std::memcpy(out,indices,sizeof(indices));Check(device.BufUnlock(i,0)!=0,"native whole-buffer index unlock (zero size)");device.PrimIndexPtr(i);}
    device.Draw(&batch,indexed?1:0);
}
static CImVector Center(CGxDeviceD3d11& device,uint32_t width=16,uint32_t height=16){
    CiRect rect(0,0,height,width);TSGrowableArray<CImVector> pixels;device.ReadTargetPixels(rect,pixels);
    Check(pixels.Count()==width*height,"native color readback count");return pixels[(height/2)*width+width/2];
}
static void Equal(CImVector actual,CImVector expected,const char* name,int tolerance=2){
    if(std::abs(int(actual.r)-expected.r)>tolerance||std::abs(int(actual.g)-expected.g)>tolerance||
       std::abs(int(actual.b)-expected.b)>tolerance||std::abs(int(actual.a)-expected.a)>tolerance){
        std::cerr<<name<<" actual RGBA="<<int(actual.r)<<','<<int(actual.g)<<','<<int(actual.b)<<','<<int(actual.a)
                 <<" expected="<<int(expected.r)<<','<<int(expected.g)<<','<<int(expected.b)<<','<<int(expected.a)<<'\n';Check(false,name);}
    Check(true,name);
}
#include "WxlMaterialChecks.h"
int main(int argc,char** argv) try {
    SetUnhandledExceptionFilter(TraceCrash);
    std::cerr<<"native DX11 backend tests: initialize\n"<<std::flush;
    StormRtlInitialize();
    std::filesystem::create_directories("Logs");CGxDevice::LogOpen();
    OsTimeManagerCreate(OsTimeManager::SystemMethod2);
    Check(argc==3||argc==4,"pass extracted native UI vertex and pixel containers, optional hardware");
    CGxDeviceD3d11 device;g_theGxDevicePtr=&device;
    std::cerr<<"native device constructed\n"<<std::flush;
    Check(device.CreateOffscreen(16,16,argc==3),"native backend WARP creation");
    std::cerr<<"native offscreen device ready\n"<<std::flush;
    device.RsSet(GxRs_DepthTest,0);device.RsSet(GxRs_DepthWrite,0);device.RsSet(GxRs_Culling,0);
    device.RsSet(GxRs_ScissorTest,0);device.RsSet(GxRs_BlendingMode,int32_t(GxBlend_Opaque));device.RsSet(GxRs_AlphaRef,0);
    device.RsSet(GxRs_ColorWrite,15);device.RsSet(GxRs_Fog,0);device.RsSet(GxRs_Lighting,0);
    C44Matrix identity;device.XformSetProjection(identity);
    const CImVector clear(31,97,151,255);device.SceneClear(3,clear);Equal(Center(device),clear,"clear/readback");
    const CImVector vertexColor(47,103,211,255);
    device.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));device.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));
    Triangle(device,vertexColor);Equal(Center(device),vertexColor,"fixed-function vertex color/BGRA");
    const auto fixture=std::filesystem::current_path()/("native-shader-test-fixture-"+std::to_string(GetTickCount64()));
    const auto vertexFile=fixture/"Shaders/Vertex/vs_3_0/UI.bls";
    const auto pixelFile=fixture/"Shaders/Pixel/ps_2_0/UI.bls";
    std::filesystem::create_directories(vertexFile.parent_path());std::filesystem::create_directories(pixelFile.parent_path());
    std::filesystem::create_directories(fixture/"Shaders/Pixel/ps_3_0");
    std::filesystem::copy_file(argv[1],vertexFile);std::filesystem::copy_file(argv[2],pixelFile);
    std::ofstream(fixture/"Shaders/Pixel/ps_3_0/UI.bls",std::ios::binary).close();
    SFile::Initialize();SFile::EnableDirectAccess(1);SFile::SetBasePath(fixture.string().c_str());SFile::SetReadFatal(false);
    CGxShader* mounted[2]{};CGxShader* duplicate[2]{};CGxShader* mountedPixel=nullptr;
    device.ShaderCreate(mounted,GxSh_Vertex,"Shaders\\Vertex","UI",2);
    Check(mounted[0]&&mounted[1]&&mounted[0]!=mounted[1],"mounted native permutation owner family");
    Check(mounted[0]->m_code.Count()&&mounted[1]->m_code.Count(),"mounted native permutation payloads");
    device.ShaderCreate(duplicate,GxSh_Vertex,"Shaders\\Vertex","UI",2);
    Check(duplicate[0]==mounted[0]&&duplicate[1]==mounted[1]&&mounted[0]->m_refCount==2,"backend family cache reference ownership");
    device.ShaderCreate(&mountedPixel,GxSh_Pixel,"Shaders\\Pixel","UI",1);
    Check(mountedPixel&&mountedPixel->m_code.Count(),"empty profile falls back to mounted ps_2_0");
    const auto originalSize=mounted[1]->m_code.Count();std::vector<uint8_t> original(mounted[1]->m_code.Ptr(),mounted[1]->m_code.Ptr()+originalSize);
    {std::ofstream broken(vertexFile,std::ios::binary|std::ios::trunc);broken<<"bad";}
    device.IShaderReload(mounted[1],"Shaders\\Vertex","UI");
    Check(mounted[1]->m_code.Count()==originalSize&&!std::memcmp(original.data(),mounted[1]->m_code.Ptr(),originalSize),"failed mounted reload preserves prior code");
    CGxShader* refused[1]{mounted[0]};device.ShaderCreate(refused,GxSh_Vertex,"Shaders\\Vertex","UI",1);
    Check(!refused[0],"malformed mounted family fails closed");
    std::filesystem::copy_file(argv[1],vertexFile,std::filesystem::copy_options::overwrite_existing);
    device.IShaderReload(mounted[1],"Shaders\\Vertex","UI");
    Check(mounted[1]->m_code.Count()==originalSize&&!std::memcmp(original.data(),mounted[1]->m_code.Ptr(),originalSize),"reload retains permutation index");
    device.ShaderCreate(refused,GxSh_Vertex,"Shaders\\Vertex","UI",1);Check(!refused[0],"exact family count required");
    for(auto& shader:duplicate)device.ShaderDestroy(shader);
    for(auto& shader:mounted)device.ShaderDestroy(shader);device.ShaderDestroy(mountedPixel);
    SFile::EnableDirectAccess(0);
    auto vs=Load(argv[1],GxSh_Vertex);auto ps=Load(argv[2],GxSh_Pixel);
    device.IShaderCreate(vs.get());device.IShaderCreate(ps.get());Check(vs->m_valid&&ps->m_valid,"native shader Valid/API creation");
    device.RsSet(GxRs_VertexShader,vs.get());device.RsSet(GxRs_PixelShader,ps.get());
    device.ShaderConstantsSet(GxSh_Vertex,0,reinterpret_cast<const float*>(&identity),4);
    Pixels data;data.data={CImVector(207,99,17,255)};auto* texture=Texture(device,data);
    device.RsSet(GxRs_Texture0,texture);device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255),.25f,true);
    Equal(Center(device),data.data[0],"actual native UI texture/BGRA/indexed draw");
    for(uint32_t dimension:{1u,2u,4u}){
        CGxTexFlags flags(GxTex_Nearest,0,0,0,0,0);CGxTex* compressed=nullptr;
        Check(device.TexCreate(GxTex_2d,dimension,dimension,1,GxTex_Dxt1,GxTex_Dxt1,flags,nullptr,CompressedData,"small DXT1",compressed)!=0,"legacy compressed texture owner");
        device.RsSet(GxRs_Texture0,compressed);device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
        Equal(Center(device),CImVector(0,0,dimension<4?248:255,255),"compressed texture/native CPU decode at sub-block dimensions");device.TexDestroy(compressed);
    }
    device.RsSet(GxRs_Texture0,texture);
    auto alt=Load(argv[1],GxSh_Vertex,1);device.RsSet(GxRs_VertexShader,alt.get());
    device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));Equal(Center(device),data.data[0],"second native UI permutation");
    device.RsSet(GxRs_VertexShader,vs.get());data.data[0].a=64;CiRect full(0,0,1,1);device.TexMarkForUpdate(texture,full,1);
    device.RsSet(GxRs_AlphaRef,128);device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));Equal(Center(device),clear,"translated native alpha rejection");
    device.RsSet(GxRs_AlphaRef,0);data.data[0]=CImVector(191,127,63,127);device.TexMarkForUpdate(texture,full,1);
    device.RsSet(GxRs_BlendingMode,int32_t(GxBlend_Screen));device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
    auto screen=[](int s,int d){return uint8_t(std::min(255,(s*(255-d)+127)/255+d));};
    Equal(Center(device),CImVector(screen(191,31),screen(127,97),screen(63,151),255),"particle Screen equation");
    device.RsSet(GxRs_BlendingMode,int32_t(GxBlend_BlendAdd));device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
    auto blend=[](int s,int d){return uint8_t(std::min(255,s+(d*128+127)/255));};
    Equal(Center(device),CImVector(blend(191,31),blend(127,97),blend(63,151),255),"mesh BlendAdd equation");
    device.RsSet(GxRs_BlendingMode,int32_t(GxBlend_Opaque));device.RsSet(GxRs_DepthWrite,1);device.RsSet(GxRs_DepthTest,1);device.RsSet(GxRs_DepthFunc,0);
    device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255),.25f);
    CiRect region(0,0,16,16);TSGrowableArray<float> depth;device.DeviceReadDepths(region,depth);
    Check(depth.Count()==256&&std::abs(depth[136]-.25f)<0.00001f,"depth render/readback");
    device.RsSet(GxRs_DepthWrite,0);device.RsSet(GxRs_DepthTest,0);
    Pixels renderPixels;renderPixels.width=8;renderPixels.height=8;auto* target=Texture(device,renderPixels,true);
    device.DeviceSetRenderTarget(GxBuffers_Color,target,0);device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
    Equal(Center(device,8,8),data.data[0],"native render-target switch");
    device.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);device.SceneClear(3,clear);
    C2iVector zero,size;zero.x=0;zero.y=0;size.x=8;size.y=8;Check(device.TexCopy(target,zero,zero,size,0,0)!=0,"native rectangle copy");
    device.DeviceSetRenderTarget(GxBuffers_Color,target,0);Equal(Center(device,8,8),clear,"rectangle copy content");device.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);
    CGxQuery* query=nullptr;device.QueryCreate(query,GxQueryType_Occlusion);Check(device.QueryBegin(query),"occlusion begin");
    Triangle(device,CImVector(255,255,255,255));Check(device.QueryEnd(query),"occlusion end");Center(device);uint32_t samples=0;
    Check(device.QueryGetData(query,samples)&&samples>0,"occlusion result");device.QueryDestroy(query);
    // Native write-mask channel order is R,B,G,A.
    device.RsSet(GxRs_ColorWrite,2);device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
    Equal(Center(device),CImVector(data.data[0].b,clear.g,clear.r,clear.a),"blue-only native color mask");
    device.RsSet(GxRs_ColorWrite,15);
    // A real pixel shader may be combined with a generated fixed-function VS.
    device.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));device.SceneClear(3,clear);
    Triangle(device,CImVector(255,255,255,255));Equal(Center(device),data.data[0],"mixed fixed VS/native PS");
    device.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));
    device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));Equal(Center(device),data.data[0],"fixed-function texture cascade");
    device.RsSet(GxRs_Texture0,static_cast<void*>(nullptr));
    CGxLight light;light.m_flags=1;light.m_dir=C3Vector(0,0,-1);light.m_ambientColor=C3Vector(0,0,0);
    light.m_dirColor=C3Vector(.5f,.25f,1);device.LightSet(0,light,C3Vector(0,0,0));device.LightEnable(0,1);
    device.RsSet(GxRs_Lighting,1);device.RsSet(GxRs_ColorMaterial,0);device.RsSet(GxRs_MatEmissive,int32_t(0));
    device.SceneClear(3,clear);Triangle(device,CImVector(255,255,255,255));
    Equal(Center(device),CImVector(255,64,128,255),"fixed-function directional lighting");
    device.RsSet(GxRs_Lighting,0);device.LightEnable(0,0);
    device.RsSet(GxRs_VertexShader,vs.get());device.RsSet(GxRs_PixelShader,ps.get());device.RsSet(GxRs_Texture0,texture);
    // Stretch into an ordinary sampled texture, then copy to a differently sized target.
    Pixels sampled;sampled.width=4;sampled.height=4;sampled.data.resize(16);auto* ordinary=Texture(device,sampled);
    device.SceneClear(3,clear);Check(device.TexStretch(ordinary,nullptr,nullptr,0,0)!=0,"stretch into sampled texture");
    device.RsSet(GxRs_Texture0,ordinary);Triangle(device,CImVector(255,255,255,255));Equal(Center(device),clear,"stretched sampled texture contents");
    device.DeviceCopyTex(ordinary,0,target,0);device.DeviceSetRenderTarget(GxBuffers_Color,target,0);
    Equal(Center(device,8,8),clear,"resized texture-to-texture copy");device.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);
    device.TexDestroy(ordinary);device.RsSet(GxRs_Texture0,texture);
    CGxTexFlags renderFlags(GxTex_Nearest,0,0,0,0,1);CGxTex* cube=nullptr;
    Check(device.TexCreate(GxTex_CubeMap,8,8,1,GxTex_Argb8888,GxTex_Argb8888,renderFlags,nullptr,nullptr,"cube target",cube)!=0,"cube target owner");
    const CImVector cubeColor(83,179,29,255);device.DeviceSetRenderTarget(GxBuffers_Color,cube,4);device.SceneClear(3,cubeColor);
    Equal(Center(device,8,8),cubeColor,"selected cube face rendering/readback");
    CiRect backRect(0,0,16,16);TSGrowableArray<CImVector> backPixels;device.DeviceReadPixels(backRect,backPixels);
    Check(backPixels.Count()==256,"native readback retains backbuffer dimensions while cube target bound");
    Equal(backPixels[136],clear,"native readback retains backbuffer authority");
    size.x=size.y=8;Check(device.TexCopy(target,zero,zero,size,0,0)!=0,"copy selected cube face");
    device.DeviceSetRenderTarget(GxBuffers_Color,target,0);Equal(Center(device,8,8),cubeColor,"selected cube face copy contents");
    device.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);device.TexDestroy(cube);
    CGxTex* scalar=nullptr;Check(device.TexCreate(GxTex_2d,8,8,1,GxTex_R32F,GxTex_R32F,renderFlags,nullptr,nullptr,"float target",scalar)!=0,"float target owner");
    device.DeviceSetRenderTarget(GxBuffers_Color,scalar,0);device.SceneClear(3,clear);
    Equal(Center(device,8,8),CImVector(0,0,clear.r,255),"float target conversion/readback");
    device.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);device.TexDestroy(scalar);
    Check(device.RecoverDevice(),"native device recreation");device.SceneClear(3,clear);
    Triangle(device,CImVector(255,255,255,255),.25f,true);Equal(Center(device),data.data[0],"shader/texture/buffer owners after recovery");
    device.TexDestroy(target);device.TexDestroy(texture);
    auto format=device.m_format;format.m_size.x=8;format.m_size.y=8;Check(device.DeviceSetFormat(format)!=0,"native resize/reset");
    device.SceneClear(3,clear);Equal(Center(device,8,8),clear,"resized target/readback");
    for(auto depthFormat:{CGxFormat::Fmt_Ds160,CGxFormat::Fmt_Ds248,CGxFormat::Fmt_Ds320}){
        format.m_depthFormat=depthFormat;format.m_multisampleCount=4;
        Check(device.DeviceSetFormat(format)!=0,"native MSAA/depth format resize");device.SceneClear(3,clear);
        Equal(Center(device,8,8),clear,"MSAA color resolve/readback");CiRect r(0,0,8,8);TSGrowableArray<float> depths;
        device.DeviceReadDepths(r,depths);Check(depths.Count()==64&&depths[32]==1.f,"MSAA depth clear/readback");
    }
    WxlMaterialChecks(device,vs.get(),ps.get());
    device.DeviceDestroy();g_theGxDevicePtr=nullptr;
    if(argc==4&&(!std::strcmp(argv[3],"window")||!std::strcmp(argv[3],"fullscreen"))){
        SFile::EnableDirectAccess(1);CGxDeviceD3d11 window;g_theGxDevicePtr=&window;
        C2iVector dimensions;dimensions.x=256;dimensions.y=256;
        CGxFormat f(true,dimensions,CGxFormat::Fmt_Argb8888,CGxFormat::Fmt_Ds248,0,0,true,false,false,true,0);
        Check(window.DeviceCreate(NativeWindowCallback,f)!=0,"native hardware window/swapchain creation");
        char title[64]{};GetWindowTextA(static_cast<HWND>(window.DeviceWindow()),title,sizeof(title));
        Check(!std::strcmp(title,"World of Warcraft"),"native ANSI window callback contract");
        window.SceneClear(3,clear);Equal(Center(window,256,256),clear,"native swapchain backbuffer pixels");window.ScenePresent();
        f.m_size.x=128;f.m_size.y=96;Check(window.DeviceSetFormat(f)!=0,"native window/swapchain resize");
        window.SceneClear(3,clear);Equal(Center(window,128,96),clear,"resized swapchain pixels");window.ScenePresent();
        const auto handle=window.DeviceWindow();window.DeviceWM(GxWM_KillFocus,0,0);
        Check(window.RecoverDevice()&&window.DeviceWindow()==handle&&!window.m_windowFocus,"window/owner/focus survive recreation");
        window.SceneClear(3,clear);Equal(Center(window,128,96),clear,"recreated swapchain pixels");window.ScenePresent();
        if(!std::strcmp(argv[3],"fullscreen")){
            const auto hwnd=static_cast<HWND>(window.DeviceWindow());
            ShowWindow(hwnd,SW_RESTORE);SetForegroundWindow(hwnd);SetFocus(hwnd);
            MSG message{};while(PeekMessageA(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageA(&message);}
            DEVMODEW mode{};mode.dmSize=sizeof(mode);Check(EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&mode)!=0,"current desktop mode for bounded exclusive test");
            f.m_window=0;f.m_size.x=mode.dmPelsWidth;f.m_size.y=mode.dmPelsHeight;f.m_refreshRate=mode.dmDisplayFrequency;
            const bool exclusive=GetForegroundWindow()==hwnd&&window.DeviceSetFormat(f)!=0;
            if(!exclusive)std::cerr<<"PENDING exclusive fullscreen/gamma: foreground or display access unavailable; require interactive session\n";
            if(exclusive){
            window.DeviceWM(GxWM_SetFocus,0,0);window.DeviceSetGamma(1.2f);
            window.SceneClear(3,clear);Equal(Center(window,f.m_size.x,f.m_size.y),clear,"exclusive fullscreen target pixels");window.ScenePresent();
            window.DeviceWM(GxWM_KillFocus,0,0);Check(!window.m_windowFocus,"exclusive focus-loss gamma restore path");
            window.DeviceWM(GxWM_SetFocus,0,0);Check(window.m_windowFocus!=0,"exclusive focus-return gamma reapply path");
            window.DeviceSetGamma(1.f);
            }
            f.m_window=1;f.m_size.x=128;f.m_size.y=96;f.m_refreshRate=0;
            Check(window.DeviceSetFormat(f)!=0,"return from exclusive fullscreen to native window");
            window.SceneClear(3,clear);Equal(Center(window,128,96),clear,"post-fullscreen native target pixels");window.ScenePresent();
        }
        inputRetired=true;window.DeviceDestroy();Check(!retiredCallbacks,"window teardown does not call retired native input/IME owner");
        inputRetired=false;g_theGxDevicePtr=nullptr;SFile::EnableDirectAccess(0);
    }
    // Exercise actual GxDevDestroy/WXL lifecycle owners, not test substitutes.
    auto* lifecycle=new CGxDeviceD3d11();g_theGxDevicePtr=lifecycle;
    Check(lifecycle->CreateOffscreen(8,8,true),"owned device for real WXL teardown");
    WxlProgram(*lifecycle,GxSh_Vertex,"WXL_Mesh_AddAlpha21_VS_20261016",kCombinersModAddAlphaVs,sizeof(kCombinersModAddAlphaVs));
    WxlProgram(*lifecycle,GxSh_Pixel,"WXL_Mesh_AddAlpha21_PS_20261016",kCombinersModAddAlphaPs,sizeof(kCombinersModAddAlphaPs));
    WxlProgram(*lifecycle,GxSh_Vertex,"WXL.NativeLayer.Unlit.v1",wxl::modern::particlelayers::programs::Unlit,sizeof(wxl::modern::particlelayers::programs::Unlit));
    WxlProgram(*lifecycle,GxSh_Pixel,"WXL.NativeLayer.TwoColor.v1",wxl::modern::particlelayers::programs::TwoColorThreeAlpha,sizeof(wxl::modern::particlelayers::programs::TwoColorThreeAlpha));
    WxlProgram(*lifecycle,GxSh_Pixel,"WXL_BloodRefraction_PS_20261016",kBloodRefractionShader,sizeof(kBloodRefractionShader));
    GxDevDestroy(lifecycle);Check(!g_theGxDevicePtr,"real WXL native destroy detaches device owner");
    CGxDevice::LogClose();
    std::cout<<"PASS native D3D11 backend checks="<<checks<<" (fixed and native shaders, indexed draw, texture, alpha, blend, depth, targets, copy, query, reset)\n";
    return 0;
}catch(const std::exception& e){CGxDevice::LogClose();std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
