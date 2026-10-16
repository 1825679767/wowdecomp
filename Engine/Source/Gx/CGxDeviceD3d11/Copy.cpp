#include "Internal.h"
using namespace gx11;

bool CGxDeviceD3d11::Impl::Blit(ID3D11ShaderResourceView* source,ID3D11RenderTargetView* target,
    const CiRect& dst,const CiRect& src,uint32_t sw,uint32_t sh,bool depthCopy){
    if(!source||!target||!sw||!sh||dst.minX>=dst.maxX||dst.minY>=dst.maxY)return false;
    auto compile=[&](const char* text,const char* profile,ComPtr<ID3DBlob>& code){ComPtr<ID3DBlob> error;
        const HRESULT hr=D3DCompile(text,std::strlen(text),"Native DX11 resource copy",nullptr,nullptr,"main",profile,
            D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&error);
        if(FAILED(hr)&&error)CGxDevice::Log("DX11 copy shader: %s",static_cast<const char*>(error->GetBufferPointer()));
        return Check(hr,"copy shader compile");};
    if(!blitVs){const char* text="struct O{float4 p:SV_Position;float2 uv:TEXCOORD;};O main(uint n:SV_VertexID){O o;o.uv=float2((n<<1)&2,n&2);o.p=float4(o.uv*float2(2,-2)+float2(-1,1),0,1);return o;}";
        ComPtr<ID3DBlob> code;if(!compile(text,"vs_4_0",code)||!Check(device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&blitVs),"copy VS"))return false;
    }
    ComPtr<ID3D11PixelShader> ps;
    if(depthCopy){D3D11_SHADER_RESOURCE_VIEW_DESC view{};source->GetDesc(&view);
        const bool ms=view.ViewDimension==D3D11_SRV_DIMENSION_TEXTURE2DMS;
        const std::string text=ms?
            "Texture2DMS<float> t:register(t0);float main(float4 p:SV_Position):SV_Target{uint w,h,n;t.GetDimensions(w,h,n);float d=1;for(uint i=0;i<n;++i)d=min(d,t.Load(int2(p.xy),i));return d;}":
            "Texture2D<float> t:register(t0);float main(float4 p:SV_Position):SV_Target{return t.Load(int3(p.xy,0));}";
        ComPtr<ID3DBlob> code;if(!compile(text.c_str(),ms?"ps_5_0":"ps_4_0",code)||!Check(device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&ps),"depth copy PS"))return false;
    }else{
        if(!blitPs){const char* text="cbuffer C:register(b0){float4 region;}Texture2D t:register(t0);SamplerState s:register(s0);float4 main(float4 p:SV_Position,float2 uv:TEXCOORD):SV_Target{return t.Sample(s,region.xy+uv*region.zw);}";
            ComPtr<ID3DBlob> code;if(!compile(text,"ps_4_0",code)||!Check(device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&blitPs),"copy PS"))return false;}
        ps=blitPs;
    }
    if(!blitConstants){D3D11_BUFFER_DESC d{};d.ByteWidth=16;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        if(!Check(device->CreateBuffer(&d,nullptr,&blitConstants),"copy constants"))return false;}
    const float constants[]={float(src.minX)/sw,float(src.minY)/sh,float(src.maxX-src.minX)/sw,float(src.maxY-src.minY)/sh};
    context->UpdateSubresource(blitConstants.Get(),0,nullptr,constants,0,0);
    UnbindViews();context->OMSetRenderTargets(1,&target,nullptr);context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);context->VSSetShader(blitVs.Get(),nullptr,0);context->PSSetShader(ps.Get(),nullptr,0);
    D3D11_RASTERIZER_DESC rd{};rd.FillMode=D3D11_FILL_SOLID;rd.CullMode=D3D11_CULL_NONE;rd.DepthClipEnable=TRUE;
    ComPtr<ID3D11RasterizerState> raster;if(!Check(device->CreateRasterizerState(&rd,&raster),"copy raster"))return false;context->RSSetState(raster.Get());
    D3D11_DEPTH_STENCIL_DESC dd{};dd.DepthEnable=FALSE;dd.DepthFunc=D3D11_COMPARISON_ALWAYS;
    ComPtr<ID3D11DepthStencilState> depthState;if(!Check(device->CreateDepthStencilState(&dd,&depthState),"copy depth"))return false;context->OMSetDepthStencilState(depthState.Get(),0);
    context->OMSetBlendState(nullptr,nullptr,0xffffffff);
    D3D11_VIEWPORT vp{float(dst.minX),float(dst.minY),float(dst.maxX-dst.minX),float(dst.maxY-dst.minY),0,1};context->RSSetViewports(1,&vp);
    ID3D11Buffer* cb=blitConstants.Get();context->PSSetConstantBuffers(0,1,&cb);context->PSSetShaderResources(0,1,&source);
    D3D11_SAMPLER_DESC sd{};sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD=D3D11_FLOAT32_MAX;sd.MaxAnisotropy=1;sd.ComparisonFunc=D3D11_COMPARISON_NEVER;
    ComPtr<ID3D11SamplerState> sampler;if(!Check(device->CreateSamplerState(&sd,&sampler),"copy sampler"))return false;
    ID3D11SamplerState* sp=sampler.Get();context->PSSetSamplers(0,1,&sp);context->Draw(3,0);UnbindViews();self->IRsForceUpdate();return true;
}

bool CGxDeviceD3d11::Impl::StretchColor(ID3D11Texture2D* source,uint32_t sourceSub,
    ID3D11Texture2D* destination,uint32_t destSub,const CiRect& dr,const CiRect& sr){
    if(!source||!destination||dr.minX<0||dr.minY<0||sr.minX<0||sr.minY<0||
       dr.minX>=dr.maxX||dr.minY>=dr.maxY||sr.minX>=sr.maxX||sr.minY>=sr.maxY)return false;
    D3D11_TEXTURE2D_DESC a{},b{};source->GetDesc(&a);destination->GetDesc(&b);
    if(sourceSub>=a.MipLevels*a.ArraySize||destSub>=b.MipLevels*b.ArraySize)return false;
    const UINT sw=std::max(1u,a.Width>>(sourceSub%a.MipLevels)),sh=std::max(1u,a.Height>>(sourceSub%a.MipLevels));
    const UINT dw=std::max(1u,b.Width>>(destSub%b.MipLevels)),dh=std::max(1u,b.Height>>(destSub%b.MipLevels));
    if(sr.maxX>int(sw)||sr.maxY>int(sh)||dr.maxX>int(dw)||dr.maxY>int(dh)||b.SampleDesc.Count!=1)return false;
    a.Width=sw;a.Height=sh;a.ArraySize=1;a.MipLevels=1;a.SampleDesc.Count=1;a.SampleDesc.Quality=0;
    a.Usage=D3D11_USAGE_DEFAULT;a.BindFlags=D3D11_BIND_SHADER_RESOURCE;a.CPUAccessFlags=0;a.MiscFlags=0;
    ComPtr<ID3D11Texture2D> copy;if(!Check(device->CreateTexture2D(&a,nullptr,&copy),"stretch source copy"))return false;
    D3D11_TEXTURE2D_DESC original{};source->GetDesc(&original);UnbindViews();
    if(original.SampleDesc.Count>1)context->ResolveSubresource(copy.Get(),0,source,sourceSub,a.Format);
    else context->CopySubresourceRegion(copy.Get(),0,0,0,0,source,sourceSub,nullptr);
    ComPtr<ID3D11ShaderResourceView> srv;if(!Check(device->CreateShaderResourceView(copy.Get(),nullptr,&srv),"stretch source view"))return false;
    b.Width=dw;b.Height=dh;b.MipLevels=1;b.ArraySize=1;b.BindFlags=D3D11_BIND_RENDER_TARGET;
    b.Usage=D3D11_USAGE_DEFAULT;b.CPUAccessFlags=0;b.MiscFlags=0;
    ComPtr<ID3D11Texture2D> target;if(!Check(device->CreateTexture2D(&b,nullptr,&target),"stretch destination"))return false;
    ComPtr<ID3D11RenderTargetView> rtv;if(!Check(device->CreateRenderTargetView(target.Get(),nullptr,&rtv),"stretch destination view"))return false;
    if(!Blit(srv.Get(),rtv.Get(),dr,sr,sw,sh))return false;
    D3D11_BOX region{UINT(dr.minX),UINT(dr.minY),0,UINT(dr.maxX),UINT(dr.maxY),1};
    context->CopySubresourceRegion(destination,destSub,dr.minX,dr.minY,0,target.Get(),0,&region);return true;
}
int32_t CGxDeviceD3d11::TexStretch(CGxTex* dst,const CiRect* destRect,const CiRect* sourceRect,uint32_t level,uint32_t face){
    if(!m_context||!dst||!TextureReady(dst))return 0;auto& to=m->textures[dst];
    if(level>=to.mips||face>=to.faces||dst->m_format==GxTex_D24X8)return 0;
    auto* source=m->CurrentColorResource();if(!source)return 0;D3D11_TEXTURE2D_DESC desc{};source->GetDesc(&desc);
    const auto& current=m_textureTarget[GxBuffers_Color];const UINT sourceSub=D3D11CalcSubresource(0,current.m_texture?current.m_plane:0,desc.MipLevels);
    const uint32_t w=std::max(1u,to.width>>level),h=std::max(1u,to.height>>level);
    const CiRect allDest(0,0,h,w),allSource(0,0,desc.Height,desc.Width);
    const bool result=m->StretchColor(source,sourceSub,to.gpu.Get(),D3D11CalcSubresource(level,face,to.mips),destRect?*destRect:allDest,sourceRect?*sourceRect:allSource);
    if(result)to.mipGenerationDirty=true;BindTargets();return result;
}

void CGxDeviceD3d11::DeviceReadPixels(CiRect& rect,TSGrowableArray<CImVector>& pixels){
    ReadColorPixels(rect,pixels,true); // Native Wrath screenshots read the backbuffer.
}
void CGxDeviceD3d11::ReadTargetPixels(CiRect& rect,TSGrowableArray<CImVector>& pixels){ReadColorPixels(rect,pixels,false);}
void CGxDeviceD3d11::ReadColorPixels(CiRect& rect,TSGrowableArray<CImVector>& pixels,bool backbuffer){
    pixels.SetCount(0);if(!m_context)return;
    auto* source=backbuffer?m->color.Get():m->CurrentColorResource();if(!source)return;D3D11_TEXTURE2D_DESC desc{};source->GetDesc(&desc);
    rect.minX=std::max(0,rect.minX);rect.minY=std::max(0,rect.minY);rect.maxX=std::min(int(desc.Width),rect.maxX);rect.maxY=std::min(int(desc.Height),rect.maxY);
    if(rect.minX>=rect.maxX||rect.minY>=rect.maxY)return;
    const auto& target=m_textureTarget[GxBuffers_Color];UINT sub=D3D11CalcSubresource(0,!backbuffer&&target.m_texture?target.m_plane:0,desc.MipLevels);
    ComPtr<ID3D11Texture2D> resolve;
    if(desc.SampleDesc.Count>1){auto d=desc;d.SampleDesc.Count=1;d.SampleDesc.Quality=0;d.BindFlags=0;d.MipLevels=1;d.ArraySize=1;
        if(!Check(m->device->CreateTexture2D(&d,nullptr,&resolve),"readback resolve"))return;
        m->context->ResolveSubresource(resolve.Get(),0,source,sub,d.Format);source=resolve.Get();sub=0;
    }
    ComPtr<ID3D11Texture2D> converted;
    if(desc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM&&desc.Format!=DXGI_FORMAT_B8G8R8A8_UNORM){
        auto d=desc;d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.MipLevels=1;d.ArraySize=1;d.SampleDesc.Count=1;
        d.SampleDesc.Quality=0;d.BindFlags=0;d.MiscFlags=0;
        if(!Check(m->device->CreateTexture2D(&d,nullptr,&converted),"color conversion target"))return;
        CiRect all(0,0,desc.Height,desc.Width);
        if(!m->StretchColor(source,sub,converted.Get(),0,all,all)){BindTargets();return;}
        source=converted.Get();sub=0;desc=d;
    }
    auto d=desc;d.Width=rect.maxX-rect.minX;d.Height=rect.maxY-rect.minY;d.MipLevels=1;d.ArraySize=1;d.SampleDesc.Count=1;d.SampleDesc.Quality=0;
    d.Usage=D3D11_USAGE_STAGING;d.BindFlags=0;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;d.MiscFlags=0;
    ComPtr<ID3D11Texture2D> staging;if(!Check(m->device->CreateTexture2D(&d,nullptr,&staging),"color staging"))return;
    m->UnbindViews();D3D11_BOX box{UINT(rect.minX),UINT(rect.minY),0,UINT(rect.maxX),UINT(rect.maxY),1};
    m->context->CopySubresourceRegion(staging.Get(),0,0,0,0,source,sub,&box);
    D3D11_MAPPED_SUBRESOURCE mapped{};if(!Check(m->context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"color readback"))return;
    pixels.SetCount(d.Width*d.Height);
    for(uint32_t y=0;y<d.Height;++y)for(uint32_t x=0;x<d.Width;++x){const auto* p=static_cast<const uint8_t*>(mapped.pData)+y*mapped.RowPitch+x*4;
        pixels[y*d.Width+x]=desc.Format==DXGI_FORMAT_R8G8B8A8_UNORM?CImVector(p[2],p[1],p[0],p[3]):CImVector(p[0],p[1],p[2],p[3]);}
    m->context->Unmap(staging.Get(),0);BindTargets();
}
void CGxDeviceD3d11::DeviceReadDepths(CiRect& rect,TSGrowableArray<float>& values){
    values.SetCount(0);if(!m_context||!m->CurrentDepth())return;
    ComPtr<ID3D11Resource> resource;m->CurrentDepth()->GetResource(&resource);ComPtr<ID3D11Texture2D> source;
    if(FAILED(resource.As(&source)))return;D3D11_TEXTURE2D_DESC desc{};source->GetDesc(&desc);
    if(desc.ArraySize>1){D3D11_DEPTH_STENCIL_VIEW_DESC current{};m->CurrentDepth()->GetDesc(&current);
        const UINT face=current.ViewDimension==D3D11_DSV_DIMENSION_TEXTURE2DARRAY?current.Texture2DArray.FirstArraySlice:0;
        auto d=desc;d.ArraySize=1;d.MipLevels=1;d.MiscFlags=0;ComPtr<ID3D11Texture2D> single;
        if(!Check(m->device->CreateTexture2D(&d,nullptr,&single),"depth face readback copy"))return;
        m->UnbindViews();m->context->CopySubresourceRegion(single.Get(),0,0,0,0,source.Get(),D3D11CalcSubresource(0,face,desc.MipLevels),nullptr);
        source=single;desc=d;
    }
    rect.minX=std::max(0,rect.minX);rect.minY=std::max(0,rect.minY);rect.maxX=std::min(int(desc.Width),rect.maxX);rect.maxY=std::min(int(desc.Height),rect.maxY);
    if(rect.minX>=rect.maxX||rect.minY>=rect.maxY)return;
    D3D11_SHADER_RESOURCE_VIEW_DESC view{};view.Format=desc.Format==DXGI_FORMAT_R16_TYPELESS?DXGI_FORMAT_R16_UNORM:
        (desc.Format==DXGI_FORMAT_R32_TYPELESS?DXGI_FORMAT_R32_FLOAT:DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
    view.ViewDimension=desc.SampleDesc.Count>1?D3D11_SRV_DIMENSION_TEXTURE2DMS:D3D11_SRV_DIMENSION_TEXTURE2D;view.Texture2D.MipLevels=1;
    ComPtr<ID3D11ShaderResourceView> srv;if(!Check(m->device->CreateShaderResourceView(source.Get(),&view,&srv),"depth read SRV"))return;
    D3D11_TEXTURE2D_DESC td{};td.Width=desc.Width;td.Height=desc.Height;td.MipLevels=1;td.ArraySize=1;td.Format=DXGI_FORMAT_R32_FLOAT;
    td.SampleDesc.Count=1;td.BindFlags=D3D11_BIND_RENDER_TARGET;ComPtr<ID3D11Texture2D> copy;
    if(!Check(m->device->CreateTexture2D(&td,nullptr,&copy),"depth resolve target"))return;
    ComPtr<ID3D11RenderTargetView> rtv;if(!Check(m->device->CreateRenderTargetView(copy.Get(),nullptr,&rtv),"depth resolve RTV"))return;
    CiRect all(0,0,desc.Height,desc.Width);if(!m->Blit(srv.Get(),rtv.Get(),all,all,desc.Width,desc.Height,true)){BindTargets();return;}
    td.Width=rect.maxX-rect.minX;td.Height=rect.maxY-rect.minY;td.BindFlags=0;td.Usage=D3D11_USAGE_STAGING;td.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;if(!Check(m->device->CreateTexture2D(&td,nullptr,&staging),"depth staging"))return;
    D3D11_BOX box{UINT(rect.minX),UINT(rect.minY),0,UINT(rect.maxX),UINT(rect.maxY),1};m->context->CopySubresourceRegion(staging.Get(),0,0,0,0,copy.Get(),0,&box);
    D3D11_MAPPED_SUBRESOURCE mapped{};if(!Check(m->context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"depth readback"))return;
    values.SetCount(td.Width*td.Height);for(UINT y=0;y<td.Height;++y)std::memcpy(values.Ptr()+y*td.Width,static_cast<const uint8_t*>(mapped.pData)+y*mapped.RowPitch,td.Width*4);
    m->context->Unmap(staging.Get(),0);BindTargets();
}
