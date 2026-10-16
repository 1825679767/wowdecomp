#pragma once
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include <cstring>

namespace wxl::native {
// Cached caps are safe on loader workers. Shader conversion and GPU access
// remain on the native render thread, exactly as in the D3D9 owners.
inline bool LegacyMaterialBackend(CGxDevice* device,uint32_t tmus=2){
 if(!device)return false;
 const auto api=device->m_api;
 bool supported=api==GxApi_D3d9||api==GxApi_D3d9Ex;
#ifdef WXL_ENABLE_DX11
 supported=supported||api==GxApi_D3d11;
#endif
 const auto& caps=device->Caps();
 return supported&&caps.m_shaderTargets[GxSh_Vertex]>=3&&caps.m_shaderTargets[GxSh_Pixel]>=4&&caps.m_numTmus>=tmus;
}
inline void SyncMaterialState(CGxDevice& device){
 if(device.m_api==GxApi_D3d9)static_cast<CGxDeviceD3d&>(device).IStateSync();
 else if(device.m_api==GxApi_D3d9Ex)static_cast<CGxDeviceD3d9Ex&>(device).IStateSync();
 // D3D11 rebuilds the bindings from Gx state at each Draw. Never cast it to
 // either D3D9 class, including when unwinding a custom RsPush/RsPop scope.
}
template<class T,size_t N>inline bool ExactMaterialProgram(CGxDevice& device,EGxRenderState state,const T(&bytes)[N]){
 auto* shader=static_cast<CGxShader*>(device.m_appRenderStates[state].m_value.m_data.p);
 return shader&&shader->m_valid&&shader->m_code.Count()==sizeof(bytes)&&!std::memcmp(shader->m_code.Ptr(),bytes,sizeof(bytes));
}
}
