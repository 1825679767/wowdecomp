#include "Internal.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyContainer.h"
#include <cstdio>

using namespace gx11;

namespace {
bool LoadFamily(EGxShTarget target,int32_t profile,const char* path,const char* name,uint32_t count,
    gx::shader::LegacyContainer& container,std::string& error){
    if(!path||!name||std::strlen(path)>128||std::strlen(name)>128||!count||count>65536){error="invalid shader family request";return false;}
    for(int32_t p=profile;p>0;--p){
        const char* model=target==GxSh_Vertex?CGxDevice::s_vertexShaderTargetNames[p]:CGxDevice::s_pixelShaderTargetNames[p];
        char filename[512];const int n=std::snprintf(filename,sizeof(filename),"%s\\%s\\%s.bls",path,model,name);
        if(n<0||n>=int(sizeof(filename))){error="shader family path exceeds limit";return false;}
        SFileImpl* file=nullptr;if(!SFileOpenFile(filename,&file)||!file)continue;
        struct Close{SFileImpl* file;~Close(){SFileCloseFile(file);}} close{file};
        uint32_t high=0;const uint32_t size=SFileGetFileSize(file,&high);
        if(!high&&!size)continue; // Native stock archives contain empty profile placeholders.
        if(high||size<12||size>gx::shader::LegacyContainer::MaxFileBytes){error="shader family file size outside limits";return false;}
        std::vector<uint8_t> bytes(size);uint32_t actual=0;
        if(!SFileReadFile(file,bytes.data(),size,&actual,nullptr,0)||actual!=size){error="short mounted shader read";return false;}
        if(!container.Open(bytes.data(),bytes.size(),error))return false;
        if(container.Permutations().size()!=count){error="mounted shader family permutation count mismatch";return false;}
        for(const auto& row:container.Permutations())if(!row.code.empty()&&
            !gx::shader::ValidateLegacyBytecode(row.code.data(),row.code.size(),target==GxSh_Vertex,error))return false;
        return true;
    }
    error="no mounted native shader family for requested profiles";return false;
}
void Assign(CGxShader* shader,const gx::shader::LegacyPermutation& row){
    shader->m_code.SetCount(uint32_t(row.code.size()));if(!row.code.empty())std::memcpy(shader->m_code.Ptr(),row.code.data(),row.code.size());
    shader->unk_003C=row.flags;shader->unk_0040=row.unknown;shader->unk_0044=row.wordA;shader->unk_0046=row.wordB;
    shader->m_loaded=0;shader->m_valid=0;shader->m_patched=0;shader->m_apiSpecific=nullptr;
}
}

void CGxDeviceD3d11::ShaderCreate(CGxShader** shaders,EGxShTarget target,const char* path,const char* name,uint32_t count){
    if(!shaders||!count||count>65536)return;std::fill(shaders,shaders+count,nullptr);
    if((target!=GxSh_Vertex&&target!=GxSh_Pixel)||!path||!name||std::strlen(path)>128||std::strlen(name)>128)return;
    std::vector<CGxShader*> candidate,created;std::vector<std::string> keys;
    try{
        candidate.resize(count);created.reserve(count);keys.reserve(count);
        const std::string family="dx11|"+std::to_string(target)+"|"+std::to_string(m_caps.m_shaderTargets[target])+"|"+path+"|"+name+"|"+std::to_string(count)+"|";
        bool missing=false;
        for(uint32_t i=0;i<count;++i){keys.push_back(family+std::to_string(i));candidate[i]=m_shaderList[target].Ptr(keys.back().c_str());if(!candidate[i])missing=true;}
        gx::shader::LegacyContainer container;
        if(missing){std::string error;
            if(!LoadFamily(target,m_caps.m_shaderTargets[target],path,name,count,container,error)){Log("DX11 shader family %s refused: %s",name,error.c_str());return;}
            for(uint32_t i=0;i<count;++i)if(!candidate[i]){
                auto* shader=m_shaderList[target].New(keys[i].c_str(),0,0);if(!shader)throw std::bad_alloc();
                created.push_back(shader);shader->m_target=target;Assign(shader,container.Permutations()[i]);candidate[i]=shader;
                m->shaderSources[shader]={path,name,count,i};
            }
        }
        for(uint32_t i=0;i<count;++i){++candidate[i]->m_refCount;shaders[i]=candidate[i];}
    }catch(const std::bad_alloc&){
        for(auto* shader:created){m->shaderSources.erase(shader);m_shaderList[target].Delete(shader);}
        Log("DX11 shader family allocation failed");
    }
}

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
        m->shaderSources.erase(shader);
        for(auto i=m->variants.begin();i!=m->variants.end();)if(i->first.compare(0,prefix.size(),prefix)==0)i=m->variants.erase(i);else ++i;
        shader->m_apiSpecific=nullptr;shader->m_valid=0;shader->m_loaded=0;
    }
    CGxDevice::ShaderDestroy(shader);
}
void CGxDeviceD3d11::IShaderReload(CGxShader* shader,const char* path,const char* name){
    if(!shader||!path||!name)return;const auto prefix=Key(shader);
    auto found=m->shaderSources.find(shader);if(found==m->shaderSources.end()){Log("DX11 reload refused for an unowned shader family");return;}
    const auto source=found->second;gx::shader::LegacyContainer container;std::string error;
    if(!LoadFamily(EGxShTarget(shader->m_target),m_caps.m_shaderTargets[shader->m_target],source.path.c_str(),source.name.c_str(),source.count,container,error)){
        Log("DX11 shader reload kept previous family: %s",error.c_str());return;
    }
    m->shaderObjects.erase(shader);
    for(auto i=m->variants.begin();i!=m->variants.end();)if(i->first.compare(0,prefix.size(),prefix)==0)i=m->variants.erase(i);else ++i;
    Assign(shader,container.Permutations()[source.index]);
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
    if(ps&&!pixel)pixel=m->Shader(ps,vertex->translated.outputs,state);
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

uint64_t CGxDeviceD3d11::SubmittedDraws() const{return m->drawCalls;}
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
    ++m->drawCalls;
    char trace[8]{};
    if(m->presentedFrames==90&&GetEnvironmentVariableA("WXL_DX11_TRACE_DRAWS",trace,sizeof(trace))){
        static uint32_t draw=0;++draw;
        FILE* file=nullptr;fopen_s(&file,"Logs/dx11-draw-trace.txt","a");
        if(file){
            fprintf(file,"draw=%u count=%u indexed=%d format=%u target=%p\n",draw,batch->m_count,indexed,m_primVertexFormat,m_textureTarget[0].m_texture);
            for(auto state:{GxRs_VertexShader,GxRs_PixelShader}){
                auto* shader=static_cast<CGxShader*>(m_appRenderStates[state].m_value.m_data.p);auto it=m->shaderSources.find(shader);
                if(it!=m->shaderSources.end())fprintf(file,"shader=%s/%s permutation=%u\n",it->second.path.c_str(),it->second.name.c_str(),it->second.index);
                else fprintf(file,"shader=%p\n",shader);
            }
            for(uint32_t i=0;i<8;++i){auto* t=static_cast<CGxTex*>(m_appRenderStates[GxRs_Texture0+i].m_value.m_data.p);
                if(t)fprintf(file,"texture%u=%p %ux%u format=%u data=%u\n",i,t,t->m_width,t->m_height,t->m_format,t->m_dataFormat);}
            fclose(file);
        }
        if(draw<=32){
            TSGrowableArray<CImVector> pixels;CiRect rect(0,0,m->height,m->width);ReadTargetPixels(rect,pixels);
            if(pixels.Count()==m->width*m->height){
                char name[128];std::snprintf(name,sizeof(name),"Logs/dx11-draw-%02u.bmp",draw);fopen_s(&file,name,"wb");
                if(file){BITMAPFILEHEADER header{};BITMAPINFOHEADER info{};
                    header.bfType=0x4d42;header.bfOffBits=sizeof(header)+sizeof(info);header.bfSize=header.bfOffBits+pixels.Count()*4;
                    info.biSize=sizeof(info);info.biWidth=m->width;info.biHeight=-LONG(m->height);info.biPlanes=1;info.biBitCount=32;
                    fwrite(&header,sizeof(header),1,file);fwrite(&info,sizeof(info),1,file);fwrite(pixels.Ptr(),4,pixels.Count(),file);fclose(file);
                }
            }
        }
    }
    if(auto* target=m_textureTarget[GxBuffers_Color].m_texture)m->textures[target].mipGenerationDirty=true;
}
