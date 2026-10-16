// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "wxl/AppearanceApi.h"
#include <array>
#include <span>
namespace wxl::waystone::appearance {
// Portable counterpart of WXL CharacterGeosets whole-material policy. No native layouts.
using WholeLayers=std::array<WXL_TextureLayer,32>;
inline WholeLayers SelectWholeLayers(uint32_t race,uint32_t sex,std::span<const WXL_TextureLayer> layers,std::span<const uint32_t> choices){
 WholeLayers selected{};const bool dragon=race==52 || race==70;
 for(const auto& layer:layers){const auto type=layer.textureType;const bool whole=type==6 || type==8 || type==9 || type==10 || type==19 || type==20 || type==22 || type==24 || (dragon && (type==7 || type==25));if(type>=32 || !whole || !layer.materialResourceId)continue;
  const auto& previous=selected[type];const bool complete=type==6 || type==19 || (dragon && (type==7 || type==10));
  if(!previous.materialResourceId || (complete && layer.blendMode==1 && previous.blendMode!=1) || ((!complete || (layer.blendMode==1)==(previous.blendMode==1)) && layer.layer>=previous.layer))selected[type]=layer;
 }
 if(!selected[9].materialResourceId && sex<=1){
  bool blindfold=false;for(auto choice:choices){if(race==10)blindfold=blindfold || (sex==0?choice>=1721 && choice<=1731:choice>=1858 && choice<=1868);if(race==4)blindfold=blindfold || (sex==0?choice>=790 && choice<=800:choice>=902 && choice<=912);}
  if(blindfold)selected[9]={9,race==10?(sex?17u:13u):0u,1,0xffffffffu,race==10?(sex?1104893u:1104895u):(sex?1104901u:1104903u)};
 }
 if(race==23 && sex<=1 && !selected[6].materialResourceId)for(auto choice:choices){uint32_t material=0;if(!sex && choice>=2427 && choice<=2431)material=159727+choice-2427;if(sex && choice>=2515 && choice<=2519)material=128788+choice-2515;if(material){selected[6]={6,2,1,0xffffffffu,material};break;}}
 return selected;
}
}
