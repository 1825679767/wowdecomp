#pragma once
#include "WXL/GrassWind.hpp"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyContainer.h"
#include "Engine/Source/Gx/ShaderContainer/LegacyShaderTranslator.h"
static void GrassWindChecks(bool warp){
    CGxDeviceD3d11 d;g_theGxDevicePtr=&d;Check(d.CreateOffscreen(16,16,warp),"grass motion native device");
    const char* pixelSource="float4 main(float4 c:COLOR0):COLOR{return c;}";ID3DBlob* pixelCode=nullptr;ID3DBlob* errors=nullptr;
    const auto compiled=D3DCompile(pixelSource,std::strlen(pixelSource),"GrassLightingRegression",nullptr,nullptr,"main","ps_3_0",0,0,&pixelCode,&errors);if(errors)errors->Release();Check(SUCCEEDED(compiled)&&pixelCode,"grass comparison pixel program");
    auto* pixel=WxlProgram(d,GxSh_Pixel,"WXL.GrassRegression.PS",pixelCode->GetBufferPointer(),pixelCode->GetBufferSize());pixelCode->Release();
    d.RsSet(GxRs_PixelShader,pixel);d.RsSet(GxRs_DepthTest,0);d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_Culling,0);d.RsSet(GxRs_ScissorTest,0);d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_Fog,0);d.RsSet(GxRs_AlphaRef,0);
    struct GrassVertex{float x,y,z,nx,ny,nz;CImVector color;float u,v;};const GrassVertex geometry[]={{-1,-1,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f},{-1,3,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f},{3,-1,.5f,0,0,1,CImVector(255,255,255,255),.5f,.5f}};
    auto* vertices=d.BufStream(GxPoolTarget_Vertex,sizeof(GrassVertex),3);auto* memory=d.BufLock(vertices);Check(memory!=nullptr,"grass comparison native stream");std::memcpy(memory,geometry,sizeof(geometry));d.BufUnlock(vertices,0);
    const CGxVertexAttrib attributes[]={{GxVA_Position,4,0,36},{GxVA_Normal,4,12,36},{GxVA_Color0,0,24,36},{GxVA_TexCoord0,3,28,36}};d.PrimVertexFormat(vertices,attributes,4);d.PrimVertexMask((1<<GxVA_Position)|(1<<GxVA_Normal)|(1<<GxVA_Color0)|(1<<GxVA_TexCoord0));
    float native[23][4]{};for(unsigned i=0;i<4;++i)native[i][i]=native[4+i][i]=1;native[8][1]=native[8][2]=1;native[10][2]=1;for(unsigned i=0;i<3;++i){native[11][i]=.1f;native[12][i]=.2f;native[14+3*i][2]=5;native[15+3*i][i]=.2f;native[16+3*i][0]=1;}
    const float still[8][4]={{0,0,0,0},{1,.5f,0,.001f},{0,0,0,0},{0,0,2,0},{0,0,0,0},{0,0,0,0},{.737f,1.311f,1.7f,.015f},{1,0,-.3f,1/.7f}};
    d.ShaderConstantsSet(GxSh_Vertex,0,&native[0][0],23);d.ShaderConstantsSet(GxSh_Vertex,32,&still[0][0],8);
    CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_count=3;batch.m_maxIndex=2;
    auto draw=[&](CGxShader* shader){d.RsSet(GxRs_VertexShader,shader);d.SceneClear(3,CImVector(0,0,0,255));const auto before=d.SubmittedDraws();d.Draw(&batch,0);Check(d.SubmittedDraws()==before+1,"grass permutation submits native GPU geometry");return Center(d);};
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
            const auto stock=draw(shader);Check(stock.r>20&&stock.g>20&&stock.b>20,"stock grass lighting comparison is nonempty");
            auto* previous=CDetailDoodad::s_vertexShader[i];CDetailDoodad::s_vertexShader[i]=shader;
            const auto* injected=wxl::native::grass_wind::VertexCode(*shader,count);Check(injected&&count>permutation.code.size(),"exact native grass identity receives donor sway/repulsion instructions");
            // The stock variant was already compiled above. Identity-aware keys
            // must now compile the injected variant, including every sincos path.
            shader->m_loaded=0;Check(shader->Valid()!=0,"donor grass shader converts and creates real D3D11 vertex shader");
            Check(shader->m_code.Count()==permutation.code.size()&&!std::memcmp(shader->m_code.Ptr(),permutation.code.data(),permutation.code.size()),"grass injection preserves immutable native shader source");
            Equal(draw(shader),stock,"zero motion preserves exact native directional and point lighting",1);
            float moving[8][4];std::memcpy(moving,still,sizeof(moving));moving[2][3]=.35f;moving[3][3]=1.5707963f;moving[4][0]=1;moving[6][2]=moving[6][3]=0;moving[7][2]=0;moving[7][3]=1;
            d.ShaderConstantsSet(GxSh_Vertex,32,&moving[0][0],8);draw(shader);CiRect edge(8,0,9,1);TSGrowableArray<CImVector> moved;d.ReadTargetPixels(edge,moved);
            Check(moved.Count()==1,"grass displacement readback size");
            if(moved[0].r||moved[0].g||moved[0].b){
                std::cerr<<"grass motion profile="<<profile<<" permutation="<<i<<'\n';float actual[32]{};d.CopyShaderConstants(GxSh_Vertex,32,8,actual);for(unsigned row=0;row<8;++row)std::cerr<<"c"<<row+32<<'='<<actual[row*4]<<','<<actual[row*4+1]<<','<<actual[row*4+2]<<','<<actual[row*4+3]<<'\n';
                ID3DBlob* assembly=nullptr;if(SUCCEEDED(D3DDisassemble(injected,count,0,nullptr,&assembly))&&assembly){std::ofstream out("failed-grass-injected.asm",std::ios::binary);out.write(static_cast<const char*>(assembly->GetBufferPointer()),assembly->GetBufferSize());assembly->Release();}
                gx::shader::TranslationState state;state.primitive=4;gx::shader::TranslatedShader translated;std::string conversionError;
                std::vector<gx::shader::Semantic> inputs{{0,0,0,15,0,3},{3,0,1,15,0,3},{10,0,2,15,4,3},{5,0,3,15,0,3}};
                if(gx::shader::TranslateLegacy(injected,count,true,inputs,state,translated,conversionError)){assembly=nullptr;if(SUCCEEDED(D3DDisassemble(translated.dxbc.data(),translated.dxbc.size(),0,nullptr,&assembly))&&assembly){std::ofstream out("failed-grass-dxbc.asm",std::ios::binary);out.write(static_cast<const char*>(assembly->GetBufferPointer()),assembly->GetBufferSize());assembly->Release();}}
            }
            Equal(moved[0],CImVector(0,0,0,255),"enabled grass sway displaces native geometry on GPU",1);
            d.ShaderConstantsSet(GxSh_Vertex,32,&still[0][0],8);Equal(draw(shader),stock,"disabling grass motion restores native geometry and lighting",1);
            Check(d.RecoverDevice()&&shader->Valid(),"grass GPU recreation preserves native identity and source");
            CDetailDoodad::s_vertexShader[i]=previous;d.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));d.ShaderDestroy(shader);
        }
    }
    d.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));d.ShaderDestroy(pixel);d.DeviceDestroy();g_theGxDevicePtr=nullptr;
}
