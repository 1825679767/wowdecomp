#include "Engine/Source/Gx/ShaderContainer/LegacyShaderTranslator.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

using Microsoft::WRL::ComPtr;
using Bytes=std::vector<uint8_t>;
static uint32_t Read(const uint8_t*p){uint32_t v;std::memcpy(&v,p,4);return v;}
static std::vector<Bytes> Load(const char* path) {
    const auto size=std::filesystem::file_size(path);
    if(size<12||size>32*1024*1024)throw std::runtime_error("file size");
    Bytes raw(static_cast<size_t>(size));std::ifstream in(path,std::ios::binary);
    if(!in.read(reinterpret_cast<char*>(raw.data()),raw.size()))throw std::runtime_error("read");
    if(Read(raw.data())!=0x47585348||Read(raw.data()+4)!=0x10003)throw std::runtime_error("requires v3 container");
    const uint32_t count=Read(raw.data()+8);if(!count||count>65536)throw std::runtime_error("count");
    std::vector<Bytes> code;size_t pos=12;
    for(uint32_t i=0;i<count;++i) {
        if(pos>raw.size()||raw.size()-pos<16)throw std::runtime_error("record header");
        uint32_t n=Read(raw.data()+pos+12);if(n>raw.size()-pos-16)throw std::runtime_error("code bounds");
        code.emplace_back(raw.begin()+pos+16,raw.begin()+pos+16+n);pos+=16+((size_t(n)+3)&~size_t(3));
    }
    if(pos!=raw.size())throw std::runtime_error("trailing container data");return code;
}
int main(int argc,char**argv) try {
    if(argc<3){std::cerr<<"usage: LegacyShaderCheck vertex-v3.bls pixel-v3.bls [disassemble]\n";return 2;}
    const auto vs=Load(argv[1]),ps=Load(argv[2]);
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;D3D_FEATURE_LEVEL fl;
    HRESULT hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,&fl,&context);
    if(FAILED(hr))throw std::runtime_error("WARP creation");
    gx::shader::TranslationState state;uint32_t checks=0;
    for(size_t vi=0;vi<vs.size();++vi) {
        if(vs[vi].empty())continue;
        if(argc==4){ComPtr<ID3DBlob> asmCode;if(SUCCEEDED(D3DDisassemble(vs[vi].data(),vs[vi].size(),0,nullptr,&asmCode)))
            std::cout.write(static_cast<const char*>(asmCode->GetBufferPointer()),asmCode->GetBufferSize());}
        gx::shader::TranslatedShader v;std::string error;
        if(!gx::shader::TranslateLegacy(vs[vi].data(),vs[vi].size(),true,{},state,v,error))throw std::runtime_error("vertex "+std::to_string(vi)+": "+error);
        ComPtr<ID3D11VertexShader> shader;
        hr=device->CreateVertexShader(v.dxbc.data(),v.dxbc.size(),nullptr,&shader);
        if(FAILED(hr)){std::cerr<<"VS HRESULT="<<std::hex<<uint32_t(hr)<<std::dec<<'\n';throw std::runtime_error("converted vertex creation");}
        ++checks;
        std::cout<<"VS "<<vi<<" inputs="<<v.inputs.size()<<" outputs="<<v.outputs.size()<<" float_words="<<v.floatWords<<'\n';
        for(const auto&s:v.inputs)std::cout<<" input usage="<<s.usage<<" index="<<s.index<<" reg="<<s.reg<<'\n';
        for(const auto&s:v.outputs)std::cout<<" output usage="<<s.usage<<" index="<<s.index<<" reg="<<s.reg<<" mask="<<s.mask<<'\n';
        for(size_t pi=0;pi<ps.size();++pi) {
            if(ps[pi].empty())continue;
            if(argc==4&&vi==0){ComPtr<ID3DBlob> asmCode;if(SUCCEEDED(D3DDisassemble(ps[pi].data(),ps[pi].size(),0,nullptr,&asmCode)))
                std::cout.write(static_cast<const char*>(asmCode->GetBufferPointer()),asmCode->GetBufferSize());}
            gx::shader::TranslatedShader p;
            if(!gx::shader::TranslateLegacy(ps[pi].data(),ps[pi].size(),false,v.outputs,state,p,error))throw std::runtime_error("pixel "+std::to_string(pi)+": "+error);
            ComPtr<ID3D11PixelShader> shader;
            hr=device->CreatePixelShader(p.dxbc.data(),p.dxbc.size(),nullptr,&shader);
            if(FAILED(hr)){std::cerr<<"PS HRESULT="<<std::hex<<uint32_t(hr)<<std::dec<<'\n';throw std::runtime_error("converted pixel creation");}
            ++checks;std::cout<<"PS "<<pi<<" linked_vs="<<vi<<" float_words="<<p.floatWords<<'\n';
        }
    }
    std::cout<<"PASS actual legacy shader conversions/API creations="<<checks<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
