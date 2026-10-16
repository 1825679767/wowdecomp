// Modern WMO material draw module; preserves native lighting/fog and owns only material overrides.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/WmoMaterials.hpp"
#include "platform/waystone/WmoMaterialPlan.hpp"
#include "WXL/NativeFeatures.hpp"
#include "WmoShaderText.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "common/Log.hpp"
#include <d3dcompiler.h>
#include <atomic>
#include <array>
#include <cstring>
#include <limits>
#include <cstdlib>
#ifndef WXL_ENABLE_WMO_MATERIALS
#define WXL_ENABLE_WMO_MATERIALS 1
#endif
namespace wxl::native {
namespace wmo_materials {namespace {
template<class T>struct Com {T* p=nullptr;~Com(){if(p)p->Release();}Com()=default;Com(const Com&)=delete;Com& operator=(const Com&)=delete;};
using AssembleFn=HRESULT(WINAPI*)(LPCVOID,SIZE_T,LPCSTR,const D3D_SHADER_MACRO*,ID3DInclude*,UINT,ID3DBlob**,ID3DBlob**);
struct Compiler {HMODULE library=nullptr;decltype(&D3DDisassemble) disassemble=nullptr;AssembleFn assemble=nullptr;
 Compiler(){library=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);if(library){disassemble=reinterpret_cast<decltype(disassemble)>(GetProcAddress(library,"D3DDisassemble"));assemble=reinterpret_cast<AssembleFn>(GetProcAddress(library,"D3DAssemble"));}}~Compiler(){if(library)FreeLibrary(library);}bool Ready() const{return disassemble&&assemble;}};
Compiler& Tools(){static Compiler c;return c;}
std::atomic<bool> configured{WXL_ENABLE_WMO_MATERIALS!=0},capable{false},failed{false};
thread_local std::shared_ptr<const GroupSource> current;thread_local unsigned material=0;
bool Changed(CVar*,const char*,const char* v,void*){configured.store(v&&std::strtol(v,nullptr,10)!=0,std::memory_order_release);return true;}
IDirect3DDevice9* Native(CGxDevice* d){if(!d)return nullptr;if(d->m_api==GxApi_D3d9)return static_cast<CGxDeviceD3d*>(d)->m_d3dDevice;if(d->m_api==GxApi_D3d9Ex)return static_cast<CGxDeviceD3d9Ex*>(d)->m_d3dDevice;return nullptr;}
struct Pair {std::vector<DWORD> vsCode,psCode;bool layered=false;Com<IDirect3DVertexShader9> vs;Com<IDirect3DPixelShader9> ps;int tint=-1;};
struct Textures {unsigned material=0;std::array<CTexture*,8> values{};~Textures(){for(auto* t:values)if(t)HandleClose(reinterpret_cast<HOBJECT>(t));}};
struct GroupGpu {std::weak_ptr<const GroupSource> source;Com<IDirect3DVertexBuffer9> stream;std::vector<std::unique_ptr<Textures>> textures;size_t bytes=0;};
struct HardwareState {Com<IDirect3DStateBlock9> block;bool valid=false;explicit HardwareState(IDirect3DDevice9& d){valid=SUCCEEDED(d.CreateStateBlock(D3DSBT_ALL,&block.p))&&block.p&&SUCCEEDED(block.p->Capture());}~HardwareState(){if(valid&&FAILED(block.p->Apply())){failed=true;WLOG_WARN("native-wmo-materials: hardware restore failed; overrides disabled");}}};
std::string Disassemble(const std::vector<DWORD>& code){Com<ID3DBlob> text;auto& c=Tools();if(!c.Ready()||FAILED(c.disassemble(code.data(),code.size()*4,0,nullptr,&text.p))||!text.p)return {};const auto* begin=static_cast<const char*>(text.p->GetBufferPointer());size_t size=text.p->GetBufferSize();while(size&&!begin[size-1])--size;if(std::memchr(begin,0,size))return {};return {begin,size};}
bool Assemble(const std::string& text,Com<ID3DBlob>& code){Com<ID3DBlob> errors;return !text.empty()&&Tools().Ready()&&SUCCEEDED(Tools().assemble(text.data(),text.size(),"WxlWmo",nullptr,nullptr,0,&code.p,&errors.p))&&code.p;}
template<class T>bool Code(T* shader,std::vector<DWORD>& output){UINT size=0;if(!shader||FAILED(shader->GetFunction(nullptr,&size))||size<8||size>0x20000||size%4)return false;output.resize(size/4);return SUCCEEDED(shader->GetFunction(output.data(),&size))&&size==output.size()*4;}
}}
struct WmoMaterialResources {
 std::vector<std::unique_ptr<wmo_materials::Pair>> pairs;std::vector<std::unique_ptr<wmo_materials::GroupGpu>> groups;
 wmo_materials::Com<IDirect3DVertexDeclaration9> declaration;size_t bytes=0;
};
namespace wmo_materials {namespace {
void Trim(WmoMaterialResources& r){for(auto it=r.groups.begin();it!=r.groups.end();)if((*it)->source.expired()){r.bytes-=(*it)->bytes;it=r.groups.erase(it);}else ++it;}
Pair* Program(WmoMaterialResources& r,IDirect3DDevice9& d,bool layers){Com<IDirect3DVertexShader9> vs;Com<IDirect3DPixelShader9> ps;if(FAILED(d.GetVertexShader(&vs.p))||FAILED(d.GetPixelShader(&ps.p)))return nullptr;
 std::vector<DWORD> v,p;if(!Code(ps.p,p)||(layers&&!Code(vs.p,v)))return nullptr;
 for(auto& pair:r.pairs)if(pair->layered==layers&&pair->vsCode==v&&pair->psCode==p)return pair->ps.p?pair.get():nullptr;
 if(r.pairs.size()>=64)return nullptr;auto pair=std::make_unique<Pair>();pair->layered=layers;pair->vsCode=std::move(v);pair->psCode=std::move(p);
 std::string psText=Disassemble(pair->psCode),vsText;Com<ID3DBlob> vp,pp;bool ok=false;
 if(layers){layered::VsMapping mapping{};vsText=layered::BuildLayeredVsText(Disassemble(pair->vsCode),mapping);if(!vsText.empty())psText=layered::BuildLayeredPsText(psText,mapping,pair->tint);else psText.clear();
  ok=Assemble(vsText,vp)&&Assemble(psText,pp)&&SUCCEEDED(d.CreateVertexShader(static_cast<const DWORD*>(vp.p->GetBufferPointer()),&pair->vs.p))&&SUCCEEDED(d.CreatePixelShader(static_cast<const DWORD*>(pp.p->GetBufferPointer()),&pair->ps.p));
 }else {psText=composite::InjectComposite(psText);ok=Assemble(psText,pp)&&SUCCEEDED(d.CreatePixelShader(static_cast<const DWORD*>(pp.p->GetBufferPointer()),&pair->ps.p));}
 auto* result=ok?pair.get():nullptr;r.pairs.push_back(std::move(pair));if(!ok)WLOG_WARN("native-wmo-materials: unsupported native shader pair layers=%u",unsigned(layers));return result;
}
GroupGpu* Group(WmoMaterialResources& r,IDirect3DDevice9& d,const std::shared_ptr<const GroupSource>& source){for(auto& g:r.groups)if(g->source.lock()==source)return g.get();Trim(r);
 if(r.groups.size()>=256||source->stream.empty()||source->stream.size()>32u*1024u*1024u-r.bytes)return nullptr;
 auto g=std::make_unique<GroupGpu>();g->source=source;g->bytes=source->stream.size();if(FAILED(d.CreateVertexBuffer(UINT(g->bytes),D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&g->stream.p,nullptr))||!g->stream.p)return nullptr;
 void* memory=nullptr;if(FAILED(g->stream.p->Lock(0,UINT(g->bytes),&memory,0))||!memory)return nullptr;std::memcpy(memory,source->stream.data(),g->bytes);if(FAILED(g->stream.p->Unlock()))return nullptr;
 if(!r.declaration.p){const D3DVERTEXELEMENT9 decl[]{
 {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},{0,24,D3DDECLTYPE_D3DCOLOR,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR,0},
 {1,0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},{1,8,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1},{1,16,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,2},{1,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,3},{1,32,D3DDECLTYPE_D3DCOLOR,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR,1},D3DDECL_END()};if(FAILED(d.CreateVertexDeclaration(decl,&r.declaration.p)))return nullptr;}
 r.bytes+=g->bytes;auto* result=g.get();r.groups.push_back(std::move(g));return result;
}
Textures* Material(GroupGpu& group,const wxl::waystone::wmo::MaterialRecord& record,unsigned id){for(auto& m:group.textures)if(m->material==id)return m.get();if(group.textures.size()>=256)return nullptr;
 auto next=std::make_unique<Textures>();next->material=id;constexpr unsigned paths[]{0,1,2,3,5,6,7,8};for(unsigned i=0;i<8;++i){const auto& name=record.paths[paths[i]];if(name.empty())return nullptr;next->values[i]=TextureCreate(name.c_str(),CGxTexFlags(GxTex_Linear,1,1),nullptr,0);if(!next->values[i])return nullptr;}auto* result=next.get();group.textures.push_back(std::move(next));return result;
}
IDirect3DBaseTexture9* Texture(CGxDevice& owner,CTexture* t){auto* gx=TextureGetGxTex(t,0,nullptr);if(!gx||gx->m_target!=GxTex_2d)return nullptr;owner.ITexMarkAsUpdated(gx);
 if(owner.m_api==GxApi_D3d9Ex){auto& ex=static_cast<CGxDeviceD3d9Ex&>(owner);if(!gx->m_apiSpecificData){if(!gx->m_apiSpecificData2)return nullptr;ex.ITexAcquire(gx);if(gx->m_apiSpecificData)ex.ITexUploadAll(gx);}ex.ITexTouch(gx);}
 return static_cast<IDirect3DBaseTexture9*>(gx->m_apiSpecificData);
}
}
void Register(){auto* cv=CVar::Register("wxlWmoMaterials","WXL modern WMO overlay alpha and four-layer height/tint materials; reload world after changing",0,WXL_ENABLE_WMO_MATERIALS?"1":"0",&Changed,0,false,nullptr,false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,nullptr);}
bool Enabled() noexcept{return WXL_ENABLE_WMO_MATERIALS&&NativeEnabled()&&WmoEnabled()&&configured.load(std::memory_order_acquire)&&!failed.load();}
bool LayeredReady() noexcept{return Enabled()&&capable.load(std::memory_order_acquire);}
void DeviceReady(CGxDevice& owner) noexcept {try{auto* d=Native(&owner);D3DCAPS9 caps{};capable=d&&SUCCEEDED(d->GetDeviceCaps(&caps))&&caps.VertexShaderVersion>=D3DVS_VERSION(3,0)&&caps.PixelShaderVersion>=D3DPS_VERSION(3,0)&&caps.MaxStreams>=2&&caps.MaxSimultaneousTextures>=8&&Tools().Ready();}catch(...){capable=false;}}
void Release(CGxDevice& d) noexcept {d.m_wxlWmoMaterials.reset();}
void EndWorld() noexcept {try{if(g_theGxDevicePtr&&g_theGxDevicePtr->m_wxlWmoMaterials)Trim(*g_theGxDevicePtr->m_wxlWmoMaterials);}catch(...){}}
BatchScope::BatchScope(CMapObj& root,CMapObjGroup& group,unsigned index) noexcept:previous(std::move(current)),previousMaterial(material){current=FindSource(root,group);material=index;}
BatchScope::~BatchScope(){current=std::move(previous);material=previousMaterial;}
bool BatchScope::Modern() const noexcept{return current!=nullptr;}
bool Draw(IDirect3DDevice9* d,uint32_t primitive,int32_t base,uint32_t min,uint32_t vertices,uint32_t start,uint32_t count) noexcept {bool required=false;try{
 auto source=current;if(!source||!source->materials||material>=source->materials->records.size())return false;const auto& record=source->materials->records[material];required=record.layered;
 if(!Enabled()||!d||Native(g_theGxDevicePtr)!=d)return required;if(!required&&record.nativeShader!=6)return false;
 if(!count||!vertices||primitive!=D3DPT_TRIANGLELIST)return required;
 auto& owner=*g_theGxDevicePtr;if(!owner.m_wxlWmoMaterials)owner.m_wxlWmoMaterials=std::make_shared<WmoMaterialResources>();auto& r=*owner.m_wxlWmoMaterials;
 auto* pair=Program(r,*d,required);if(!pair)return required;HardwareState state(*d);if(!state.valid)return required;
 if(required){if(!LayeredReady()||base<0||min>source->vertices||vertices>source->vertices-min)return true;auto* group=Group(r,*d,source);if(!group)return true;auto* textures=Material(*group,record,material);if(!textures)return true;
  // Pin every resolved COM texture until the draw: Ex cache moves may otherwise recycle earlier slots.
  std::array<Com<IDirect3DBaseTexture9>,8> held;for(unsigned i=0;i<8;++i){held[i].p=Texture(owner,textures->values[i]);if(!held[i].p)return true;held[i].p->AddRef();}
  Com<IDirect3DVertexBuffer9> stream0;UINT offset=0,stride=0;D3DVERTEXBUFFER_DESC desc{};if(FAILED(d->GetStreamSource(0,&stream0.p,&offset,&stride))||!stream0.p||stride<28||stride>256||FAILED(stream0.p->GetDesc(&desc)))return true;
  uint64_t begin=uint64_t(offset)+uint64_t(base)*stride,end=begin+uint64_t(min+vertices)*stride;if(end>desc.Size||begin>std::numeric_limits<UINT>::max())return true;
  if(FAILED(d->SetStreamSource(0,stream0.p,UINT(begin),stride))||FAILED(d->SetStreamSource(1,group->stream.p,0,36))||FAILED(d->SetStreamSourceFreq(0,1))||FAILED(d->SetStreamSourceFreq(1,1))||FAILED(d->SetVertexDeclaration(r.declaration.p))||FAILED(d->SetVertexShader(pair->vs.p))||FAILED(d->SetPixelShader(pair->ps.p)))return true;
  uint32_t tint=0,flags=0;std::memcpy(&tint,record.native.data()+28,4);std::memcpy(&flags,record.native.data(),4);float rgba[]{float((tint>>16)&255)/255.f,float((tint>>8)&255)/255.f,float(tint&255)/255.f,(tint&0xffffff)?1.f:0.f};
  if(pair->tint<0||FAILED(d->SetPixelShaderConstantF(UINT(pair->tint),rgba,1)))return true;
  for(unsigned i=0;i<8;++i)if(FAILED(d->SetTexture(i,held[i].p))||FAILED(d->SetSamplerState(i,D3DSAMP_ADDRESSU,flags&0x40?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP))||FAILED(d->SetSamplerState(i,D3DSAMP_ADDRESSV,flags&0x80?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP))||FAILED(d->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR))||FAILED(d->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR))||FAILED(d->SetSamplerState(i,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR))||FAILED(d->SetSamplerState(i,D3DSAMP_SRGBTEXTURE,FALSE)))return true;
  d->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(primitive),0,min,vertices,start,count);return true;
 }
 if(FAILED(d->SetPixelShader(pair->ps.p)))return false;d->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(primitive),base,min,vertices,start,count);return true;
 }catch(...){return required;}}
}}
