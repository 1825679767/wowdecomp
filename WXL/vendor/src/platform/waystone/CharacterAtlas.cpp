// Copyright (C) 2026 WarcraftXL. GPLv3.
#include "platform/waystone/CharacterAtlas.hpp"
#include <algorithm>
#include <cmath>
#include "engine/assets/shared/textures/blp/BlpTranscode.hpp"
namespace wxl::waystone::appearance {
uint32_t BlindfoldMaterial(uint32_t retailRace,uint32_t sex,std::span<const uint32_t> choices){
 if(sex>1)return 0;for(auto choice:choices){if(retailRace==10 && (sex==0?(choice>=1721 && choice<=1731):(choice>=1858 && choice<=1868)))return sex?1104893:1104895;
 if(retailRace==4 && (sex==0?(choice>=790 && choice<=800):(choice>=902 && choice<=912)))return sex?1104901:1104903;}return 0;
}
bool FacialOption(std::string_view option){return option=="Facial Hair" || option=="Beard" || option=="Mustache" || option=="Piercings" || option=="Earrings" || option=="Ears" || option=="FacialJewelry" || option=="Tusks" || option=="Horns";}
uint32_t ChoiceOrdinal(std::string_view option,uint32_t count,const Inputs& inputs){
 if(!count)return 0;
 uint32_t index=0;
 if(option=="Skin Color" || option=="Skin Type" || option=="Fur Color")index=inputs.skin;
 else if(option=="Face")index=inputs.face;
 else if(option=="Hair Style")index=inputs.hairStyle;
 else if(option=="Hair Color")index=inputs.hairColor;
 else if(FacialOption(option))index=inputs.facialStyle;
 // Other options retain table defaults until their own UI/server and child seams exist.
 return index%count;
}
bool DecodeLayerImage(std::span<const uint8_t> source,Image& image,bool blended){
 // WXL's character sampler uses BC1's transparent endpoint for blended layers,
 // including legacy layered files whose alpha-depth field is zero.
 // Blit sources and the general asset decoder retain opaque BC1 semantics.
 if(blended && source.size()>=148 && source[0]=='B' && source[1]=='L' && source[2]=='P' && source[3]=='2' && source[8]==2 && source[9]==0 && source[10]==0){
  std::vector<uint8_t> corrected(source.begin(),source.end());corrected[9]=1;
  return wxl::modern::assets::textures::blp::DecodeBlp(corrected,image.width,image.height,image.rgba);
 }
 return wxl::modern::assets::textures::blp::DecodeBlp(source,image.width,image.height,image.rgba);
}
bool Paint(std::span<uint32_t> dest,uint32_t width,uint32_t height,const Image& source,Rect span,Rect r,uint32_t blend){
 if(!width || !height || dest.size()!=size_t(width)*height || !source.width || !source.height || source.rgba.size()!=size_t(source.width)*source.height*4 || !r.width || !r.height || uint64_t(r.x)+r.width>width || uint64_t(r.y)+r.height>height || !span.width || !span.height)return false;
 if(blend!=1 && blend!=4 && blend!=6 && blend!=7 && blend!=9 && blend!=15 && blend!=FeatherBottom)return false;
 // A file covers the union only when both axes describe the same authored scale.
 if(uint64_t(source.width)*span.height!=uint64_t(source.height)*span.width)span=r;
 if(r.x<span.x || r.y<span.y || uint64_t(r.x)+r.width>uint64_t(span.x)+span.width || uint64_t(r.y)+r.height>uint64_t(span.y)+span.height)return false;
 struct SampleAxis {uint32_t first,second;double weight;};
 const auto axis=[](uint32_t at,uint32_t origin,uint32_t sourceSize,uint32_t spanSize){const double position=(double(at-origin)+.5)*sourceSize/spanSize-.5;const int index=int(std::floor(position));return SampleAxis{uint32_t(std::clamp(index,0,int(sourceSize)-1)),uint32_t(std::clamp(index+1,0,int(sourceSize)-1)),position-index};};
 std::vector<SampleAxis> columns;columns.reserve(r.width);for(uint32_t x=r.x;x<r.x+r.width;++x)columns.push_back(axis(x,span.x,source.width,span.width));
 for(uint32_t y=r.y;y<r.y+r.height;++y){const auto row=axis(y,span.y,source.height,span.height);
 for(uint32_t x=r.x;x<r.x+r.width;++x){
  const auto column=columns[x-r.x];const auto* a=source.rgba.data()+(size_t(row.first)*source.width+column.first)*4;const auto* b=source.rgba.data()+(size_t(row.first)*source.width+column.second)*4;const auto* c=source.rgba.data()+(size_t(row.second)*source.width+column.first)*4;const auto* d=source.rgba.data()+(size_t(row.second)*source.width+column.second)*4;
  const double fx=column.weight,fy=row.weight;uint32_t src[4]{};
  for(unsigned channel=0;channel<4;++channel){double value=a[channel]*(1-fx)*(1-fy);value+=b[channel]*fx*(1-fy);value+=c[channel]*(1-fx)*fy;value+=d[channel]*fx*fy;src[channel]=uint32_t(value+.5);}
  if(blend==FeatherBottom && r.height>=4 && y-r.y>=r.height-r.height/4)src[3]=src[3]*(r.height-1-(y-r.y))/(r.height/4);
  auto& pixel=dest[size_t(y)*width+x];uint32_t out=0xff000000;
  for(unsigned c=0;c<3;++c){const unsigned shift=(2-c)*8;const uint32_t had=(pixel>>shift)&255;uint32_t got=src[c];
   if(blend==4)got=had*got/255;
   else if(blend==6)got=got<128?2*had*got/255:255-2*(255-had)*(255-got)/255;
   else if(blend==7)got=255-(255-had)*(255-got)/255;
   if(blend!=1)got=(got*src[3]+had*(255-src[3]))/255;
   out|=got<<shift;
  }pixel=out;
 }}return true;
}
bool PaintBaseDetails(std::span<uint32_t> dest,uint32_t width,uint32_t height,const Image& source,Rect span,std::span<const Rect> sections,uint32_t layout){
 // Match current WXL's authored appendage cells; other layouts fill only unnamed body cells.
 if(width==2048 && height==1024 && (layout==113 || layout==114 || layout==145 || layout==146 || layout==123 || layout==124 || layout==129 || layout==130))return Paint(dest,width,height,source,span,{0,640,512,384},1);
 const auto right=width/2;std::vector<uint32_t> xs{0,right},ys{0,height};
 for(auto r:sections){if(uint64_t(r.x)+r.width>width || uint64_t(r.y)+r.height>height)return false;if(r.x>=right)continue;xs.push_back(r.x);xs.push_back(std::min(right,r.x+r.width));ys.push_back(r.y);ys.push_back(r.y+r.height);}
 std::sort(xs.begin(),xs.end());xs.erase(std::unique(xs.begin(),xs.end()),xs.end());std::sort(ys.begin(),ys.end());ys.erase(std::unique(ys.begin(),ys.end()),ys.end());
 for(size_t y=1;y<ys.size();++y)for(size_t x=1;x<xs.size();++x){Rect c{xs[x-1],ys[y-1],xs[x]-xs[x-1],ys[y]-ys[y-1]};bool covered=false;for(auto r:sections)if(r.x<=c.x && r.y<=c.y && r.x+r.width>=c.x+c.width && r.y+r.height>=c.y+c.height){covered=true;break;}if(!covered && !Paint(dest,width,height,source,span,c,1))return false;}return true;
}
uint32_t PrivateOverlayRegion(uint32_t type,uint32_t texture){
 // Native CharSections has lower face first, upper face second. Hair's
 // independent palette occupies index 0, followed by lower/upper face overlays.
 if((type==1 || type==2) && texture<2)return texture?8:9;
 if(type==3 && texture>=1 && texture<=2)return texture==1?9:8;
 if(type==4 && texture<2)return texture?3:5;
 return 10;
}
bool OwnFirstBodyPass(uint32_t race,uint32_t layout,uint32_t region,uint32_t& regions){
 // WXL substitutes the first native body source for Worgen/Gilnean layouts,
 // even when a recycled component no longer lists that handle as a variation.
 if(race!=12 || layout<125 || layout>128 || region>=10)return false;
 const uint32_t bit=1u<<region;const bool first=!(regions&bit);regions|=bit;return first;
}
bool OwnRetailFaceRegion(uint32_t layout,uint32_t region){
 return region>=8 && region<10 && (layout==113 || layout==114 || layout==145 || layout==146 || layout==123 || layout==124 || layout==129 || layout==130 || layout==157 || layout==158);
}
bool HidePrivatePreviewCape(uint32_t race,bool preview,std::span<const uint16_t> ids,std::span<uint32_t> shown){
 if(race!=23 || !preview || ids.size()!=shown.size())return false;
 bool changed=false;for(size_t i=0;i<ids.size();++i)if(ids[i]/100==15 && shown[i]){shown[i]=0;changed=true;}return changed;
}
void Mips(uint32_t w,uint32_t h,std::vector<std::vector<uint32_t>>& levels){
 if(!w || !h || levels.empty() || levels[0].size()!=size_t(w)*h)return;
 levels.resize(1);
 while(w>1 || h>1){uint32_t nw=std::max(1u,w/2),nh=std::max(1u,h/2);std::vector<uint32_t> next(size_t(nw)*nh);
  for(uint32_t y=0;y<nh;++y)for(uint32_t x=0;x<nw;++x){uint32_t out=0;for(unsigned shift=0;shift<32;shift+=8){uint32_t sum=0;for(unsigned j=0;j<2;++j)for(unsigned i=0;i<2;++i)sum+=(levels.back()[size_t(std::min(h-1,y*2+j))*w+std::min(w-1,x*2+i)]>>shift)&255;out|=((sum+2)/4)<<shift;}next[size_t(y)*nw+x]=out;}
  levels.push_back(std::move(next));w=nw;h=nh;
 }
}
bool SelectGeosetGroup(std::span<const uint16_t> ids,std::span<uint32_t> shown,std::span<const uint16_t> chosen,uint16_t first,uint16_t last){
 if(ids.size()!=shown.size() || first>last)return false;bool changed=false;
 for(size_t i=0;i<ids.size();++i)if(ids[i]>=first && ids[i]<=last){const bool wanted=std::find(chosen.begin(),chosen.end(),ids[i])!=chosen.end();if(bool(shown[i])!=wanted){shown[i]=wanted;changed=true;}}
 return changed;
}
bool SelectGeosets(std::span<const uint16_t> ids,std::span<uint32_t> shown,std::span<const uint16_t> owned,std::span<const uint16_t> chosen){
 if(ids.size()!=shown.size())return false;bool changed=false;
 const auto has=[](auto values,uint16_t id){return std::find(values.begin(),values.end(),id)!=values.end();};
 for(size_t i=0;i<ids.size();++i){const auto id=ids[i];if(!id)continue;bool manage=has(owned,id) || id>2000;
  if(!manage)continue;bool visible=has(chosen,id);
  if(id>2000){const auto group=id/100;bool claimed=false;for(auto value:owned)if(value/100==group)claimed=true;
   if(!claimed){uint16_t lowest=65535;for(auto value:ids)if(value>2000 && value/100==group)lowest=std::min(lowest,value);visible=id==lowest;}
  }
  if((shown[i]!=0)!=visible){shown[i]=visible?1:0;changed=true;}
 }return changed;
}
}
