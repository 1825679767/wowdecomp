#include "QuestWorldDraw.hpp"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include <array>
#include <cmath>
namespace wxl::native::quest_marker {namespace {
CTexture* atlas=nullptr;
template<class T>struct Com {T* p=nullptr;~Com(){if(p)p->Release();}};
struct Pop {CGxDevice& owner;explicit Pop(CGxDevice& d):owner(d){owner.RsPush();}~Pop(){owner.RsPop();}};
struct State {Com<IDirect3DStateBlock9> block;bool ready=false;explicit State(IDirect3DDevice9& d){ready=SUCCEEDED(d.CreateStateBlock(D3DSBT_ALL,&block.p))&&block.p&&SUCCEEDED(block.p->Capture());}~State(){if(ready)block.p->Apply();}};
struct Vertex {float x,y,z;DWORD color;float u,v;};
}
unsigned DrawWorldNavigation(std::span<const WorldPoint> points,float scale,unsigned alpha){
 auto* owner=g_theGxDevicePtr;if(!owner||points.empty()||points.size()>65||!std::isfinite(scale)||scale<=0||scale>8||alpha>255)return 0;
 IDirect3DDevice9* device=nullptr;if(owner->m_api==GxApi_D3d9)device=static_cast<CGxDeviceD3d*>(owner)->m_d3dDevice;else if(owner->m_api==GxApi_D3d9Ex)device=static_cast<CGxDeviceD3d9Ex*>(owner)->m_d3dDevice;if(!device)return 0;
 if(!atlas)atlas=TextureCreate("Interface\\Navigation\\InGameNavigationUI",CGxTexFlags(GxTex_Linear),nullptr,0);auto* gx=atlas?TextureGetGxTex(atlas,0,nullptr):nullptr;if(!gx)return 0;
 // Bind through the actual native texture owner, including Ex cache residency.
 // RsPop marks the previous desired texture dirty; hardware overrides restore first.
 Pop nativeState(*owner);owner->RsSet(GxRs_Texture0,gx);if(owner->m_api==GxApi_D3d9)static_cast<CGxDeviceD3d*>(owner)->IStateSync();else static_cast<CGxDeviceD3d9Ex*>(owner)->IStateSync();
 auto* texture=static_cast<IDirect3DBaseTexture9*>(gx->m_apiSpecificData);if(!texture)return 0;State hardware(*device);if(!hardware.ready)return 0;
 C44Matrix view;GxXformView(view);auto projection=owner->m_projNative;D3DVIEWPORT9 viewport{};if(FAILED(device->GetViewport(&viewport))||!viewport.Height)return 0;
 const auto* v=reinterpret_cast<const float*>(&view);auto* pr=reinterpret_cast<float*>(&projection);if(!std::isfinite(pr[5])||std::abs(pr[5])<1e-5f)return 0;
 const float nearZ=-pr[14]/pr[10];if(!std::isfinite(nearZ)||nearZ<=0||nearZ>=100000)return 0;pr[10]=100000.f/(100000.f-nearZ);pr[14]=-nearZ*100000.f/(100000.f-nearZ);
 C3Vector right{v[0],v[4],v[8]},up{v[1],v[5],v[9]};if(right.SquaredMag()<.5f||up.SquaredMag()<.5f)return 0;
 D3DMATRIX identity{};identity._11=identity._22=identity._33=identity._44=1;
 bool ok=SUCCEEDED(device->SetVertexShader(nullptr))&&SUCCEEDED(device->SetPixelShader(nullptr))&&SUCCEEDED(device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1))&&SUCCEEDED(device->SetTransform(D3DTS_WORLD,&identity))&&SUCCEEDED(device->SetTransform(D3DTS_VIEW,reinterpret_cast<const D3DMATRIX*>(&view)))&&SUCCEEDED(device->SetTransform(D3DTS_PROJECTION,reinterpret_cast<const D3DMATRIX*>(&projection)))&&SUCCEEDED(device->SetTexture(0,texture));
 ok=SUCCEEDED(device->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE))&&ok;
 struct Setting {D3DRENDERSTATETYPE key;DWORD value;};const Setting settings[]{{D3DRS_ZENABLE,TRUE},{D3DRS_ZFUNC,D3DCMP_LESSEQUAL},{D3DRS_ZWRITEENABLE,FALSE},{D3DRS_ALPHABLENDENABLE,TRUE},{D3DRS_BLENDOP,D3DBLENDOP_ADD},{D3DRS_SRCBLEND,D3DBLEND_SRCALPHA},{D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA},{D3DRS_SEPARATEALPHABLENDENABLE,FALSE},{D3DRS_CULLMODE,D3DCULL_NONE},{D3DRS_LIGHTING,FALSE},{D3DRS_FOGENABLE,FALSE},{D3DRS_STENCILENABLE,FALSE},{D3DRS_COLORWRITEENABLE,15},{D3DRS_ALPHATESTENABLE,FALSE}};
 for(auto s:settings)ok=SUCCEEDED(device->SetRenderState(s.key,s.value))&&ok;
 ok=SUCCEEDED(device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE))&&SUCCEEDED(device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE))&&ok;
 for(unsigned s=1;s<8;++s)ok=SUCCEEDED(device->SetTextureStageState(s,D3DTSS_COLOROP,D3DTOP_DISABLE))&&SUCCEEDED(device->SetTextureStageState(s,D3DTSS_ALPHAOP,D3DTOP_DISABLE))&&ok;
 ok=SUCCEEDED(device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE))&&SUCCEEDED(device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR))&&SUCCEEDED(device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR))&&SUCCEEDED(device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP))&&SUCCEEDED(device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP))&&ok;if(!ok)return 0;
 unsigned drawn=0;for(const auto& point:points){const auto& p=point.position;const float z=p.x*v[2]+p.y*v[6]+p.z*v[10]+v[14];if(!std::isfinite(z)||z<.1f)continue;const float half=16.f*(point.corpse?1:scale)*2.f*z/(std::abs(pr[5])*viewport.Height);if(!std::isfinite(half)||half<=0||half>1e7f)continue;
  const float aspect=point.corpse?28.f/34:28.f/35;const DWORD color=(point.corpse?255:alpha)<<24|0xffffff;const float u0=point.corpse?.002f:.4395f,u1=point.corpse?.4355f:.8725f,v1=point.corpse?.52825f:.543875f;
  const float xy[6][2]{{-1,1},{-1,-1},{1,1},{1,1},{-1,-1},{1,-1}};Vertex vertices[6];for(unsigned i=0;i<6;++i){auto pos=p+right*(xy[i][0]*half*aspect)+up*(xy[i][1]*half);vertices[i]={pos.x,pos.y,pos.z,color,xy[i][0]<0?u0:u1,xy[i][1]>0?.002f:v1};}
  if(FAILED(device->SetRenderState(D3DRS_ALPHATESTENABLE,point.corpse?TRUE:FALSE)))continue;if(point.corpse&&(FAILED(device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL))||FAILED(device->SetRenderState(D3DRS_ALPHAREF,160))))continue;
  if(SUCCEEDED(device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,vertices,sizeof(Vertex))))drawn|=point.corpse?2u:1u;
 }return drawn;
}
void DestroyWorldNavigation() noexcept {if(atlas)HandleClose(reinterpret_cast<HOBJECT>(atlas));atlas=nullptr;}
}
