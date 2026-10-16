#include "WXL/GrassWind.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "common/Log.hpp"
#include <d3dcompiler.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#ifndef WXL_ENABLE_GRASS_WIND
#define WXL_ENABLE_GRASS_WIND 1
#endif
namespace wxl::native::grass_wind {namespace {
    const char* kMotionInject =
        "    dp3 r2.x, c0, r0\n"
        "    dp3 r2.y, c1, r0\n"
        "    add r2.xy, r2, c14\n"
        "    mov r4.xy, r2\n"
        "    dp3 r3.z, c2, r0\n"
        "    add r3.z, r3.z, c14.z\n"
        "    add r3.z, r3.z, -c16.z\n"
        "    add r3.z, r3.z, -c15.z\n"
        "    mul_sat r4.z, r3.z, c15.x\n"
        "    add r2.xy, r2, -c16\n"
        "    mul r2.z, r2.x, r2.x\n"
        "    mad r2.z, r2.y, r2.y, r2.z\n"
        "    add r2.z, r2.z, c15.w\n"
        "    rsq r2.w, r2.z\n"
        "    mul r3.x, r2.z, r2.w\n"
        "    mul_sat r3.y, r3.x, c15.y\n"
        "    mad r3.y, c17.x, r3.y, c17.y\n"
        "    slt r3.z, r3.x, c17.z\n"
        "    mul r3.y, r3.y, r3.z\n"
        "    mul r3.y, r3.y, r4.z\n"
        "    mul r3.y, r3.y, r2.w\n"
        "    mul r2.xy, r2, r3.y\n"
        "    add r5.z, c13.y, -v3.y\n"
        "    add r5.z, r5.z, c21.z\n"
        "    mul_sat r5.z, r5.z, c21.w\n"
        "    mul r5.z, r5.z, r5.z\n"
        "    mov r5.w, c13.y\n"
        "    mad r5.w, r0.z, c20.w, r5.w\n"
        "    rcp r5.w, r5.w\n"
        "    mul r5.z, r5.z, r5.w\n"
        "    mul r3.x, r4.x, c20.x\n"
        "    mad r3.x, r4.y, c20.y, r3.x\n"
        "    frc r3.x, r3.x\n"
        "    mad r2.z, r3.x, c21.y, c21.x\n"
        "    mul r5.z, r5.z, r2.z\n"
        "    mul r3.x, r3.x, c20.z\n"
        "    mul r3.y, r4.x, c18.z\n"
        "    mad r3.y, r4.y, c18.w, r3.y\n"
        "    add r3.y, r3.y, r3.x\n"
        "    add r3.y, r3.y, c17.w\n"
        "    mul r3.z, r4.x, c19.z\n"
        "    mad r3.z, r4.y, c19.w, r3.z\n"
        "    add r3.z, r3.z, r3.x\n"
        "    add r3.z, r3.z, c14.w\n"
        "    sincos r4.xy, r3.y\n"
        "    add r4.w, r4.y, c16.w\n"
        "    sincos r5.xy, r3.z\n"
        "    mul r3.zw, c18.xxxy, r4.wwww\n"
        "    mad r3.zw, c19.xxxy, r5.yyyy, r3\n"
        "    mul r3.zw, r3, r5.zzzz\n"
        "    add r2.xy, r2, r3.zwzw\n"
        "    mul r3.xyz, c0, r2.x\n"
        "    mad r3.xyz, c1, r2.y, r3.xyz\n"
        "    add r0.xyz, r0, r3\n";


constexpr float Pi2=6.2831853f;
struct Parameter {const char* name;const char* value;float low,high;std::atomic<float> number;};
Parameter parameters[]{
 {"wxlGrassWind","1",0,1,1},{"wxlGrassSway","1",0,1,1},{"wxlGrassRepulsion","1",0,1,1},
 {"wxlGrassDirection","45",-360,360,45},{"wxlGrassSpeed","3",0,30,3},{"wxlGrassAmplitude","0.060",0,2,.060f},{"wxlGrassWavelength","18",.1f,200,18},
 {"wxlGrassCrossAmplitude","0.02",0,2,.02f},{"wxlGrassCrossWavelength","6.5",.1f,200,6.5f},{"wxlGrassCrossAngle","35",-360,360,35},
 {"wxlGrassLean","0.35",0,5,.35f},{"wxlGrassVariance","0.6",0,1,.6f},{"wxlGrassDistanceFade","0.015",0,1,.015f},{"wxlGrassAnchor","0.3",0,.9f,.3f},
 {"wxlGrassRepulsionRadius","2",.1f,30,2},{"wxlGrassForceCenter","0.5",0,10,.5f},{"wxlGrassForceEdge","0",0,10,0},{"wxlGrassConeHeight","0.5",.05f,20,.5f},{"wxlGrassMinHeight","0.3",0,10,.3f}};
bool Changed(CVar*,const char*,const char* text,void* arg){auto* p=static_cast<Parameter*>(arg);if(!p||!text)return false;char* end=nullptr;const float n=std::strtof(text,&end);if(end==text||*end||!std::isfinite(n))return false;p->number=std::clamp(n,p->low,p->high);return true;}
float Value(unsigned i){return parameters[i].number.load(std::memory_order_relaxed);}
struct Library {HMODULE handle=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);~Library(){if(handle)FreeLibrary(handle);}};
using AssembleFn=HRESULT(WINAPI*)(LPCVOID,SIZE_T,LPCSTR,const D3D_SHADER_MACRO*,ID3DInclude*,UINT,ID3DBlob**,ID3DBlob**);
template<class T>struct Com{T* p=nullptr;~Com(){if(p)p->Release();}};
struct Entry {std::vector<uint8_t> source;std::vector<DWORD> injected;};
struct Storage {std::vector<Entry> code;std::map<const CGxShader*,void*> created;};
// Process-lifetime CPU metadata: native shader destructors can run during CRT shutdown.
Storage& Data(){static auto* data=new Storage;return *data;}
const std::vector<DWORD>* Inject(const uint8_t* bytes,uint32_t length){auto& data=Data();for(auto& e:data.code)if(e.source.size()==length&&!std::memcmp(e.source.data(),bytes,length))return e.injected.empty()?nullptr:&e.injected;if(data.code.size()>=12)return nullptr;
 Entry e;e.source.assign(bytes,bytes+length);Library loaded;auto library=loaded.handle;if(!library)return nullptr;
 auto disassemble=reinterpret_cast<decltype(&D3DDisassemble)>(GetProcAddress(library,"D3DDisassemble"));auto assemble=reinterpret_cast<AssembleFn>(GetProcAddress(library,"D3DAssemble"));
 Com<ID3DBlob> text,code,error;bool ok=disassemble&&assemble&&SUCCEEDED(disassemble(bytes,length,0,nullptr,&text.p))&&text.p;
 if(ok){std::string source(static_cast<const char*>(text.p->GetBufferPointer()),text.p->GetBufferSize());while(!source.empty()&&!source.back())source.pop_back();auto at=source.find("add r0, r0, c3");if(at!=std::string::npos)at=source.find('\n',at);if(at==std::string::npos)ok=false;else{source.insert(at+1,kMotionInject);ok=SUCCEEDED(assemble(source.data(),source.size(),"WxlGrassMotion",nullptr,nullptr,0,&code.p,&error.p))&&code.p;if(ok){const auto size=code.p->GetBufferSize();if(!size||size%4||size>0x20000)ok=false;else{e.injected.resize(size/4);std::memcpy(e.injected.data(),code.p->GetBufferPointer(),size);}}}}
 data.code.push_back(std::move(e));if(!ok){WLOG_WARN("native-grass-wind: unsupported shader shape; native grass retained");return nullptr;}return &data.code.back().injected;
}
}
void Register(){for(auto& p:parameters){auto* cv=CVar::Register(p.name,"WXL grass sway/parting parameter",0,p.value,Changed,0,false,&p,false);if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,&p);}}
bool Enabled() noexcept{return WXL_ENABLE_GRASS_WIND&&NativeEnabled()&&Value(0)!=0;}
void Forget(CGxShader& shader) noexcept {try{Data().created.erase(&shader);}catch(...){}}
const void* VertexCode(CGxShader& shader,uint32_t& length) noexcept {length=0;try{
 if(!WXL_ENABLE_GRASS_WIND||!NativeEnabled())return nullptr;bool grass=false;for(auto* actual:CDetailDoodad::s_vertexShader)if(actual==&shader)grass=true;if(!grass)return nullptr;
 const auto size=shader.m_code.Count();if(!shader.m_code.m_data||size<8)return nullptr;uint32_t version=0;std::memcpy(&version,shader.m_code.m_data,4);
 constexpr uint32_t v2[]{552,644,788,1428,1520,1664},v3[]{548,676,844,1328,1456,1624};bool known=false;for(unsigned i=0;i<6;++i)known|=(version==0xfffe0200u&&size==v2[i])||(version==0xfffe0300u&&size==v3[i]);if(!known)return nullptr;
 auto* code=Inject(shader.m_code.m_data,size);if(!code)return nullptr;length=uint32_t(code->size()*sizeof(DWORD));return code->data();
 }catch(...){return nullptr;}}
bool CreateVertex(CGxDevice& owner,CGxShader& shader) noexcept {try{
 Forget(shader);uint32_t length=0;auto* code=static_cast<const DWORD*>(VertexCode(shader,length));if(!code)return false;IDirect3DDevice9* d=nullptr;if(owner.m_api==GxApi_D3d9)d=static_cast<CGxDeviceD3d&>(owner).m_d3dDevice;else if(owner.m_api==GxApi_D3d9Ex)d=static_cast<CGxDeviceD3d9Ex&>(owner).m_d3dDevice;if(!d)return false;
 Com<IDirect3DVertexShader9> created;if(FAILED(d->CreateVertexShader(code,&created.p))||!created.p)return false;
 // Publish the CPU identity before transferring COM ownership; exceptions cannot leak a shader.
 Data().created[&shader]=created.p;shader.m_apiSpecific=created.p;created.p=nullptr;shader.m_valid=1;shader.m_loaded=1;return true;
 }catch(...){return false;}}
void Upload(CGxShader& shader) noexcept {try{
 if(!g_theGxDevicePtr||(g_theGxDevicePtr->m_api!=GxApi_D3d9&&g_theGxDevicePtr->m_api!=GxApi_D3d9Ex&&g_theGxDevicePtr->m_api!=GxApi_D3d11)||!shader.Valid())return;
 if(g_theGxDevicePtr->m_api==GxApi_D3d11){uint32_t length=0;if(!VertexCode(shader,length))return;}
 else {const auto it=Data().created.find(&shader);if(it==Data().created.end()||it->second!=shader.m_apiSpecific||!shader.m_valid)return;}
 auto* frame=CGWorldFrame::s_currentWorldFrame;if(!frame||!frame->m_camera)return;const auto cam=frame->m_camera->m_position;auto player=cam;bool hasPlayer=false;if(auto* active=ClntObjMgrGetActivePlayerObj()){player=active->GetModelPosition();hasPlayer=true;}
 const bool enabled=Enabled(),sway=enabled&&Value(1)!=0,rep=enabled&&Value(2)!=0&&hasPlayer;
 const float t=(GetTickCount()%1000000u)*.001f,a1=Value(3)*Pi2/360.f,a2=(Value(3)+Value(9))*Pi2/360.f,k1=Pi2/Value(6),k2=Pi2/Value(8),d1x=std::cos(a1),d1y=std::sin(a1),d2x=std::cos(a2),d2y=std::sin(a2);
 const float amp1=sway?Value(5):0,amp2=sway?Value(7):0,center=rep?Value(15):0,edge=rep?Value(16):0,var=Value(11),anchor=Value(13);
 const float constants[8][4]{
 {cam.x,cam.y,cam.z,-std::fmod(k2*Value(4)*1.37f*t,Pi2)},
 {1.f/Value(17),1.f/Value(14),Value(18),.001f},
 {player.x,player.y,player.z,sway?Value(10):0},
 {edge-center,center,Value(14),-std::fmod(k1*Value(4)*t,Pi2)},
 {d1x*amp1,d1y*amp1,d1x*k1,d1y*k1},{d2x*amp2,d2y*amp2,d2x*k2,d2y*k2},
 {.737f,1.311f,1.7f,Value(12)},{1.f-var*.5f,var,-anchor,1.f/(1.f-anchor)}};
 for(auto& row:constants)for(float n:row)if(!std::isfinite(n))return;
 // Native base constants were just uploaded. Only this proven patched permutation receives c14..21;
 // unpatched stock point-light slots are never overwritten.
 g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex,14,&constants[0][0],8);
 }catch(...){}}
}
