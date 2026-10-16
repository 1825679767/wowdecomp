// Explicit native-compatible modern procedural payload policy.
#pragma once
#include "WXL/ItemGraph.hpp"
#include <array>
#include <bit>
#include <cmath>
namespace wxl::native {
inline bool DecodeProceduralPayload(const ItemGraphProcedural& row,std::array<float,4>& v){
 for(uint32_t i=0;i<4;++i){v[i]=std::bit_cast<float>(row.rawValues[i]);if(!std::isfinite(v[i]))return false;}
 const bool color=v[0]>=0&&v[0]<=16777215&&std::trunc(v[0])==v[0];
 auto time=[](float seconds){return seconds>=0&&double(seconds)*1000.0<=double(INT32_MAX);};
 if(row.type==1)return color&&v[1]==0&&v[2]==0&&v[3]==0;
 if(row.type==13)return color&&time(v[1])&&time(v[2])&&v[2]>0&&double(v[1]+double(v[2]))*1000.0<=double(INT32_MAX)&&v[3]==0;
 if(row.type==14)return v[0]>=0&&v[0]<=1&&v[1]==0&&time(v[2])&&v[3]==0;
 return false;
}
}
