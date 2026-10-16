// Source-native UI/transport adapter. Packet callbacks never run Lua under protocol locks.
#include "ExtensionApi.hpp"
#include "WXL/RetailUi.hpp"
#include "WXL/NativeFeatures.hpp"
#include "WXL/NativeProtocol.hpp"
#include "WXL/FdidCompat.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include <array>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#ifndef WXL_ENABLE_RETAIL_UI
#define WXL_ENABLE_RETAIL_UI 1
#endif
namespace {
namespace protocol=wxl::native::protocol;
using wxl_retail_ui::PacketCallback;
struct Route{std::string name;lua_CFunction callback=nullptr;};
struct Incoming{uint16_t opcode=0;uint64_t generation=0;std::vector<uint8_t> bytes;};
struct PacketOwner{PacketCallback callback=nullptr;void* user=nullptr;};
std::array<Route,96> routes;size_t routeCount=0;
std::map<uint16_t,PacketOwner> receivers;std::vector<protocol::NativeDeclaration> declarations;
std::map<std::string,std::string> scripts;std::vector<std::string> scriptOrder;
std::mutex inboxMutex;std::deque<Incoming> inbox;size_t inboxBytes=0;
CVar* enabled=nullptr;bool bound=false,collecting=false,ready=false,pumping=false;lua_State* readyContext=nullptr;
thread_local uint64_t dispatchGeneration=0;
struct SessionScope{uint64_t previous;explicit SessionScope(uint64_t value):previous(dispatchGeneration){dispatchGeneration=value;}~SessionScope(){dispatchGeneration=previous;}};
void ResetState(){wxl_retail_ui::ResetItemVariants();wxl_retail_ui::ResetWeeklyRewards();wxl_retail_ui::ResetCollections();wxl_retail_ui::ResetEquipment();wxl_retail_ui::ResetApply();wxl_retail_ui::ResetOutfits();wxl_retail_ui::ResetCustomSets();wxl_retail_ui::ResetSituations();}
void Reset(void*) noexcept {try{{std::lock_guard lock(inboxMutex);inbox.clear();inboxBytes=0;}ResetState();}catch(...){}}
void Receive(uint16_t opcode,const uint8_t* data,uint32_t size,uint64_t generation,void*) noexcept {try{
 if(!wxl::native::retail_ui::Enabled()||!size||size>protocol::MaxPayload)return;std::lock_guard lock(inboxMutex);if(inbox.size()>=128||size>4*1024*1024-inboxBytes){inbox.clear();inboxBytes=0;return;}
 Incoming next{opcode,generation,{data,data+size}};inboxBytes+=size;inbox.push_back(std::move(next));}catch(...){}}
template<size_t I>int Invoke(lua_State* L){if(L!=FrameScript_GetContext())return 0;try{SessionScope session(protocol::SessionGeneration(L));if(!wxl::native::retail_ui::Enabled()){Reset(nullptr);return 0;}return routes[I].callback?routes[I].callback(L):0;}catch(...){lua_pushnil(L);return 1;}}
template<size_t... I>constexpr auto Invokers(std::index_sequence<I...>){return std::array<lua_CFunction,sizeof...(I)>{&Invoke<I>...};}
constexpr auto invokers=Invokers(std::make_index_sequence<96>{});
int RegisterFunction(const char* name,lua_CFunction callback){if(!name||!callback)return 0;size_t i=0;for(;i<routeCount;++i)if(routes[i].name==name)break;if(i==routeCount){if(routeCount==routes.size())return 0;++routeCount;}routes[i]={name,callback};FrameScript_RegisterFunction(name,invokers[i]);return 1;}
int Script(const char* name,const char* source){if(!name||!source)return 0;if(!scripts.contains(name))scriptOrder.emplace_back(name);scripts[name]=source;return 1;}
int Execute(const char* source,const char* name){if(!source||!ready||readyContext!=FrameScript_GetContext()||!wxl::native::retail_ui::Enabled())return 0;FrameScript_Execute(source,name?name:"WXLNativeRetailUI",nullptr);return 1;}
int Client(uint16_t opcode,const char*){if(!collecting)return bound?1:0;for(const auto& e:declarations)if(e.opcode==opcode&&e.outbound)return 0;declarations.push_back({opcode,true});return 1;}
int Server(uint16_t opcode,const char*,PacketCallback callback,void* user){if(!callback)return 0;if(!collecting){auto i=receivers.find(opcode);return bound&&i!=receivers.end()&&i->second.callback==callback&&i->second.user==user;}
 if(!receivers.emplace(opcode,PacketOwner{callback,user}).second)return 0;declarations.push_back({opcode,false,&Receive,&Reset});return 1;}
int Send(uint16_t opcode,const void* bytes,uint32_t size){return wxl::native::retail_ui::Enabled()&&bound&&dispatchGeneration&&protocol::SendInSession(opcode,bytes,size,dispatchGeneration);}
int Pump(lua_State* L){if(pumping||!ready||L!=readyContext)return 0;if(!wxl::native::retail_ui::Enabled()){Reset(nullptr);return 0;}const auto generation=protocol::SessionGeneration(L);if(!generation)return 0;
 struct Guard{Guard(){pumping=true;}~Guard(){pumping=false;}} guard;
 std::deque<Incoming> messages;{std::lock_guard lock(inboxMutex);messages.swap(inbox);inboxBytes=0;}
 for(auto& m:messages){if(readyContext!=FrameScript_GetContext()||protocol::SessionGeneration(L)!=generation)break;if(m.generation!=generation)continue;auto i=receivers.find(m.opcode);if(i==receivers.end())continue;
  try{SessionScope session(m.generation);i->second.callback(m.bytes.data(),uint32_t(m.bytes.size()),i->second.user);}catch(...){Reset(nullptr);break;}}
 return 0;}
const wxl_retail_ui::NativeFrameScript framescript{&RegisterFunction,&Script,&Execute};const wxl_retail_ui::NativeNetwork network{&Client,&Server,&Send};const wxl_retail_ui::NativeFdid fdid{&wxl::native::ResolveTexture};
}
namespace wxl_retail_ui {const NativeFrameScript* FrameScript(){return &framescript;}const NativeNetwork* Network(){return &network;}const NativeFdid* Fdid(){return &fdid;}}
namespace wxl::native::retail_ui {
bool Enabled() noexcept {return WXL_ENABLE_RETAIL_UI&&NativeEnabled()&&enabled&&enabled->m_intValue;}
void Install(){if(!enabled)enabled=CVar::Register("wxlRetailUi","Native retail UI, wardrobe and weekly rewards bridges",0,WXL_ENABLE_RETAIL_UI?"1":"0",nullptr,0,false,nullptr,false);
 ready=false;readyContext=nullptr;Reset(nullptr);if(!Enabled())return;collecting=!bound;if(collecting){declarations.clear();receivers.clear();}
 bool ok=wxl_retail_ui::InstallRetailUiCompatibility();ok=wxl_retail_ui::InstallItemVariants()&&ok;ok=wxl_retail_ui::InstallRetailTransmogPreview()&&ok;ok=wxl_retail_ui::InstallWeeklyRewards()&&ok;ok=wxl_retail_ui::InstallRetailTransmog()&&ok;ok=wxl_retail_ui::InstallRetailTransmogEquipment()&&ok;ok=wxl_retail_ui::InstallRetailTransmogApply()&&ok;ok=wxl_retail_ui::InstallRetailTransmogOutfits()&&ok;ok=wxl_retail_ui::InstallRetailTransmogCustomSets()&&ok;ok=wxl_retail_ui::InstallRetailTransmogSituations()&&ok;
 if(!bound&&ok)bound=protocol::ClaimRawDispatch(declarations.data(),uint32_t(declarations.size()));collecting=false;if(!bound)WLOG_WARN("native-retail-ui: owner registration refused; native sends remain disabled");FrameScript_RegisterFunction("WXL_PumpRetailUi",&Pump);
}
void UiReady(){if(!Enabled())return;readyContext=FrameScript_GetContext();ready=readyContext!=nullptr;if(!ready)return;
 for(const auto& name:scriptOrder)Execute(scripts[name].c_str(),name.c_str());
 Execute("if not WXL_NativeRetailUiDriver then local f=CreateFrame('Frame');WXL_NativeRetailUiDriver=f;f:SetScript('OnUpdate',function()WXL_PumpRetailUi()end)end","WXLNativeRetailUiPump");
}
void Destroy() noexcept {ready=false;readyContext=nullptr;Reset(nullptr);}
}
