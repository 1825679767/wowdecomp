#include "Internal.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/Gx.h"

using namespace gx11;

std::shared_ptr<CGxDeviceD3d11::Impl::Variant> CGxDeviceD3d11::Impl::Shader(CGxShader* shader,
    const std::vector<gx::shader::Semantic>& decl,const gx::shader::TranslationState& state){
    if(!shader||!shader->m_code.Count())return {};
    std::string key=Key(shader);
    for(const auto& d:decl)key+=Key(d);
    key+=Key(state.alphaFunc)+Key(state.primitive)+Key(state.clipPlanes)+Key(state.alphaTest)+Key(state.fog)+Key(state.fogMode)+Key(state.shadowSamplers);
    for(auto type:state.samplerTypes)key+=Key(type);
    auto found=variants.find(key);if(found!=variants.end())return found->second;
    auto candidate=std::make_shared<Variant>();std::string error;
    const bool vertex=shader->m_target==GxSh_Vertex;
    if(!gx::shader::TranslateLegacy(shader->m_code.Ptr(),shader->m_code.Count(),vertex,decl,state,candidate->translated,error)){
        if(lastError!=error){CGxDevice::Log("DX11 shader conversion refused: %s",error.c_str());lastError=error;}return {};
    }
    auto& code=candidate->translated.dxbc;
    const HRESULT hr=vertex?device->CreateVertexShader(code.data(),code.size(),nullptr,&candidate->vertex):
        device->CreatePixelShader(code.data(),code.size(),nullptr,&candidate->pixel);
    if(!Check(hr,"converted shader"))return {};
    if(variants.size()>=4096)variants.erase(variants.begin());
    variants.emplace(std::move(key),candidate);return candidate;
}

void CGxDeviceD3d11::IShaderCreate(CGxShader* shader){
    if(!shader)return;shader->m_loaded=1;shader->m_valid=0;shader->m_apiSpecific=nullptr;
    if(!m_context)return;gx::shader::TranslationState state;
    std::vector<gx::shader::Semantic> decl;
    if(shader->m_target==GxSh_Vertex){
        for(uint32_t a=0;a<GxVAs_Last;++a)decl.push_back({Usage(a),UsageIndex(a),a,15,(a==4||a==5)?4u:0u,3});
    }else if(shader->m_target==GxSh_Pixel){
        decl.push_back({0,0,0,15});decl.push_back({10,0,1,15});decl.push_back({10,1,2,15});
        for(uint32_t i=0;i<8;++i)decl.push_back({5,i,i+3,15});decl.push_back({11,0,11,1});
    }else return;
    auto v=m->Shader(shader,decl,state);if(!v)return;
    m->shaderObjects[shader]=v;
    shader->m_valid=1;shader->m_apiSpecific=v->vertex?static_cast<void*>(v->vertex.Get()):static_cast<void*>(v->pixel.Get());
}
void CGxDeviceD3d11::ShaderDestroy(CGxShader*& shader){
    if(!shader)return;
    if(shader->m_refCount==1){const auto prefix=Key(shader);
        m->shaderObjects.erase(shader);
        for(auto i=m->variants.begin();i!=m->variants.end();)if(i->first.compare(0,prefix.size(),prefix)==0)i=m->variants.erase(i);else ++i;
        shader->m_apiSpecific=nullptr;shader->m_valid=0;shader->m_loaded=0;
    }
    CGxDevice::ShaderDestroy(shader);
}
void CGxDeviceD3d11::IShaderReload(CGxShader* shader,const char* path,const char* name){
    if(!shader||!path||!name)return;const auto prefix=Key(shader);
    m->shaderObjects.erase(shader);
    for(auto i=m->variants.begin();i!=m->variants.end();)if(i->first.compare(0,prefix.size(),prefix)==0)i=m->variants.erase(i);else ++i;
    shader->m_apiSpecific=nullptr;shader->m_loaded=0;shader->m_valid=0;shader->m_code.SetCount(0);
    CGxShader* one=shader;IShaderLoad(&one,EGxShTarget(shader->m_target),path,name,1);
}

bool CGxDeviceD3d11::Impl::UploadConstants(const Variant& shader,bool vertex){
    const auto& c=shader.translated;uint32_t words[1024]{};
    std::memcpy(words,self->ShaderConstantsLock(vertex?GxSh_Vertex:GxSh_Pixel),sizeof(words));
    if(shader.fixed)FixedConstants(vertex,words);
    for(const auto& v:c.inlineConstants[0]){if(v.index>1020)return false;std::memcpy(words+v.index,v.words,16);}
    const uint32_t stage=vertex?0:1;context->UpdateSubresource(constants[stage][0].Get(),0,nullptr,words,0,0);
    uint32_t ints[64]{};for(const auto& v:c.inlineConstants[1]){if(v.index>60)return false;std::memcpy(ints+v.index,v.words,16);}
    context->UpdateSubresource(constants[stage][1].Get(),0,nullptr,ints,0,0);
    uint32_t bools[16]{};for(const auto& v:c.inlineConstants[2]){if(v.index>=16)return false;bools[v.index]=v.words[0];}
    context->UpdateSubresource(constants[stage][2].Get(),0,nullptr,bools,0,0);
    float ext[64]{};
    if(vertex){const float w=self->m_curWindowRect.maxX-self->m_curWindowRect.minX,h=self->m_curWindowRect.maxY-self->m_curWindowRect.minY;
        ext[0]=w?1/w:0;ext[1]=h?-1/h:0;ext[4]=self->m_appRenderStates[GxRs_PointScale].m_value.m_data.f[0];ext[5]=1;ext[6]=64;
        for(uint32_t i=0;i<6;++i)std::memcpy(ext+8+i*4,&self->m_clipPlane[i],16);
    }else{
        CImVector fog;std::memcpy(&fog,&self->m_appRenderStates[GxRs_FogColor].m_value.m_data.u[0],4);
        ext[0]=fog.r/255.f;ext[1]=fog.g/255.f;ext[2]=fog.b/255.f;
        ext[3]=self->m_appRenderStates[GxRs_AlphaRef].m_value.m_data.i[0]/255.f;
        ext[4]=self->m_appRenderStates[GxRs_FogStart].m_value.m_data.f[0];ext[5]=self->m_appRenderStates[GxRs_FogEnd].m_value.m_data.f[0];
        ext[6]=ext[5]!=ext[4]?1/(ext[5]-ext[4]):0;
    }
    context->UpdateSubresource(constants[stage][3].Get(),0,nullptr,ext,0,0);
    ID3D11Buffer* buffers[]={constants[stage][0].Get(),constants[stage][1].Get(),constants[stage][2].Get(),constants[stage][3].Get()};
    if(vertex)context->VSSetConstantBuffers(0,4,buffers);else context->PSSetConstantBuffers(0,4,buffers);
    return true;
}

bool CGxDeviceD3d11::SyncDraw(EGxPrim primitive){
    if(!m_context||!m->device)return false;
    std::vector<CGxVertexAttrib> attributes;std::vector<CGxBuf*> attributeBuffers;
    if(m_primVertexFormat<GxVertexBufferFormats_Last&&m_primVertexBuf){
        const auto& desc=Buffer::s_vertexBufDesc[m_primVertexFormat];
        attributes.assign(desc.attribs,desc.attribs+desc.attribCount);attributeBuffers.assign(desc.attribCount,m_primVertexBuf);
    }else for(uint32_t a=0;a<GxVAs_Last;++a)if((m_primVertexMask&(1<<a))&&m_primVertexFormatBuf[a]){
        attributes.push_back(m_primVertexFormatAttrib[a]);attributeBuffers.push_back(m_primVertexFormatBuf[a]);
    }
    if(attributes.empty())return false;
    static const DXGI_FORMAT formats[]={DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UINT,DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R32G32_FLOAT,DXGI_FORMAT_R32G32B32_FLOAT,DXGI_FORMAT_R16G16_SINT,DXGI_FORMAT_R32_FLOAT};
    std::vector<gx::shader::Semantic> inputs;
    std::vector<D3D11_INPUT_ELEMENT_DESC> elements;std::vector<CGxBuf*> streams;
    ID3D11Buffer* buffers[16]{};UINT strides[16]{},offsets[16]{};
    for(size_t i=0;i<attributes.size();++i){const auto& a=attributes[i];auto* buf=attributeBuffers[i];
        if(a.m_attrib>=GxVAs_Last||a.m_type>=7||!buf||!buf->m_valid||!PoolReady(buf->m_pool))return false;
        auto stream=std::find(streams.begin(),streams.end(),buf);uint32_t slot=uint32_t(stream-streams.begin());
        if(stream==streams.end()){if(slot>=16)return false;streams.push_back(buf);buffers[slot]=m->pools[buf->m_pool].gpu.Get();
            strides[slot]=a.m_bufSize?a.m_bufSize:buf->m_itemSize;offsets[slot]=buf->m_index;}
        if(strides[slot]!=(a.m_bufSize?a.m_bufSize:buf->m_itemSize))return false;
        D3D11_INPUT_ELEMENT_DESC d{};d.SemanticName=SemanticName(a.m_attrib);d.SemanticIndex=UsageIndex(a.m_attrib);
        d.Format=formats[a.m_type];d.InputSlot=slot;d.AlignedByteOffset=a.m_offset;d.InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
        elements.push_back(d);
        inputs.push_back({Usage(a.m_attrib),UsageIndex(a.m_attrib),uint32_t(i),15,a.m_type==0?4u:((a.m_type==1||a.m_type==5)?1u:0u),a.m_type==1?1u:(a.m_type==5?2u:3u)});
    }
    gx::shader::TranslationState state;state.primitive=primitive==GxPrim_Points?1u:(primitive<GxPrim_Triangles?2u:4u);
    state.alphaTest=m_appRenderStates[GxRs_AlphaRef].m_value.m_data.i[0]>0;state.alphaFunc=7;
    state.fog=(m_appMasterEnables&(1<<GxMasterEnable_Fog))&&m_appRenderStates[GxRs_Fog].m_value.m_data.i[0];state.fogMode=0;
    state.clipPlanes=m_appRenderStates[GxRs_ClipPlaneMask].m_value.m_data.u[0]&63;
    for(uint32_t i=0;i<16;++i){auto* t=static_cast<CGxTex*>(m_appRenderStates[GxRs_Texture0+i].m_value.m_data.p);
        if(t){if(!TextureReady(t))return false;state.samplerTypes[i]=t->m_target==GxTex_CubeMap?2:1;if(t->m_format==GxTex_D24X8)state.shadowSamplers|=1<<i;}
    }
    if(!BindTargets())return false;
    auto* vs=static_cast<CGxShader*>(m_appRenderStates[GxRs_VertexShader].m_value.m_data.p);
    auto* ps=static_cast<CGxShader*>(m_appRenderStates[GxRs_PixelShader].m_value.m_data.p);
    std::shared_ptr<Impl::Variant> vertex,pixel;
    if(vs)vertex=m->Shader(vs,inputs,state);
    if(vertex&&ps)pixel=m->Shader(ps,vertex->translated.outputs,state);
    if((vs&&!vertex)||(ps&&vertex&&!pixel))return false;
    if((!vs||!ps)&&!m->FixedShaders(vertex,pixel,inputs,state))return false;
    if(!vertex||!pixel)return false;
    std::string layoutKey;
    for(const auto& a:attributes)layoutKey+=Key(a.m_attrib)+Key(a.m_type)+Key(a.m_offset)+Key(a.m_bufSize);
    for(auto* b:attributeBuffers){const auto i=std::find(streams.begin(),streams.end(),b);const uint32_t slot=uint32_t(i-streams.begin());layoutKey+=Key(slot);}
    auto found=vertex->layouts.find(layoutKey);
    if(found==vertex->layouts.end()){
        ComPtr<ID3D11InputLayout> layout;const auto& code=vertex->translated.dxbc;
        // Include only semantics consumed by this shader. Unused native streams
        // remain available for other permutations without changing shader linkage.
        std::vector<D3D11_INPUT_ELEMENT_DESC> used;
        for(size_t i=0;i<inputs.size();++i)for(const auto& s:vertex->translated.inputs)
            if(inputs[i].usage==s.usage&&inputs[i].index==s.index){used.push_back(elements[i]);break;}
        if(!Check(m->device->CreateInputLayout(used.data(),UINT(used.size()),code.data(),code.size(),&layout),"input layout"))return false;
        if(vertex->layouts.size()>=32)vertex->layouts.clear();found=vertex->layouts.emplace(layoutKey,std::move(layout)).first;
    }
    m->context->IASetInputLayout(found->second.Get());m->context->IASetVertexBuffers(0,UINT(streams.size()),buffers,strides,offsets);
    m->context->VSSetShader(vertex->vertex.Get(),nullptr,0);m->context->PSSetShader(pixel->pixel.Get(),nullptr,0);
    if(!m->UploadConstants(*vertex,true)||!m->UploadConstants(*pixel,false)||!m->States())return false;
    for(uint32_t i=0;i<16;++i){auto* t=static_cast<CGxTex*>(m_appRenderStates[GxRs_Texture0+i].m_value.m_data.p);
        ID3D11ShaderResourceView* srv=nullptr;
        if(t&&t!=m_textureTarget[GxBuffers_Color].m_texture&&t!=m_textureTarget[GxBuffers_Depth].m_texture)srv=m->textures[t].srv.Get();
        m->context->PSSetShaderResources(i,1,&srv);
        if(i<4)m->context->VSSetShaderResources(i,1,&srv);
    }
    return true;
}

void CGxDeviceD3d11::Draw(CGxBatch* batch,int32_t indexed){
    // Wrath's second argument is an indexed/non-indexed flag, not an array count.
    if(!batch||!batch->m_count||batch->m_primType>=GxPrims_Last||!SyncDraw(batch->m_primType))return;
    static const D3D11_PRIMITIVE_TOPOLOGY topology[]={D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
        D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST};
    m->context->IASetPrimitiveTopology(topology[batch->m_primType]);
    if(batch->m_primType==GxPrim_TriangleFan){
        if(batch->m_count<3||batch->m_count>1024*1024)return;std::vector<uint32_t> indices((batch->m_count-2)*3);
        const uint16_t* native=nullptr;
        if(indexed){if(!m_primIndexBuf||!PoolReady(m_primIndexBuf->m_pool))return;const auto& pool=m->pools[m_primIndexBuf->m_pool];
            const uint64_t at=uint64_t(m_primIndexBuf->m_index)+uint64_t(batch->m_start)*2;
            if(at+uint64_t(batch->m_count)*2>pool.cpu.size())return;native=reinterpret_cast<const uint16_t*>(pool.cpu.data()+at);}
        auto index=[&](uint32_t i){return native?uint32_t(native[i]):batch->m_start+i;};
        for(uint32_t i=0;i<batch->m_count-2;++i){indices[i*3]=index(0);indices[i*3+1]=index(i+1);indices[i*3+2]=index(i+2);}
        D3D11_BUFFER_DESC d{};d.ByteWidth=UINT(indices.size()*4);d.Usage=D3D11_USAGE_IMMUTABLE;d.BindFlags=D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA data{};data.pSysMem=indices.data();ComPtr<ID3D11Buffer> buffer;
        if(!Check(m->device->CreateBuffer(&d,&data,&buffer),"triangle fan indices"))return;
        m->context->IASetIndexBuffer(buffer.Get(),DXGI_FORMAT_R32_UINT,0);m->context->DrawIndexed(UINT(indices.size()),0,0);
    }else if(indexed){
        if(!m_primIndexBuf||!m_primIndexBuf->m_valid||!PoolReady(m_primIndexBuf->m_pool))return;
        const uint64_t end=uint64_t(m_primIndexBuf->m_index)+uint64_t(batch->m_start+uint64_t(batch->m_count))*2;
        if(end>m->pools[m_primIndexBuf->m_pool].cpu.size())return;
        m->context->IASetIndexBuffer(m->pools[m_primIndexBuf->m_pool].gpu.Get(),DXGI_FORMAT_R16_UINT,m_primIndexBuf->m_index);
        m->context->DrawIndexed(batch->m_count,batch->m_start,0);
    }else m->context->Draw(batch->m_count,batch->m_start);
}
