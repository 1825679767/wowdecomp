#include "ExtensionApi.hpp"
#include "TransmogOutfitWire.hpp"


#include <mutex>
#include <cmath>
namespace {
namespace W=WXL::TransmogOutfitWire;
std::mutex lock;uint32_t requested=0;bool ready=false;W::State result;
void __cdecl Receive(const uint8_t* data,uint32_t size,void*){W::State v;if(!W::Decode({data,size},v))return;std::lock_guard guard(lock);if(v.Id!=requested||ready)return;result=std::move(v);ready=true;}
int __cdecl Send(lua_State* state){size_t size=0;auto text=lua_tolstring(state,1,&size);W::Request r;bool sent=false;
 if(text&&W::Decode({reinterpret_cast<const uint8_t*>(text),size},r)){{std::lock_guard guard(lock);if(!++requested)++requested;r.Id=requested;ready=false;}auto bytes=W::Encode(r);sent=wxl_retail_ui::Network()->Send(W::RequestOpcode,bytes.data(),uint32_t(bytes.size()))!=0;}
 if(sent)lua_pushnumber(state,r.Id);else lua_pushnil(state);return 1;}
int __cdecl Read(lua_State* state){std::lock_guard guard(lock);if(!ready){lua_pushnil(state);return 1;}auto push=lua_pushnumber;push(state,result.Id);push(state,result.Status);push(state,result.Owned);push(state,result.Price);push(state,result.Rows.size());return 5;}
int __cdecl Row(lua_State* state){double i=lua_tonumber(state,1);std::lock_guard guard(lock);if(!ready||!std::isfinite(i)||i!=std::floor(i)||i<1||i>result.Rows.size()){lua_pushnil(state);return 1;}auto const&r=result.Rows[size_t(i)-1];auto n=lua_pushnumber;auto s=lua_pushstring;n(state,r.Slot);n(state,r.Revision);s(state,r.Name.c_str());s(state,r.Payload.c_str());lua_pushboolean(state,r.Deleted);return 5;}
}
bool wxl_retail_ui::InstallRetailTransmogOutfits(){auto n=Network();auto s=FrameScript();bool ok=n->RegisterClientOpcode(W::RequestOpcode,"CMSG_WXL_TRANSMOG_OUTFIT_REQUEST")!=0;ok&=n->RegisterServerOpcode(W::ResponseOpcode,"SMSG_WXL_TRANSMOG_OUTFIT_STATE",Receive,nullptr)!=0;ok&=s->RegisterFunction("_WXL_TRANSMOG_OUTFIT_SEND",Send)!=0;ok&=s->RegisterFunction("_WXL_TRANSMOG_OUTFIT_STATE",Read)!=0;ok&=s->RegisterFunction("_WXL_TRANSMOG_OUTFIT_ROW",Row)!=0;return ok;}

void wxl_retail_ui::ResetOutfits(){std::lock_guard guard(lock);if(!++requested)++requested;ready=false;result={};}
