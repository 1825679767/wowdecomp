#pragma once
static void OutlineChecks(bool warp){
    CGxDeviceD3d11 d;g_theGxDevicePtr=&d;Check(d.CreateOffscreen(64,64,warp),"outline device");
    d.RsSet(GxRs_DepthTest,0);d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_Culling,0);d.RsSet(GxRs_ScissorTest,0);
    d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_AlphaRef,0);d.RsSet(GxRs_Fog,0);d.RsSet(GxRs_ColorWrite,15);
    auto compile=[&](const char* text,EGxShTarget stage,const char* name){ID3DBlob* code=nullptr;ID3DBlob* error=nullptr;
        auto hr=D3DCompile(text,std::strlen(text),name,nullptr,nullptr,"main",stage==GxSh_Vertex?"vs_3_0":"ps_3_0",0,0,&code,&error);if(error)error->Release();Check(SUCCEEDED(hr)&&code,"outline fixture shader compile");
        auto* program=WxlProgram(d,stage,name,code->GetBufferPointer(),code->GetBufferSize());code->Release();return program;};
    auto* vs=compile("struct O{float4 p:POSITION;float4 c:COLOR0;float2 uv:TEXCOORD0;};O main(float3 p:POSITION,float4 c:COLOR0,float2 uv:TEXCOORD0){O o;o.p=float4(p,1);o.c=c;o.uv=uv;return o;}",GxSh_Vertex,"OutlineRegression.VS");
    auto* ps=compile("sampler2D s:register(s0);float4 tint:register(c2);float4 main(float4 c:COLOR0,float2 uv:TEXCOORD0):COLOR{return tex2D(s,uv)*c+tint;}",GxSh_Pixel,"OutlineRegression.PS");
    d.RsSet(GxRs_VertexShader,vs);d.RsSet(GxRs_PixelShader,ps);const float zero[4]{};d.ShaderConstantsSet(GxSh_Pixel,2,zero,1);
    Pixels white;white.data={CImVector(255,255,255,255)};auto* texture=Texture(d,white);d.RsSet(GxRs_Texture0,texture);
    const float red[4]={1,0,0,1};
    auto small=[&](float z){const Vertex v[]={{-.5f,-.5f,z,CImVector(255,255,255,255),.5f,.5f},{-.5f,.5f,z,CImVector(255,255,255,255),.5f,.5f},{.5f,-.5f,z,CImVector(255,255,255,255),.5f,.5f}};
        auto* vb=d.BufStream(GxPoolTarget_Vertex,sizeof(Vertex),3);auto* p=d.BufLock(vb);Check(p!=nullptr,"outline vertices");std::memcpy(p,v,sizeof(v));d.BufUnlock(vb,0);d.PrimVertexPtr(vb,GxVBF_PCT);
        const uint16_t indices[]={0,1,2};auto* ib=d.BufStream(GxPoolTarget_Index,2,3);p=d.BufLock(ib);Check(p!=nullptr,"outline indices");std::memcpy(p,indices,sizeof(indices));d.BufUnlock(ib,0);d.PrimIndexPtr(ib);
        CGxBatch b{};b.m_primType=GxPrim_Triangles;b.m_count=3;b.m_maxIndex=2;d.Draw(&b,1);return b;};
    auto read=[&](){TSGrowableArray<CImVector> pixels;CiRect rect(0,0,64,64);d.ReadTargetPixels(rect,pixels);Check(pixels.Count()==4096,"outline pixels");return std::vector<CImVector>(pixels.Ptr(),pixels.Ptr()+pixels.Count());};
    auto edges=[&](const auto& p){uint32_t n=0;for(uint32_t i=0;i<p.size();++i)if(p[i].r>30&&p[i].g<5&&p[i].b<5)++n;return n;};
    d.SceneClear(3,CImVector(0,0,0,255));d.BeginUnitOutlineFrame();auto b=small(.5f);
    Check(d.StampUnitOutline(b,true,red,false),"opaque outline native batch stamp");Check(d.EndUnitOutlineFrame(),"outline composite");auto pixels=read();
    Check(edges(pixels)>40,"reaction-colored edge pixels");Equal(pixels[40*64+20],CImVector(255,255,255,255),"outline preserves interior material");Equal(pixels[2*64+2],CImVector(0,0,0,255),"outline preserves distant background");
    Check(!d.EndUnitOutlineFrame(),"outline composite consumed once");
    Triangle(d,CImVector(255,255,255,255),.25f,true);Equal(Center(d,64,64),CImVector(255,255,255,255),"native shader/constants/target restored after outline");
    d.SceneClear(3,CImVector(0,0,0,255));d.BeginUnitOutlineFrame();Check(!d.EndUnitOutlineFrame(),"empty outline frame has no stale mask");Check(edges(read())==0,"empty frame preserves clear");
    white.data[0]=CImVector(255,255,255,0);d.TexMarkForUpdate(texture,CiRect(0,0,1,1),1);d.RsSet(GxRs_AlphaRef,128);d.BeginUnitOutlineFrame();b=small(.5f);
    Check(d.StampUnitOutline(b,true,red,false),"alpha-tested outline native batch stamp");Check(d.EndUnitOutlineFrame(),"cutout outline composite");Check(edges(read())==0,"transparent cutout casts no silhouette");
    white.data[0]=CImVector(255,255,255,255);d.TexMarkForUpdate(texture,CiRect(0,0,1,1),1);d.RsSet(GxRs_AlphaRef,0);
    for(bool player:{true,false}){
        d.SceneClear(3,CImVector(0,0,0,255));d.RsSet(GxRs_DepthTest,1);d.RsSet(GxRs_DepthWrite,1);Triangle(d,CImVector(255,255,255,255),.2f,true);d.RsSet(GxRs_DepthWrite,0);
        d.BeginUnitOutlineFrame();b=small(.8f);Check(d.StampUnitOutline(b,true,red,player),"depth-policy silhouette stamp");Check(d.EndUnitOutlineFrame(),"depth-policy composite");
        auto p=read();uint32_t colored=0;for(uint32_t i=0;i<p.size();++i)if(p[i].r>p[i].g+15)++colored;
        Check(player?colored==0:colored>40,player?"player silhouette remains occluded":"NPC silhouette remains visible through geometry");
    }
    d.RsSet(GxRs_DepthTest,0);d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_BlendingMode,GxBlend_Alpha);d.BeginUnitOutlineFrame();b=small(.5f);Check(!d.StampUnitOutline(b,true,red,false),"blended model batches excluded");Check(!d.EndUnitOutlineFrame(),"excluded batches leave no mask");d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);
    auto format=d.m_format;format.m_multisampleCount=4;Check(d.DeviceSetFormat(format)!=0,"outline MSAA resize");d.SceneClear(3,CImVector(0,0,0,255));d.BeginUnitOutlineFrame();b=small(.5f);Check(d.StampUnitOutline(b,true,red,false)&&d.EndUnitOutlineFrame(),"MSAA silhouette resolve/composite");Check(edges(read())>40,"MSAA outline edge pixels");
    Check(d.RecoverDevice(),"outline owner recovery");d.SceneClear(3,CImVector(0,0,0,255));d.BeginUnitOutlineFrame();b=small(.5f);Check(d.StampUnitOutline(b,true,red,false)&&d.EndUnitOutlineFrame(),"outline recreation after device recovery");Check(edges(read())>40,"recovered outline pixels");
    d.RsSet(GxRs_Texture0,static_cast<void*>(nullptr));d.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));d.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));d.TexDestroy(texture);d.ShaderDestroy(vs);d.ShaderDestroy(ps);d.DeviceDestroy();g_theGxDevicePtr=nullptr;
}
