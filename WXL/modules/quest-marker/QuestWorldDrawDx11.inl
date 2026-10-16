// Native Gx fixed-function quads; no D3D9 ownership or raw hardware mutation.
#include <array>
#include <cmath>
#include <cstring>
namespace wxl::native::quest_marker {namespace {
C44Matrix& NavigationMatrix(CGxMatrixStack& stack){return stack.m_mtx[stack.m_level];}
struct NavigationDx11State {
 CGxDeviceD3d11& owner;CGxVertexAttrib attributes[14];CGxBuf* buffers[14];CGxBuf* index;
 uint32_t mask,masters;C44Matrix projection,world,texture,generation;EGxVertexBufferFormat format;CGxBuf* vertex;uint32_t vertexSize;
 CGxDeviceD3d11::SamplerOverride sampler;
 explicit NavigationDx11State(CGxDeviceD3d11& d):owner(d),index(d.m_primIndexBuf),mask(d.m_primVertexMask),masters(d.m_appMasterEnables),projection(d.m_projNative),world(NavigationMatrix(d.m_xforms[GxXform_World])),texture(NavigationMatrix(d.m_xforms[0])),generation(NavigationMatrix(d.m_texGen[0])),format(d.m_primVertexFormat),vertex(d.m_primVertexBuf),vertexSize(d.m_primVertexSize){
  std::memcpy(attributes,d.m_primVertexFormatAttrib,sizeof(attributes));std::memcpy(buffers,d.m_primVertexFormatBuf,sizeof(buffers));d.RsPush();sampler=d.ExchangeSamplerOverride(0,{true,false,false,false});
 }
 ~NavigationDx11State(){
  owner.ExchangeSamplerOverride(0,sampler);owner.m_projNative=projection;NavigationMatrix(owner.m_xforms[GxXform_World])=world;NavigationMatrix(owner.m_xforms[0])=texture;NavigationMatrix(owner.m_texGen[0])=generation;owner.m_appMasterEnables=masters;owner.m_primVertexFormat=format;owner.m_primVertexBuf=vertex;owner.m_primVertexSize=vertexSize;
  std::memcpy(owner.m_primVertexFormatAttrib,attributes,sizeof(attributes));std::memcpy(owner.m_primVertexFormatBuf,buffers,sizeof(buffers));owner.PrimVertexMask(mask);owner.PrimIndexPtr(index);owner.RsPop();
 }
};
unsigned DrawWorldNavigationDx11(CGxDeviceD3d11& d,CGxTex& atlas,std::span<const WorldPoint> points,float scale,unsigned alpha){
 if(atlas.m_target!=GxTex_2d)return 0;
 const auto view=NavigationMatrix(d.m_xforms[GxXform_View]);auto projection=d.m_projNative;const auto* v=reinterpret_cast<const float*>(&view);auto* pr=reinterpret_cast<float*>(&projection);
 const float height=(d.m_curWindowRect.maxY-d.m_curWindowRect.minY)*(d.m_viewport.m_y.m_h-d.m_viewport.m_y.m_l);
 if(!std::isfinite(height)||height<=0||!std::isfinite(pr[5])||std::abs(pr[5])<1e-5f||!std::isfinite(pr[10])||std::abs(pr[10])<1e-10f)return 0;
 const float nearZ=-pr[14]/pr[10];if(!std::isfinite(nearZ)||nearZ<=0||nearZ>=100000)return 0;
 pr[10]=100000.f/(100000.f-nearZ);pr[14]=-nearZ*100000.f/(100000.f-nearZ);
 const C3Vector right{v[0],v[4],v[8]},up{v[1],v[5],v[9]};if(right.SquaredMag()<.5f||up.SquaredMag()<.5f)return 0;
 struct NavigationVertex {float x,y,z;CImVector color;float u,v;};static_assert(sizeof(NavigationVertex)==24);
 std::array<NavigationVertex,65*6> vertices{};std::array<bool,65> corpse{};unsigned count=0;
 for(const auto& point:points){const auto& p=point.position;const float z=p.x*v[2]+p.y*v[6]+p.z*v[10]+v[14];if(!std::isfinite(z)||z<.1f)continue;
  const float half=16.f*(point.corpse?1:scale)*2.f*z/(std::abs(pr[5])*height);if(!std::isfinite(half)||half<=0||half>1e7f)continue;
  const float aspect=point.corpse?28.f/34:28.f/35,u0=point.corpse?.002f:.4395f,u1=point.corpse?.4355f:.8725f,v1=point.corpse?.52825f:.543875f;const CImVector color(255,255,255,point.corpse?255:alpha);
  constexpr float xy[6][2]{{-1,1},{-1,-1},{1,1},{1,1},{-1,-1},{1,-1}};
  for(unsigned i=0;i<6;++i){const auto pos=p+right*(xy[i][0]*half*aspect)+up*(xy[i][1]*half);vertices[count*6+i]={pos.x,pos.y,pos.z,color,xy[i][0]<0?u0:u1,xy[i][1]>0?.002f:v1};}corpse[count++]=point.corpse;
 }
 if(!count)return 0;
 auto* buffer=d.BufStream(GxPoolTarget_Vertex,sizeof(NavigationVertex),count*6);if(!buffer)return 0;auto* target=d.BufLock(buffer);if(!target)return 0;std::memcpy(target,vertices.data(),count*6*sizeof(NavigationVertex));if(!d.BufUnlock(buffer,0))return 0;
 NavigationDx11State saved(d);C44Matrix identity;identity.Identity();NavigationMatrix(d.m_xforms[GxXform_World])=identity;NavigationMatrix(d.m_xforms[0])=identity;NavigationMatrix(d.m_texGen[0])=identity;d.m_projNative=projection;
 d.MasterEnableSet(GxMasterEnable_DepthTest,1);d.MasterEnableSet(GxMasterEnable_DepthWrite,1);
 d.RsSet(GxRs_VertexShader,static_cast<void*>(nullptr));d.RsSet(GxRs_PixelShader,static_cast<void*>(nullptr));d.RsSet(GxRs_Texture0,&atlas);for(unsigned i=1;i<8;++i)d.RsSet(static_cast<EGxRenderState>(GxRs_Texture0+i),static_cast<void*>(nullptr));
 d.RsSet(GxRs_TextureCoord0,0);d.RsSet(GxRs_TexGen0,GxTexGen_Disabled);d.RsSet(GxRs_ColorOp0,0);d.RsSet(GxRs_AlphaOp0,0);d.RsSet(GxRs_TextureShader0,0);
 d.RsSet(GxRs_DepthTest,1);d.RsSet(GxRs_DepthFunc,0);d.RsSet(GxRs_DepthWrite,0);d.RsSet(GxRs_BlendingMode,GxBlend_Alpha);d.RsSet(GxRs_Culling,0);d.RsSet(GxRs_Lighting,0);d.RsSet(GxRs_Fog,0);d.RsSet(GxRs_ColorWrite,15);
 d.PrimVertexPtr(buffer,GxVBF_PCT);unsigned drawn=0;
 for(unsigned i=0;i<count;++i){d.RsSet(GxRs_AlphaRef,corpse[i]?160:0);CGxBatch batch{};batch.m_primType=GxPrim_Triangles;batch.m_start=i*6;batch.m_count=6;batch.m_maxIndex=count*6-1;const auto before=d.SubmittedDraws();d.Draw(&batch,0);if(d.SubmittedDraws()>before)drawn|=corpse[i]?2u:1u;}
 return drawn;
}
}}
