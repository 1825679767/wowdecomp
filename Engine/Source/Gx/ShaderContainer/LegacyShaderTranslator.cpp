#include "LegacyShaderTranslator.h"
#undef WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dhal.h>
#define _d3d9TYPES_H_
#define D3D12_TOKENIZED_PROGRAM_FORMAT_HEADER
#include <d3d10umddi.h>
#include <ShaderConv.h>
#include <algorithm>
#include <cstring>
#include <memory>
#include <sstream>

namespace gx::shader {
namespace {
using Bytes=std::vector<uint8_t>;
uint32_t Read(const uint8_t* p) { uint32_t v; std::memcpy(&v,p,4); return v; }
void Put(Bytes& b,size_t pos,uint32_t v) { std::memcpy(b.data()+pos,&v,4); }
bool Fail(std::string& error,const std::string& text) {error=text;return false;}
const char* Name(uint32_t usage,bool input) {
    static const char* names[]={"POSITION","BLENDWEIGHT","BLENDINDICES","NORMAL","PSIZE","TEXCOORD",
        "TANGENT","BINORMAL","TESSFACTOR","POSITIONT","COLOR","FOG","DEPTH","SAMPLE",
        "SV_IsFrontFace","SV_Position","SV_ClipDistance","POINTSPRITE"};
    if(!input&&(usage==D3DDECLUSAGE_POSITION||usage==D3DDECLUSAGE_VPOS))return "SV_Position";
    return usage<18?names[usage]:"INVALID";
}
uint32_t System(uint32_t usage) {
    switch(usage){case D3DDECLUSAGE_POSITION:case D3DDECLUSAGE_VPOS:return 1;
        case D3DDECLUSAGE_VFACE:return 9;case D3DDECLUSAGE_CLIPDISTANCE:return 2;default:return 0;}
}
// Pre-SM2 instruction lengths, including opcode. Only shader instructions are
// accepted; comments and END are handled separately by the bounded tokenizer.
uint32_t LegacyLength(uint32_t op) {
    switch(op) {
    case D3DSIO_NOP:case D3DSIO_PHASE:return 1;
    case D3DSIO_MOV:case D3DSIO_RCP:case D3DSIO_RSQ:case D3DSIO_EXP:case D3DSIO_LOG:
    case D3DSIO_LIT:case D3DSIO_FRC:case D3DSIO_ABS:case D3DSIO_NRM:
    case D3DSIO_EXPP:case D3DSIO_LOGP:return 3;
    case D3DSIO_ADD:case D3DSIO_SUB:case D3DSIO_MUL:case D3DSIO_DP3:case D3DSIO_DP4:
    case D3DSIO_MIN:case D3DSIO_MAX:case D3DSIO_SLT:case D3DSIO_SGE:case D3DSIO_DST:
    case D3DSIO_M4x4:case D3DSIO_M4x3:case D3DSIO_M3x4:case D3DSIO_M3x3:case D3DSIO_M3x2:return 4;
    case D3DSIO_MAD:case D3DSIO_LRP:case D3DSIO_CND:case D3DSIO_CMP:case D3DSIO_DP2ADD:return 5;
    case D3DSIO_DEF:return 6;
    case D3DSIO_TEXCOORD:case D3DSIO_TEXKILL:case D3DSIO_TEX:return 2;
    case D3DSIO_TEXBEM:case D3DSIO_TEXBEML:case D3DSIO_TEXREG2AR:case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXM3x2PAD:case D3DSIO_TEXM3x2TEX:case D3DSIO_TEXM3x3PAD:case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3VSPEC:case D3DSIO_TEXREG2RGB:case D3DSIO_TEXDP3TEX:case D3DSIO_TEXM3x2DEPTH:
    case D3DSIO_TEXDP3:case D3DSIO_TEXM3x3:case D3DSIO_TEXDEPTH:return 3;
    case D3DSIO_TEXM3x3SPEC:return 4;
    default:return 0;
    }
}
bool TokenBounds(const Bytes& code,bool vertex,std::string& error) {
    if(code.size()<8||code.size()>1024*1024||code.size()%4)return Fail(error,"invalid legacy bytecode size");
    const uint32_t version=Read(code.data()),major=(version>>8)&255,minor=version&255;
    if((version>>16)!=(vertex?0xfffeu:0xffffu)||major<1||major>3)return Fail(error,"legacy shader version/stage mismatch");
    size_t i=4;
    while(i<code.size()) {
        const uint32_t token=Read(code.data()+i),op=token&65535;
        if(op==D3DSIO_END)return i+4==code.size()||Fail(error,"trailing data after shader END");
        uint32_t words;
        if(op==D3DSIO_COMMENT)words=1+((token>>16)&0x7fff);
        else if(major>=2)words=1+((token>>24)&15);
        else {
            words=LegacyLength(op);
            if(!vertex&&minor==4&&(op==D3DSIO_TEX||op==D3DSIO_TEXCOORD))words=3;
        }
        if(!words||size_t(words)>((code.size()-i)/4))return Fail(error,"truncated or unsupported legacy instruction span");
        i+=words*4;
    }
    return Fail(error,"missing shader END");
}
Bytes Signature(std::vector<Semantic> values,bool input,bool vertexInput,bool pixelOutput=false) {
    std::stable_sort(values.begin(),values.end(),[](const auto&a,const auto&b){return a.reg<b.reg;});
    Bytes b(8+24*values.size());Put(b,0,uint32_t(values.size()));Put(b,4,8);
    for(size_t i=0;i<values.size();++i) {
        const auto& v=values[i];const size_t at=8+24*i;
        const char* name=pixelOutput?(v.usage==D3DDECLUSAGE_DEPTH?"SV_Depth":"SV_Target"):Name(v.usage,vertexInput);
        Put(b,at,uint32_t(b.size()));Put(b,at+4,v.index);
        Put(b,at+8,pixelOutput?(v.usage==D3DDECLUSAGE_DEPTH?65:64):(vertexInput?0:System(v.usage)));
        Put(b,at+12,v.usage==D3DDECLUSAGE_VFACE?1:(vertexInput?v.componentType:3));Put(b,at+16,v.reg);
        b[at+20]=uint8_t(v.mask);b[at+21]=input?uint8_t(v.mask):0;
        b.insert(b.end(),name,name+std::strlen(name)+1);
    }
    while(b.size()%4)b.push_back(0);return b;
}
bool BuildDxbc(const ShaderConv::ByteCode& tokens,const Bytes& in,const Bytes& out,Bytes& result,std::string& error) {
    if(!tokens.m_pByteCode||tokens.m_byteCodeSize<8||tokens.m_byteCodeSize>4*1024*1024)return Fail(error,"invalid converted token stream");
    result.assign(44,0);std::memcpy(result.data(),"DXBC",4);Put(result,20,1);Put(result,28,3);
    const Bytes body(static_cast<const uint8_t*>(tokens.m_pByteCode),static_cast<const uint8_t*>(tokens.m_pByteCode)+tokens.m_byteCodeSize);
    const Bytes* parts[]={&in,&out,&body};const char* ids[]={"ISGN","OSGN","SHDR"};
    for(size_t i=0;i<3;++i) {
        Put(result,32+i*4,uint32_t(result.size()));const size_t at=result.size();result.resize(at+8);
        std::memcpy(result.data()+at,ids[i],4);Put(result,at+4,uint32_t(parts[i]->size()));
        result.insert(result.end(),parts[i]->begin(),parts[i]->end());
    }
    Put(result,24,uint32_t(result.size()));
    // Official Microsoft signer, loaded only from the executable/system folder.
    static HMODULE signer=LoadLibraryExW(L"dxbcSigner.dll",nullptr,LOAD_LIBRARY_SEARCH_APPLICATION_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    using Sign=HRESULT (WINAPI*)(BYTE*,UINT32);
    const auto sign=signer?reinterpret_cast<Sign>(GetProcAddress(signer,"SignDxbc")):nullptr;
    if(!sign)return Fail(error,"official dxbcSigner.dll is unavailable");
    const HRESULT hr=sign(result.data(),uint32_t(result.size()));
    if(FAILED(hr))return Fail(error,"DXBC signing failed");return true;
}
}

bool TranslateLegacy(const void* data,size_t size,bool vertex,const std::vector<Semantic>& declaration,
                     const TranslationState& state,TranslatedShader& output,std::string& error) {
    output={};error.clear();
    if(!data||size<8||size>1024*1024||declaration.size()>64)return Fail(error,"translation input outside limits");
    try {
        Bytes code(static_cast<const uint8_t*>(data),static_cast<const uint8_t*>(data)+size);
        if(!TokenBounds(code,vertex,error))return false;
        ShaderConv::RasterStates raster;
        raster.TCIMapping=ShaderConv::TCIMASK_PASSTHRU;raster.PrimitiveType=state.primitive;
        raster.AlphaTestEnable=state.alphaTest;raster.AlphaFunc=state.alphaFunc;raster.FogEnable=state.fog;
        raster.FogTableMode=state.fogMode;raster.UserClipPlanes=state.clipPlanes;
        raster.HardwareShadowMappingRequiredPS=state.shadowSamplers;
        for(uint32_t i=0;i<16;++i)if(state.samplerTypes[i])raster.PSSamplers[i].TextureType=state.samplerTypes[i];
        ShaderConv::VSInputDecls inputs(16);ShaderConv::VSOutputDecls outputs;
        for(const auto& v:declaration) {
            if(v.usage>17||v.index>15||v.reg>15||!v.mask||v.mask>15)return Fail(error,"invalid semantic declaration");
            if(vertex)inputs.AddDecl(v.usage,v.index,v.reg,0,v.conversion);
            else outputs.AddDecl(v.usage,v.index,v.reg,v.mask<<16);
        }
        ShaderConv::ConvertShaderArgs args(9,0,raster);
        args.type=vertex?ShaderConv::ConvertShaderArgs::SHADER_TYPE::SHADER_TYPE_VERTEX:ShaderConv::ConvertShaderArgs::SHADER_TYPE::SHADER_TYPE_PIXEL;
        args.pVsInputDecl=&inputs;args.pVsOutputDecl=&outputs;args.pPsInputDecl=vertex?nullptr:&outputs;
        args.legacyByteCode.m_pByteCode=code.data();args.legacyByteCode.m_byteCodeSize=code.size();
        ShaderConv::ShaderConverterAPI converter;
        const HRESULT hr=converter.ConvertShader(args);
        struct Cleanup {ShaderConv::ByteCode& code;~Cleanup(){ShaderConv::ShaderConverterAPI::CleanUpConvertedShader(code);}} cleanup{args.convertedByteCode};
        if(FAILED(hr)){std::ostringstream s;s<<"legacy translation HRESULT=0x"<<std::hex<<uint32_t(hr);return Fail(error,s.str());}
        TranslatedShader candidate;
        if(vertex) {
            for(uint32_t i=0;i<inputs.GetSize();++i){const auto& v=inputs[i];
                uint32_t type=3;for(const auto& d:declaration)if(d.usage==v.Usage&&d.index==v.UsageIndex)type=d.componentType;
                candidate.inputs.push_back({v.Usage,v.UsageIndex,v.RegIndex,15,v.InputConversion,type});}
            for(uint32_t i=0;i<outputs.GetSize();++i){const auto& v=outputs[i];candidate.outputs.push_back({v.Usage,v.UsageIndex,v.RegIndex,v.WriteMask});}
        } else {
            candidate.inputs=declaration;
            for(const auto& v:args.AddedSystemSemantics)candidate.inputs.push_back({v.Usage,v.UsageIndex,v.RegIndex,v.WriteMask});
            for(uint32_t i=0;i<4;++i)if(args.outputRegistersMask&(1<<i))candidate.outputs.push_back({D3DDECLUSAGE_COLOR,i,i,15});
            if(args.outputRegistersMask&ShaderConv::DEPTH_OUTPUT_MASK)candidate.outputs.push_back({D3DDECLUSAGE_DEPTH,0,0xffffffff,1});
        }
        if(!BuildDxbc(args.convertedByteCode,Signature(candidate.inputs,true,vertex),Signature(candidate.outputs,false,false,!vertex),candidate.dxbc,error))return false;
        candidate.floatWords=args.maxFloatConstsUsed;candidate.intWords=args.maxIntConstsUsed;candidate.boolWords=args.maxBoolConstsUsed;
        for(uint32_t c=0;c<3;++c)for(const auto& v:args.m_inlineConsts[c]) {
            InlineConstant r;r.index=v.RegIndex;std::memcpy(r.words,v.Value,sizeof(r.words));candidate.inlineConstants[c].push_back(r);
        }
        output=std::move(candidate);return true;
    }catch(const std::exception& e){return Fail(error,e.what());}
}
}
