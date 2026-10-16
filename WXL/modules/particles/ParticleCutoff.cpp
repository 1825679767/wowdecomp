// Source-native per-particle cutoff rendering through the existing Gx shader API.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ParticleCutoff.hpp"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include <algorithm>
#include <cmath>
namespace wxl::native {
float ParticleCutoffTrack::Sample(float age)const noexcept{
 if(times.empty()||times.size()!=values.size()||!std::isfinite(age))return 0;
 const float position=std::clamp(age,0.0f,1.0f)*65535.0f;
 if(position<=times.front())return values.front()/65535.0f;
 if(position>=times.back())return values.back()/65535.0f;
 const auto upper=std::upper_bound(times.begin(),times.end(),position);
 const size_t i=static_cast<size_t>(upper-times.begin());
 const float blend=(position-times[i-1])/float(times[i]-times[i-1]);
 return (float(values[i-1])+(float(values[i])-values[i-1])*blend)/65535.0f;
}
bool DrawParticleCutoffVertices(CGxBuf* buffer,EGxVertexBufferFormat format,uint32_t vertices,
 uint32_t perParticle,uint32_t indices,const std::vector<float>& cutoffs){
 if(!g_theGxDevicePtr||!CParticleEmitter2::s_indexBuf||!buffer||!vertices||!perParticle||!indices||vertices%perParticle||
    cutoffs.size()!=vertices/perParticle||vertices>16384||
    !CShaderEffect::s_enableShaders||!CShaderEffect::s_curEffect)return false;
 // These are the exact native SetDefaultShaders(0) vertex/light selections.
 // Pixel permutation bit8 is native floating alpha-reference shader support,
 // avoiding 8-bit fixed-function thresholds on both D3D9 and OpenGL.
 const auto shadow=(std::min)(CShaderEffect::s_shadowValue,2u);
 const auto vertex=CShaderEffect::s_lightEnabled+2*(CShaderEffect::s_localLightCount+15*shadow);
 const auto pixel=shadow+4*(CShaderEffect::s_usePcfFiltering+2);
 auto* effect=CShaderEffect::s_curEffect;
 if(vertex>=90||pixel>=16||!effect->m_vertexShaders[vertex]||!effect->m_pixelShaders[pixel]||
    !effect->m_vertexShaders[vertex]->Valid()||!effect->m_pixelShaders[pixel]->Valid())return false;
 for(const float value:cutoffs)if(!std::isfinite(value)||value<0||value>1)return false;
 if(!CParticleEmitter2::s_indexBuf->m_hasData||!CParticleEmitter2::s_indexBuf->m_valid)
  CParticleEmitter2::RenderIndices(CParticleEmitter2::s_indexBuf);
 const float saved=CShaderEffect::s_fogColorAlphaRef.w;
 C44Matrix identity;g_theGxDevicePtr->XformSetView(identity);
 GxPrimVertexPtr(buffer,format);g_theGxDevicePtr->PrimIndexPtr(CParticleEmitter2::s_indexBuf);
 CShaderEffect::SetTexMtx_Identity(0);
 CShaderEffect::SetShaders(vertex,pixel);
 CShaderEffect::UpdateWorldViewMatrix();
 // Preserve native sort order. Only adjacent equal thresholds may share a draw.
 for(size_t begin=0;begin<cutoffs.size();){
  const float threshold=(std::max)(saved,cutoffs[begin]);size_t end=begin+1;
  while(end<cutoffs.size()&&(std::max)(saved,cutoffs[end])==threshold)++end;
  CShaderEffect::SetAlphaRef(threshold);
  CGxBatch batch;batch.m_primType=GxPrim_Triangles;
  batch.m_start=static_cast<uint32_t>(begin)*indices;
  batch.m_count=static_cast<uint32_t>(end-begin)*indices;
  batch.m_minIndex=static_cast<uint16_t>(begin*perParticle);
  batch.m_maxIndex=static_cast<uint16_t>(end*perParticle-1);
  g_theGxDevicePtr->Draw(&batch,1);begin=end;
 }
 // Update the same cached native state that was used to apply thresholds.
 CShaderEffect::SetAlphaRef(saved);CShaderEffect::SetDefaultShaders(0);
 return true;
}
}
