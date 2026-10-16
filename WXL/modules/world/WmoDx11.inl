// Included only by the material owner; shares the exact donor shader builders.
#include "Engine/Source/Gx/CGxBuf.h"
#include <string_view>
namespace wxl::native {
struct WmoDx11Resources {
 struct Pair {std::vector<DWORD> vertex,pixel;CGxShader* vs=nullptr,*ps=nullptr;bool layered=false;int tint=-1;};
 struct Group {
  CGxDeviceD3d11& owner;std::weak_ptr<const wmo_materials::GroupSource> source;
  CGxPool* pool=nullptr;CGxBuf* stream=nullptr;size_t bytes=0;
  std::vector<std::unique_ptr<wmo_materials::Textures>> textures;
  explicit Group(CGxDeviceD3d11& d):owner(d){}
  ~Group(){if(stream)owner.BufDestroy(stream);if(pool)owner.PoolDestroy(pool);}
 };
 std::vector<std::unique_ptr<Pair>> pairs;std::vector<std::unique_ptr<Group>> groups;size_t bytes=0;
};
namespace wmo_materials {namespace {
CGxShader* DxProgram(CGxDeviceD3d11& d,EGxShTarget stage,ID3DBlob* code){
 if(!code||code->GetBufferSize()<8)return nullptr;
 const auto hash=std::hash<std::string_view>{}({static_cast<const char*>(code->GetBufferPointer()),code->GetBufferSize()});
 char name[96];std::snprintf(name,sizeof(name),"WXL.WMO.DX11.%u.%llx",unsigned(stage),static_cast<unsigned long long>(hash));
 auto* shader=d.m_shaderList[stage].Ptr(name);
 if(shader)return shader->m_code.Count()==code->GetBufferSize()&&!std::memcmp(shader->m_code.Ptr(),code->GetBufferPointer(),code->GetBufferSize())?shader:nullptr;
 shader=d.m_shaderList[stage].New(name,0,0);if(!shader)return nullptr;shader->m_refCount=1;shader->m_target=stage;shader->m_code.SetCount(uint32_t(code->GetBufferSize()));std::memcpy(shader->m_code.Ptr(),code->GetBufferPointer(),code->GetBufferSize());return shader;
}
WmoDx11Resources::Pair* DxPair(WmoDx11Resources& r,CGxDeviceD3d11& d,bool layeredMaterial){
 auto* vs=static_cast<CGxShader*>(d.m_appRenderStates[GxRs_VertexShader].m_value.m_data.p);
 auto* ps=static_cast<CGxShader*>(d.m_appRenderStates[GxRs_PixelShader].m_value.m_data.p);
 if(!ps||!ps->m_code.Count()||ps->m_code.Count()%4||(layeredMaterial&&(!vs||!vs->m_code.Count()||vs->m_code.Count()%4)))return nullptr;
 std::vector<DWORD> v(layeredMaterial?vs->m_code.Count()/4:0),p(ps->m_code.Count()/4);
 if(!v.empty())std::memcpy(v.data(),vs->m_code.Ptr(),vs->m_code.Count());std::memcpy(p.data(),ps->m_code.Ptr(),ps->m_code.Count());
 for(auto& pair:r.pairs)if(pair->layered==layeredMaterial&&pair->vertex==v&&pair->pixel==p)return pair->ps?pair.get():nullptr;
 if(r.pairs.size()>=64)return nullptr;auto pair=std::make_unique<WmoDx11Resources::Pair>();pair->layered=layeredMaterial;pair->vertex=std::move(v);pair->pixel=std::move(p);
 std::string pixel=Disassemble(pair->pixel);Com<ID3DBlob> vp,pp;
 if(layeredMaterial){layered::VsMapping mapping{};auto vertex=layered::BuildLayeredVsText(Disassemble(pair->vertex),mapping);pixel=vertex.empty()?std::string{}:layered::BuildLayeredPsText(pixel,mapping,pair->tint);
  if(Assemble(vertex,vp)&&Assemble(pixel,pp)){pair->vs=DxProgram(d,GxSh_Vertex,vp.p);pair->ps=DxProgram(d,GxSh_Pixel,pp.p);if(!pair->vs)pair->ps=nullptr;}
 }else if(Assemble(composite::InjectComposite(pixel),pp))pair->ps=DxProgram(d,GxSh_Pixel,pp.p);
 auto* result=pair->ps?pair.get():nullptr;r.pairs.push_back(std::move(pair));return result;
}
WmoDx11Resources::Group* DxGroup(WmoDx11Resources& r,CGxDeviceD3d11& d,const std::shared_ptr<const GroupSource>& source){
 for(auto i=r.groups.begin();i!=r.groups.end();)if((*i)->source.expired()){r.bytes-=(*i)->bytes;i=r.groups.erase(i);}else{if((*i)->source.lock()==source)return i->get();++i;}
 if(r.groups.size()>=256||source->stream.empty()||source->stream.size()!=uint64_t(source->vertices)*36||source->stream.size()>32u*1024u*1024u-r.bytes)return nullptr;
 auto group=std::make_unique<WmoDx11Resources::Group>(d);group->source=source;group->bytes=source->stream.size();
 group->pool=d.PoolCreate(GxPoolTarget_Vertex,GxPoolUsage_Static,int32_t(group->bytes),GxPoolHintBit_Unk0,"WXL DX11 WMO UV/tint");if(!group->pool)return nullptr;
 group->stream=d.BufCreate(group->pool,36,source->vertices,0);if(!group->stream)return nullptr;auto* memory=d.BufLock(group->stream);if(!memory)return nullptr;
 std::memcpy(memory,source->stream.data(),group->bytes);if(!d.BufUnlock(group->stream,0))return nullptr;
 r.bytes+=group->bytes;auto* result=group.get();r.groups.push_back(std::move(group));return result;
}
Textures* DxTextures(WmoDx11Resources::Group& group,const wxl::waystone::wmo::MaterialRecord& record,unsigned id){
 for(auto& t:group.textures)if(t->material==id)return t.get();if(group.textures.size()>=256)return nullptr;
 auto next=std::make_unique<Textures>();next->material=id;constexpr unsigned paths[]{0,1,2,3,5,6,7,8};
 for(unsigned i=0;i<8;++i){const auto& path=record.paths[paths[i]];if(path.empty())return nullptr;next->values[i]=TextureCreate(path.c_str(),CGxTexFlags(GxTex_Linear,1,1),&GetGlobalStatusObj(),0);if(!next->values[i])return nullptr;}
 auto* result=next.get();group.textures.push_back(std::move(next));return result;
}
struct DxScope {
 CGxDeviceD3d11& d;CGxVertexAttrib attributes[14];CGxBuf* buffers[14];CGxBuf* indices;uint32_t mask;int tint;float rgba[4]{};
 std::array<CGxDeviceD3d11::SamplerOverride,8> sampler{};bool samplersChanged=false;
 DxScope(CGxDeviceD3d11& device,int slot):d(device),indices(d.m_primIndexBuf),mask(d.m_primVertexMask),tint(slot){std::memcpy(attributes,d.m_primVertexFormatAttrib,sizeof(attributes));std::memcpy(buffers,d.m_primVertexFormatBuf,sizeof(buffers));if(tint>=0)ReadMaterialConstants(d,GxSh_Pixel,uint32_t(tint),1,rgba);d.RsPush();}
 ~DxScope(){if(samplersChanged)for(unsigned i=0;i<8;++i)d.ExchangeSamplerOverride(i,sampler[i]);if(tint>=0)d.ShaderConstantsSet(GxSh_Pixel,uint32_t(tint),rgba,1);std::memcpy(d.m_primVertexFormatAttrib,attributes,sizeof(attributes));std::memcpy(d.m_primVertexFormatBuf,buffers,sizeof(buffers));d.PrimVertexMask(mask);d.PrimIndexPtr(indices);d.RsPop();}
};
}
void TrimDx11(WmoMaterialResources& resources){if(!resources.dx11)return;auto& r=*resources.dx11;for(auto i=r.groups.begin();i!=r.groups.end();)if((*i)->source.expired()){r.bytes-=(*i)->bytes;i=r.groups.erase(i);}else ++i;}
bool Draw(CGxDeviceD3d11& owner,const CGxBatch& batch) noexcept {static thread_local bool replay=false;if(replay)return false;bool required=false;try{
 auto source=current;if(!source||!source->materials||material>=source->materials->records.size())return false;const auto& record=source->materials->records[material];required=record.layered;
 if(!Enabled()||!LegacyMaterialBackend(&owner,8))return required;if(!required&&record.nativeShader!=6)return false;
 if(batch.m_primType!=GxPrim_Triangles||!batch.m_count)return required;
 if(!owner.m_wxlWmoMaterials)owner.m_wxlWmoMaterials=std::make_shared<WmoMaterialResources>();auto& resources=*owner.m_wxlWmoMaterials;
 if(!resources.dx11)resources.dx11=std::make_shared<WmoDx11Resources>();auto& r=*resources.dx11;auto* pair=DxPair(r,owner,required);if(!pair)return required;
 DxScope state(owner,pair->tint);
 if(required){
  if(!LayeredReady()||batch.m_maxIndex>=source->vertices)return true;auto* group=DxGroup(r,owner,source);if(!group)return true;auto* textures=DxTextures(*group,record,material);if(!textures)return true;
  uint32_t flags=0,tint=0;std::memcpy(&flags,record.native.data(),4);std::memcpy(&tint,record.native.data()+28,4);
  float rgba[]={float((tint>>16)&255)/255.f,float((tint>>8)&255)/255.f,float(tint&255)/255.f,(tint&0xffffff)?1.f:0.f};if(pair->tint<0)return true;owner.ShaderConstantsSet(GxSh_Pixel,uint32_t(pair->tint),rgba,1);
  for(unsigned i=0;i<8;++i){auto* texture=TextureGetGxTex(textures->values[i],0,nullptr);if(!texture||texture->m_target!=GxTex_2d)return true;owner.RsSet(static_cast<EGxRenderState>(GxRs_Texture0+i),texture);}
  for(unsigned i=0;i<8;++i)state.sampler[i]=owner.ExchangeSamplerOverride(i,{true,true,!(flags&0x40),!(flags&0x80)});state.samplersChanged=true;
  const CGxVertexAttrib attributes[]={{GxVA_TexCoord0,3,0,36},{GxVA_TexCoord1,3,8,36},{GxVA_TexCoord2,3,16,36},{GxVA_TexCoord3,3,24,36},{GxVA_Color1,0,32,36}};
  owner.PrimVertexFormat(group->stream,attributes,5);owner.PrimVertexMask((1<<GxVA_Position)|(1<<GxVA_Normal)|(1<<GxVA_Color0)|(1<<GxVA_Color1)|(1<<GxVA_TexCoord0)|(1<<GxVA_TexCoord1)|(1<<GxVA_TexCoord2)|(1<<GxVA_TexCoord3));owner.RsSet(GxRs_VertexShader,pair->vs);
 }
 owner.RsSet(GxRs_PixelShader,pair->ps);struct Replay{bool& flag;Replay(bool& f):flag(f){flag=true;}~Replay(){flag=false;}} guard(replay);
 CGxBatch copy=batch;owner.Draw(&copy,1);return true;
 }catch(...){return required;}}
}}
