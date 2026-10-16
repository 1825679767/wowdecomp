#include "Engine/Source/Gx/ShaderContainer/LegacyContainer.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyShaderTranslator.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <cstring>
using Microsoft::WRL::ComPtr;
int main(int argc,char** argv) try {
    if(argc!=2){std::cerr<<"usage: LegacyShaderAudit extracted-shader-directory\n";return 2;}
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;D3D_FEATURE_LEVEL level;
    if(FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,&level,&context)))throw std::runtime_error("WARP device");
    std::vector<gx::shader::Semantic> vertexInputs,pixelInputs{{0,0,0,15},{10,0,1,15},{10,1,2,15}};
    for(uint32_t a=0;a<14;++a){const uint32_t usage=a<4?a:(a<6?10:5),index=a<5?0:(a==5?1:a-6);
        vertexInputs.push_back({usage,index,a,15,(a==4||a==5)?4u:(a==2?1u:0u),a==2?1u:3u});}
    for(uint32_t i=0;i<8;++i)pixelInputs.push_back({5,i,i+3,15});pixelInputs.push_back({11,0,11,1});
    uint32_t files=0,converted=0,empty=0,emptyFiles=0,failed=0;std::cout<<"{\"results\":[\n";bool first=true;
    for(const auto& entry:std::filesystem::recursive_directory_iterator(argv[1])){
        if(!entry.is_regular_file()||entry.path().extension()!=L".bls")continue;
        const auto size=entry.file_size();if(size>gx::shader::LegacyContainer::MaxFileBytes)throw std::runtime_error("asset size");
        if(!size){++emptyFiles;continue;}
        std::vector<uint8_t> bytes(static_cast<size_t>(size));std::ifstream in(entry.path(),std::ios::binary);
        if(!in.read(reinterpret_cast<char*>(bytes.data()),bytes.size()))throw std::runtime_error("asset read");
        gx::shader::LegacyContainer family;std::string error;
        if(!family.Open(bytes.data(),bytes.size(),error))throw std::runtime_error(entry.path().string()+": "+error);
        ++files;uint32_t index=0;
        for(const auto& row:family.Permutations()){
            if(row.code.empty()){++empty;++index;continue;}
            uint32_t token=0;if(row.code.size()>=4)std::memcpy(&token,row.code.data(),4);const bool vertex=(token>>16)==0xfffe;
            gx::shader::TranslationState state;gx::shader::TranslatedShader translated;
            bool ok=gx::shader::TranslateLegacy(row.code.data(),row.code.size(),vertex,vertex?vertexInputs:pixelInputs,state,translated,error);
            if(ok){HRESULT hr;
                if(vertex){ComPtr<ID3D11VertexShader> shader;hr=device->CreateVertexShader(translated.dxbc.data(),translated.dxbc.size(),nullptr,&shader);}
                else{ComPtr<ID3D11PixelShader> shader;hr=device->CreatePixelShader(translated.dxbc.data(),translated.dxbc.size(),nullptr,&shader);}
                if(FAILED(hr)){ok=false;error="D3D11 API creation HRESULT="+std::to_string(uint32_t(hr));}
            }
            if(ok)++converted;else++failed;
            if(!first)std::cout<<",\n";first=false;
            std::cout<<"{\"file\":"<<std::quoted(entry.path().generic_string())<<",\"permutation\":"<<index<<",\"passed\":"<<(ok?"true":"false");
            if(!ok)std::cout<<",\"error\":"<<std::quoted(error);std::cout<<"}";++index;
        }
        std::cerr<<"audited files="<<files<<" converted="<<converted<<" refused="<<failed<<'\n';
    }
    std::cout<<"\n],\"files\":"<<files<<",\"converted\":"<<converted<<",\"empty\":"<<empty<<",\"empty_files\":"<<emptyFiles<<",\"failed\":"<<failed<<"}\n";
    return failed||!converted?1:0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
