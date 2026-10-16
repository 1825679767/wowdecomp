// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/WmoMaterialPlan.hpp"
#include "platform/waystone/WmoChunkIndex.hpp"
#include "engine/assets/shared/models/wmo/ShaderFallback.hpp"
#include <cstring>
namespace wxl::waystone::wmo {
namespace {
uint32_t Read(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
void Write(uint8_t* p,uint32_t v){std::memcpy(p,&v,4);}
}
bool PlanMaterials(std::span<const uint8_t> bytes,TextureResolver resolve,bool effects,
                   MaterialPlan& output,std::string& reason) {
    reason.clear();auto fail=[&](const char* why){reason=why;return false;};
    RootIndex index;if(!resolve || !IndexRoot(bytes,index) || !index.modern)return fail("modern material root required");
    const auto range=index.slots[2];if(!range.present || range.size/64>65535)return fail("material table required/bounded");
    MaterialPlan result;result.records.reserve(range.size/64);
    for(uint32_t at=range.offset;at<range.offset+range.size;at+=64) {
        MaterialRecord record;const auto* source=bytes.data()+at;
        record.sourceShader=Read(source+4);if(record.sourceShader>23)return fail("unknown material shader");
        record.layered=record.sourceShader==23;
        if(record.layered){constexpr uint32_t offsets[]{12,24,36,40,44,48,52,56,60};
            for(unsigned i=0;i<9;++i)record.fileIds[i]=Read(source+offsets[i]);++result.layered;
        }else {record.fileIds[0]=Read(source+12);record.fileIds[1]=Read(source+24);}
        auto name=[&](unsigned slot){if(!record.fileIds[slot])return true;const char* p=resolve(record.fileIds[slot]);
            if(!p || !*p || std::strlen(p)>900)return false;record.paths[slot]=p;return true;};
        if(record.layered){for(unsigned i=0;i<9;++i)if(!name(i))return fail("unresolved layered texture");}
        else {
            if(!name(0))return fail("unresolved primary texture");
            if(record.paths[0].empty())record.paths[0]="createcrappygreentexture.blp";
        }
        record.nativeShader=wxl::assets::wmo::NativeShaderFor(record.sourceShader);
        const bool second=effects && (record.nativeShader==3 || record.nativeShader==5 || record.nativeShader==6) && record.fileIds[1];
        if(!record.layered && second && !name(1))return fail("unresolved secondary texture");
        if(!second && (record.nativeShader==3 || record.nativeShader==5 || record.nativeShader==6))record.nativeShader=4;
        if(record.nativeShader!=record.sourceShader)++result.remapped;
        std::memcpy(record.native.data(),source,64);Write(record.native.data()+4,record.nativeShader);
        std::memset(record.native.data()+48,0,16); // conservative serialized compatibility tail; build4 refs are separate
        result.records.push_back(std::move(record));
    }
    output=std::move(result);return true;
}
}
