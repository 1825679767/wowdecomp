// Preview-owned outfit visibility. No fake class, player mutation or saved appearance edit.
#include "WXL/CreationEquipment.hpp"
#include "WXL/CreationEffects.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include <cmath>
namespace wxl::native::creation_equipment {namespace {
struct Owner{CCharacterComponent* component=nullptr;CM2Model* model=nullptr;uint64_t serial=0;uint32_t handle=0;bool hidden=false;} owner;
CCharacterComponent* Current(){auto* c=CCharacterCreation::m_character;auto* f=CCharacterCreation::m_charCustomizeFrame;if(!NativeEnabled()||!AppearancesEnabled()||CCharacterCreation::m_existingCharacterIndex>=0||!f||!f->m_isVisible||!c||!c->m_data.m_model||!c->m_data.m_model->IsLoaded(0,0))return nullptr;return c;}
bool Same(){auto* c=CCharacterCreation::m_character;return c&&c==owner.component&&c->m_wxlSerial==owner.serial&&c->m_data.m_model&&c->m_data.m_model==owner.model&&c->m_data.m_model->m_handle==owner.handle;}
void Clear(CCharacterComponent& c){for(uint32_t i=0;i<NUM_ITEM_SLOTS;++i)c.RemoveItem(static_cast<ITEM_SLOT>(i));for(uint32_t sheath=SHEATHE_NONE;sheath<=SHEATHE_SHIELD;++sheath)for(uint32_t slot=INVSLOT_MAINHAND;slot<=INVSLOT_RANGED;++slot){CCharacterComponent::RemoveHandItem(c.m_data.m_model,static_cast<INVENTORY_SLOTS>(slot),static_cast<SHEATHE_TYPE>(sheath),false);CCharacterComponent::RemoveHandItem(c.m_data.m_model,static_cast<INVENTORY_SLOTS>(slot),static_cast<SHEATHE_TYPE>(sheath),true);}c.m_dirtySections=0xffffffffu;c.m_flags|=5u;}
int Visible(lua_State* L){if(L!=FrameScript_GetContext())return 0;if(!Current()){lua_pushnil(L);return 1;}lua_pushboolean(L,!Same()||!owner.hidden);return 1;}
int Set(lua_State* L){bool ok=false;try{auto* c=L==FrameScript_GetContext()?Current():nullptr;double n=lua_tonumber(L,1);if(c&&lua_type(L,1)==LUA_TNUMBER&&std::isfinite(n)){creation_effects::StopCreation();owner={c,c->m_data.m_model,c->m_wxlSerial,c->m_data.m_model->m_handle,n==0};CCharacterCreation::Dress();ok=true;}}catch(...){ok=false;}lua_pushboolean(L,ok);return 1;}
constexpr char Bridge[]=R"lua(
do
 local f=CharacterCreate and CharacterCreate.CustomizationFrame
 local b=f and f.ToggleUIButton
 if b and not b.WXLNativeGearOwner then
  b.WXLNativeGearOwner=true
  b:SetScript("OnClick",function()
   local visible=WXL_IsCreationGearVisible()
   if visible~=nil then WXL_SetCreationGearVisible(visible and 0 or 1) end
  end)
 end
 if b and b.ButtonTexture then
  local visible=WXL_IsCreationGearVisible()
  if visible~=nil then CHAR_UNDRESSED=not visible;CHARACTER_CREATE_DRESSED=visible;b.ButtonTexture:SetVertexColor(visible and .72 or 1,visible and .72 or .82,visible and .72 or 0,1) end
 end
end
)lua";
}
bool DressHidden() noexcept {try{if(Same()&&owner.hidden&&CCharacterCreation::m_existingCharacterIndex<0){Clear(*CCharacterCreation::m_character);return true;}}catch(...){return true;}return false;}
void Reset() noexcept {try{bool restore=Same()&&owner.hidden;owner={};if(restore)CCharacterCreation::Dress();}catch(...){owner={};}}
void InitializeLua(){Reset();FrameScript_RegisterFunction("WXL_IsCreationGearVisible",Visible);FrameScript_RegisterFunction("WXL_SetCreationGearVisible",Set);FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_GEAR_VISIBLE",Set);}
void UpdateUi(){if(NativeEnabled()&&AppearancesEnabled())FrameScript_Execute(Bridge,"native-creation-gear-visibility",nullptr);}
}
