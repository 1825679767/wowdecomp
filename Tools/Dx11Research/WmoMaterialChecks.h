#pragma once
#include "WXL/WmoMaterials.hpp"
#include "WXL/vendor/src/platform/waystone/WmoMaterialPlan.hpp"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/Handle.h"
static void WmoMaterialChecks(bool warp){
    namespace wmo=wxl::native::wmo_materials;
    CGxDeviceD3d11 d;g_theGxDevicePtr=&d;Check(d.CreateOffscreen(16,16,warp),"WMO material device");wmo::DeviceReady(d);Check(wmo::LayeredReady(),"WMO DX11 capability admission");
    d.RsSet(GxRs_DepthTest,0);d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_Culling,0);d.RsSet(GxRs_ScissorTest,0);d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_AlphaRef,0);d.RsSet(GxRs_Fog,0);d.RsSet(GxRs_ColorWrite,15);
    auto compile=[&](const char* text,EGxShTarget stage,const char* name){ID3DBlob* code=nullptr;ID3DBlob* error=nullptr;auto hr=D3DCompile(text,std::strlen(text),name,nullptr,nullptr,"main",stage==GxSh_Vertex?"vs_3_0":"ps_3_0",0,0,&code,&error);if(error)error->Release();Check(SUCCEEDED(hr)&&code,"WMO fixture shader compile");auto* result=WxlProgram(d,stage,name,code->GetBufferPointer(),code->GetBufferSize());code->Release();return result;};
    auto* vs=compile("struct O{float4 p:POSITION;float4 c:COLOR0;float2 uv:TEXCOORD0;};O main(float3 p:POSITION,float3 n:NORMAL,float4 c:COLOR0,float2 uv:TEXCOORD0){O o;o.p=float4(p,1);o.c=c;o.uv=uv;return o;}",GxSh_Vertex,"WMORegression.VS");
    auto* ps=compile("sampler2D s:register(s0);float4 main(float4 c:COLOR0,float2 uv:TEXCOORD0):COLOR{return tex2D(s,uv)*c;}",GxSh_Pixel,"WMORegression.PS");
    d.RsSet(GxRs_VertexShader,vs);d.RsSet(GxRs_PixelShader,ps);
    Pixels pixels[8];CTexture* handles[8]{};CGxTexFlags flags(GxTex_LinearMipNearest,1,1);
    auto plan=std::make_shared<wxl::waystone::wmo::MaterialPlan>();plan->records.resize(1);auto& record=plan->records[0];record.layered=true;record.sourceShader=23;
    constexpr unsigned paths[]={0,1,2,3,5,6,7,8};
    for(unsigned i=0;i<8;++i){pixels[i].width=pixels[i].height=8;pixels[i].data.assign(64,i==1?CImVector(0,255,0,255):CImVector(255,255,255,255));
        auto name="WXL_DX11_WMO_FIXTURE_"+std::to_string(i);handles[i]=TextureCreate(GxTex_2d,8,8,1,GxTex_Argb8888,GxTex_Argb8888,flags,&pixels[i],TextureData,name.c_str(),0);Check(handles[i]!=nullptr,"WMO real CTexture owner");TextureCacheNewTexture(handles[i],flags);record.paths[paths[i]]=name;}
    auto* originalTexture=TextureGetGxTex(handles[0],0,nullptr);Check(originalTexture!=nullptr,"WMO original native texture");d.RsSet(GxRs_Texture0,originalTexture);
    struct Geometry{float x,y,z,nx,ny,nz;CImVector color;float u,v;};const Geometry vertices[]={{-1,-1,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f},{-1,3,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f},{3,-1,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f}};
    auto* vb=d.BufStream(GxPoolTarget_Vertex,sizeof(Geometry),3);auto* data=d.BufLock(vb);Check(data!=nullptr,"WMO geometry lock");std::memcpy(data,vertices,sizeof(vertices));d.BufUnlock(vb,0);
    const CGxVertexAttrib attrs[]={{GxVA_Position,4,0,36},{GxVA_Normal,4,12,36},{GxVA_Color0,0,24,36},{GxVA_TexCoord0,3,28,36}};d.PrimVertexFormat(vb,attrs,4);d.PrimVertexMask((1<<GxVA_Position)|(1<<GxVA_Normal)|(1<<GxVA_Color0)|(1<<GxVA_TexCoord0));
    const uint16_t indices[]={0,1,2};auto* ib=d.BufStream(GxPoolTarget_Index,2,3);data=d.BufLock(ib);Check(data!=nullptr,"WMO indices lock");std::memcpy(data,indices,sizeof(indices));d.BufUnlock(ib,0);d.PrimIndexPtr(ib);
    auto source=std::make_shared<wmo::GroupSource>();source->materials=plan;source->vertices=3;source->stream.resize(3*36);
    for(unsigned i=0;i<3;++i){const float uv[8]={.5f,.5f,.5f,.5f,.5f,.5f,.5f,.5f};std::memcpy(source->stream.data()+i*36,uv,32);const CImVector weight(0,0,255,255);std::memcpy(source->stream.data()+i*36+32,&weight,4);}
    CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_count=3;batch.m_maxIndex=2;
    auto draw=[&](){const auto before=d.SubmittedDraws();{wmo::BatchScope scope(source,0);d.Draw(&batch,1);}Check(d.SubmittedDraws()==before+1,"WMO material submits exactly one native batch");Check(d.m_appRenderStates[GxRs_VertexShader].m_value.m_data.p==vs&&d.m_appRenderStates[GxRs_PixelShader].m_value.m_data.p==ps&&d.m_appRenderStates[GxRs_Texture0].m_value.m_data.p==originalTexture&&d.m_primIndexBuf==ib,"WMO native shader/texture/index state restored");};
    d.SceneClear(3,CImVector(0,0,0,255));draw();Equal(Center(d),CImVector(255,255,255,255),"four-layer WMO height blend selects native first layer");
    const uint32_t tint=0x00804020;std::memcpy(record.native.data()+28,&tint,4);d.SceneClear(3,CImVector(0,0,0,255));draw();Equal(Center(d),CImVector(32,64,128,255),"four-layer WMO per-material tint");
    for(auto* handle:handles){auto* texture=TextureGetGxTex(handle,0,nullptr);Check(texture&&texture->m_flags.m_wrapU&&texture->m_flags.m_wrapV,"WMO material sampler policy preserves shared texture flags");}
    const auto old=d.ExchangeSamplerOverride(0,{});Check(!old.enabled,"WMO restores stage-local sampler override");d.ExchangeSamplerOverride(0,old);
    Check(d.RecoverDevice(),"WMO material owner recovery");wmo::DeviceReady(d);d.SceneClear(3,CImVector(0,0,0,255));draw();Equal(Center(d),CImVector(32,64,128,255),"WMO native material resources recreated after recovery");
    auto* compositeVs=compile("struct O{float4 p:POSITION;float4 c:COLOR0;float4 s:COLOR1;float2 uv:TEXCOORD0;};O main(float3 p:POSITION,float4 c:COLOR0,float2 uv:TEXCOORD0){O o;o.p=float4(p,1);o.c=c;o.s=0;o.uv=uv;return o;}",GxSh_Vertex,"WMORegression.CompositeVS");
    const char* compositeText="ps_3_0\ndcl_color0 v0\ndcl_color1 v1\ndcl_texcoord0 v2.xy\ndcl_2d s0\ndcl_2d s1\ntexld r0,v2,s0\ntexld r1,v2,s1\nlrp r2,v1.w,r0,r1\nmul oC0,r2,v0\n";
    using Assemble=HRESULT(WINAPI*)(LPCVOID,SIZE_T,LPCSTR,const D3D_SHADER_MACRO*,ID3DInclude*,UINT,ID3DBlob**,ID3DBlob**);
    auto compiler=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);Check(compiler!=nullptr,"WMO native assembler library");auto assemble=reinterpret_cast<Assemble>(GetProcAddress(compiler,"D3DAssemble"));ID3DBlob* code=nullptr;ID3DBlob* errors=nullptr;
    const auto assembled=assemble?assemble(compositeText,std::strlen(compositeText),"WMO composite fixture",nullptr,nullptr,0,&code,&errors):E_FAIL;if(errors)errors->Release();Check(SUCCEEDED(assembled)&&code,"WMO native composite assembly");auto* compositePs=WxlProgram(d,GxSh_Pixel,"WMORegression.CompositePS",code->GetBufferPointer(),code->GetBufferSize());code->Release();FreeLibrary(compiler);
    Pixels overlay;overlay.data={CImVector(0,0,255,128)};auto* overlayTexture=Texture(d,overlay);d.RsSet(GxRs_VertexShader,compositeVs);d.RsSet(GxRs_PixelShader,compositePs);d.RsSet(GxRs_Texture1,overlayTexture);record.layered=false;record.nativeShader=6;
    d.SceneClear(3,CImVector(0,0,0,255));const auto compositeBefore=d.SubmittedDraws();{wmo::BatchScope scope(source,0);d.Draw(&batch,1);}Check(d.SubmittedDraws()==compositeBefore+1,"WMO composite submits once");Equal(Center(d),CImVector(127,127,255,191),"WMO composite uses overlay alpha and correct layer direction");
    Check(d.m_appRenderStates[GxRs_VertexShader].m_value.m_data.p==compositeVs&&d.m_appRenderStates[GxRs_PixelShader].m_value.m_data.p==compositePs&&d.m_appRenderStates[GxRs_Texture1].m_value.m_data.p==overlayTexture,"WMO composite restores native programs and overlay binding");
    d.SceneClear(3,CImVector(0,0,0,255));d.Draw(&batch,1);Equal(Center(d),CImVector(0,0,255,128),"ordinary surrounding composite draw retains stock vertex-alpha policy");
    for(unsigned i=0;i<8;++i)d.RsSet(static_cast<EGxRenderState>(GxRs_Texture0+i),static_cast<void*>(nullptr));wmo::Release(d);Check(!d.m_wxlWmoMaterials,"WMO device cache released");for(auto* handle:handles)HandleClose(reinterpret_cast<HOBJECT>(handle));TextureGxCacheClear();
    d.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));d.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));d.TexDestroy(overlayTexture);d.ShaderDestroy(compositeVs);d.ShaderDestroy(compositePs);d.ShaderDestroy(vs);d.ShaderDestroy(ps);d.DeviceDestroy();g_theGxDevicePtr=nullptr;
}
