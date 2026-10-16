// Source-native WMO material module. GPL-3.0-or-later.
#pragma once
namespace wxl::waystone::wmo {struct MaterialPlan;}
#include <memory>
#include <vector>
class CGxDevice;class CMapObj;class CMapObjGroup;struct IDirect3DDevice9;
namespace wxl::native {struct WmoMaterialResources;
namespace wmo_materials {
struct GroupSource {std::shared_ptr<const wxl::waystone::wmo::MaterialPlan> materials;std::vector<uint8_t> stream,auxiliary;uint32_t vertices=0;};
std::shared_ptr<const GroupSource> FindSource(CMapObj&,CMapObjGroup&) noexcept;
void Register();bool Enabled() noexcept;bool LayeredReady() noexcept;
void DeviceReady(CGxDevice&) noexcept;void Release(CGxDevice&) noexcept;void EndWorld() noexcept;
class BatchScope {public:BatchScope(CMapObj&,CMapObjGroup&,unsigned) noexcept;~BatchScope();bool Modern() const noexcept;private:std::shared_ptr<const GroupSource> previous;unsigned previousMaterial;};
bool Draw(IDirect3DDevice9*,uint32_t primitive,int32_t base,uint32_t min,uint32_t vertices,uint32_t start,uint32_t count) noexcept;
}}
