#pragma once
#include "WXL/GrassWind.hpp"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyContainer.h"
static void GrassWindChecks(bool warp){
    CGxDeviceD3d11 d;g_theGxDevicePtr=&d;Check(d.CreateOffscreen(16,16,warp),"grass motion native device");
    const auto fixtureRoot=std::filesystem::path(__FILE__).parent_path()/"Fixtures";
    for(const auto* profile:{"vs20","vs30"}){
        const auto path=fixtureRoot/(std::string("detaildoodad-")+profile+".bls");
        std::ifstream file(path,std::ios::binary);Check(bool(file),"exact native grass family fixture");
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)),{});gx::shader::LegacyContainer family;std::string error;
        Check(family.Open(bytes.data(),bytes.size(),error)&&family.Permutations().size()==6,"native grass permutation family");
        for(unsigned i=0;i<6;++i){
            const auto& permutation=family.Permutations()[i];const auto name=std::string("WXL.GrassRegression.")+profile+std::to_string(i);
            auto* shader=WxlProgram(d,GxSh_Vertex,name.c_str(),permutation.code.data(),permutation.code.size());
            uint32_t count=0;Check(!wxl::native::grass_wind::VertexCode(*shader,count)&&!count,"stock sibling identity never receives grass patch");
            auto* previous=CDetailDoodad::s_vertexShader[i];CDetailDoodad::s_vertexShader[i]=shader;
            const auto* injected=wxl::native::grass_wind::VertexCode(*shader,count);Check(injected&&count>permutation.code.size(),"exact native grass identity receives donor sway/repulsion instructions");
            // The stock variant was already compiled above. Identity-aware keys
            // must now compile the injected variant, including every sincos path.
            shader->m_loaded=0;Check(shader->Valid()!=0,"donor grass shader converts and creates real D3D11 vertex shader");
            Check(shader->m_code.Count()==permutation.code.size()&&!std::memcmp(shader->m_code.Ptr(),permutation.code.data(),permutation.code.size()),"grass injection preserves immutable native shader source");
            Check(d.RecoverDevice()&&shader->Valid(),"grass GPU recreation preserves native identity and source");
            CDetailDoodad::s_vertexShader[i]=previous;d.ShaderDestroy(shader);
        }
    }
    d.DeviceDestroy();g_theGxDevicePtr=nullptr;
}
