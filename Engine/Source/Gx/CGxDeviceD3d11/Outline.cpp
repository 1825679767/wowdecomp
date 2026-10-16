#include "Internal.h"
#include "Engine/Source/Gx/Gx.h"
#include "WXL/modules/unit-outline/Hlsl.hpp"
using namespace gx11;

namespace {
// Every touched binding is restored, including empty MRT slots. The native
// state cache remains truthful after both successful and abandoned passes.
struct OutlineState {
    ID3D11DeviceContext* c;
    ComPtr<ID3D11RenderTargetView> rt[8];ComPtr<ID3D11DepthStencilView> dsv;
    ComPtr<ID3D11BlendState> blend;float factors[4]{};UINT mask=0;
    ComPtr<ID3D11DepthStencilState> depth;UINT reference=0;
    ComPtr<ID3D11RasterizerState> raster;
    ComPtr<ID3D11PixelShader> ps;ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11Buffer> cb;ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11SamplerState> sampler;ComPtr<ID3D11InputLayout> layout;
    D3D11_PRIMITIVE_TOPOLOGY topology{};
    D3D11_VIEWPORT viewport[16]{};UINT views=16;
    explicit OutlineState(ID3D11DeviceContext* context):c(context){
        ID3D11RenderTargetView* targets[8]{};c->OMGetRenderTargets(8,targets,&dsv);
        for(UINT i=0;i<8;++i)rt[i].Attach(targets[i]);
        c->OMGetBlendState(&blend,factors,&mask);c->OMGetDepthStencilState(&depth,&reference);
        c->RSGetState(&raster);c->RSGetViewports(&views,viewport);
        c->PSGetShader(&ps,nullptr,nullptr);c->VSGetShader(&vs,nullptr,nullptr);
        c->PSGetConstantBuffers(0,1,&cb);c->PSGetShaderResources(0,1,&srv);c->PSGetSamplers(0,1,&sampler);
        c->IAGetInputLayout(&layout);c->IAGetPrimitiveTopology(&topology);
    }
    ~OutlineState(){
        ID3D11ShaderResourceView* empty=nullptr;c->PSSetShaderResources(0,1,&empty);
        ID3D11RenderTargetView* targets[8]{};for(UINT i=0;i<8;++i)targets[i]=rt[i].Get();
        c->OMSetRenderTargets(8,targets,dsv.Get());c->OMSetBlendState(blend.Get(),factors,mask);
        c->OMSetDepthStencilState(depth.Get(),reference);c->RSSetState(raster.Get());c->RSSetViewports(views,viewport);
        c->PSSetShader(ps.Get(),nullptr,0);c->VSSetShader(vs.Get(),nullptr,0);
        auto* buffer=cb.Get();c->PSSetConstantBuffers(0,1,&buffer);
        auto* texture=srv.Get();c->PSSetShaderResources(0,1,&texture);auto* s=sampler.Get();c->PSSetSamplers(0,1,&s);
        c->IASetInputLayout(layout.Get());c->IASetPrimitiveTopology(topology);
    }
};
bool CompileOutline(const char* source,const char* profile,ComPtr<ID3DBlob>& code){
    ComPtr<ID3DBlob> error;const auto hr=D3DCompile(source,std::strlen(source),"WXL native DX11 silhouette",nullptr,nullptr,"main",profile,D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&error);
    if(FAILED(hr)&&error)CGxDevice::Log("DX11 outline shader: %s",static_cast<const char*>(error->GetBufferPointer()));return Check(hr,"outline shader compile");
}
}

bool CGxDeviceD3d11::Impl::EnsureOutline(){
    auto& r=outline;auto* target=CurrentColorResource();if(!target)return false;
    D3D11_TEXTURE2D_DESC desc{};target->GetDesc(&desc);
    if(!desc.Width||!desc.Height||desc.Width>16384||desc.Height>16384)return false;
    if(r.mask&&(r.width!=desc.Width||r.height!=desc.Height||r.samples!=desc.SampleDesc.Count||r.quality!=desc.SampleDesc.Quality)){
        if(r.stamped)return false;r.mask.Reset();r.resolved.Reset();r.view.Reset();r.srv.Reset();
    }
    if(!r.mask){
        desc.MipLevels=desc.ArraySize=1;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.Usage=D3D11_USAGE_DEFAULT;
        desc.BindFlags=D3D11_BIND_RENDER_TARGET;desc.CPUAccessFlags=desc.MiscFlags=0;
        ComPtr<ID3D11Texture2D> mask,resolved;ComPtr<ID3D11RenderTargetView> view;ComPtr<ID3D11ShaderResourceView> srv;
        if(!Check(device->CreateTexture2D(&desc,nullptr,&mask),"outline mask")||!Check(device->CreateRenderTargetView(mask.Get(),nullptr,&view),"outline mask view"))return false;
        const auto sample=desc.SampleDesc;desc.SampleDesc.Count=1;desc.SampleDesc.Quality=0;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
        if(!Check(device->CreateTexture2D(&desc,nullptr,&resolved),"outline resolved mask")||!Check(device->CreateShaderResourceView(resolved.Get(),nullptr,&srv),"outline mask sampling"))return false;
        r.width=desc.Width;r.height=desc.Height;r.samples=sample.Count;r.quality=sample.Quality;
        r.mask=std::move(mask);r.resolved=std::move(resolved);r.view=std::move(view);r.srv=std::move(srv);
    }
    if(!r.constants){D3D11_BUFFER_DESC d{};d.ByteWidth=4096;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_CONSTANT_BUFFER;if(!Check(device->CreateBuffer(&d,nullptr,&r.constants),"outline constants"))return false;}
    if(!r.opaque||!r.blend){D3D11_BLEND_DESC d{};d.RenderTarget[0].RenderTargetWriteMask=15;if(!Check(device->CreateBlendState(&d,&r.opaque),"outline mask blend"))return false;
        auto& b=d.RenderTarget[0];b.BlendEnable=TRUE;b.SrcBlend=D3D11_BLEND_SRC_ALPHA;b.DestBlend=D3D11_BLEND_INV_SRC_ALPHA;b.BlendOp=D3D11_BLEND_OP_ADD;b.SrcBlendAlpha=D3D11_BLEND_ONE;b.DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;b.BlendOpAlpha=D3D11_BLEND_OP_ADD;
        if(!Check(device->CreateBlendState(&d,&r.blend),"outline composite blend"))return false;}
    if(!r.depth||!r.through){D3D11_DEPTH_STENCIL_DESC d{};d.DepthEnable=TRUE;d.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ZERO;d.DepthFunc=D3D11_COMPARISON_LESS_EQUAL;
        if(!Check(device->CreateDepthStencilState(&d,&r.depth),"outline occlusion"))return false;d.DepthEnable=FALSE;if(!Check(device->CreateDepthStencilState(&d,&r.through),"outline through-wall state"))return false;}
    if(!r.raster){D3D11_RASTERIZER_DESC d{};d.FillMode=D3D11_FILL_SOLID;d.CullMode=D3D11_CULL_NONE;d.DepthClipEnable=TRUE;d.MultisampleEnable=TRUE;if(!Check(device->CreateRasterizerState(&d,&r.raster),"outline screen raster"))return false;}
    if(!r.sampler){D3D11_SAMPLER_DESC d{};d.Filter=D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;d.AddressU=d.AddressV=d.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;d.ComparisonFunc=D3D11_COMPARISON_NEVER;d.MaxLOD=0;if(!Check(device->CreateSamplerState(&d,&r.sampler),"outline screen sampler"))return false;}
    return true;
}
void CGxDeviceD3d11::BeginUnitOutlineFrame(){m->outline.active=m_context!=0;m->outline.stamped=false;m->outline.destination.Reset();}
bool CGxDeviceD3d11::StampUnitOutline(const CGxBatch& batch,bool indexed,const float* rgba,bool depthTest){
    auto& r=m->outline;
    if(!m_context||!r.active||!rgba||!indexed||batch.m_primType!=GxPrim_Triangles||!batch.m_count||!m->drawVertex||!m_primIndexBuf)return false;
    const auto blend=m_appRenderStates[GxRs_BlendingMode].m_value.m_data.u[0];if(blend>=GxBlend_Alpha)return false;
    if(r.stamped&&r.destination.Get()!=m->CurrentColor())return false;
    if(!m->EnsureOutline())return false;
    OutlineState state(m->context.Get());if(!state.rt[0]||(depthTest&&!state.dsv))return false;
    const bool cutout=m_appRenderStates[GxRs_AlphaRef].m_value.m_data.u[0]>=8;
    auto& program=r.programs[cutout?1:0];
    if(!program){ComPtr<ID3DBlob> code;if(!CompileOutline(cutout?wxl_unit_outline::hlsl::kCutoutColor:wxl_unit_outline::hlsl::kColor,"ps_2_0",code))return false;
        program=std::make_unique<CGxShader>();program->m_target=GxSh_Pixel;program->m_code.SetCount(uint32_t(code->GetBufferSize()));std::memcpy(program->m_code.Ptr(),code->GetBufferPointer(),code->GetBufferSize());}
    gx::shader::TranslationState translation;translation.samplerTypes[0]=1;
    auto shader=m->Shader(program.get(),m->drawVertex->translated.outputs,translation);if(!shader)return false;
    float constants[1024]{};std::memcpy(constants,rgba,16);m->context->UpdateSubresource(r.constants.Get(),0,nullptr,constants,0,0);
    auto* target=r.view.Get();m->context->OMSetRenderTargets(1,&target,depthTest?state.dsv.Get():nullptr);
    m->context->OMSetDepthStencilState(depthTest?r.depth.Get():r.through.Get(),0);m->context->OMSetBlendState(r.opaque.Get(),nullptr,state.mask);
    if(!r.stamped){const float clear[4]{};m->context->ClearRenderTargetView(r.view.Get(),clear);r.destination=state.rt[0];}
    m->context->PSSetShader(shader->pixel.Get(),nullptr,0);auto* cb=r.constants.Get();m->context->PSSetConstantBuffers(0,1,&cb);
    m->context->DrawIndexed(batch.m_count,batch.m_start,0);r.stamped=true;return true;
}
bool CGxDeviceD3d11::EndUnitOutlineFrame(){
    auto& r=m->outline;const bool submit=m_context&&r.active&&r.stamped;r.active=false;r.stamped=false;
    auto destination=std::move(r.destination);if(!submit||destination.Get()!=m->CurrentColor())return false;
    OutlineState state(m->context.Get());
    if(!r.screen){const char* vs="struct O{float4 p:SV_Position;float2 uv:TEXCOORD;};O main(uint n:SV_VertexID){O o;o.uv=float2((n<<1)&2,n&2);o.p=float4(o.uv*float2(2,-2)+float2(-1,1),0,1);return o;}";ComPtr<ID3DBlob> code;
        if(!CompileOutline(vs,"vs_4_0",code)||!Check(m->device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&r.screen),"outline screen VS"))return false;}
    if(!r.edge){const char* ps=
        "cbuffer C:register(b0){float4 px;}Texture2D m:register(t0);SamplerState s:register(s0);"
        "float4 main(float4 p:SV_Position,float2 uv:TEXCOORD):SV_Target{float2 o=px.xy*px.z;"
        "float4 a=m.Sample(s,uv+float2(o.x,0))+m.Sample(s,uv-float2(o.x,0))+m.Sample(s,uv+float2(0,o.y))+m.Sample(s,uv-float2(0,o.y))"
        "+m.Sample(s,uv+o)+m.Sample(s,uv-o)+m.Sample(s,uv+float2(o.x,-o.y))+m.Sample(s,uv+float2(-o.x,o.y));"
        "float line=saturate(a.a*.2)*(1-m.Sample(s,uv).a);clip(line-.02);return float4(a.a>.001?a.rgb/a.a:float3(1,1,1),saturate(line*1.4));}";
        ComPtr<ID3DBlob> code;if(!CompileOutline(ps,"ps_4_0",code)||!Check(m->device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&r.edge),"outline screen PS"))return false;}
    m->context->OMSetRenderTargets(0,nullptr,nullptr);
    if(r.samples>1)m->context->ResolveSubresource(r.resolved.Get(),0,r.mask.Get(),0,DXGI_FORMAT_R8G8B8A8_UNORM);else m->context->CopyResource(r.resolved.Get(),r.mask.Get());
    auto* target=destination.Get();m->context->OMSetRenderTargets(1,&target,nullptr);m->context->OMSetDepthStencilState(r.through.Get(),0);m->context->OMSetBlendState(r.blend.Get(),nullptr,0xffffffff);
    m->context->RSSetState(r.raster.Get());D3D11_VIEWPORT viewport{0,0,float(r.width),float(r.height),0,1};m->context->RSSetViewports(1,&viewport);
    m->context->IASetInputLayout(nullptr);m->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);m->context->VSSetShader(r.screen.Get(),nullptr,0);m->context->PSSetShader(r.edge.Get(),nullptr,0);
    float constants[1024]{};constants[0]=1.f/r.width;constants[1]=1.f/r.height;constants[2]=1.5f;m->context->UpdateSubresource(r.constants.Get(),0,nullptr,constants,0,0);
    auto* cb=r.constants.Get();m->context->PSSetConstantBuffers(0,1,&cb);auto* srv=r.srv.Get();m->context->PSSetShaderResources(0,1,&srv);auto* sampler=r.sampler.Get();m->context->PSSetSamplers(0,1,&sampler);m->context->Draw(3,0);return true;
}
