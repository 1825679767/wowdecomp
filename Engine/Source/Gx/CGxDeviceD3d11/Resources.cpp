#include "Internal.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Gx/Gx.h"
#include <limits>

using namespace gx11;

void CGxDeviceD3d11::PoolSizeSet(CGxPool* pool,uint32_t size){
    if(!pool||!size||size>512u*1024*1024)return;
    auto& p=m->pools[pool];p.cpu.resize(size);p.gpu.Reset();p.dirtyBegin=0;p.dirtyEnd=size;
    pool->m_size=int32_t(size);pool->m_apiSpecific=nullptr;pool->m_offset=0;pool->Invalidate();
}
void CGxDeviceD3d11::PoolDestroy(CGxPool* pool){if(!pool)return;m->pools.erase(pool);pool->m_apiSpecific=nullptr;CGxDevice::PoolDestroy(pool);}
bool CGxDeviceD3d11::PoolReady(CGxPool* pool){
    if(!pool||pool->m_size<=0||!m->device)return false;
    auto& p=m->pools[pool];if(p.cpu.size()!=uint32_t(pool->m_size))p.cpu.resize(pool->m_size);
    if(!p.gpu){D3D11_BUFFER_DESC d{};d.ByteWidth=pool->m_size;d.Usage=D3D11_USAGE_DEFAULT;
        d.BindFlags=pool->m_target==GxPoolTarget_Vertex?D3D11_BIND_VERTEX_BUFFER:D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA data{};data.pSysMem=p.cpu.data();
        if(!Check(m->device->CreateBuffer(&d,&data,&p.gpu),"pool buffer"))return false;p.dirtyBegin=0xffffffff;p.dirtyEnd=0;
    }else if(p.dirtyBegin<p.dirtyEnd){D3D11_BOX box{p.dirtyBegin,0,0,p.dirtyEnd,1,1};
        m->context->UpdateSubresource(p.gpu.Get(),0,&box,p.cpu.data()+p.dirtyBegin,0,0);p.dirtyBegin=0xffffffff;p.dirtyEnd=0;
    }
    pool->m_apiSpecific=p.gpu.Get();return true;
}
char* CGxDeviceD3d11::BufLock(CGxBuf* buf){
    if(!buf||!buf->m_pool||!buf->m_size)return nullptr;CGxDevice::BufLock(buf);auto* pool=buf->m_pool;
    if(pool->m_size<=0||buf->m_size>uint32_t(pool->m_size)){m_bufLocked[pool->m_target]=nullptr;return nullptr;}
    auto& p=m->pools[pool];if(p.cpu.size()!=uint32_t(pool->m_size))p.cpu.resize(pool->m_size);
    if(pool->m_usage==GxPoolUsage_Stream){const uint32_t alignment=std::max(1u,buf->m_itemSize);
        uint64_t at=(uint64_t(pool->m_offset)+alignment-1)/alignment*alignment;
        if(at+buf->m_size>p.cpu.size()){pool->Discard();at=0;}
        buf->m_index=uint32_t(at);pool->m_offset=buf->m_index+buf->m_size;
    }
    if(buf->m_index>p.cpu.size()||buf->m_size>p.cpu.size()-buf->m_index){m_bufLocked[pool->m_target]=nullptr;return nullptr;}
    return reinterpret_cast<char*>(p.cpu.data()+buf->m_index);
}
int32_t CGxDeviceD3d11::BufUnlock(CGxBuf* buf,uint32_t size){
    if(!buf||!buf->m_pool||size>buf->m_size)return 0;auto& p=m->pools[buf->m_pool];
    if(buf->m_index>p.cpu.size()||size>p.cpu.size()-buf->m_index)return 0;
    p.dirtyBegin=std::min(p.dirtyBegin,buf->m_index);p.dirtyEnd=std::max(p.dirtyEnd,buf->m_index+size);
    buf->m_valid=1;buf->m_hasData=1;buf->m_dirty=0;CGxDevice::BufUnlock(buf,size);return 1;
}
void CGxDeviceD3d11::BufData(CGxBuf* buf,const void* data,uint32_t size,uint32_t offset){
    if(!buf||!data||offset>buf->m_size||size>buf->m_size-offset)return;
    char* dst=BufLock(buf);if(!dst)return;std::memcpy(dst+offset,data,size);BufUnlock(buf,offset+size);
}

bool CGxDeviceD3d11::TextureReady(CGxTex* t){
    if(!t||!m->device||!t->m_width||!t->m_height||t->m_width>16384||t->m_height>16384)return false;
    auto found=m->textures.find(t);
    if(t->m_needsCreation||found==m->textures.end()){
        Impl::Texture candidate;uint32_t end;
        ITexWHDStartEnd(t,candidate.width,candidate.height,candidate.baseMip,end);
        candidate.width=std::max(1u,candidate.width);candidate.height=std::max(1u,candidate.height);
        candidate.mips=end-candidate.baseMip;candidate.faces=t->m_target==GxTex_CubeMap?6:1;candidate.storage=t->m_format;
        if(!candidate.mips)return false;
        if((t->m_format==GxTex_Argb4444||t->m_format==GxTex_Argb1555||t->m_format==GxTex_Rgb565)&&!m_caps.m_texFmt[t->m_format])candidate.storage=GxTex_Argb8888;
        D3D11_TEXTURE2D_DESC d{};d.Width=candidate.width;d.Height=candidate.height;d.MipLevels=candidate.mips;d.ArraySize=candidate.faces;
        d.Format=Format(candidate.storage);d.SampleDesc.Count=1;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_SHADER_RESOURCE;
        if(t->m_format==GxTex_D24X8)d.BindFlags|=D3D11_BIND_DEPTH_STENCIL;
        else if(t->m_flags.m_renderTarget)d.BindFlags|=D3D11_BIND_RENDER_TARGET;
        if(candidate.faces==6)d.MiscFlags|=D3D11_RESOURCE_MISC_TEXTURECUBE;
        if(t->m_flags.m_generateMipMaps&&t->m_format!=GxTex_D24X8){
            UINT support=0;if(FAILED(m->device->CheckFormatSupport(d.Format,&support))||!(support&D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))return false;
            d.MipLevels=0;d.BindFlags|=D3D11_BIND_RENDER_TARGET;d.MiscFlags|=D3D11_RESOURCE_MISC_GENERATE_MIPS;
            candidate.mips=1;for(uint32_t n=std::max(d.Width,d.Height);n>1;n>>=1)++candidate.mips;
        }
        if(!Check(m->device->CreateTexture2D(&d,nullptr,&candidate.gpu),"texture"))return false;
        D3D11_SHADER_RESOURCE_VIEW_DESC srv{};srv.Format=t->m_format==GxTex_D24X8?DXGI_FORMAT_R24_UNORM_X8_TYPELESS:d.Format;
        srv.ViewDimension=candidate.faces==6?D3D11_SRV_DIMENSION_TEXTURECUBE:D3D11_SRV_DIMENSION_TEXTURE2D;
        srv.Texture2D.MipLevels=candidate.mips;
        if(!Check(m->device->CreateShaderResourceView(candidate.gpu.Get(),&srv,&candidate.srv),"texture SRV"))return false;
        for(uint32_t face=0;face<candidate.faces;++face){
            if(t->m_format==GxTex_D24X8){D3D11_DEPTH_STENCIL_VIEW_DESC view{};view.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
                view.ViewDimension=candidate.faces==6?D3D11_DSV_DIMENSION_TEXTURE2DARRAY:D3D11_DSV_DIMENSION_TEXTURE2D;
                if(candidate.faces==6){view.Texture2DArray.FirstArraySlice=face;view.Texture2DArray.ArraySize=1;}
                if(!Check(m->device->CreateDepthStencilView(candidate.gpu.Get(),&view,&candidate.dsv[face]),"texture DSV"))return false;
            }else if(t->m_flags.m_renderTarget){D3D11_RENDER_TARGET_VIEW_DESC view{};view.Format=d.Format;
                view.ViewDimension=candidate.faces==6?D3D11_RTV_DIMENSION_TEXTURE2DARRAY:D3D11_RTV_DIMENSION_TEXTURE2D;
                if(candidate.faces==6){view.Texture2DArray.FirstArraySlice=face;view.Texture2DArray.ArraySize=1;}
                if(!Check(m->device->CreateRenderTargetView(candidate.gpu.Get(),&view,&candidate.rtv[face]),"texture RTV"))return false;
            }
        }
        m->textures[t]=std::move(candidate);t->m_needsCreation=0;t->m_needsUpdate=1;
        t->m_apiSpecificData=m->textures[t].srv.Get();t->m_apiSpecificData2=m->textures[t].gpu.Get();
    }
    auto& texture=m->textures[t];
    if(t->m_needsUpdate&&!t->m_flags.m_renderTarget&&t->m_userFunc){
        uint32_t pitch=0;const void* bits=nullptr;
        t->m_userFunc(GxTex_Lock,t->m_width,t->m_height,0,0,t->m_userArg,pitch,bits);
        struct Unlock {CGxTex* t;~Unlock(){uint32_t pitch=0;const void* bits=nullptr;t->m_userFunc(GxTex_Unlock,t->m_width,t->m_height,0,0,t->m_userArg,pitch,bits);}} unlock{t};
        for(uint32_t face=0;face<texture.faces;++face){
            const uint32_t levels=t->m_flags.m_generateMipMaps?1:texture.mips;
            for(uint32_t level=0;level<levels;++level){const uint32_t mip=texture.baseMip+level;
                const uint32_t w=std::max(1u,texture.width>>level),h=std::max(1u,texture.height>>level);
                bits=nullptr;pitch=0;t->m_userFunc(GxTex_Latch,w,h,face,mip,t->m_userArg,pitch,bits);
                if(!bits||!pitch)return false;
                uint32_t left=std::min(w,uint32_t(std::max(0,t->m_updateRect.minX))>>mip);
                uint32_t top=std::min(h,uint32_t(std::max(0,t->m_updateRect.minY))>>mip);
                uint32_t right=std::min(w,std::max(left+1,uint32_t(std::max(0,t->m_updateRect.maxX))>>mip));
                uint32_t bottom=std::min(h,std::max(top+1,uint32_t(std::max(0,t->m_updateRect.maxY))>>mip));
                if(left>=right||top>=bottom)continue;
                const bool compressed=t->m_dataFormat>=GxTex_Dxt1&&t->m_dataFormat<=GxTex_Dxt5;
                if(compressed){left&=~3u;top&=~3u;right=std::min(w,(right+3)&~3u);bottom=std::min(h,(bottom+3)&~3u);}
                const auto* source=static_cast<const uint8_t*>(bits);
                if(!t->m_flags.m_bit15){source+=compressed?(top/4)*pitch+(left/4)*s_texFormatBytesPerBlock[t->m_dataFormat]:top*pitch+left*s_texFormatBytesPerBlock[t->m_dataFormat];}
                const uint32_t rowPitch=GxCalcTexelStrideInBytes(texture.storage,right-left);
                const uint32_t rows=texture.storage>=GxTex_Dxt1&&texture.storage<=GxTex_Dxt5?(bottom-top+3)/4:bottom-top;
                if(!rowPitch||uint64_t(rowPitch)*rows>256u*1024*1024)return false;
                std::vector<uint8_t> upload(size_t(rowPitch)*rows);C2iVector size;size.x=right-left;size.y=bottom-top;
                if(!Blit(size,BlitAlpha_0,source,pitch,GxGetBlitFormat(t->m_dataFormat),upload.data(),rowPitch,GxGetBlitFormat(texture.storage)))return false;
                D3D11_BOX box{left,top,0,right,bottom,1};
                m->context->UpdateSubresource(texture.gpu.Get(),D3D11CalcSubresource(level,face,texture.mips),&box,upload.data(),rowPitch,0);
            }
        }
        if(t->m_flags.m_generateMipMaps)m->context->GenerateMips(texture.srv.Get());
    }
    CGxDevice::ITexMarkAsUpdated(t);return true;
}
void CGxDeviceD3d11::ITexMarkAsUpdated(CGxTex* texture){if(m_context)TextureReady(texture);}
void CGxDeviceD3d11::TexDestroy(CGxTex* t){
    if(!t)return;
    for(uint32_t i=0;i<2;++i)if(m_textureTarget[i].m_texture==t){m_textureTarget[i].m_texture=nullptr;m_textureTarget[i].m_apiSpecific=nullptr;}
    for(uint32_t i=GxRs_Texture0;i<=GxRs_Texture15;++i)if(m_appRenderStates[i].m_value.m_data.p==t)RsSet(EGxRenderState(i),static_cast<void*>(nullptr));
    if(m->context)m->UnbindViews();m->textures.erase(t);t->m_apiSpecificData=nullptr;t->m_apiSpecificData2=nullptr;
    CGxDevice::TexDestroy(t);if(m_context)BindTargets();
}

ID3D11RenderTargetView* CGxDeviceD3d11::Impl::CurrentColor(){auto& target=self->m_textureTarget[GxBuffers_Color];
    if(!target.m_texture)return colorView.Get();auto it=textures.find(target.m_texture);
    return it!=textures.end()&&target.m_plane<it->second.faces?it->second.rtv[target.m_plane].Get():nullptr;
}
ID3D11Texture2D* CGxDeviceD3d11::Impl::CurrentColorResource(){auto* t=self->m_textureTarget[GxBuffers_Color].m_texture;return t?textures[t].gpu.Get():color.Get();}
ID3D11DepthStencilView* CGxDeviceD3d11::Impl::CurrentDepth(){
    if(depthOverride)return depthOverride.Get();auto& target=self->m_textureTarget[GxBuffers_Depth];
    if(target.m_texture){auto it=textures.find(target.m_texture);return it!=textures.end()&&target.m_plane<it->second.faces?it->second.dsv[target.m_plane].Get():nullptr;}
    auto* colorTarget=self->m_textureTarget[GxBuffers_Color].m_texture;
    if(colorTarget){auto& t=textures[colorTarget];auto it=auxiliaryDepth.find({t.width,t.height});return it!=auxiliaryDepth.end()?it->second.second.Get():nullptr;}
    return depthView.Get();
}
bool CGxDeviceD3d11::BindTargets(){
    if(!m->context)return false;auto* color=m_textureTarget[GxBuffers_Color].m_texture;
    if(color&&!TextureReady(color))return false;
    auto* depth=m_textureTarget[GxBuffers_Depth].m_texture;if(depth&&!TextureReady(depth))return false;
    if(color&&!depth&&!m->depthOverride){auto& t=m->textures[color];const auto key=std::make_pair(t.width,t.height);
        if(!m->auxiliaryDepth.count(key)){D3D11_TEXTURE2D_DESC d{};d.Width=t.width;d.Height=t.height;d.MipLevels=1;d.ArraySize=1;
            d.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;d.SampleDesc.Count=1;d.BindFlags=D3D11_BIND_DEPTH_STENCIL;
            std::pair<ComPtr<ID3D11Texture2D>,ComPtr<ID3D11DepthStencilView>> candidate;
            if(!Check(m->device->CreateTexture2D(&d,nullptr,&candidate.first),"auxiliary depth")||!Check(m->device->CreateDepthStencilView(candidate.first.Get(),nullptr,&candidate.second),"auxiliary DSV"))return false;
            if(m->auxiliaryDepth.size()>=32)m->auxiliaryDepth.clear();m->auxiliaryDepth.emplace(key,std::move(candidate));
        }
    }
    auto* rtv=m->CurrentColor();auto* dsv=m->CurrentDepth();if(!rtv)return false;
    // Avoid simultaneously binding the same subresource for sampling and output.
    ID3D11ShaderResourceView* nulls[16]{};m->context->PSSetShaderResources(0,16,nulls);m->context->VSSetShaderResources(0,16,nulls);
    m->context->OMSetRenderTargets(1,&rtv,dsv);return true;
}
void CGxDeviceD3d11::DeviceSetRenderTarget(EGxBuffer buffer,CGxTex* t,uint32_t plane){
    if(buffer>=GxBuffers_Last||!m_context)return;
    if(t&&(!TextureReady(t)||plane>=m->textures[t].faces))return;
    if(buffer==GxBuffers_Depth)m->depthOverride.Reset();
    CGxDevice::DeviceSetRenderTarget(buffer,t,plane);BindTargets();m_viewportDirty=1;m_scissorRectDirty=1;
}
void CGxDeviceD3d11::DeviceSaveDepthSurface(){m->savedDepth=m->CurrentDepth();}
void CGxDeviceD3d11::DeviceRestoreDepthSurface(){m->depthOverride=m->savedDepth;m->savedDepth.Reset();if(m_context)BindTargets();}
void CGxDeviceD3d11::DeviceResolveDepthBuffer(CGxTex* t){
    // Preserve this Wrath virtual's actual behavior: it changes the depth surface.
    if(!m_context||!t||!TextureReady(t)||!m->textures[t].dsv[0])return;
    m->depthOverride=m->textures[t].dsv[0];BindTargets();
}

void CGxDeviceD3d11::DeviceCopyTex(CGxTex* src,uint32_t srcPlane,CGxTex* dst,uint32_t dstPlane){
    if(!m_context||!dst||!TextureReady(dst)||(src&&!TextureReady(src)))return;
    auto& to=m->textures[dst];if(dstPlane>=to.faces)return;
    ID3D11Texture2D* from=src?m->textures[src].gpu.Get():m->CurrentColorResource();
    D3D11_TEXTURE2D_DESC a{},b{};from->GetDesc(&a);to.gpu->GetDesc(&b);
    if(srcPlane>=a.ArraySize||a.Width!=b.Width||a.Height!=b.Height||a.Format!=b.Format){Log("DX11 incompatible DeviceCopyTex request");return;}
    m->UnbindViews();const UINT source=D3D11CalcSubresource(0,srcPlane,a.MipLevels),dest=D3D11CalcSubresource(0,dstPlane,b.MipLevels);
    if(a.SampleDesc.Count>1&&b.SampleDesc.Count==1)m->context->ResolveSubresource(to.gpu.Get(),dest,from,source,b.Format);
    else if(a.SampleDesc.Count==b.SampleDesc.Count)m->context->CopySubresourceRegion(to.gpu.Get(),dest,0,0,0,from,source,nullptr);
    BindTargets();
}
int32_t CGxDeviceD3d11::TexCopy(CGxTex* dst,const C2iVector& destPos,const C2iVector& sourcePos,const C2iVector& size,uint32_t level,uint32_t plane){
    if(!m_context||!dst||!TextureReady(dst)||size.x<=0||size.y<=0||destPos.x<0||destPos.y<0||sourcePos.x<0||sourcePos.y<0)return 0;
    auto& to=m->textures[dst];if(level>=to.mips||plane>=to.faces)return 0;
    auto* source=m->CurrentColorResource();D3D11_TEXTURE2D_DESC a{};source->GetDesc(&a);
    const uint32_t w=std::max(1u,to.width>>level),h=std::max(1u,to.height>>level);
    if(uint64_t(destPos.x)+size.x>w||uint64_t(destPos.y)+size.y>h||uint64_t(sourcePos.x)+size.x>a.Width||uint64_t(sourcePos.y)+size.y>a.Height)return 0;
    ComPtr<ID3D11Texture2D> resolved;
    if(a.SampleDesc.Count>1){auto d=a;d.SampleDesc.Count=1;d.SampleDesc.Quality=0;d.BindFlags=0;
        if(!Check(m->device->CreateTexture2D(&d,nullptr,&resolved),"copy resolve"))return 0;
        m->context->ResolveSubresource(resolved.Get(),0,source,0,a.Format);source=resolved.Get();
    }
    if(Format(to.storage)!=a.Format)return TexStretch(dst,nullptr,nullptr,level,plane);
    D3D11_BOX box{UINT(sourcePos.x),UINT(sourcePos.y),0,UINT(sourcePos.x+size.x),UINT(sourcePos.y+size.y),1};
    m->UnbindViews();m->context->CopySubresourceRegion(to.gpu.Get(),D3D11CalcSubresource(level,plane,to.mips),destPos.x,destPos.y,0,source,0,&box);BindTargets();return 1;
}

void CGxDeviceD3d11::QueryDestroy(CGxQuery*& q){if(!q)return;if(q->m_apiSpecific){static_cast<IUnknown*>(q->m_apiSpecific)->Release();q->m_apiSpecific=nullptr;}CGxDevice::QueryDestroy(q);}
bool CGxDeviceD3d11::QueryBegin(CGxQuery* q){
    if(!q||!m_context)return false;if(!q->m_apiSpecific){D3D11_QUERY_DESC d{D3D11_QUERY_OCCLUSION,0};ID3D11Query* query=nullptr;
        if(!Check(m->device->CreateQuery(&d,&query),"occlusion query"))return false;q->m_apiSpecific=query;}
    m->context->Begin(static_cast<ID3D11Query*>(q->m_apiSpecific));return true;
}
bool CGxDeviceD3d11::QueryEnd(CGxQuery* q){if(!q||!q->m_apiSpecific||!m_context)return false;m->context->End(static_cast<ID3D11Query*>(q->m_apiSpecific));return true;}
bool CGxDeviceD3d11::QueryGetParam(CGxQuery* q,EGxQueryParam p,uint32_t& data){
    if(!q||!q->m_apiSpecific||!m_context)return false;if(p==GxQueryParam_Unk0){data=4;return true;}
    if(p==GxQueryParam_Unk1)data=m->context->GetData(static_cast<ID3D11Query*>(q->m_apiSpecific),nullptr,0,D3D11_ASYNC_GETDATA_DONOTFLUSH)==S_OK;
    return false; // Native Unk1 updates readiness but returns false.
}
bool CGxDeviceD3d11::QueryGetData(CGxQuery* q,uint32_t& data){
    if(!q||!q->m_apiSpecific||!m_context)return false;UINT64 samples=0;
    if(m->context->GetData(static_cast<ID3D11Query*>(q->m_apiSpecific),&samples,sizeof(samples),D3D11_ASYNC_GETDATA_DONOTFLUSH)!=S_OK)return false;
    data=uint32_t(std::min(samples,UINT64(0xffffffff)));return true;
}
