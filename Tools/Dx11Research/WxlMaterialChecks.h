// Integration checks use the exact current WXL programs and native Gx layouts.
#include "WXL/MaterialBackend.hpp"
#include "WXL/modules/materials/shaders/CombinersModAddAlphaVs.h"
#include "WXL/modules/materials/shaders/CombinersModAddAlphaPs.h"
#include "WXL/modules/materials/shaders/CombinersModAddAlphaSkinnedSource.hpp"
#include "WXL/modules/materials/shaders/DeathDecayNative.hpp"
#include "WXL/modules/materials/shaders/DeathDecayMaskVs.hpp"
#include "WXL/modules/materials/shaders/DeathDecayFadePs.h"
#include "WXL/modules/materials/shaders/DeathDecayColourPs.h"
#include "WXL/modules/materials/shaders/DeathDecayMaskPs.h"
#include "WXL/modules/materials/shaders/DeathDecayDetailPs.h"
#include "WXL/modules/particles/shaders/ParticlePrograms.hpp"
#include "WXL/modules/particles/shaders/BloodRefractionPs.h"
#include <d3dcompiler.h>

static CGxShader* WxlProgram(CGxDeviceD3d11& d,EGxShTarget target,const char* name,const void* code,size_t bytes){
 auto* shader=d.m_shaderList[target].New(name,0,0);Check(shader!=nullptr,"WXL program owner");
 shader->m_refCount=1;shader->m_target=target;shader->m_code.SetCount(uint32_t(bytes));std::memcpy(shader->m_code.Ptr(),code,bytes);
 Check(shader->Valid()!=0,"current WXL legacy program conversion/API creation");return shader;
}
static void WxlMaterialChecks(CGxDeviceD3d11& d,CGxShader* uiVs,CGxShader* uiPs){
 Check(wxl::native::LegacyMaterialBackend(&d,3),"combined WXL backend admission");
 auto format=d.m_format;format.m_size.x=format.m_size.y=16;format.m_multisampleCount=1;
 Check(d.DeviceSetFormat(format)!=0,"WXL integration target format");
 d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_DepthTest,0);d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_AlphaRef,0);d.RsSet(GxRs_Fog,0);
 float constants[256][4]{};
 for(unsigned row=0;row<4;++row)constants[2+row][row]=1;
 for(unsigned base:{31u,34u,37u})for(unsigned row=0;row<3;++row)constants[base+row][row]=1;
 constants[6][0]=constants[7][1]=constants[8][0]=constants[9][1]=1;
 for(unsigned lane=0;lane<4;++lane)constants[11][lane]=constants[28][lane]=1;
 constants[30][1]=constants[30][2]=1;
 d.ShaderConstantsSet(GxSh_Vertex,0,&constants[0][0],256);
 const float white[]={1,1,1,1},parameters[]={1,1,0,0};
 d.ShaderConstantsSet(GxSh_Pixel,0,parameters,1);d.ShaderConstantsSet(GxSh_Pixel,2,white,1);
 Pixels base,overlay;base.data={CImVector(29,71,113,255)};overlay.data={CImVector(0,0,0,0)};
 auto* t0=Texture(d,base);auto* t1=Texture(d,overlay);
 d.RsSet(GxRs_Texture0,t0);d.RsSet(GxRs_Texture1,t1);d.RsSet(GxRs_Texture2,t0);
 std::vector<CGxShader*> owners;
 auto program=[&](EGxShTarget target,const char* name,const auto& code){auto* shader=WxlProgram(d,target,name,code,sizeof(code));owners.push_back(shader);return shader;};
 auto* rigid=program(GxSh_Vertex,"Integration.Mesh.Rigid",kCombinersModAddAlphaVs);
 auto* add=program(GxSh_Pixel,"Integration.Mesh.Add",kCombinersModAddAlphaPs);
 ID3DBlob* compiled=nullptr;ID3DBlob* errors=nullptr;
 const auto hr=D3DCompile(kCombinersModAddAlphaSkinnedSource,sizeof(kCombinersModAddAlphaSkinnedSource)-1,"WXL/current-weighted",nullptr,nullptr,"main","vs_3_0",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&compiled,&errors);
 if(errors){std::cerr.write(static_cast<const char*>(errors->GetBufferPointer()),errors->GetBufferSize());errors->Release();}
 Check(SUCCEEDED(hr)&&compiled,"current weighted WXL VS compile");
 auto* weighted=WxlProgram(d,GxSh_Vertex,"Integration.Mesh.Weighted",compiled->GetBufferPointer(),compiled->GetBufferSize());compiled->Release();owners.push_back(weighted);
 struct MeshVertex {float x,y,z;uint8_t weights[4],indices[4];float nx,ny,nz,u0,v0,u1,v1;};
 static_assert(sizeof(MeshVertex)==48);
 auto mesh=[&](CGxShader* vs,CGxShader* ps,bool weightedInput){
  MeshVertex vertices[3]={{-1,-1,.25f,{128,127,0,0},{1,2,0,0},0,0,-1,.5f,.5f,.5f,.5f},
   {-1,3,.25f,{128,127,0,0},{1,2,0,0},0,0,-1,.5f,.5f,.5f,.5f},
   {3,-1,.25f,{128,127,0,0},{1,2,0,0},0,0,-1,.5f,.5f,.5f,.5f}};
  if(!weightedInput)for(auto& v:vertices){v.weights[0]=255;v.weights[1]=0;v.indices[0]=0;}
  auto* buffer=d.BufStream(GxPoolTarget_Vertex,48,3);auto* bytes=d.BufLock(buffer);Check(bytes!=nullptr,"native WXL mesh stream");
  std::memcpy(bytes,vertices,sizeof(vertices));d.BufUnlock(buffer,sizeof(vertices));d.PrimVertexPtr(buffer,GxVBF_PBNT2);
  d.RsSet(GxRs_VertexShader,vs);d.RsSet(GxRs_PixelShader,ps);d.SceneClear(3,CImVector(0,0,0,0));
  CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_count=3;batch.m_maxIndex=2;
  const auto before=d.SubmittedDraws();d.Draw(&batch,0);Check(d.SubmittedDraws()==before+1,"WXL exact VS/PS paired signature and typed layout submit");
 };
 mesh(rigid,add,false);auto actual=Center(d);actual.a=255;Equal(actual,base.data[0],"current rigid AddAlpha RGB equation");
 d.RsSet(GxRs_ColorWrite,0);d.RsSet(GxRs_DepthWrite,1);mesh(rigid,add,false);
 Equal(Center(d),CImVector(0,0,0,0),"custom material depth pass preserves disabled color writes");
 CiRect depthRegion(0,0,16,16);TSGrowableArray<float> customDepth;d.DeviceReadDepths(depthRegion,customDepth);
 Check(customDepth.Count()==256&&std::abs(customDepth[136]-.25f)<.00001f,"custom material depth-only pass writes native depth");
 d.RsSet(GxRs_ColorWrite,15);d.RsSet(GxRs_DepthWrite,0);
 // Bone0 is outside the viewport. UINT indices1/2 must select the two real
 // identity palettes; normalized indices or unnormalized weights miss pixels.
 constants[31][3]=50;d.ShaderConstantsSet(GxSh_Vertex,31,constants[31],1);
 mesh(weighted,add,true);actual=Center(d);actual.a=255;Equal(actual,base.data[0],"weighted palette indices and normalized byte weights");
 constants[31][3]=0;d.ShaderConstantsSet(GxSh_Vertex,31,constants[31],1);
 using namespace wxl::modern::materialblend;
 auto* nativeDnd=program(GxSh_Vertex,"Integration.Dnd.NativeVS",kDeathDecayNativeVS);
 auto* nativePs=program(GxSh_Pixel,"Integration.Dnd.NativePS",kDeathDecayNativePS);
 auto* maskVs=program(GxSh_Vertex,"Integration.Dnd.MaskVS",kDeathDecayMaskVS);
 auto* fade=program(GxSh_Pixel,"Integration.Dnd.Fade",kDeathDecayFadeShader);
 auto* colour=program(GxSh_Pixel,"Integration.Dnd.Colour",kDeathDecayColourShader);
 auto* mask=program(GxSh_Pixel,"Integration.Dnd.Mask",kDeathDecayMaskPS);
 auto* detail=program(GxSh_Pixel,"Integration.Dnd.Detail",kDeathDecayDetailPS);
 mesh(nativeDnd,nativePs,false);mesh(nativeDnd,fade,false);mesh(nativeDnd,colour,false);
 const auto originalFlags=t0->m_flags;auto previous=d.ExchangeSamplerOverride(2,{true,true});
 mesh(maskVs,mask,false);mesh(maskVs,detail,false);d.ExchangeSamplerOverride(2,previous);
 Check(!(t0->m_flags!=originalFlags),"DnD stage2 override preserves shared stage0 texture flags");
 namespace p=wxl::modern::particlelayers::programs;
 auto* unlit=program(GxSh_Vertex,"Integration.Particle.Unlit",p::Unlit);
 auto* directional=program(GxSh_Vertex,"Integration.Particle.Directional",p::Directional);
 auto* point=program(GxSh_Vertex,"Integration.Particle.Point",p::DirectionalPoint);
 auto* twoPoints=program(GxSh_Vertex,"Integration.Particle.TwoPoints",p::DirectionalTwoPoints);
 auto* twoColor=program(GxSh_Pixel,"Integration.Particle.TwoColor",p::TwoColorThreeAlpha);
 auto* threeColor=program(GxSh_Pixel,"Integration.Particle.ThreeColor",p::ThreeColorThreeAlpha);
 auto* nativeParticleVs=program(GxSh_Vertex,"Integration.Particle.NativeVS",p::NativeUnlit);
 auto* nativeParticlePs=program(GxSh_Pixel,"Integration.Particle.NativePS",p::NativePixel);
 auto* refraction=program(GxSh_Pixel,"Integration.Particle.Refraction",kBloodRefractionShader);
 struct LayerVertex {float x,y,z,nx,ny,nz;CImVector color;float uv[6];};
 static_assert(sizeof(LayerVertex)==52);
 for(auto* vs:{unlit,directional,point,twoPoints})for(auto* ps:{twoColor,threeColor}){
  LayerVertex vertices[3]={{-1,-1,.25f,0,0,-1,CImVector(255,255,255,255),{.5f,.5f,.5f,.5f,.5f,.5f}},
   {-1,3,.25f,0,0,-1,CImVector(255,255,255,255),{.5f,.5f,.5f,.5f,.5f,.5f}},
   {3,-1,.25f,0,0,-1,CImVector(255,255,255,255),{.5f,.5f,.5f,.5f,.5f,.5f}}};
  auto* buffer=d.BufStream(GxPoolTarget_Vertex,52,3);auto* out=d.BufLock(buffer);Check(out!=nullptr,"layered particle stream");std::memcpy(out,vertices,sizeof(vertices));d.BufUnlock(buffer,sizeof(vertices));
  CGxVertexAttrib attributes[]={{GxVA_Position,4,0,52},{GxVA_Normal,4,12,52},{GxVA_Color0,0,24,52},{GxVA_TexCoord0,3,28,52},{GxVA_TexCoord1,3,36,52},{GxVA_TexCoord2,3,44,52}};
  d.PrimVertexFormat(buffer,attributes,6);d.PrimVertexMask((1<<GxVA_Position)|(1<<GxVA_Normal)|(1<<GxVA_Color0)|(1<<GxVA_TexCoord0)|(1<<GxVA_TexCoord1)|(1<<GxVA_TexCoord2));
  d.RsSet(GxRs_VertexShader,vs);d.RsSet(GxRs_PixelShader,ps);CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_count=3;batch.m_maxIndex=2;
  const auto before=d.SubmittedDraws();d.Draw(&batch,0);Check(d.SubmittedDraws()==before+1,"layered particle native custom stride and paired shader submission");
 }
 // Read active RT0 rather than the backbuffer, preserve alpha and replace
 // stale dimensions. A partial viewport must fail before reusing old content.
 Pixels targetPixels;targetPixels.width=targetPixels.height=8;auto* target=Texture(d,targetPixels,true);
 d.DeviceSetRenderTarget(GxBuffers_Color,target,0);const CImVector sceneColor(81,119,157,193);d.SceneClear(3,sceneColor);
 auto* capture=d.CaptureCurrentColor();Check(capture&&capture->m_width==8&&capture->m_height==8,"active target scene capture ownership/dimensions");
 d.DeviceSetRenderTarget(GxBuffers_Color,capture,0);Equal(Center(d,8,8),sceneColor,"active scene capture pixels and alpha");
 d.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);d.SceneClear(3,sceneColor);capture=d.CaptureCurrentColor();Check(capture&&capture->m_width==16,"scene capture dimension replacement");
 d.RsSet(GxRs_VertexShader,nativeParticleVs);d.RsSet(GxRs_PixelShader,nativeParticlePs);d.RsSet(GxRs_Texture0,t0);
 Check(wxl::native::ExactMaterialProgram(d,GxRs_VertexShader,p::NativeUnlit)&&wxl::native::ExactMaterialProgram(d,GxRs_PixelShader,p::NativePixel),"DX11 exact donor native source contract");
 d.RsSet(GxRs_PixelShader,refraction);d.RsSet(GxRs_Texture1,capture);d.RsSet(GxRs_ColorWrite,7);
 const float refractionParameters[]={1.f/16,1.f/16,64,4};d.ShaderConstantsSet(GxSh_Pixel,0,refractionParameters,1);
 auto sampler=d.ExchangeSamplerOverride(1,{true,false});const auto submitted=d.SubmittedDraws();
 Triangle(d,CImVector(255,255,255,255));Check(d.SubmittedDraws()==submitted+1,"donor refraction VPOS and paired native shader submit");
 Equal(Center(d),sceneColor,"donor scene refraction samples active scene and preserves alpha");d.ExchangeSamplerOverride(1,sampler);d.RsSet(GxRs_ColorWrite,15);
 d.RsSet(GxRs_VertexShader,uiVs);d.RsSet(GxRs_PixelShader,uiPs);d.ShaderConstantsSet(GxSh_Vertex,0,&constants[2][0],4);
 d.RsSet(GxRs_Texture0,capture);Triangle(d,CImVector(255,255,255,255));Equal(Center(d),sceneColor,"captured scene sampled through native UI");
 d.DeviceSetRenderTarget(GxBuffers_Color,nullptr,0);d.RsSet(GxRs_Texture0,static_cast<void*>(nullptr));d.RsSet(GxRs_Texture1,static_cast<void*>(nullptr));d.RsSet(GxRs_Texture2,static_cast<void*>(nullptr));
 d.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));d.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));
 d.TexDestroy(target);d.TexDestroy(t0);d.TexDestroy(t1);for(auto*& shader:owners)d.ShaderDestroy(shader);
 Check(d.RecoverDevice(),"scene capture and custom owners survive clean recovery boundary");
}
