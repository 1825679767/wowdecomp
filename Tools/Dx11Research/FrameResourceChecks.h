// One bounded regression for the actual native backend's uncapped frame path.
// No game, account, MPQ or visible window is opened by this mode.
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
static uint64_t PrivateBytes(){
    PROCESS_MEMORY_COUNTERS_EX memory{};memory.cb=sizeof(memory);
    Check(GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory),sizeof(memory))!=0,"process private-byte observation");
    return memory.PrivateUsage;
}
static int FrameResourceChecks(bool warp){
    CGxDeviceD3d11 device;g_theGxDevicePtr=&device;
    Check(device.CreateOffscreen(16,16,warp),"frame-resource native device");
    device.m_format.m_vsync=0;device.m_format.m_fixLag=0;GxSetMaxFPS(0);GxSetMaxFPSBk(0);
    device.RsSet(GxRs_DepthTest,0);device.RsSet(GxRs_DepthWrite,0);device.RsSet(GxRs_Culling,0);
    device.RsSet(GxRs_ScissorTest,0);device.RsSet(GxRs_BlendingMode,GxBlend_Opaque);device.RsSet(GxRs_AlphaRef,0);device.RsSet(GxRs_Fog,0);device.RsSet(GxRs_ColorWrite,15);
    auto program=[&](const char* text,EGxShTarget stage,const char* name){
        ID3DBlob* code=nullptr;ID3DBlob* error=nullptr;
        const auto hr=D3DCompile(text,std::strlen(text),name,nullptr,nullptr,"main",stage==GxSh_Vertex?"vs_3_0":"ps_3_0",0,0,&code,&error);
        if(error)error->Release();Check(SUCCEEDED(hr)&&code,"frame-resource shader compile");
        auto* shader=WxlProgram(device,stage,name,code->GetBufferPointer(),code->GetBufferSize());code->Release();return shader;
    };
    auto* vs=program("float4 main(float3 p:POSITION):POSITION{return float4(p,1);}",GxSh_Vertex,"FrameResources.VS");
    auto* ps=program("float4 c:register(c0);float4 main():COLOR{return c;}",GxSh_Pixel,"FrameResources.PS");
    device.RsSet(GxRs_VertexShader,vs);device.RsSet(GxRs_PixelShader,ps);
    const CImVector white(255,255,255,255);
    const Vertex vertices[]={{4,4,.25f,white,0,0},{4,6,.25f,white,0,0},{6,4,.25f,white,0,0},
        {-1,-1,.25f,white,0,0},{-1,3,.25f,white,0,0},{3,-1,.25f,white,0,0}};
    auto* v=device.BufStream(GxPoolTarget_Vertex,sizeof(Vertex),6);auto* vertexData=device.BufLock(v);
    Check(vertexData!=nullptr,"fan resource vertices");std::memcpy(vertexData,vertices,sizeof(vertices));Check(device.BufUnlock(v,0)!=0,"fan vertices commit");device.PrimVertexPtr(v,GxVBF_PCT);
    const uint16_t indices[]={3,4,5};auto* i=device.BufStream(GxPoolTarget_Index,2,3);auto* indexData=device.BufLock(i);
    Check(indexData!=nullptr,"fan resource indices");std::memcpy(indexData,indices,sizeof(indices));Check(device.BufUnlock(i,0)!=0,"fan indices commit");device.PrimIndexPtr(i);
    CGxBatch fan{};fan.m_primType=GxPrim_TriangleFan;fan.m_count=3;fan.m_minIndex=0;fan.m_maxIndex=5;
    uint64_t baseline=0,peak=0;CGxDeviceD3d11::FrameResourceStats warm{};
    for(uint32_t frame=0;frame<240;++frame){
        const float color[]={frame&1?.75f:.25f,.5f,.25f,1};device.ShaderConstantsSet(GxSh_Pixel,0,color,1);
        for(uint32_t draw=0;draw<64;++draw){fan.m_start=draw&1?0:3;device.Draw(&fan,draw&1);}
        device.ScenePresent();Check(device.CapsHasContext(-1)!=0,"frame-resource device remains live");
        const auto stats=device.FrameResources();Check(stats.pendingFrames<=1&&stats.peakPendingFrames<=2,"bounded GPU frame queue with VSync/fixLag/FPS limits off");
        if(frame==31){baseline=PrivateBytes();peak=baseline;warm=stats;}
        if(frame>=32){const auto bytes=PrivateBytes();peak=std::max(peak,bytes);Check(bytes<=baseline+64ull*1024*1024,"bounded post-warmup process private-byte growth");}
    }
    const auto end=device.FrameResources();
    Check(end.retiredFrames==239&&end.pendingFrames==1,"oldest frame retired before query reuse");
    Check(device.SubmittedDraws()==240*64,"all bounded workload draws reached GPU submission");
    Check(end.fanBufferCreates==2&&end.fanBufferCreates==warm.fanBufferCreates,"fan buffers reused after warmup");
    Check(end.constantUploads-warm.constantUploads<=210,"unchanged per-draw constants do not upload repeatedly");
    Check(end.textureObjects==warm.textureObjects&&end.poolObjects==warm.poolObjects,"native resource owners stable after warmup");
    // CImVector's constructor is BGRA, while the shader/Equal report is RGBA.
    Equal(Center(device),CImVector(64,128,191,255),"indexed fan and base-vertex fan pixels after cached constants",3);
    device.m_format.m_fixLag=1;device.ScenePresent();Check(device.FrameResources().pendingFrames==0,"gxFixLag drains every pending frame");
    Check(device.RecoverDevice(),"frame resource recovery");
    Check(device.FrameResources().pendingFrames==0&&device.FrameResources().constantUploads==0,"recovery invalidates resource snapshots and fences");
    std::cout<<"PASS bounded uncapped native DX11 frames=240 draws=15360 peak_pending="<<end.peakPendingFrames
        <<" retired="<<end.retiredFrames<<" constant_uploads="<<end.constantUploads<<" fan_buffer_creates="<<end.fanBufferCreates
        <<" baseline_private_bytes="<<baseline<<" peak_private_bytes="<<peak<<" final_private_bytes="<<PrivateBytes()<<" hardware="<<!warp<<'\n';
    device.DeviceDestroy();g_theGxDevicePtr=nullptr;CGxDevice::LogClose();return 0;
}
