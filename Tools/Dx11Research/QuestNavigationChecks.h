#pragma once
#include "WXL/modules/quest-marker/QuestWorldDraw.hpp"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/Handle.h"
static void QuestNavigationChecks(bool warp){
    namespace quest=wxl::native::quest_marker;
    CGxDeviceD3d11 d;g_theGxDevicePtr=&d;Check(d.CreateOffscreen(64,64,warp),"quest navigation native device");
    Pixels pixels;pixels.width=256;pixels.height=64;pixels.data.resize(256*64);
    for(unsigned y=0;y<64;++y)for(unsigned x=0;x<256;++x)pixels.data[y*256+x]=x<112?CImVector(0,0,255,255):CImVector(0,255,0,128);
    CGxTexFlags flags(GxTex_Linear);auto* texture=TextureCreate(GxTex_2d,256,64,1,GxTex_Argb8888,GxTex_Argb8888,flags,&pixels,TextureData,"Interface\\Navigation\\InGameNavigationUI",0);
    Check(texture!=nullptr,"real quest atlas texture owner");TextureCacheNewTexture(texture,flags);
    C44Matrix identity;identity.Identity();C44Matrix projection=identity;projection.c2=100.f/99;projection.c3=1;projection.d2=-100.f/99;projection.d3=0;
    d.RsSet(GxRs_DepthTest,1);d.RsSet(GxRs_DepthFunc,0);d.RsSet(GxRs_DepthWrite,1);d.RsSet(GxRs_Culling,0);d.RsSet(GxRs_ScissorTest,0);d.RsSet(GxRs_BlendingMode,GxBlend_Opaque);d.RsSet(GxRs_Fog,0);d.RsSet(GxRs_Lighting,0);
    d.XformSetProjection(identity);d.SceneClear(3,CImVector(0,0,0,255));Triangle(d,CImVector(255,255,255,255));
    const auto nativeBuffer=d.m_primVertexBuf;const auto nativeMask=d.m_primVertexMask;d.m_projNative=projection;
    auto& world=d.m_xforms[GxXform_World];world.m_mtx[world.m_level]=identity;world.m_mtx[world.m_level].d0=7;
    auto& uv=d.m_xforms[0];uv.m_mtx[uv.m_level]=identity;uv.m_mtx[uv.m_level].a0=3;
    const auto savedWorld=world.m_mtx[world.m_level],savedUv=uv.m_mtx[uv.m_level];
    const quest::WorldPoint marker{{0,0,10},false},corpse{{0,0,10},true},behind{{0,0,-10},false};
    d.SceneClear(3,CImVector(0,0,0,255));Check(quest::DrawWorldNavigation({&marker,1},1,255)==1,"DX11 world quest billboard submits through native Gx");
    Equal(Center(d,64,64),CImVector(0,128,0,191),"quest atlas UV region and source alpha");
    Check(d.m_primVertexBuf==nativeBuffer&&d.m_primVertexMask==nativeMask&&d.m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]==GxBlend_Opaque,"quest navigation restores native vertex and blend state");
    Check(!std::memcmp(&projection,&d.m_projNative,sizeof(projection))&&!std::memcmp(&savedWorld,&world.m_mtx[world.m_level],sizeof(savedWorld))&&!std::memcmp(&savedUv,&uv.m_mtx[uv.m_level],sizeof(savedUv)),"quest navigation restores projection/world/texture transforms");
    const auto sampler=d.ExchangeSamplerOverride(0,{});Check(!sampler.enabled,"quest navigation restores sampler scope");d.ExchangeSamplerOverride(0,sampler);
    CiRect center(32,32,33,33);TSGrowableArray<float> depth;d.DeviceReadDepths(center,depth);Check(depth.Count()==1&&depth[0]>.999f,"quest billboard never writes scene depth");
    d.SceneClear(3,CImVector(0,0,0,255));Check(quest::DrawWorldNavigation({&corpse,1},1,17)==2,"DX11 corpse billboard has independent alpha policy");Equal(Center(d,64,64),CImVector(0,0,255,255),"corpse atlas region and opaque tint");
    for(unsigned y=0;y<64;++y)for(unsigned x=0;x<112;++x)pixels.data[y*256+x].a=128;
    auto* atlas=TextureGetGxTex(texture,0,nullptr);d.TexMarkForUpdate(atlas,CiRect(0,0,256,64),0);d.SceneClear(3,CImVector(0,0,0,255));quest::DrawWorldNavigation({&corpse,1},1,255);Equal(Center(d,64,64),CImVector(0,0,0,255),"corpse alpha coverage rejects texels below 160");
    world.m_mtx[world.m_level]=identity;d.m_projNative=identity;d.SceneClear(3,CImVector(0,0,0,255));Triangle(d,CImVector(255,0,0,255),.25f);d.m_projNative=projection;
    quest::DrawWorldNavigation({&marker,1},1,255);Equal(Center(d,64,64),CImVector(255,0,0,255),"quest billboard respects existing scene occlusion");
    const auto before=d.SubmittedDraws();Check(!quest::DrawWorldNavigation({&behind,1},1,255)&&d.SubmittedDraws()==before,"behind-camera quest markers never submit");
    Check(d.RecoverDevice(),"quest atlas GPU recovery");d.SceneClear(3,CImVector(0,0,0,255));Check(quest::DrawWorldNavigation({&marker,1},1,255)==1,"quest atlas survives device recreation");Equal(Center(d,64,64),CImVector(0,128,0,191),"recreated quest billboard pixels");
    quest::DestroyWorldNavigation();HandleClose(reinterpret_cast<HOBJECT>(texture));TextureGxCacheClear();d.DeviceDestroy();g_theGxDevicePtr=nullptr;
}
