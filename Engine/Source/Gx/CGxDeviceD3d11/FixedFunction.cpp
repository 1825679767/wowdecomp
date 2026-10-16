#include "Internal.h"
#include "Engine/Source/Gx/Gx.h"
#include <sstream>
using namespace gx11;

namespace {
const char* UsageName(uint32_t usage){static const char* names[]={"POSITION","BLENDWEIGHT","BLENDINDICES","NORMAL","PSIZE","TEXCOORD",
    "TANGENT","BINORMAL","TESSFACTOR","POSITIONT","COLOR","FOG","DEPTH","SAMPLE"};return usage<14?names[usage]:"INVALID";}
void Color(uint32_t packed,float* out){CImVector c;std::memcpy(&c,&packed,4);out[0]=c.r/255.f;out[1]=c.g/255.f;out[2]=c.b/255.f;out[3]=c.a/255.f;}
}

bool CGxDeviceD3d11::Impl::FixedShaders(std::shared_ptr<Variant>& vertex,std::shared_ptr<Variant>& pixel,
    const std::vector<gx::shader::Semantic>& inputs,const gx::shader::TranslationState& state){
    auto native=[&](EGxRenderState s){return self->m_appRenderStates[s].m_value.m_data.u[0];};
    // D3D9 disables the cascade at its first unbound texture stage.
    std::vector<uint32_t> stages;for(uint32_t i=0;i<8;++i){if(!self->m_appRenderStates[GxRs_Texture0+i].m_value.m_data.p)break;stages.push_back(i);}
    auto compile=[&](const std::string& source,bool vs,const std::vector<gx::shader::Semantic>& declaration)->std::shared_ptr<Variant>{
        const std::string key=std::string(vs?"V":"P")+source;auto found=fixedSources.find(key);
        if(found==fixedSources.end()){
            ComPtr<ID3DBlob> code,errors;
            const HRESULT hr=D3DCompile(source.data(),source.size(),"Native WotLK fixed-function adapter",nullptr,nullptr,"main",
                vs?"vs_3_0":"ps_3_0",D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&errors);
            if(FAILED(hr)){if(errors)CGxDevice::Log("DX11 fixed-function compile: %s",static_cast<const char*>(errors->GetBufferPointer()));return {};}
            auto shader=std::make_unique<CGxShader>();shader->m_target=vs?GxSh_Vertex:GxSh_Pixel;
            shader->m_code.SetCount(uint32_t(code->GetBufferSize()));std::memcpy(shader->m_code.Ptr(),code->GetBufferPointer(),code->GetBufferSize());
            if(fixedSources.size()>=256){variants.clear();fixedSources.clear();}
            found=fixedSources.emplace(key,std::move(shader)).first;
        }
        auto result=Shader(found->second.get(),declaration,state);if(result)result->fixed=true;return result;
    };
    if(!vertex){
        auto has=[&](uint32_t usage,uint32_t index=0){for(const auto& v:inputs)if(v.usage==usage&&v.index==index)return true;return false;};
        const bool lighting=(self->m_appMasterEnables&(1<<GxMasterEnable_Lighting))&&native(GxRs_Lighting);
        const bool specular=lighting&&self->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0]>0&&native(GxRs_MatSpecular);
        const auto colorMaterial=native(GxRs_ColorMaterial);
        std::ostringstream h;h<<"float4 c[96]:register(c0); struct I {";
        for(const auto& v:inputs){if(v.usage==0||v.usage==3)h<<"float3";else h<<"float4";
            h<<" v"<<v.usage<<'_'<<v.index<<":"<<UsageName(v.usage)<<v.index<<";";}
        h<<"};struct O{float4 p:POSITION;float4 color:COLOR0;float4 spec:COLOR1;float fog:FOG;";
        for(auto i:stages)h<<"float4 t"<<i<<":TEXCOORD"<<i<<";";
        h<<"}; O main(I v){O o;float4 p=float4(v.v0_0,1);o.p=float4(dot(p,c[0]),dot(p,c[1]),dot(p,c[2]),dot(p,c[3]));"
            "float3 vp=float3(dot(p,c[4]),dot(p,c[5]),dot(p,c[6]));";
        h<<"float3 n="<<(has(3)?"v.v3_0":"float3(0,0,1)")<<";n=float3(dot(n,c[8].xyz),dot(n,c[9].xyz),dot(n,c[10].xyz));";
        if(native(GxRs_NormalizeNormals))h<<"n=normalize(n);";
        h<<"float4 diffuse="<<(has(10)&&(!lighting||colorMaterial==0)?"v.v10_0":"c[12]")<<";o.spec=0;";
        if(lighting){
            h<<"float3 emissive="<<(has(10)&&colorMaterial==2?"v.v10_0.rgb":"c[13].rgb")<<";float3 lit=0;float3 spec=0;";
            for(uint32_t i=0;i<4;++i){const uint32_t b=16+i*5;
                h<<"{float3 d=c["<<b<<"].w>0?c["<<b<<"].xyz-vp:-c["<<b<<"].xyz;float distance=length(d);"
                    "float attenuation=c["<<b<<"].w>0?1/max(dot(c["<<b+4<<"].xyz,float3(1,distance,distance*distance)),0.00001):1;"
                    "float ndl=max(dot(normalize(d),n),0);lit+=(c["<<b+2<<"].rgb+c["<<b+1<<"].rgb*ndl)*attenuation;";
                if(specular)h<<"if(ndl>0)spec+=c["<<b+3<<"].rgb*pow(max(dot(n,normalize(normalize(d)+float3(0,0,-1))),0),c[15].x)*attenuation;";
                h<<"}";
            }h<<"diffuse.rgb=saturate(emissive+diffuse.rgb*lit);";
            if(specular)h<<"o.spec.rgb=saturate(spec*"<<(has(10)&&colorMaterial==1?"v.v10_0.rgb":"c[14].rgb")<<");";
        }
        h<<"o.color=diffuse;o.fog=saturate((c[36].y-abs(vp.z))*c[36].z);";
        for(auto i:stages){const uint32_t b=40+i*4,coord=std::min(native(EGxRenderState(GxRs_TextureCoord0+i)),7u);
            const uint32_t gen=native(EGxRenderState(GxRs_TexGen0+i));
            h<<"{float4 tc=";
            if(gen==GxTexGen_ObjectLinear)h<<"p";
            else if(gen==GxTexGen_EyeLinear)h<<"float4(vp,1)";
            else if(gen==GxTexGen_Normal)h<<"float4(n,1)";
            else if(gen==GxTexGen_ReflectionMap)h<<"float4(reflect(normalize(vp),n),1)";
            else if(gen==GxTexGen_SphereMap)h<<"float4(reflect(normalize(vp),n),1)";
            else if(has(5,coord))h<<"v.v5_"<<coord;
            else h<<"float4(0,0,0,1)";
            h<<";";
            if(gen==GxTexGen_SphereMap)h<<"tc.xy=tc.xy/(2*max(length(float3(tc.xy,tc.z+1)),0.00001))+0.5;tc.z=0;tc.w=1;";
            h<<"o.t"<<i<<"=float4(dot(tc,c["<<b<<"]),dot(tc,c["<<b+1<<"]),dot(tc,c["<<b+2<<"]),dot(tc,c["<<b+3<<"]));}";
        }h<<"return o;}";
        vertex=compile(h.str(),true,inputs);if(!vertex)return false;
    }
    if(!pixel&&!self->m_appRenderStates[GxRs_PixelShader].m_value.m_data.p){
        bool color=false,specular=false,fog=false;std::vector<uint32_t> coords;
        for(const auto& v:vertex->translated.outputs){if(v.usage==10&&v.index==0)color=true;if(v.usage==10&&v.index==1)specular=true;if(v.usage==11)fog=true;if(v.usage==5)coords.push_back(v.index);}
        std::ostringstream h;h<<"float4 c[2]:register(c0);struct I{";
        if(color)h<<"float4 color:COLOR0;";if(specular)h<<"float4 spec:COLOR1;";if(fog)h<<"float fog:FOG;";
        for(auto i:coords)h<<"float4 t"<<i<<":TEXCOORD"<<i<<";";
        h<<"};";for(auto i:stages)h<<(state.samplerTypes[i]==2?"samplerCUBE":"sampler2D")<<" s"<<i<<":register(s"<<i<<");";
        h<<"float4 main(I v):COLOR{float4 diffuse="<<(color?"v.color":"float4(1,1,1,1)")<<";float4 current=diffuse;";
        for(auto i:stages){const uint32_t coord=std::min(native(EGxRenderState(GxRs_TextureCoord0+i)),7u);
            h<<"{float4 tc=";
            if(std::find(coords.begin(),coords.end(),i)!=coords.end())h<<"v.t"<<i;
            else if(std::find(coords.begin(),coords.end(),coord)!=coords.end())h<<"v.t"<<coord;
            else h<<"float4(0,0,0,1)";
            h<<";";if(native(EGxRenderState(GxRs_TextureShader0+i))==2)h<<"tc.xyz/=tc.w;";
            h<<"float4 sample="<<(state.samplerTypes[i]==2?"texCUBE":"tex2D")<<"(s"<<i<<",tc."<<(state.samplerTypes[i]==2?"xyz":"xy")<<");";
            auto expression=[&](uint32_t op,const char* components){std::ostringstream e;
                switch(op){case 1:e<<"2*sample."<<components<<"*current."<<components;break;
                case 2:e<<"sample."<<components<<"+current."<<components;break;
                case 3:e<<"current."<<components;break;
                case 4:e<<"lerp(sample."<<components<<",current."<<components<<",current.a)";break;
                case 5:e<<"lerp(current."<<components<<",sample."<<components<<",diffuse.a)";break;
                default:e<<"sample."<<components<<"*current."<<components;break;}return e.str();};
            h<<"current=float4(saturate("<<expression(native(EGxRenderState(GxRs_ColorOp0+i)),"rgb")<<"),saturate("
                <<expression(native(EGxRenderState(GxRs_AlphaOp0+i)),"a")<<"));}";
        }
        if(specular)h<<"current.rgb=saturate(current.rgb+v.spec.rgb);";
        // The legacy converter applies fog after specular and texture stages.
        h<<"return current;}";
        pixel=compile(h.str(),false,vertex->translated.outputs);if(!pixel)return false;
    }
    return true;
}

void CGxDeviceD3d11::Impl::FixedConstants(bool vertex,uint32_t* words){
    std::memset(words,0,4096);auto* c=reinterpret_cast<float*>(words);
    auto native=[&](EGxRenderState s){return self->m_appRenderStates[s].m_value.m_data.u[0];};
    if(!vertex){Color(native(GxRs_FogColor),c);return;}
    const auto& world=self->m_xforms[GxXform_World];const auto& view=self->m_xforms[GxXform_View];
    const C44Matrix wv=world.m_mtx[world.m_level]*view.m_mtx[view.m_level];
    const C44Matrix wvp=(wv*self->m_projNative).Transpose();const auto transposed=wv.Transpose();
    std::memcpy(c,&wvp,64);std::memcpy(c+16,&transposed,64);
    const C44Matrix normal=wv.Inverse();std::memcpy(c+32,&normal,64);
    Color(native(GxRs_MatDiffuse),c+48);Color(native(GxRs_MatEmissive),c+52);Color(native(GxRs_MatSpecular),c+56);
    c[60]=self->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0];
    for(uint32_t i=0;i<4;++i){const auto& light=self->m_lights[i];float* out=c+(16+i*5)*4;
        const auto& transform=view.m_mtx[view.m_level];const auto& d=light.m_dir;
        out[0]=d.x*transform.a0+d.y*transform.b0+d.z*transform.c0+d.w*transform.d0;
        out[1]=d.x*transform.a1+d.y*transform.b1+d.z*transform.c1+d.w*transform.d1;
        out[2]=d.x*transform.a2+d.y*transform.b2+d.z*transform.c2+d.w*transform.d2;out[3]=d.w;
        if(light.m_enable){std::memcpy(out+4,&light.m_dirColor,12);std::memcpy(out+8,&light.m_ambColor,12);std::memcpy(out+12,&light.m_specColor,12);}
        out[16]=light.m_constantAttenuation;out[17]=light.m_linearAttenuation;out[18]=light.m_quadraticAttenuation;
    }
    c[144]=self->m_appRenderStates[GxRs_FogStart].m_value.m_data.f[0];c[145]=self->m_appRenderStates[GxRs_FogEnd].m_value.m_data.f[0];c[146]=c[145]!=c[144]?1/(c[145]-c[144]):0;
    for(uint32_t i=0;i<8;++i){const auto& tex=self->m_xforms[i];const auto& gen=self->m_texGen[i];
        const auto matrix=(gen.m_mtx[gen.m_level]*tex.m_mtx[tex.m_level]).Transpose();std::memcpy(c+(40+i*4)*4,&matrix,64);}
}
