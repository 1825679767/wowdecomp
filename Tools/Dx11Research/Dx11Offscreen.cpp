#include "TestFixtures.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl/client.h>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using Microsoft::WRL::ComPtr;
static void HR(HRESULT hr, const char* label) {
    if (FAILED(hr)) { std::ostringstream s; s << label << " HRESULT=0x" << std::hex << uint32_t(hr); throw std::runtime_error(s.str()); }
}
static ComPtr<ID3DBlob> Compile(const char* source, const char* entry, const char* profile) {
    ComPtr<ID3DBlob> code, errors;
    const HRESULT hr=D3DCompile(source,strlen(source),"DX11 isolated proof",nullptr,nullptr,entry,profile,
        D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_WARNINGS_ARE_ERRORS,0,&code,&errors);
    if (FAILED(hr) && errors) std::cerr.write(static_cast<const char*>(errors->GetBufferPointer()),errors->GetBufferSize());
    HR(hr,"D3DCompile"); return code;
}
static gx::shader::Permutation RoundTrip(ID3DBlob* blob, uint16_t constants, uint32_t stage, fixtures::Bytes& encoded) {
    const auto* begin=static_cast<const uint8_t*>(blob->GetBufferPointer());
    encoded=fixtures::Container({fixtures::Record({begin,begin+blob->GetBufferSize()},constants)});
    gx::shader::Container container; gx::shader::Permutation p; std::string error;
    if (!container.Open(encoded.data(),encoded.size(),error) || !container.Extract(0,p,error)) throw std::runtime_error(error);
    if (!gx::shader::HasDxbcSignature(p) || p.code.size()!=blob->GetBufferSize() ||
        memcmp(p.code.data(),begin,p.code.size()) || p.constantBytes!=constants) throw std::runtime_error("DXBC container round-trip mismatch");
    ComPtr<ID3D11ShaderReflection> reflection;
    HR(D3DReflect(p.code.data(),p.code.size(),IID_ID3D11ShaderReflection,reinterpret_cast<void**>(reflection.GetAddressOf())),"D3DReflect");
    D3D11_SHADER_DESC desc{}; HR(reflection->GetDesc(&desc),"GetDesc");
    if (D3D11_SHVER_GET_TYPE(desc.Version)!=stage || desc.ConstantBuffers!=1) throw std::runtime_error("unexpected shader stage/buffer count");
    D3D11_SHADER_BUFFER_DESC cb{}; HR(reflection->GetConstantBufferByIndex(0)->GetDesc(&cb),"constant reflection");
    D3D11_SHADER_INPUT_BIND_DESC binding{}; HR(reflection->GetResourceBindingDescByName(cb.Name,&binding),"buffer binding");
    if (cb.Size!=constants || binding.BindPoint!=0 || binding.Type!=D3D_SIT_CBUFFER) throw std::runtime_error("constant buffer contract mismatch");
    return p;
}
int main(int argc, char** argv) try {
    const char* hlsl=R"(
cbuffer VertexConstants : register(b0) { row_major float4x4 transform : packoffset(c0); };
struct Input { float3 position : POSITION; float4 color : COLOR; float2 uv : TEXCOORD; };
struct Output { float4 position : SV_POSITION; float4 color : COLOR; float2 uv : TEXCOORD; };
Output VS(Input v) { Output o; o.position=mul(float4(v.position,1),transform); o.color=v.color; o.uv=v.uv; return o; }
)";
    const char* pixel=R"(
cbuffer PixelConstants : register(b0) { float4 multiplier : packoffset(c0); float4 additive : packoffset(c1); };
Texture2D sourceTexture : register(t0); SamplerState sourceSampler : register(s0);
struct Input { float4 position : SV_POSITION; float4 color : COLOR; float2 uv : TEXCOORD; };
float4 PS(Input v) : SV_TARGET { return sourceTexture.Sample(sourceSampler,v.uv)*v.color*multiplier+additive; }
)";
    const auto vs=Compile(hlsl,"VS","vs_4_0"), ps=Compile(pixel,"PS","ps_4_0");
    fixtures::Bytes vsBls, psBls;
    const auto vertex=RoundTrip(vs.Get(),64,D3D11_SHVER_VERTEX_SHADER,vsBls);
    const auto fragment=RoundTrip(ps.Get(),32,D3D11_SHVER_PIXEL_SHADER,psBls);
    ComPtr<ID3D11Device> device; ComPtr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL actual{}, requested=D3D_FEATURE_LEVEL_11_0;
    HR(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,&requested,1,D3D11_SDK_VERSION,&device,&actual,&context),"WARP device");
    ComPtr<ID3D11VertexShader> vertexShader; ComPtr<ID3D11PixelShader> pixelShader;
    HR(device->CreateVertexShader(vertex.code.data(),vertex.code.size(),nullptr,&vertexShader),"vertex shader");
    HR(device->CreatePixelShader(fragment.code.data(),fragment.code.size(),nullptr,&pixelShader),"pixel shader");
    D3D11_INPUT_ELEMENT_DESC elements[]={
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0}};
    ComPtr<ID3D11InputLayout> layout;
    HR(device->CreateInputLayout(elements,3,vertex.code.data(),vertex.code.size(),&layout),"input layout");
    struct V { float x,y,z; uint32_t color; float u,v; };
    static_assert(sizeof(V)==24,"proof vertex layout");
    const V vertices[]={{-1,-1,0,0xffffffff,.5f,.5f},{-1,3,0,0xffffffff,.5f,.5f},{3,-1,0,0xffffffff,.5f,.5f}};
    D3D11_BUFFER_DESC bd{}; bd.ByteWidth=sizeof(vertices); bd.Usage=D3D11_USAGE_IMMUTABLE; bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initial{}; initial.pSysMem=vertices;
    ComPtr<ID3D11Buffer> vb; HR(device->CreateBuffer(&bd,&initial,&vb),"vertex buffer");
    const float identity[]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    bd.ByteWidth=sizeof(identity); bd.BindFlags=D3D11_BIND_CONSTANT_BUFFER; initial.pSysMem=identity;
    ComPtr<ID3D11Buffer> vcb; HR(device->CreateBuffer(&bd,&initial,&vcb),"vertex constants");
    const float pc[]={.5f,1,.5f,1,.1f,0,0,0}; bd.ByteWidth=sizeof(pc); initial.pSysMem=pc;
    ComPtr<ID3D11Buffer> pcb; HR(device->CreateBuffer(&bd,&initial,&pcb),"pixel constants");
    const uint8_t texel[]={64,128,192,255};
    D3D11_TEXTURE2D_DESC td{}; td.Width=1;td.Height=1;td.MipLevels=1;td.ArraySize=1;
    td.Format=DXGI_FORMAT_R8G8B8A8_UNORM;td.SampleDesc.Count=1;td.Usage=D3D11_USAGE_IMMUTABLE;td.BindFlags=D3D11_BIND_SHADER_RESOURCE;
    initial={};initial.pSysMem=texel;initial.SysMemPitch=4;
    ComPtr<ID3D11Texture2D> texture; HR(device->CreateTexture2D(&td,&initial,&texture),"source texture");
    ComPtr<ID3D11ShaderResourceView> srv; HR(device->CreateShaderResourceView(texture.Get(),nullptr,&srv),"source view");
    D3D11_SAMPLER_DESC sd{};sd.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;sd.MaxLOD=D3D11_FLOAT32_MAX;sd.ComparisonFunc=D3D11_COMPARISON_NEVER;sd.MaxAnisotropy=1;
    ComPtr<ID3D11SamplerState> sampler; HR(device->CreateSamplerState(&sd,&sampler),"sampler");
    td.Width=4;td.Height=4;td.Usage=D3D11_USAGE_DEFAULT;td.BindFlags=D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> target; HR(device->CreateTexture2D(&td,nullptr,&target),"render target");
    ComPtr<ID3D11RenderTargetView> rtv; HR(device->CreateRenderTargetView(target.Get(),nullptr,&rtv),"target view");
    td.Usage=D3D11_USAGE_STAGING;td.BindFlags=0;td.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging; HR(device->CreateTexture2D(&td,nullptr,&staging),"readback texture");
    D3D11_RASTERIZER_DESC rd{};rd.FillMode=D3D11_FILL_SOLID;rd.CullMode=D3D11_CULL_NONE;rd.DepthClipEnable=TRUE;
    ComPtr<ID3D11RasterizerState> raster; HR(device->CreateRasterizerState(&rd,&raster),"raster state");
    D3D11_DEPTH_STENCIL_DESC dd{};dd.DepthEnable=FALSE;dd.DepthFunc=D3D11_COMPARISON_ALWAYS;
    ComPtr<ID3D11DepthStencilState> depth; HR(device->CreateDepthStencilState(&dd,&depth),"depth state");
    ID3D11Buffer* buffer=vb.Get();UINT stride=sizeof(V),offset=0;
    context->IASetVertexBuffers(0,1,&buffer,&stride,&offset);context->IASetInputLayout(layout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vertexShader.Get(),nullptr,0);context->PSSetShader(pixelShader.Get(),nullptr,0);
    buffer=vcb.Get();context->VSSetConstantBuffers(0,1,&buffer);buffer=pcb.Get();context->PSSetConstantBuffers(0,1,&buffer);
    ID3D11ShaderResourceView* view=srv.Get();context->PSSetShaderResources(0,1,&view);
    ID3D11SamplerState* samplerPtr=sampler.Get();context->PSSetSamplers(0,1,&samplerPtr);
    ID3D11RenderTargetView* output=rtv.Get();context->OMSetRenderTargets(1,&output,nullptr);
    context->OMSetDepthStencilState(depth.Get(),0);context->RSSetState(raster.Get());
    D3D11_VIEWPORT viewport{0,0,4,4,0,1};context->RSSetViewports(1,&viewport);
    const float clear[]={1,0,1,1};context->ClearRenderTargetView(rtv.Get(),clear);context->Draw(3,0);
    context->OMSetRenderTargets(0,nullptr,nullptr);context->CopyResource(staging.Get(),target.Get());
    D3D11_MAPPED_SUBRESOURCE mapped{};HR(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"readback map");
    bool matched=true; const int expected[]={58,128,96,255};
    std::array<uint8_t,4> observed{};
    for(UINT y=0;y<4;++y)for(UINT x=0;x<4;++x)for(UINT k=0;k<4;++k) {
        const auto value=static_cast<const uint8_t*>(mapped.pData)[y*mapped.RowPitch+x*4+k];
        if(!x&&!y)observed[k]=value;
        if(std::abs(int(value)-expected[k])>1)matched=false;
    }
    context->Unmap(staging.Get(),0);context->ClearState();
    if(!matched)throw std::runtime_error("offscreen texture/constant/vertex pixel mismatch");
    if(argc==2) {
        std::filesystem::create_directories(argv[1]);
        for(const auto& item: {std::make_pair("proof-vs.bls",&vsBls),std::make_pair("proof-ps.bls",&psBls)}) {
            const auto path=std::filesystem::path(argv[1])/item.first;
            if(std::filesystem::exists(path))throw std::runtime_error("refusing fixture overwrite");
            std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(item.second->data()),item.second->size());
            if(!out)throw std::runtime_error("fixture write failed");
        }
    }
    std::cout << "PASS WARP feature_level=0x" << std::hex << actual << std::dec
              << " compiled_DXBC_container_roundtrip=2 reflected_constant_buffers=64,32"
              << " rendered_pixels=16 rgba=" << int(observed[0]) << ',' << int(observed[1]) << ','
              << int(observed[2]) << ',' << int(observed[3]) << '\n';
    std::cout << "Standalone proof only; WotLK shaders/backend, hardware, swapchain and gameplay remain unverified.\n";
    return 0;
} catch(const std::exception& e) { std::cerr << "FAIL " << e.what() << '\n';return 1; }
