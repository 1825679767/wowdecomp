// Source-native donor silhouette policy; no addresses or event-hook ABI.
#include "WXL/UnitOutline.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Hlsl.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#if WXL_ENABLE_DX11
#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#endif
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "common/Log.hpp"
#include <d3dcompiler.h>
#include <array>
#include <cstring>
#include <memory>
#ifndef WXL_ENABLE_UNIT_OUTLINE
#define WXL_ENABLE_UNIT_OUTLINE 1
#endif
namespace wxl::native {
struct UnitOutlineResources {
 IDirect3DTexture9* texture=nullptr;IDirect3DSurface9* surface=nullptr,*multisample=nullptr;
 IDirect3DPixelShader9* color=nullptr,*cutout=nullptr,*edge=nullptr;
 UINT width=0,height=0,quality=0;D3DMULTISAMPLE_TYPE samples=D3DMULTISAMPLE_NONE;
 ~UnitOutlineResources(){if(multisample)multisample->Release();if(surface)surface->Release();if(texture)texture->Release();if(color)color->Release();if(cutout)cutout->Release();if(edge)edge->Release();}
};
namespace unit_outline {namespace {
CVar* enabled=nullptr;thread_local CM2Model* current=nullptr;bool active=false,stamped=false,failed=false;
struct Target {CM2Model* model=nullptr;uint32_t handle=0;bool player=false;float rgba[4]{};};
std::array<Target,2> targets;uint32_t targetCount=0;CGxDevice* frameDevice=nullptr;
template<class T>struct Com{T* p=nullptr;~Com(){if(p)p->Release();}};
IDirect3DDevice9* Native(CGxDevice* d){if(!d)return nullptr;if(d->m_api==GxApi_D3d9)return static_cast<CGxDeviceD3d*>(d)->m_d3dDevice;if(d->m_api==GxApi_D3d9Ex)return static_cast<CGxDeviceD3d9Ex*>(d)->m_d3dDevice;return nullptr;}
void Sync(CGxDevice& d){if(d.m_api==GxApi_D3d9)static_cast<CGxDeviceD3d&>(d).IStateSync();else if(d.m_api==GxApi_D3d9Ex)static_cast<CGxDeviceD3d9Ex&>(d).IStateSync();}
struct HardwareState {
 IDirect3DDevice9& d;Com<IDirect3DStateBlock9> block;std::array<Com<IDirect3DSurface9>,4> rt;Com<IDirect3DSurface9> depth;D3DVIEWPORT9 viewport{};UINT count=0;bool valid=false;
 explicit HardwareState(IDirect3DDevice9& device):d(device){D3DCAPS9 caps{};if(FAILED(d.GetDeviceCaps(&caps))||!caps.NumSimultaneousRTs||caps.NumSimultaneousRTs>4)return;count=caps.NumSimultaneousRTs;
  if(FAILED(d.CreateStateBlock(D3DSBT_ALL,&block.p))||!block.p||FAILED(block.p->Capture())||FAILED(d.GetViewport(&viewport)))return;
  for(UINT i=0;i<count;++i){auto hr=d.GetRenderTarget(i,&rt[i].p);if(FAILED(hr)&&hr!=D3DERR_NOTFOUND)return;}if(!rt[0].p)return;
  auto hr=d.GetDepthStencilSurface(&depth.p);if(FAILED(hr)&&hr!=D3DERR_NOTFOUND)return;valid=true;}
 ~HardwareState(){if(!valid)return;bool ok=SUCCEEDED(d.SetDepthStencilSurface(nullptr));for(UINT i=0;i<count;++i)ok=SUCCEEDED(d.SetRenderTarget(i,rt[i].p))&&ok;ok=SUCCEEDED(d.SetDepthStencilSurface(depth.p))&&ok;ok=SUCCEEDED(block.p->Apply())&&ok;ok=SUCCEEDED(d.SetViewport(&viewport))&&ok;if(!ok){failed=true;WLOG_WARN("native-unit-outline: hardware restore failed; effect disabled");}}
};
bool Shader(IDirect3DDevice9& d,decltype(&D3DCompile) compile,const char* text,IDirect3DPixelShader9** output){Com<ID3DBlob> code,error;return SUCCEEDED(compile(text,std::strlen(text),"WXLUnitOutline",nullptr,nullptr,"main","ps_2_0",D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code.p,&error.p))&&code.p&&SUCCEEDED(d.CreatePixelShader(static_cast<const DWORD*>(code.p->GetBufferPointer()),output));}
bool Ensure(IDirect3DDevice9& d){auto* owner=g_theGxDevicePtr;if(owner!=frameDevice||Native(owner)!=&d)return false;
 Com<IDirect3DSurface9> rt;D3DSURFACE_DESC desc{};if(FAILED(d.GetRenderTarget(0,&rt.p))||!rt.p||FAILED(rt.p->GetDesc(&desc))||!desc.Width||!desc.Height||desc.Width>16384||desc.Height>16384)return false;
 auto& existing=owner->m_wxlUnitOutline;if(existing&&existing->width==desc.Width&&existing->height==desc.Height&&existing->samples==desc.MultiSampleType&&existing->quality==desc.MultiSampleQuality)return true;
 // Resizing halfway through accumulated silhouettes cannot mix two mask sizes.
 if(stamped){stamped=false;return false;}auto next=std::make_shared<UnitOutlineResources>();next->width=desc.Width;next->height=desc.Height;next->samples=desc.MultiSampleType;next->quality=desc.MultiSampleQuality;
 if(FAILED(d.CreateTexture(desc.Width,desc.Height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&next->texture,nullptr))||!next->texture||FAILED(next->texture->GetSurfaceLevel(0,&next->surface)))return false;
 if(desc.MultiSampleType!=D3DMULTISAMPLE_NONE&&FAILED(d.CreateRenderTarget(desc.Width,desc.Height,D3DFMT_A8R8G8B8,desc.MultiSampleType,desc.MultiSampleQuality,FALSE,&next->multisample,nullptr)))return false;
 auto library=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);if(!library)return false;auto compile=reinterpret_cast<decltype(&D3DCompile)>(GetProcAddress(library,"D3DCompile"));
 bool ok=compile&&Shader(d,compile,wxl_unit_outline::hlsl::kColor,&next->color)&&Shader(d,compile,wxl_unit_outline::hlsl::kCutoutColor,&next->cutout)&&Shader(d,compile,wxl_unit_outline::hlsl::kEdge,&next->edge);FreeLibrary(library);if(!ok)return false;existing=std::move(next);return true;
}
int FindBounded(){auto* model=current;for(int hop=0;model&&hop<8;++hop,model=model->m_attachParent)for(uint32_t i=0;i<targetCount;++i)if(model==targets[i].model&&model->m_handle==targets[i].handle)return int(i);return -1;}
void Add(const char* token,CGUnit_C* player){CGUnit_C* unit=nullptr;if(targetCount==targets.size()||!GetUnitByKeyword(token,&unit,false)||!unit||!unit->m_worldModel)return;auto* model=unit->m_worldModel;for(uint32_t i=0;i<targetCount;++i)if(targets[i].model==model&&targets[i].handle==model->m_handle)return;
 auto& t=targets[targetCount++];t.model=model;t.handle=model->m_handle;t.player=unit->m_typeId==ID_PLAYER;auto reaction=player?unit->UnitReaction(player):5;t.rgba[0]=reaction<4?1.f:0.f;t.rgba[1]=reaction>=2?1.f:0.f;t.rgba[2]=0;t.rgba[3]=1;}
}
void Register(){enabled=CVar::Register("wxlUnitOutline","WXL reaction-colored target and mouseover silhouettes",0,WXL_ENABLE_UNIT_OUTLINE?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept{return WXL_ENABLE_UNIT_OUTLINE&&NativeEnabled()&&enabled&&enabled->m_intValue&&!failed;}
ModelScope::ModelScope(CM2Model* model) noexcept:previous(current){current=model;}ModelScope::~ModelScope(){current=previous;}
void BeginWorld() noexcept {active=false;stamped=false;targetCount=0;frameDevice=nullptr;try{if(!Enabled()||!CGGameUI::m_visible||!ClntObjMgrGetCurrent()||!g_theGxDevicePtr)return;
 bool supported=Native(g_theGxDevicePtr)!=nullptr;
#if WXL_ENABLE_DX11
 supported=supported||g_theGxDevicePtr->m_api==GxApi_D3d11;
#endif
 if(!supported)return;auto* player=ClntObjMgrGetActivePlayerObj();Add("mouseover",player);Add("target",player);frameDevice=g_theGxDevicePtr;active=targetCount!=0;
#if WXL_ENABLE_DX11
 if(frameDevice->m_api==GxApi_D3d11)static_cast<CGxDeviceD3d11*>(frameDevice)->BeginUnitOutlineFrame();
#endif
 }catch(...){active=false;}}
bool CurrentTarget(CGxDevice& device,float* rgba,bool& depthTest) noexcept {if(!active||!Enabled()||frameDevice!=&device||frameDevice!=g_theGxDevicePtr||!rgba)return false;const int index=FindBounded();if(index<0)return false;std::memcpy(rgba,targets[index].rgba,sizeof(targets[index].rgba));depthTest=targets[index].player;return true;}
void Stamp(IDirect3DDevice9* d,uint32_t primitive,int32_t base,uint32_t min,uint32_t vertices,uint32_t start,uint32_t count) noexcept {try{
 if(!active||!Enabled()||!d||!count||!vertices||primitive!=D3DPT_TRIANGLELIST||frameDevice!=g_theGxDevicePtr)return;int index=FindBounded();if(index<0)return;DWORD blended=1;if(FAILED(d->GetRenderState(D3DRS_ALPHABLENDENABLE,&blended))||blended||!Ensure(*d))return;HardwareState state(*d);if(!state.valid)return;
 auto& r=*frameDevice->m_wxlUnitOutline;auto* surface=r.multisample?r.multisample:r.surface;DWORD alpha=0,ref=0;d->GetRenderState(D3DRS_ALPHATESTENABLE,&alpha);d->GetRenderState(D3DRS_ALPHAREF,&ref);
 if(FAILED(d->SetDepthStencilSurface(nullptr)))return;for(UINT i=1;i<state.count;++i)if(FAILED(d->SetRenderTarget(i,nullptr)))return;if(FAILED(d->SetRenderTarget(0,surface))||FAILED(d->SetViewport(&state.viewport)))return;
 if(targets[index].player){if(!state.depth.p||FAILED(d->SetDepthStencilSurface(state.depth.p))||FAILED(d->SetRenderState(D3DRS_ZENABLE,TRUE))||FAILED(d->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL)))return;}else if(FAILED(d->SetRenderState(D3DRS_ZENABLE,FALSE)))return;
 if(FAILED(d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE))||FAILED(d->SetRenderState(D3DRS_STENCILENABLE,FALSE))||FAILED(d->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE))||FAILED(d->SetRenderState(D3DRS_COLORWRITEENABLE,15))||FAILED(d->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE)))return;
 if(!stamped&&FAILED(d->Clear(0,nullptr,D3DCLEAR_TARGET,0,1,0)))return;
 if(FAILED(d->SetPixelShader(alpha&&ref>=8?r.cutout:r.color))||FAILED(d->SetPixelShaderConstantF(0,targets[index].rgba,1)))return;
 if(SUCCEEDED(d->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(primitive),base,min,vertices,start,count)))stamped=true;
 }catch(...){active=false;}}
void EndWorld() noexcept {try{
#if WXL_ENABLE_DX11
 if(frameDevice&&frameDevice==g_theGxDevicePtr&&frameDevice->m_api==GxApi_D3d11){if(Enabled())static_cast<CGxDeviceD3d11*>(frameDevice)->EndUnitOutlineFrame();else static_cast<CGxDeviceD3d11*>(frameDevice)->BeginUnitOutlineFrame();active=false;stamped=false;targetCount=0;frameDevice=nullptr;return;}
#endif
 if(active&&stamped&&Enabled()&&frameDevice==g_theGxDevicePtr&&frameDevice->m_wxlUnitOutline){Sync(*frameDevice);if(!active||!frameDevice||frameDevice!=g_theGxDevicePtr||!frameDevice->m_wxlUnitOutline){active=false;stamped=false;targetCount=0;frameDevice=nullptr;return;}auto* d=Native(frameDevice);if(!d){active=false;stamped=false;targetCount=0;frameDevice=nullptr;return;}auto& r=*frameDevice->m_wxlUnitOutline;HardwareState state(*d);if(state.valid&&(!r.multisample||SUCCEEDED(d->StretchRect(r.multisample,nullptr,r.surface,nullptr,D3DTEXF_NONE)))){
  struct Vertex {float x,y,z,rhw,u,v;};float w=float(r.width),h=float(r.height);Vertex quad[]={{-.5f,-.5f,0,1,0,0},{w-.5f,-.5f,0,1,1,0},{-.5f,h-.5f,0,1,0,1},{w-.5f,h-.5f,0,1,1,1}};float px[]={1.f/w,1.f/h,1.5f,0};
  bool ok=SUCCEEDED(d->SetRenderState(D3DRS_STENCILENABLE,FALSE))&&SUCCEEDED(d->SetRenderState(D3DRS_ZENABLE,FALSE))&&SUCCEEDED(d->SetRenderState(D3DRS_ZWRITEENABLE,FALSE))&&SUCCEEDED(d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE))&&SUCCEEDED(d->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE))&&SUCCEEDED(d->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD))&&SUCCEEDED(d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA))&&SUCCEEDED(d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA))&&SUCCEEDED(d->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE))&&SUCCEEDED(d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE))&&SUCCEEDED(d->SetRenderState(D3DRS_COLORWRITEENABLE,15))&&SUCCEEDED(d->SetVertexShader(nullptr))&&SUCCEEDED(d->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1))&&SUCCEEDED(d->SetTexture(0,r.texture))&&SUCCEEDED(d->SetPixelShader(r.edge))&&SUCCEEDED(d->SetPixelShaderConstantF(0,px,1));
  ok=SUCCEEDED(d->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP))&&SUCCEEDED(d->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP))&&SUCCEEDED(d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR))&&SUCCEEDED(d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR))&&SUCCEEDED(d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE))&&SUCCEEDED(d->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,FALSE))&&ok;for(UINT i=1;i<state.count;++i)ok=SUCCEEDED(d->SetRenderTarget(i,nullptr))&&ok;if(ok)d->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,quad,sizeof(Vertex));
 }}}catch(...){failed=true;}active=false;stamped=false;targetCount=0;frameDevice=nullptr;}
void Release(CGxDevice& d) noexcept {if(frameDevice==&d){active=false;stamped=false;targetCount=0;frameDevice=nullptr;}d.m_wxlUnitOutline.reset();}
}
}
