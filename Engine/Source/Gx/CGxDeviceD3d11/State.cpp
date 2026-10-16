#include "Internal.h"
using namespace gx11;

void CGxDeviceD3d11::IRsSendToHw(EGxRenderState){m->stateDirty=true;}

bool CGxDeviceD3d11::Impl::States(){
    self->IRsSync(0);
    auto state=[&](EGxRenderState s){return self->m_appRenderStates[s].m_value.m_data.u[0];};
    const uint32_t masters=self->m_appMasterEnables;
    const uint32_t blend=std::min(state(GxRs_BlendingMode),uint32_t(GxBlends_Last-1));
    static const D3D11_BLEND src[]={D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_SRC_ALPHA,
        D3D11_BLEND_DEST_COLOR,D3D11_BLEND_DEST_COLOR,D3D11_BLEND_DEST_COLOR,D3D11_BLEND_INV_SRC_ALPHA,
        D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_BLEND_FACTOR,D3D11_BLEND_ONE,D3D11_BLEND_INV_DEST_COLOR};
    static const D3D11_BLEND dst[]={D3D11_BLEND_ZERO,D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_ONE,
        D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_ZERO,
        D3D11_BLEND_ONE,D3D11_BLEND_INV_BLEND_FACTOR,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_ONE};
    auto alpha=[](D3D11_BLEND v){switch(v){case D3D11_BLEND_SRC_COLOR:return D3D11_BLEND_SRC_ALPHA;
        case D3D11_BLEND_INV_SRC_COLOR:return D3D11_BLEND_INV_SRC_ALPHA;case D3D11_BLEND_DEST_COLOR:return D3D11_BLEND_DEST_ALPHA;
        case D3D11_BLEND_INV_DEST_COLOR:return D3D11_BLEND_INV_DEST_ALPHA;default:return v;}};
    D3D11_BLEND_DESC bd{};auto& rt=bd.RenderTarget[0];rt.BlendEnable=blend>=2;
    rt.SrcBlend=src[blend];rt.DestBlend=dst[blend];rt.BlendOp=D3D11_BLEND_OP_ADD;
    rt.SrcBlendAlpha=alpha(rt.SrcBlend);rt.DestBlendAlpha=alpha(rt.DestBlend);rt.BlendOpAlpha=D3D11_BLEND_OP_ADD;
    const uint32_t mask=masters&(1<<GxMasterEnable_ColorWrite)?state(GxRs_ColorWrite):0;
    rt.RenderTargetWriteMask=UINT8((mask&9)|((mask&2)<<1)|((mask&4)>>1));
    auto bk=Key(bd);auto bi=blends.find(bk);
    if(bi==blends.end()){ComPtr<ID3D11BlendState> value;if(!Check(device->CreateBlendState(&bd,&value),"blend state"))return false;
        if(blends.size()>=256)blends.clear();bi=blends.emplace(bk,std::move(value)).first;}
    const float blendFactor[]={1,1,1,1};context->OMSetBlendState(bi->second.Get(),blendFactor,0xffffffff);
    static const D3D11_COMPARISON_FUNC cmp[]={D3D11_COMPARISON_LESS_EQUAL,D3D11_COMPARISON_EQUAL,D3D11_COMPARISON_GREATER_EQUAL,D3D11_COMPARISON_LESS};
    D3D11_DEPTH_STENCIL_DESC dd{};dd.DepthEnable=TRUE;
    dd.DepthWriteMask=(masters&(1<<GxMasterEnable_DepthWrite))&&state(GxRs_DepthWrite)?D3D11_DEPTH_WRITE_MASK_ALL:D3D11_DEPTH_WRITE_MASK_ZERO;
    dd.DepthFunc=(masters&(1<<GxMasterEnable_DepthTest))&&state(GxRs_DepthTest)?cmp[std::min(state(GxRs_DepthFunc),3u)]:D3D11_COMPARISON_ALWAYS;
    const auto dk=Key(dd);auto di=depths.find(dk);
    if(di==depths.end()){ComPtr<ID3D11DepthStencilState> value;if(!Check(device->CreateDepthStencilState(&dd,&value),"depth state"))return false;
        if(depths.size()>=64)depths.clear();di=depths.emplace(dk,std::move(value)).first;}
    context->OMSetDepthStencilState(di->second.Get(),0);
    D3D11_RASTERIZER_DESC rd{};rd.FillMode=masters&(1<<GxMasterEnable_PolygonFill)?D3D11_FILL_SOLID:D3D11_FILL_WIREFRAME;
    const auto cull=(masters&(1<<GxMasterEnable_Culling))?state(GxRs_Culling):0;
    rd.CullMode=cull?D3D11_CULL_BACK:D3D11_CULL_NONE;rd.FrontCounterClockwise=cull==1;
    rd.DepthClipEnable=TRUE;rd.ScissorEnable=state(GxRs_ScissorTest)!=0;rd.MultisampleEnable=state(GxRs_Multisample)!=0;
    const float offset=self->m_appRenderStates[GxRs_PolygonOffset].m_value.m_data.f[0];
    D3D11_DEPTH_STENCIL_VIEW_DESC currentDepth{};if(CurrentDepth())CurrentDepth()->GetDesc(&currentDepth);
    const double units=currentDepth.Format==DXGI_FORMAT_D16_UNORM?65536.0:
        (currentDepth.Format==DXGI_FORMAT_D32_FLOAT?8388608.0:16777216.0);
    if(std::isfinite(offset))rd.DepthBias=int(std::max(-2147483647.0,std::min(2147483647.0,-offset*units)));
    const auto rk=Key(rd);auto ri=rasters.find(rk);
    if(ri==rasters.end()){ComPtr<ID3D11RasterizerState> value;if(!Check(device->CreateRasterizerState(&rd,&value),"raster state"))return false;
        if(rasters.size()>=256)rasters.clear();ri=rasters.emplace(rk,std::move(value)).first;}
    context->RSSetState(ri->second.Get());
    const auto& window=self->m_curWindowRect;const float w=window.maxX-window.minX,h=window.maxY-window.minY;
    D3D11_VIEWPORT viewport{};viewport.TopLeftX=window.minX+self->m_viewport.m_x.m_l*w;
    viewport.TopLeftY=window.minY+(1-self->m_viewport.m_y.m_h)*h;
    viewport.Width=(self->m_viewport.m_x.m_h-self->m_viewport.m_x.m_l)*w;
    viewport.Height=(self->m_viewport.m_y.m_h-self->m_viewport.m_y.m_l)*h;
    viewport.MinDepth=self->m_viewport.m_z.m_l;viewport.MaxDepth=self->m_viewport.m_z.m_h;
    if(viewport.Width<=0||viewport.Height<=0)return false;context->RSSetViewports(1,&viewport);
    const auto& s=self->m_scissorRect;
    D3D11_RECT scissor{LONG(window.minX+s.minX*w),LONG(window.minY+(1-s.maxY)*h),LONG(window.minX+s.maxX*w),LONG(window.minY+(1-s.minY)*h)};
    context->RSSetScissorRects(1,&scissor);
    static const D3D11_FILTER filters[]={D3D11_FILTER_MIN_MAG_MIP_POINT,D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
        D3D11_FILTER_MIN_MAG_MIP_POINT,D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,D3D11_FILTER_MIN_MAG_MIP_LINEAR,D3D11_FILTER_ANISOTROPIC};
    for(uint32_t i=0;i<16;++i){auto* t=static_cast<CGxTex*>(self->m_appRenderStates[GxRs_Texture0+i].m_value.m_data.p);
        D3D11_SAMPLER_DESC sd{};sd.Filter=t?filters[std::min(t->m_flags.m_filter,5u)]:D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.AddressU=t&&t->m_flags.m_wrapU?D3D11_TEXTURE_ADDRESS_WRAP:D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV=t&&t->m_flags.m_wrapV?D3D11_TEXTURE_ADDRESS_WRAP:D3D11_TEXTURE_ADDRESS_CLAMP;sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxAnisotropy=t?std::max(1u,std::min(uint32_t(t->m_flags.m_maxAnisotropy),16u)):1;
        sd.ComparisonFunc=D3D11_COMPARISON_LESS_EQUAL;sd.MaxLOD=t&&t->m_flags.m_filter>=GxTex_NearestMipNearest?D3D11_FLOAT32_MAX:0;
        if(t&&t->m_format==GxTex_D24X8)sd.Filter=D3D11_FILTER(sd.Filter|0x80);
        const auto key=Key(sd);auto si=samplers.find(key);
        if(si==samplers.end()){ComPtr<ID3D11SamplerState> value;if(!Check(device->CreateSamplerState(&sd,&value),"sampler state"))return false;
            if(samplers.size()>=512)samplers.clear();si=samplers.emplace(key,std::move(value)).first;}
        ID3D11SamplerState* sampler=si->second.Get();context->PSSetSamplers(i,1,&sampler);if(i<4)context->VSSetSamplers(i,1,&sampler);
    }
    self->m_viewportDirty=0;self->m_scissorRectDirty=0;stateDirty=false;return true;
}
