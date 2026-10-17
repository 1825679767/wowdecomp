#pragma once
#include "CGxDeviceD3d11.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyShaderTranslator.h"
#include "Engine/Source/Gx/CGxPool.h"
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace gx11 {
using Microsoft::WRL::ComPtr;
template<class T> std::string Key(const T& value){return std::string(reinterpret_cast<const char*>(&value),sizeof(value));}
inline bool Check(HRESULT hr,const char* operation){if(SUCCEEDED(hr))return true;CGxDevice::Log("DX11 %s failed: %08x",operation,uint32_t(hr));return false;}
inline bool InvalidatedConstant(const float* value){uint32_t words[4];std::memcpy(words,value,16);return words[0]==0x7f7fffff&&words[1]==0x7f7fffff&&words[2]==0x7f7fffff&&words[3]==0x7f7fffff;}
inline DXGI_FORMAT Format(EGxTexFormat f){
    static const DXGI_FORMAT map[GxTexFormats_Last]={DXGI_FORMAT_UNKNOWN,DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,DXGI_FORMAT_B4G4R4A4_UNORM,DXGI_FORMAT_B5G5R5A1_UNORM,
        DXGI_FORMAT_B5G6R5_UNORM,DXGI_FORMAT_BC1_UNORM,DXGI_FORMAT_BC2_UNORM,DXGI_FORMAT_BC3_UNORM,
        DXGI_FORMAT_R8G8_SNORM,DXGI_FORMAT_R16G16_FLOAT,DXGI_FORMAT_R32_FLOAT,DXGI_FORMAT_R24G8_TYPELESS};
    return f<GxTexFormats_Last?map[f]:DXGI_FORMAT_UNKNOWN;
}
inline uint32_t Usage(uint32_t a){return a<4?a:(a<6?10:5);}
inline uint32_t UsageIndex(uint32_t a){return a<5?0:(a==5?1:a-6);}
inline const char* SemanticName(uint32_t a){static const char* names[]={"POSITION","BLENDWEIGHT","BLENDINDICES","NORMAL","COLOR","COLOR","TEXCOORD","TEXCOORD","TEXCOORD","TEXCOORD","TEXCOORD","TEXCOORD","TEXCOORD","TEXCOORD"};return names[a];}
}

struct CGxDeviceD3d11::Impl {
    using Bytes=std::vector<uint8_t>;
    template<class T> using ComPtr=gx11::ComPtr<T>;
    explicit Impl(CGxDeviceD3d11* owner):self(owner){}
    CGxDeviceD3d11* self;
    HWND window=nullptr;
    bool ownWindow=false,offscreen=false,warp=false,stateDirty=true,changingFormat=false;
    uint32_t width=0,height=0,samples=1;
    uint64_t presentedFrames=0,drawCalls=0;
    D3D_FEATURE_LEVEL featureLevel{};
    DXGI_FORMAT depthStorage=DXGI_FORMAT_R24G8_TYPELESS,depthTargetFormat=DXGI_FORMAT_D24_UNORM_S8_UINT,depthSampleFormat=DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    ComPtr<IDXGIOutput> gammaOutput;
    DXGI_GAMMA_CONTROL originalGamma{};
    bool gammaCaptured=false;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    // End-of-frame retirement is independent of VSync and gxFixLag. Never
    // reuse a query until its previous submission has completed.
    ComPtr<ID3D11Query> frameFences[2];
    uint32_t frameHead=0,framePending=0;
    uint64_t retiredFrames=0,constantUploads=0,fanBufferCreates=0;
    uint32_t peakPendingFrames=0;
    ComPtr<IDXGISwapChain> swapchain;
    ComPtr<ID3D11Texture2D> backbuffer,color,depth;
    ComPtr<ID3D11RenderTargetView> colorView;
    ComPtr<ID3D11DepthStencilView> depthView,savedDepth,depthOverride;
    std::map<std::pair<uint32_t,uint32_t>,std::pair<ComPtr<ID3D11Texture2D>,ComPtr<ID3D11DepthStencilView>>> auxiliaryDepth;
    ComPtr<ID3D11ShaderResourceView> depthSrv;
    ComPtr<ID3D11Buffer> constants[2][4];
    Bytes uploadedConstants[2][4];
    ComPtr<ID3D11Buffer> fanSequential,fanIndexed;
    uint32_t fanSequentialVertices=0,fanIndexedBytes=0;
    std::vector<uint32_t> fanCpu;
    // API constants retain their values between shader draws. The native
    // shadow array contains FLT_MAX invalidation markers outside dirty ranges.
    std::array<std::array<uint32_t,1024>,2> appConstants{};
    struct Pool {Bytes cpu;ComPtr<ID3D11Buffer> gpu;uint32_t dirtyBegin=0xffffffff,dirtyEnd=0;};
    std::map<CGxPool*,Pool> pools;
    struct Texture {
        ComPtr<ID3D11Texture2D> gpu;
        ComPtr<ID3D11ShaderResourceView> srv;
        ComPtr<ID3D11RenderTargetView> rtv[6];
        ComPtr<ID3D11DepthStencilView> dsv[6];
        uint32_t width=0,height=0,mips=0,faces=0,baseMip=0;
        EGxTexFormat storage=GxTex_Unknown;
        bool mipGenerationDirty=false,generateMips=false,renderTarget=false;
    };
    std::map<CGxTex*,Texture> textures;
    struct Variant {
        bool fixed=false;
        gx::shader::TranslatedShader translated;
        ComPtr<ID3D11VertexShader> vertex;
        ComPtr<ID3D11PixelShader> pixel;
        std::map<std::string,ComPtr<ID3D11InputLayout>> layouts;
    };
    std::map<std::string,std::shared_ptr<Variant>> variants;
    std::map<CGxShader*,std::shared_ptr<Variant>> shaderObjects;
    struct Source {std::string path,name;uint32_t count=0,index=0;};
    std::map<CGxShader*,Source> shaderSources;
    std::map<std::string,std::unique_ptr<CGxShader>> fixedSources;
    std::map<std::string,ComPtr<ID3D11BlendState>> blends;
    std::map<std::string,ComPtr<ID3D11DepthStencilState>> depths;
    std::map<std::string,ComPtr<ID3D11RasterizerState>> rasters;
    std::map<std::string,ComPtr<ID3D11SamplerState>> samplers;
    SamplerOverride samplerOverrides[16];
    CGxTex* sceneCapture=nullptr;
    ComPtr<ID3D11VertexShader> blitVs;
    ComPtr<ID3D11PixelShader> blitPs,depthCopyPs;
    ComPtr<ID3D11Buffer> blitConstants;
    std::string lastError;
    std::shared_ptr<Variant> Shader(CGxShader*,const std::vector<gx::shader::Semantic>&,const gx::shader::TranslationState&);
    std::shared_ptr<Variant> drawVertex;
    struct Outline {
        ComPtr<ID3D11Texture2D> mask,resolved;
        ComPtr<ID3D11RenderTargetView> view;
        ComPtr<ID3D11ShaderResourceView> srv;
        ComPtr<ID3D11RenderTargetView> destination;
        ComPtr<ID3D11PixelShader> edge;
        ComPtr<ID3D11VertexShader> screen;
        ComPtr<ID3D11Buffer> constants;
        ComPtr<ID3D11BlendState> opaque,blend;
        ComPtr<ID3D11DepthStencilState> depth,through;
        ComPtr<ID3D11RasterizerState> raster;
        ComPtr<ID3D11SamplerState> sampler;
        std::unique_ptr<CGxShader> programs[2];
        uint32_t width=0,height=0,samples=0,quality=0;
        bool active=false,stamped=false;
    } outline;
    bool EnsureOutline();
    bool UploadConstants(const Variant&,bool vertex);
    void UploadConstantBuffer(uint32_t stage,uint32_t slot,const void* data,size_t bytes);
    bool FinishFrame(bool immediate);
    bool States();
    bool Blit(ID3D11ShaderResourceView*,ID3D11RenderTargetView*,const CiRect&,const CiRect&,uint32_t,uint32_t,bool depthCopy=false);
    bool StretchColor(ID3D11Texture2D*,uint32_t,ID3D11Texture2D*,uint32_t,const CiRect&,const CiRect&);
    bool FixedShaders(std::shared_ptr<Variant>&,std::shared_ptr<Variant>&,const std::vector<gx::shader::Semantic>&,const gx::shader::TranslationState&);
    void FixedConstants(bool vertex,uint32_t* words);
    void ApplyGamma();
    void RestoreGamma();
    ID3D11RenderTargetView* CurrentColor();
    ID3D11DepthStencilView* CurrentDepth();
    ID3D11Texture2D* CurrentColorResource();
    void UnbindViews(){ID3D11ShaderResourceView* nulls[16]{};context->PSSetShaderResources(0,16,nulls);context->VSSetShaderResources(0,16,nulls);context->OMSetRenderTargets(0,nullptr,nullptr);}
};
