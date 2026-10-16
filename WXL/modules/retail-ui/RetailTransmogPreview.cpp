// Typed native preview adapter. No donor addresses or 32-bit object offsets.
#include "ExtensionApi.hpp"
#include "WeaponPreviewBounds.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/NativeStorage.hpp"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Ui/DressUpModelFrame.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include <cmath>
#include <string>
#include <vector>
namespace {
uint32_t Id(lua_State* L,int index){double v=lua_tonumber(L,index);return std::isfinite(v)&&v>=0&&v<=UINT32_MAX&&v==std::floor(v)?uint32_t(v):0;}
CGDressUpModelFrame* Dress(lua_State* L){return static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L,CGDressUpModelFrame::GetObjectType()));}
CSimpleModel* Model(lua_State* L){return static_cast<CSimpleModel*>(FrameScript_GetObjectThis(L,CSimpleModel::GetObjectType()));}
int Boolean(lua_State* L,bool value){lua_pushboolean(L,value);return 1;}
bool Loaded(CSimpleModel* f){return f&&f->m_model&&f->m_model->IsLoaded(0,0);}
void ClearHand(CGDressUpModelFrame* f,bool off){auto& hand=off?f->m_offHand:f->m_mainHand;if(hand.m_classID)CCharacterComponent::RemoveHandItem(f->m_model,static_cast<INVENTORY_SLOTS>(off?16:15),static_cast<SHEATHE_TYPE>(hand.m_sheatheType),hand.m_inventoryType==14);hand.m_classID=0;hand.m_inventoryType=0;}
int Clear(lua_State* L){auto* f=Dress(L);const auto slot=Id(L,2);bool ok=Loaded(f)&&f->m_component&&slot<=19;if(ok){if(!slot)f->Undress();else if(slot==16||slot==18)ClearHand(f,false);else if(slot==17)ClearHand(f,true);else f->m_component->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(slot-1));}return Boolean(L,ok);}
int Item(lua_State* L){auto* f=Dress(L);auto item=Id(L,2),slot=Id(L,3);bool ok=Loaded(f)&&f->m_component&&item&&item<=INT32_MAX&&slot>=16&&slot<=18;if(ok)f->TryOn(int32_t(item),nullptr,slot==17?16:15);return Boolean(L,ok);}
int Display(lua_State* L){constexpr int slots[19]={0,-1,1,2,3,4,5,6,7,8,-1,-1,-1,-1,10,-1,-1,-1,9};auto slot=Id(L,2),display=Id(L,3);bool ok=false;uint32_t observed=0;
 if(slot>=1&&slot<=19&&slots[slot-1]>=0&&display&&display<=INT32_MAX){ItemDisplayInfoRec row;ok=g_itemDisplayInfoDB.GetRecord(int32_t(display),&row);if(!ok)ok=wxl::native::ItemGraphOwnsDisplay(display);if(ok&&lua_tonumber(L,4)!=0){auto* f=Dress(L);ok=Loaded(f)&&f->m_component;if(ok){auto componentSlot=static_cast<ITEM_SLOT>(slots[slot-1]);if(lua_tonumber(L,4)==1){f->m_component->RemoveItem(componentSlot);f->m_component->AddItem(componentSlot,int32_t(display),0);}observed=f->m_component->m_itemDisplayID[slots[slot-1]];}}}
 lua_pushboolean(L,ok);lua_pushnumber(L,observed);return 2;}
int Finish(lua_State* L){auto* f=Dress(L);auto* c=f?f->m_component:nullptr;if(c&&lua_tonumber(L,2)==1)c->m_flags|=1;return Boolean(L,c&&!(c->m_flags&1));}
std::string Path(const char* name,const std::string& folder,const char* extension){if(!name||!*name)return {};std::string s=name;if(s.find_first_of("\\/")==std::string::npos)s=folder+s;auto slash=s.find_last_of("\\/"),dot=s.find_last_of('.');if(dot!=std::string::npos&&(slash==std::string::npos||dot>slash))s.resize(dot);return s+extension;}
int Asset(lua_State* L){auto display=Id(L,1);std::string model,texture,folder=Id(L,2)==14?"Item\\ObjectComponents\\Shield\\":"Item\\ObjectComponents\\Weapon\\";ItemDisplayInfoRec row;
 if(display&&display<=INT32_MAX&&g_itemDisplayInfoDB.GetRecord(int32_t(display),&row)){model=Path(row.m_modelName[0],folder,".m2");texture=Path(row.m_modelTexture[0],folder,".blp");}
 else if(wxl::native::ItemGraphOwnsDisplay(display)){std::vector<wxl::native::ItemGraphModel> entries;if(wxl::native::ItemGraphModels(display,entries)&&!entries.empty()){auto& e=entries.front();if(!e.folder.empty())folder=e.folder+"\\";model=Path(e.model.c_str(),folder,".m2");texture=Path(e.texture.c_str(),folder,".blp");}}
 if(model.empty()){lua_pushnil(L);return 1;}lua_pushstring(L,model.c_str());lua_pushstring(L,texture.c_str());std::vector<uint8_t> bytes;float center[4]{},projected[2]{};if(!wxl::native::ReadAsset(model.c_str(),bytes)||!WXL::WeaponPreviewBounds(bytes.data(),bytes.size(),center,projected))return 2;for(float v:center)lua_pushnumber(L,v);for(float v:projected)lua_pushnumber(L,v);return 8;}
int Texture(lua_State* L){auto* f=Model(L);bool ok=Loaded(f);if(ok){f->m_model->SetEmittersEnabled(0);f->m_model->SetRibbonsEnabled(0);auto* path=lua_tolstring(L,2,nullptr);if(path&&*path)f->ReplaceTexture(2,path);}return Boolean(L,ok);}
int Fit(lua_State* L){auto* f=Model(L);double x=lua_tonumber(L,2),y=lua_tonumber(L,3),z=lua_tonumber(L,4),radius=lua_tonumber(L,5);CRect rect{};bool ok=f&&std::isfinite(radius)&&radius>.001&&radius<10000;for(double v:{x,y,z})ok=ok&&std::isfinite(v)&&std::abs(v)<10000;
 if(!ok||!f->GetRect(&rect)){lua_pushnil(L);return 1;}double width=std::abs(rect.maxX-rect.minX),height=std::abs(rect.maxY-rect.minY),effective=f->m_layoutScale,conversion=NDCToDDCHeight(1.f)*(5./3.);if(!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0||!std::isfinite(effective)||effective<=0||!std::isfinite(conversion)||conversion<=0){lua_pushnil(L);return 1;}double hw=lua_tonumber(L,6),hh=lua_tonumber(L,7);if(!std::isfinite(hw)||hw<=.001)hw=radius;if(!std::isfinite(hh)||hh<=.001)hh=radius;double scale=std::min(width/hw,height/hh)*.4/(effective*conversion);lua_pushnumber(L,scale);lua_pushnumber(L,width*.5/effective-scale*conversion*(std::cos(.65)*x-std::sin(.65)*y));lua_pushnumber(L,height*.5/effective-scale*conversion*(std::sin(.65)*x+std::cos(.65)*y));lua_pushnumber(L,-scale*conversion*z);return 4;}
}
namespace wxl_retail_ui {bool InstallRetailTransmogPreview(){bool ok=true;for(auto e:{std::pair{"_WXL_TRANSMOG_PREVIEW_DISPLAY",&Display},{"_WXL_TRANSMOG_PREVIEW_CLEAR",&Clear},{"_WXL_TRANSMOG_PREVIEW_ITEM",&Item},{"_WXL_TRANSMOG_WEAPON_ASSET",&Asset},{"_WXL_TRANSMOG_WEAPON_TEXTURE",&Texture},{"_WXL_TRANSMOG_WEAPON_FIT",&Fit},{"_WXL_TRANSMOG_PREVIEW_FINISH",&Finish}})ok=FrameScript()->RegisterFunction(e.first,e.second)&&ok;return ok;}}
